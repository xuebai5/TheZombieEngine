#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolplacer_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolplacer.h"
#include "entity/nentityobjectserver.h"
#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "ngeomipmap/nfloatmap.h"
#include "nphysics/ncphysicsobj.h"
#include "nspatial/ncspatialclass.h"
#include "nspatial/ncspatialspace.h"
#include "ndebug/nceditor.h"
#include "conjurer/nconjurerapp.h"
#include "conjurer/nobjecteditorstate.h"
#include "zombieentity/ncsuperentity.h"
#include "zombieentity/nctransform.h"
#include "nlayermanager/nlayermanager.h"
#include "zombieentity/ncsubentity.h"
#include "mathlib/nmath.h"
#include "nworldinterface/nworldinterface.h"
#include "ntrigger/nctriggershape.h"
#include "zombieentity/nctransformclass.h"

nNebulaScriptClass(nInguiToolPlacer, "ninguitool");

//------------------------------------------------------------------------------
/**
*/
nInguiToolPlacer::nInguiToolPlacer():
    doPhysicPicking( true ),
    randomRotation( false ),
    sizeVariation( 0.0f )
{
    label = "Place entity";
    pickWhileIdle = false;
}

//------------------------------------------------------------------------------
/**
*/
nInguiToolPlacer::~nInguiToolPlacer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @brief Handle input in a viewport.
    @param vp Viewport in wich the mouse pointer is located
    @param ray The ray to do the pick, in world space
    @return The ray parameter of picking position (for ordering intersections), or -1 if couldn't pick.
*/
float
nInguiToolPlacer::Pick( nAppViewport* vp, vector2 mp, line3 ray)
{

    float d = -1.0f;
    if ( doPhysicPicking )
    {
        d = nInguiToolPhyPick::Pick( vp, mp, ray );
    }
    else if ( this->refCollisionHeightmap.isvalid() )
    {
        int x, z;
        if ( this->refCollisionHeightmap->Intersect(line3(ray.b, ray.b+ray.m), d, x, z) )
        {
            this->lastPos = ray.ipol( d );

            // Grid Snapping
            if ( this->refGrid.isvalid() )
            {
                this->firstPos = this->refGrid->SnapPosition( this->firstPos );
                this->lastPos = this->refGrid->SnapPosition( this->lastPos );
            }

            float h;
            vector3 n;
            if ( this->refCollisionHeightmap->GetHeightNormal( this->lastPos.x, this->lastPos.z, h, n ) )
            {
                if ( this->firstPos.y < h )
                {
                    this->firstPos.y = h;
                }
                if ( this->lastPos.y < h )
                {
                    this->lastPos.y = h;
                }
                this->pickingNormal = n;
            }
        }
        else d = -1.0f;
    }

	// If didn't pick, try with the XZ plane
	if ( d < 0.0f )
	{
		plane planeXZ(vector3(0.0f,0.0f,0.0f), vector3(1.0f,0.0f,0.0f), vector3(0.0f,0.0f,1.0f) );
		if ( planeXZ.intersect( line3(ray.b, ray.b+ray.m), d ) && d > 0.0f )
		{
			this->lastPos = ray.ipol( d );

			// Grid Snapping
			if ( this->refGrid.isvalid() )
			{
				this->firstPos = this->refGrid->SnapPosition( this->firstPos );
				this->lastPos = this->refGrid->SnapPosition( this->lastPos );
			}
		}
		else
		{
			d = -1.0f;
		}
	}

	// If still didn't pick,
    if ( d < 0.0f )
    {
        // Put object in the air, some diameters away
        if ( this->classList->Size() == 0 )
        {
            return -1.0f;
        }
        nEntityClass* entClass = nEntityClassServer::Instance()->GetEntityClass( this->classList->GetString( 0 )->Get() );
        if ( !entClass )
        {
            return -1.0f;
        }

        ncSpatialClass* spatialClass = entClass->GetComponent<ncSpatialClass>();
        if (spatialClass)
        {
            const bbox3& b = spatialClass->GetOriginalBBox();
            float diameter = b.diagonal_size();
            d = max( 1.0f, 5.0f * diameter ) / ray.len();
            this->lastPos = ray.ipol( d );    
        }
        else
        {
            return -1.0f;
        }

        // Grid Snapping
        if ( this->refGrid.isvalid() )
        {
            this->firstPos = this->refGrid->SnapPosition( this->firstPos );
            this->lastPos = this->refGrid->SnapPosition( this->lastPos );
        }
    }

    return d;
}

//------------------------------------------------------------------------------
/**
    @brief Apply the tool
*/
bool
nInguiToolPlacer::Apply( nTime /*dt*/ )
{
    if ( this->classList->Size() == 0 )
    {
        // No class was selected
        return false;
    }

    // Reset instance array if first frame
    if ( this->state <= nInguiTool::Inactive )
    {
        instanceArray.Clear();
    }

    // Are we creating a subentity?
    bool isSubentity = this->refObjState->GetSelectionMode() == nObjectEditorState::ModeSubentity;
    bool putAnyIndoor = false;
    for ( int classIndex = 0; classIndex  < this->classList->Size(); classIndex ++ )
    {
        if ( this->state <= nInguiTool::Inactive )
        {
            // Create the entity
            const char * className = this->classList->GetString( classIndex )->Get();
            if ( isSubentity )
            {
                // Subentity
                nEntityObject* brush = refObjState->GetSelectionModeEntity();
                n_assert( brush );
                this->entityInstance = nWorldInterface::Instance()->NewLocalEntity( className, this->lastPos, isSubentity, brush );
            }
            else
            {
                // Normal entity
                this->entityInstance = nWorldInterface::Instance()->NewEntity( className, this->lastPos );
            }

            if ( ! this->entityInstance )
            {
                return false;
            }

            // Log entity id in instanceArray
            instanceArray.Append( this->entityInstance->GetId() );

            nEntityClass* instanceClass = nEntityClassServer::Instance()->GetEntityClass( this->classList->GetString( classIndex )->Get() );
            n_assert( instanceClass );

            bool isIndoorClass = instanceClass->IsA( "neindoorclass" );

            if ( isIndoorClass )
            {
                putAnyIndoor = true;
            }

            // Set random orientation, if the option is enabled
            ncTransform* trComp = entityInstance->GetComponent<ncTransform>();
            if ( this->randomRotation && ! isIndoorClass )
            {
                quaternion q;
                q.set_rotate_y( n_rand() * (2*PI) );
                trComp->SetQuat( q );
            }

            // Set random size, if the option is enabled
            if ( this->sizeVariation != 0.0f )
            {
                vector3 v(1.0f,1.0f,1.0f);
                v *= max( 0.1f, 1.0f + ( n_rand() - 0.5f ) * this->sizeVariation );
                trComp->SetScale( v );
            }

			// move the indoor to its position
            trComp->SetPosition( this->lastPos );

            //check if entity has nctriggershape component
            ncTriggerShape * trigShapeComp = this->entityInstance->GetComponent<ncTriggerShape>();
            if ( trigShapeComp )
            {
                // set 0 radius shape
                trigShapeComp->SetCircle( 0.0f );
            }
        }

        if ( this->entityInstance )
        {
            // If not first frame
            if ( this->refLastPickedEntity == 0 || this->entityInstance != this->refLastPickedEntity )
            {
                // Set entity position and continue moving no-indoor entity
                ncTransform* trComp = this->entityInstance->GetComponent<ncTransform>();
                if ( trComp && this->state != nInguiTool::Finishing )
                {
                    trComp->SetPosition( this->lastPos );
                }
            }

            if ( this->state == nInguiTool::Finishing )
            {
                this->refObjState->ResetSelection();
                this->refObjState->AddEntityToSelection( this->entityInstance->GetId() );

                // Signal that an entity has being placed (mainly to allow Conjurer to set some default values)
                refObjState->SetEntityPlaced( this->entityInstance );
                
                refLastPickedEntity = 0;
            }
        }
    }

    if ( putAnyIndoor )
    {
        nSpatialServer::Instance()->ConnectIndoors();
    }

    // Signal GUI that some entity could have been created or deleted
    this->refObjState->SignalEntityModified( this->refObjState );

    return true;

}

//------------------------------------------------------------------------------
/**
    Set class names list
*/
void
nInguiToolPlacer::SetClassList( nStringList*list )
{
    nEntityClassServer* entClassServer = nEntityClassServer::Instance();

    // Check that all classes have ncTransform
    for ( int i = 0; i < list->Size(); i++ )
    {
        nString* str = list->GetString( i );
        nEntityClass* cl = entClassServer->GetEntityClass( str->Get() );
        cl->GetComponentSafe<ncTransformClass>();
    }

    this->classList = list;
}

//------------------------------------------------------------------------------
/**
    Access to class names list
*/
nArray<nEntityObjectId>*
nInguiToolPlacer::GetEntityList()
{
    return &this->instanceArray;
}

//------------------------------------------------------------------------------
/**
    Assign entity type to instantiate
*/
void
nInguiToolPlacer::SetInstanceClass( nString entClass )
{
    this->classList->Clear();
    this->classList->AppendString( entClass );
}
//------------------------------------------------------------------------------

/**
    Get entity type to instantiate
*/
nString
nInguiToolPlacer::GetInstanceClass()
{
    if ( this->classList->Size() > 0 )
    {
        return * this->classList->GetString( 0 );
    }
    else
    {
        return nString();
    }
}
//------------------------------------------------------------------------------
/**
    @brief Set outdoor entity object
*/
void
nInguiToolPlacer::SetOutdoor( nEntityObject * object )
{
    ncTerrainGMMClass * terraincomp = object->GetClassComponent<ncTerrainGMMClass>();
    if (terraincomp)
    {
        this->refCollisionHeightmap = terraincomp->GetHeightMap();
    }
}
//------------------------------------------------------------------------------
/**
    @brief Set random rotation
    @param The flag
*/
void
nInguiToolPlacer::SetRandomRotation( bool randomRot )
{
    this->randomRotation = randomRot;
}

//------------------------------------------------------------------------------
/**
    @brief Get random rotation
    @return The flag
*/
bool
nInguiToolPlacer::GetRandomRotation( void )
{
    return this->randomRotation;
}
//------------------------------------------------------------------------------
/**
    @brief Set size variation
    @param size variation factor, e.g: 0.1 = variation of +-5%
*/
void
nInguiToolPlacer::SetSizeVariation( float sizeVar )
{
    this->sizeVariation = sizeVar;
}

//------------------------------------------------------------------------------
/**
    @brief Get size variation
    @return Intensity
*/
float
nInguiToolPlacer::GetSizeVariation( void )
{
    return this->sizeVariation;
}

//------------------------------------------------------------------------------

