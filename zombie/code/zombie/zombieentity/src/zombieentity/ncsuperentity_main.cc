#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  ncSuperentity_main.cc
//------------------------------------------------------------------------------

#include "zombieentity/ncsuperentity.h"
#include "zombieentity/ncsubentity.h"
#include "entity/nentityobjectserver.h"
#include "kernel/nfileserver2.h"
#include "zombieentity/nctransform.h"
#include "entity/nobjectinstancer.h"
#include "nphysics/ncphypickableobj.h"
#include "nphysics/ncphysicsobj.h"
#include "nspatial/nspatialserver.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#endif //NGAME

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSuperentity, nComponentObject);


nEntityObject* ncSuperentity::currentSuperEntity(0);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSuperentity)
    NSCRIPT_ADDCMD_COMPOBJECT('LGSE', nObjectInstancer*, GetSubentities, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Constructor
*/
ncSuperentity::ncSuperentity():
    subentities(0)
#ifndef NGAME
    ,loaded(true)
#endif//!NGAME
{ 
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncSuperentity::~ncSuperentity()
{
    this->DeleteSubentities();
}

//------------------------------------------------------------------------------
/**
    Initialize the component
*/
void 
ncSuperentity::InitInstance(nObject::InitInstanceMsg initType)
{
#ifndef NGAME
    if( initType != nObject::ReloadedInstance )
    {
        // Register to EnterLimbo and ExitLimbo signals
        this->entityObject->BindSignal( NSIGNAL_OBJECT(nEntityObject, EnterLimbo), this, &ncSuperentity::DeleteSubentities, 0 );
        this->entityObject->BindSignal( NSIGNAL_OBJECT(nEntityObject, ExitLimbo), this, &ncSuperentity::LoadSubentities, 0 );
    }

    this->SetIsLoaded(initType != nObject::NewInstance);
#endif

    if( initType != nObject::ReloadedInstance )
    {
        this->LoadSubentities();
    }
}

//------------------------------------------------------------------------------
/**
*/
nEntityObject* 
ncSuperentity::GetCurrentSuperEntity()
{
    return ncSuperentity::currentSuperEntity;
}


//------------------------------------------------------------------------------
/**
    @brief Load subentities from instancer in a class-dependent path
*/
void
ncSuperentity::LoadSubentities()
{
    if ( this->subentities )
    {
        this->DeleteSubentities();
    }

    // Trick for mark this as current super entity, this is not ok with threads. the ncSubentity in 
    // the initInstance set the super entinty the ncSuperentity::currentSuperEntity and then update the
    // position, this solution has a problem if you load subentinties in other thread
    /// todo search a better solution, any components needs the good position in the initinstance.
    nEntityObject* previousSuperEntity = ncSuperentity::currentSuperEntity;
    ncSuperentity::currentSuperEntity = this->GetEntityObject();

    nString path;
    this->GetSubentitiesPath( path );
    path += "/subentities.n2";
    this->subentities = static_cast<nObjectInstancer*>( nKernelServer::Instance()->Load( path.Get() ) );
    ncSuperentity::currentSuperEntity = previousSuperEntity;

    if ( ! this->subentities )
    {
        return;
    }

    // Update subentities references to other entities by emitting a signal for converting persisted oid's in runtime oid's
    for ( int sub = 0; sub < this->subentities->Size(); sub++ )
    {
        nEntityObject* subEnt = static_cast<nEntityObject*>( this->subentities->At( sub ) );
        ncSubentity* subEntComp = subEnt->GetComponent<ncSubentity>();
        n_assert( subEntComp );

        subEntComp->SetSuperentity( this->entityObject );

        subEntComp->SignalUpdateEntityReferences( subEnt );
    }

    this->LoadSubentitiesLoaderComponents();

    //this->UpdateSubentities(); not update subentities because in the init instance load a good position.
    this->UpdateAABB();

    for ( int sub(0); sub < this->subentities->Size(); ++sub )
    {
        nEntityObject* subEnt(static_cast<nEntityObject*>( this->subentities->At( sub ) ));

        n_assert2( subEnt, "Data corruption in container." );
     
        ncSubentity* subEntComp(subEnt->GetComponent<ncSubentity>());

        n_assert2( subEntComp, "Missing the required component ncSubEntity." );

        subEntComp->SignalSubEntitiesCreated( subEnt );

        // insert the subentity in the world
#ifndef NGAME
        if ( nSpatialServer::Instance()->GetExecutionMode() == nSpatialServer::GAME )
#endif // !NGAME
        {
            nSpatialServer::Instance()->InsertEntity(subEnt);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Save subentities
*/
void
ncSuperentity::SaveSubentities()
{
    if ( ! this->subentities )
    {
        // Nothing to save
        return;
    }

    nString path;
    this->GetSubentitiesPath( path );
    nFileServer2::Instance()->MakePath( path.Get() );
    path += "/subentities.n2";
    this->subentities->SaveAs( path.Get() );    
}

//------------------------------------------------------------------------------
/**
    @brief Update global position of subentities
    Also Updates bounding box of superentity to contain all subentities
*/
void
ncSuperentity::UpdateSubentities()
{
    if ( this->subentities )
    {
        //ncTransform* supEntTrComp = GetComponent<ncTransform>();
        int n = this->subentities->Size();
        for ( int i=0; i<n; i++ )
        {
            nEntityObject* ent0 = static_cast<nEntityObject*>( this->subentities->At( i ) );
            ncSubentity* subEntComp = ent0->GetComponent<ncSubentity>();
            n_assert( subEntComp );
            subEntComp->SetSuperentity( this->GetEntityObject() );
            subEntComp->UpdatenTrasformFromRelativePosition();
        }

        this->UpdateAABB();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get new subentity id
    Subentity ids are of the form 0x00hhiiiiii with hh being the entity server high id and iiiiii the local number of subentity 
*/
nEntityObjectId
ncSuperentity::GetNewSubentityOID()
{
    nEntityObjectId highID = nEntityObjectServer::Instance()->GetHighId();
    highID >>= nEntityObjectServer::IDHIGHBITS;
    return ( highID ) | ( this->subentities->Size() & nEntityObjectServer::IDSUBENTITYMASK );
} 

//------------------------------------------------------------------------------
/**
    @brief Add subentity to this entity
*/
bool
ncSuperentity::AddSubentity( nEntityObject* subEnt )
{
    ncSubentity* subComp = subEnt->GetComponent<ncSubentity>();
    if ( ! subComp )
    {
        return false;
    }
    if ( ! this->subentities )
    {
        this->GetSubentities();
    }

    if( this->subentities->FindIndex( subEnt ) != -1 )
    {
        // it's already in the list (prevention code)
        return false;
    }

    this->subentities->Append( subEnt );

    subComp->SetSuperentity( this->entityObject );

    subComp->SetPersistenceOID( this->GetNewSubentityOID() );
    
#ifndef NGAME
    subComp->UpdateRelativePosition();
#endif

    this->UpdateAABB();

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Get subentities object instancer. Creates it if it's not been created yet
    @return The object instancer
    If a subentity position is modified, its method UpdateRelativePosition should be called.
    If the superentity position is modified, the method UpdateSubentities should be called.
*/
nObjectInstancer*
ncSuperentity::GetSubentities()
{
    if ( ! this->subentities )
    {
        this->subentities = static_cast<nObjectInstancer*>(nKernelServer::Instance()->New("nobjectmagicinstancer"));
        this->subentities->Init(10,10);
        n_assert( this->subentities );
    }

    return this->subentities;
}

//------------------------------------------------------------------------------
/**
    @brief Get subentities object instancer path for persistence (only directory part)
    @return the path
*/
void
ncSuperentity::GetSubentitiesPath( nString& str)
{
    str = "wc:classes/";
    str += this->GetEntityClass()->GetName();
    str = nFileServer2::Instance()->ManglePath( str.Get(), false);
}

//------------------------------------------------------------------------------
/**
    @brief Release subentities instancer
*/
void
ncSuperentity::ReleaseSubentities()
{
    if ( this->subentities )
    {
        this->subentities->Clear();
        this->subentities->Release();
        this->subentities = 0;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Delete subentities and release instancer
*/
void
ncSuperentity::DeleteSubentities()
{
    if ( this->subentities )
    {
        int n = this->subentities->Size();
        for ( int i = n-1; i>=0; i-- )
        {
            nEntityObject* obj = static_cast<nEntityObject*>( this->subentities->At( i ) );
            nEntityObjectServer::Instance()->RemoveEntityObject( obj );
        }
        this->ReleaseSubentities();
    }
}

//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncSuperentity::SaveCmds(nPersistServer* ps)
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Update all superentities of same class
*/
void
ncSuperentity::UpdateAllSuperentities()
{
    // Reload subentities of all superentities of the same class or inherited class
    nEntityObjectServer* entServer = nEntityObjectServer::Instance();
    nEntityObject* entObj = entServer->GetFirstEntityObject();
    while ( entObj )
    {
        ncSuperentity* superComp = entObj->GetComponent<ncSuperentity>();
        if ( superComp )
        {
            superComp->LoadSubentities();
        }        

        entObj = entServer->GetNextEntityObject();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Load ncLoader components of all subentities
*/
void
ncSuperentity::LoadSubentitiesLoaderComponents()
{
    if ( this->subentities )
    {
        for ( int sub = 0; sub < this->subentities->Size(); sub++ )
        {
            nEntityObject* subEnt = static_cast<nEntityObject*>( this->subentities->At( sub ) );
            N_IFDEF_ASSERTS(ncSubentity* subEntComp = )subEnt->GetComponent<ncSubentity>();
            n_assert( subEntComp );

            ncLoaderClass *loaderClass = subEnt->GetClassComponent<ncLoaderClass>();
            if (loaderClass && !loaderClass->AreResourcesValid())
            {
                if (!loaderClass->LoadResources())
                {
                    return;// false;
                }
            }

            ncLoader* loaderComp = subEnt->GetComponent<ncLoader>();
            if ( loaderComp && ! loaderComp->AreComponentsValid() )
            {
                loaderComp->LoadComponents();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Update superentity physic AABB to enclose subentities
*/
void
ncSuperentity::UpdateAABB()
{
#ifndef NGAME
    ncTransform* supEntTrComp = GetComponentSafe<ncTransform>();
    ncPhyPickableObj* supEntPhyComp = this->GetComponent<ncPhyPickableObj>();
    if ( this->subentities && supEntPhyComp )
    {
        vector3 subExtents(0.0f, 0.0f, 0.0f);
        int n = this->subentities->Size();
        for ( int i=0; i<n; i++ )
        {
            nEntityObject* ent0 = static_cast<nEntityObject*>( this->subentities->At( i ) );
            ncSubentity* subEntComp = ent0->GetComponentSafe<ncSubentity>();
            ncPhysicsObj* subPhyComp = ent0->GetComponent<ncPhysicsObj>();
            
            vector3 relPos = subEntComp->GetRelativePosition();
            relPos.abs();

            if ( subPhyComp )
            {
                nPhysicsAABB bb;
                subPhyComp->GetAABB( bb );
                vector3 subExt = vector3(bb.maxx - bb.minx, bb.maxy - bb.miny, bb.maxz - bb.minz );
                subExt.abs();
                relPos += subExt;
            }

            if ( i == 0 )
            {
                subExtents = relPos;
            }
            else
            {
                subExtents.x = max( abs( relPos.x ), subExtents.x );
                subExtents.y = max( abs( relPos.y ), subExtents.y );
                subExtents.z = max( abs( relPos.z ), subExtents.z );
            }
        }

        // Update box lengths
        supEntPhyComp->SetLengths( subExtents * 2.0f );

        // This makes the box update
        supEntTrComp->SetPosition( supEntTrComp->GetPosition() );
    }
#endif NGAME
}

#ifndef NGAME
    
//-----------------------------------------------------------------------------
/**
    Returns if the entity has been loaded or instanciated

    @return true/false

    history:
        - 19-Dec-2005   David Reyes    created
*/
const bool ncSuperentity::IsLoaded() const
{
    return this->loaded;
}

//-----------------------------------------------------------------------------
/**
    Sets if an entity has been loaded or instanciated

    @param is if the entity is been loaded or instanciated

    history:
        - 19-Dec-2005   David Reyes    created
*/
void ncSuperentity::SetIsLoaded( const bool is )
{
    this->loaded = is;
}
        
#endif//!NGAME

//------------------------------------------------------------------------------
