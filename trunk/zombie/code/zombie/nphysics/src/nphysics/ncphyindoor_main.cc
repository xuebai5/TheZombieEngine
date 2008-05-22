//-----------------------------------------------------------------------------
//  ncphyindoor_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"

#include "nphysics/ncphyindoorclass.h"
#include "nphysics/nphysicsserver.h"
#include "kernel/nfileserver2.h"
#include "entity/nobjectinstancer.h"
#include "nphysics/ncphysimpleobj.h"
#include "zombieentity/nctransform.h"

//-----------------------------------------------------------------------------
#ifdef NGAME
nNebulaComponentObject(ncPhyIndoor,ncPhysicsObj);
#else
nNebulaComponentObject(ncPhyIndoor,ncPhyCompositeObj);
#endif

//-----------------------------------------------------------------------------

ncPhyIndoor* ncPhyIndoor::lastLoading(0);

//-----------------------------------------------------------------------------
/**
    Constructor.

    history:
        - 30-May-2005   Zombie         created
*/
ncPhyIndoor::ncPhyIndoor() :
    indoorSpace(0),
    spaces(NumInitialSpaces,NumGrowthSpaces),
    objectsContainer(NumInitialObjects,NumGrowthObjects)
{
    this->SetType( Indoor );
    
    this->CreateSpace();
}

//-----------------------------------------------------------------------------
/**
    Destructor.

    history:
        - 30-May-2005   Zombie         created
*/
ncPhyIndoor::~ncPhyIndoor()
{
    for( int index(0); index < this->objectsContainer.Size(); ++index )
    {
        this->objectsContainer.GetElementAt(index)->SetOwner(0);
        /// @todo mateu.batle FIX this horrible hack for DEMO 1st Aug
        while(this->objectsContainer.GetElementAt(index)->GetEntityObject()->GetRefCount() > 1)
        {
            this->objectsContainer.GetElementAt(index)->GetEntityObject()->Release();
        }
        nEntityObjectServer::Instance()->RemoveEntityObject( 
            this->objectsContainer.GetElementAt(index)->GetEntityObject() );
    }

    // destroying subspaces
    while( this->spaces.Size() )
    {
        nPhySpace* space(this->spaces.GetElementAt(0));

        this->spaces.RemByIndex(0);

        space->Release();
    }

    // destroy the space
    if( this->indoorSpace )
    {
        this->indoorSpace->Release();
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the indoor's space.

    history:
        - 30-May-2005   Zombie         created
*/
void ncPhyIndoor::CreateSpace()
{
    this->indoorSpace = static_cast<nPhySpace*>
        (nKernelServer::Instance()->New( "nphygeomhashspace" ));

    n_assert2( this->indoorSpace, "Failed to create the indoor space." );

#ifndef NGAME
    ncPhysicsObj::MoveToSpace( this->indoorSpace );
#endif

    this->indoorSpace->SetCategories( nPhysicsGeom::Static );
    this->indoorSpace->SetCollidesWith( nPhysicsGeom::Dynamic | nPhysicsGeom::Check );
}

//-----------------------------------------------------------------------------
/**
    Sets the indoor's name.

    @param name new indoors name

    history:
        - 30-May-2005   Zombie         created
*/
void ncPhyIndoor::SetIndoorName( const nString& name )
{
    n_assert2( this->indoorSpace, "The space hasn't been created yet." );

    this->indoorSpace->SetName( name );
}

//-----------------------------------------------------------------------------
/**
    Enables the indoor.

    history:
        - 30-May-2005   Zombie         created
*/
void ncPhyIndoor::Enable()
{
    n_assert2( this->indoorSpace, "The space hasn't been created yet." );
    
    this->indoorSpace->Enable();
}

//-----------------------------------------------------------------------------
/**
    Disables the indoor.

    history:
        - 30-May-2005   Zombie         created
*/
void ncPhyIndoor::Disable()
{
    n_assert2( this->indoorSpace, "The space hasn't been created yet." );
    
    this->indoorSpace->Disable();
}

//-----------------------------------------------------------------------------
/**
    Creates the object.

    @param world it's the world where the object will be come to existance

    history:
        - 30-May-2005   Zombie         created
*/
void ncPhyIndoor::Create( nPhysicsWorld* world )
{
#ifndef NGAME
    this->AllowCreateSubSpace( false );
#endif

    n_assert2( world, "Null pointer." );

    /// adding indoor space to the world
    world->GetSpace()->Add( this->indoorSpace );

#ifdef NGAME
    /// adding objects to the world
    for( int index(0); index < this->objectsContainer.Size(); ++index )
    {
        world->Add( this->objectsContainer.GetElementAt(index)->GetEntityObject() );
    }
#endif

    this->SetWorld( world );

#ifndef NGAME
    ncPhyCompositeObj::Create( world );
#endif
}

//-----------------------------------------------------------------------------
/**
    Adds an object to this indoor.

    @param obj object to be added to the indoor

    history:
        - 30-May-2005   Zombie         created
*/
void ncPhyIndoor::Add( ncPhysicsObj* object )
{
    n_assert2( object, "Null pointer." ); 

#ifndef NGAME
    /// add geometries
    if( object->GetComponent<ncPhySimpleObj>() )
    {
        ncPhySimpleObj* obj(static_cast<ncPhySimpleObj*>(object));

        this->ncPhyCompositeObj::Add(obj->GetGeometry());

        if( !object->GetParentSpace() )
        {
            obj->GetGeometry()->GetTransform()->MoveToSpace(this->GetIndoorSpace());
            obj->ncPhysicsObj::MoveToSpace( this->GetIndoorSpace() );
        }
        else
            obj->GetGeometry()->GetTransform()->MoveToSpace(object->GetParentSpace());

        obj->GetGeometry()->AddRef();
    }
    else
    {
        ncPhyCompositeObj* obj(static_cast<ncPhyCompositeObj*>(object));

        for( int index(0); index < obj->GetNumGeometries(); ++index )
        {
            this->ncPhyCompositeObj::Add(obj->GetGeometry(index));

            if( !object->GetParentSpace() )
            {
                obj->GetGeometry(index)->GetTransform()->MoveToSpace(this->GetIndoorSpace());
                obj->ncPhysicsObj::MoveToSpace( this->GetIndoorSpace() );
            }
            else
                obj->GetGeometry(index)->GetTransform()->MoveToSpace(object->GetParentSpace());

            obj->GetGeometry(index)->AddRef();
        }
    }

    object->GetEntityObject()->AddRef();
#endif

    // sets the owner
    object->SetOwner( this );

#ifdef NGAME
    if( !object->GetParentSpace() )
        // insert in to the default space of this indoor
        object->MoveToSpace( this->GetIndoorSpace() );
#endif

    // inserting the object in the container
    this->objectsContainer.Add( int(size_t(object)), object );

    if( this->GetWorld() )
    {
        this->GetWorld()->Add( object->GetEntityObject() );
    }

}

//-----------------------------------------------------------------------------
/**
    Adds a space to the indoor.

    @param space to be added to the indoor

    history:
        - 31-May-2005   Zombie         created
*/
void ncPhyIndoor::Add( nPhySpace* space )
{
    n_assert2( space, "Null pointer." );

    this->spaces.Add( space->GetTagName().KeyMap(), space );

    /// adding this space to the indoor
    this->indoorSpace->Add( space );

    space->SetCategories( nPhysicsGeom::Static );
    space->SetCollidesWith( nPhysicsGeom::Dynamic | nPhysicsGeom::Check );
}

//-----------------------------------------------------------------------------
/**
    Inits the object.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 31-May-2005   Zombie         created
*/
void ncPhyIndoor::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhysicsObj::InitInstance(initType);

    if (initType != nObject::ReloadedInstance)
    {
        ncPhyIndoorClass const* indoorClass(this->GetClassComponent<ncPhyIndoorClass>());

        n_assert2( indoorClass, "Failed to get the class." );

        lastLoading = this;

        nString path;

        indoorClass->GetPath( path );
        
        path.Append( "spaces.n2" );

        nObjectInstancer* instancier(0);

        if( nFileServer2::Instance()->FileExists( path.Get() ) )
        {
            instancier = static_cast<nObjectInstancer*>(nKernelServer::Instance()->Load( path.Get(), false ));
            
            n_assert2( instancier, "Failed to load the indoor collision spaces." );

            for( int index(0); index < instancier->Size(); ++index )
            {
                this->Add( static_cast<nPhySpace*>(instancier->At( index )));
            }

            instancier->Release();
        }    
    }
}

//-----------------------------------------------------------------------------
/**
    Removes an object to this indoor.

    @param object object to be removed

    history:
        - 31-May-2005   Zombie         created
*/
void ncPhyIndoor::Remove( ncPhysicsObj* object )
{
    n_assert2( object, "Null pointer." );

    this->objectsContainer.Rem( int(size_t(object)) );

    object->MoveToSpace( this->GetWorld()->GetSpace() );
}
#ifdef NGAME
//-----------------------------------------------------------------------------
/**
    Sets position of the full indoor.

    @param newposition indoor's new position

    history:
        - 02-Jun-2005   Zombie         created
*/
void ncPhyIndoor::SetPosition( const vector3& newposition )
{
    vector3 offset(this->position);

    offset = newposition - offset;

    if( !offset.len() )
        return;
    // storing new position
    ncPhysicsObj::SetPosition( newposition );

    // transmiting the new offset to the rest of the objects in the indoor
    for( int index(0); index < objectsContainer.Size(); ++index )
    {
        ncPhysicsObj* obj(objectsContainer.GetElementAt( index ));

        n_assert2( obj, "Data corruption." );

        vector3 pos;

        obj->GetPosition( pos );

        pos += offset;

        obj->SetPosition( pos );
    }

    this->position = newposition;
}


//-----------------------------------------------------------------------------
/**
    Sets the rotation of this physic object.

    @param newrotation indoor's new rotation

    history:
        - 29-Jun-2005   Zombie         created
*/
void ncPhyIndoor::SetRotation( const matrix33& newrotation )
{
    vector3 euA( newrotation.to_euler() );
    vector3 euB( this->orientation.to_euler() );

    if( euA.x == euB.x )
        if( euA.y == euB.y )
            if( euA.z == euB.z )
                return;

    matrix33 newMatrix;
    
    newMatrix.from_euler( euA - euB );

    // transmiting the new offset to the rest of the objects in the indoor
    for( int index(0); index < objectsContainer.Size(); ++index )
    {
        ncPhysicsObj* obj(objectsContainer.GetElementAt( index ));

        n_assert2( obj, "Data corruption." );

        vector3 pos;

        obj->GetPosition( pos );

        pos -= this->position;

        vector3 newposition;

        newposition = newMatrix * pos;

        pos = newposition + this->position;

        obj->SetPosition( pos );

        matrix33 oldorientation;

        obj->GetOrientation( oldorientation );

        oldorientation = oldorientation * newMatrix;
        
        obj->SetRotation( oldorientation );
    }

    this->orientation = newrotation;
}

#endif

//-----------------------------------------------------------------------------
/**
    Rearranges spaces.

    history:
        - 07-Jun-2005   Zombie         created
*/
void ncPhyIndoor::RearrangeSpaces()
{
    tSpaces tempContainer( this->spaces );

    this->spaces.Clear();

    for( int index(0); index < tempContainer.Size(); ++index )
    {
        nPhySpace* space(tempContainer.GetElementAt(index));
        this->spaces.Add( space->GetTagName().KeyMap(), space );
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the last indoor beeing loaded.

    @return indoor

    history:
        - 07-Jun-2005   Zombie         created
*/
ncPhyIndoor* ncPhyIndoor::Loading()
{
    return lastLoading;
}

//-----------------------------------------------------------------------------
/**
    Initilize this for a begin to run the instancier.

    history:
        - 05-Sep-2005   Cristobal Castillo    created
*/
void ncPhyIndoor::BeginRunInstancier()
{
     ncPhyIndoor::lastLoading = this;
}
//-----------------------------------------------------------------------------
/**
    Loads a objects from an instancier.

    @param filename file

    history:
        - 07-Jun-2005   Zombie         created
*/
void ncPhyIndoor::RunInstancier( const nString& filename )
{
    nObjectInstancer* instancier(0);

    if( nFileServer2::Instance()->FileExists( filename.Get() ) )
    {
        instancier = static_cast<nObjectInstancer*>(nKernelServer::Instance()->Load( filename.Get(), false ));
        
        n_assert2( instancier, "Failed to load the indoor collision spaces." );

        for( int index(0); index < instancier->Size(); ++index )
        {
            this->Add( static_cast<nEntityObject*>(instancier->At( index ))->GetComponent<ncPhysicsObj>());
        }

        instancier->Release();
    }   
}

//-----------------------------------------------------------------------------
/**
    Ends the first load of objects.

    history:
        - 07-Jun-2005   Zombie         created
*/
void ncPhyIndoor::EndRunInstancier()
{
    ncPhyIndoorClass const* indoorClass(this->GetClassComponent<ncPhyIndoorClass>());

    n_assert2( indoorClass, "Failed to get the class." );

    lastLoading = this;

    nString path;

    indoorClass->GetPath( path );

    path.Append( "objects.n2" );

    this->RunInstancier( path );

    nPhysicsServer::Instance()->GetDefaultWorld()->Add( this->GetEntityObject() );


    /// updating the indoor position and orientation
    this->Update(this->GetComponentSafe<ncTransform>()->GetTransform());

    /// updating the object manager
    this->GetWorld()->GetObjectManager()->Init();

	///  mark end load.
    lastLoading = 0; 
}

//-----------------------------------------------------------------------------
/**
    Returns the bounding box of this geometry.

    @param boundingbox axis aligned bounding box

    history:
        - 10-Jun-2005   Zombie         created
*/
void ncPhyIndoor::GetAABB( nPhysicsAABB& boundingbox )
{
    this->GetIndoorSpace()->GetAABB( boundingbox );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
