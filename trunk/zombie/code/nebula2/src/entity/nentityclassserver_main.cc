//------------------------------------------------------------------------------
//  nentityclassserver_class.cc
//  (c) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
//------------------------------------------------------------------------------
#include "entity/nentityclassserver.h"
#include "entity/nentityobjectserver.h"
#include "entity/nentityclass.h"
#include "entity/nclassentityclass.h"
#include "kernel/nfileserver2.h"
#include "kernel/ndirectory.h"
#include "kernel/ndependencyserver.h"

//#include "nscriptclassserver/nscriptclassserver.h"

#ifndef NGAME
//#include "ndebug/nceditorclass.h"
#endif//!NGAME

//------------------------------------------------------------------------------
static const char * ClassIndexFilename = "_classindex.n2";

//------------------------------------------------------------------------------
nNebulaScriptClass(nEntityClassServer, "nroot");

//------------------------------------------------------------------------------
nEntityClassServer * nEntityClassServer::instance = 0;

//------------------------------------------------------------------------------
nEntityClassServer::nEntityClassServer(void) :
    currentClassKey(0),
    dirty(false)
{
    // initialize instance pointer
    if (!nEntityClassServer::instance)
    {
        nEntityClassServer::instance = this;
    }

    nKernelServer::Instance()->SetCallBackClassNotFound(nEntityClassServer::CallBackClassNotFound);

    // get the id 
    this->currentClassKey = nEntityObjectServer::Instance()->GetHighId();
}

//------------------------------------------------------------------------------
nEntityClassServer::~nEntityClassServer(void)
{
    // unload all classes
    this->UnloadAllEntityClasses();

    

    if (nEntityClassServer::instance == this)
    {
        nEntityClassServer::instance = 0;
    }
}

//------------------------------------------------------------------------------
nEntityClassServer * 
nEntityClassServer::Instance(void)
{
    n_assert( nEntityClassServer::instance );
    return nEntityClassServer::instance;
}

//------------------------------------------------------------------------------
void 
nEntityClassServer::SetEntityClassDir(const char * dir)
{
    // set the directory
    n_assert(dir);
    this->entityClassDir = dir;
    this->entityClassDir.StripTrailingSlash();
    this->entityClassDir += "/";

    // load the classes
    this->LoadEntityClassIndex();
}

//------------------------------------------------------------------------------
const char * 
nEntityClassServer::GetEntityClassDir(void) const
{
    return this->entityClassDir.Get();
}

//------------------------------------------------------------------------------
nString
nEntityClassServer::GetEntityClassFilename(const char * className) const
{
    n_assert(className);
    nString path;

    // append the entity class directory
    path = this->entityClassDir + className + ".n2";
    return path;
}

//------------------------------------------------------------------------------
nEntityClass * 
nEntityClassServer::LoadEntityClass(const char * className)
{
    n_assert(className);
    n_assert(!kernelServer->IsClassLoaded(className));

    // check if class exists
    int index = this->classIndex.FindIndex(className);
    if ( -1 == index || !this->classIndex[index].isEntityClass)
    {
        return 0;
    }

    nEntityClassInfo * info = &this->classIndex[index];

    // check if class has been deleted
    if (info->IsDeleted())
    {
        return 0;
    }

    // load all superclasses
    nEntityClass * parent = 0;
    for(int i = 0;i < info->superClasses.Size();i++)
    {
        parent = this->GetEntityClass(info->superClasses[i].Get());
    }
    n_assert(parent);

    // get entity class filename was found
    nString path = this->GetEntityClassFilename(className);

    // load parent classes if not already loaded
    kernelServer->PushCwd(parent);
    nEntityClass * neclass = static_cast<nEntityClass *> (kernelServer->Load(path.Get()));
    kernelServer->PopCwd();

    // check if the entity class was loaded ok
    if (neclass)
    {
        n_assert(neclass->IsA("nentityclass"));
        n_assert(neclass->nClass::IsA("nentityobject"));

        // update class index
        this->classIndex[index].SetEntityClass(neclass);
    }

    return neclass;
}

//------------------------------------------------------------------------------
bool
nEntityClassServer::SaveEntityClass(nEntityClass * entityClass) const
{
    n_assert(entityClass);

    // check if class exists
    int index = this->classIndex.FindIndex(entityClass->nClass::GetName());
    if ( -1 == index || !this->classIndex[index].isEntityClass)
    {
        return false;
    }

    // check it has not been deleted
    if (this->classIndex[index].IsDeleted())
    {
        return false;
    }

    // get path where to save the entity
    nString path = this->GetEntityClassFilename(entityClass->nClass::GetName());

    // save class
    bool valid = entityClass->SaveAs(path.Get());
#ifndef NGAME
    //if( valid )
    //{
    //    ncEditorClass * editorClass = entityClass->GetComponent<ncEditorClass>();
    //    if( editorClass )
    //    {
    //        editorClass->SaveChunk();
    //    }
    //}
#endif//!NGAME

    return valid;
}

//------------------------------------------------------------------------------
bool
nEntityClassServer::UnloadEntityClass(nEntityClass * cl)
{
    n_assert(cl);

    // find class in index
    int index = this->classIndex.FindIndex(cl->nClass::GetName());
    if ( -1 == index || !this->classIndex[index].isEntityClass)
    {
        return false;
    }

    // if there are subclasses of this entity loaded then cannot unload
    if (cl->GetHead())
    {
        return false;
    }

    // if the entity class was createdb by the user cannot be unloaded
    if (!cl->IsUserCreated())
    {
        return false;
    }

    // check if there are entities loaded using this class ?
    if (cl->nClass::GetNumInstances() >= 1)
    {
        return false;
    }

    // get entity class
    nEntityClass * neclasss = this->classIndex[index].GetEntityClass();
    neclasss->nRoot::Remove();
    neclasss->Release();

    // update entity class ptr
    this->classIndex[index].SetEntityClass(0);
    return true;
}

//------------------------------------------------------------------------------
nEntityClass * 
nEntityClassServer::FindEntityClass(const char * className) const
{
    // check if class exists
    int index = this->classIndex.FindIndex(className);
    if (-1 == index || !this->classIndex[index].isEntityClass)
    {
        return 0;
    }

    return this->classIndex[index].GetEntityClass();
}

//------------------------------------------------------------------------------
bool 
nEntityClassServer::ExistEntityClass(const char * className) const
{
    // check if class exists
    int index = this->classIndex.FindIndex(className);
    return  (-1 != index) && this->classIndex[index].isEntityClass;
}

//------------------------------------------------------------------------------
nEntityClass * 
nEntityClassServer::GetEntityClass(const char * className)
{
    n_assert(className);

    nEntityClass * neclass = this->FindEntityClass(className);
    if (0 == neclass)
    {
        neclass = this->LoadEntityClass(className);
    }

    return neclass;
}

//------------------------------------------------------------------------------
bool 
nEntityClassServer::RemoveEntityClass(const char * className)
{
    n_assert(className);

    // find class in index
    int index = this->classIndex.FindIndex(className);
    if ( -1 == index || !this->classIndex[index].isEntityClass)
    {
        return false;
    }

    nEntityClass * neclass = this->FindEntityClass(className);
    if (neclass)
    {
        // not possible to remove a class with subclasses
        if (neclass->nClass::GetRefCount() > 1)
        {
            return false;
        }

        // not possible to remove a class with instances
        if (neclass->nClass::GetNumInstances() >= 1)
        {
            return false;
        }

        // get the list with all the classes, and check one by one the parent
        const nHashList * classlist = nKernelServer::Instance()->GetClassList();
        n_assert( classlist );
        for ( nHashNode * node = classlist->GetHead(); node; node = node->GetSucc() )
        {
            if ( static_cast<nClass*>(node)->GetSuperClass() == static_cast<nClass*>(neclass) )
            {
                return false;
            }
        }
    }

    // check there are no subclasses
    nArray<nString> subclasses;
    this->GetEntitySubClassNames(className, subclasses);
    if (subclasses.Size() > 0)
    {
        return false;
    }

    // first unload the entity class
    if (neclass)
    {
        if (!this->UnloadEntityClass(neclass))
        {
            return false;
        }
    }

    // delete the entry from class index (really deleted on save)
    nEntityClassInfo * info = &this->classIndex[index];
    info->Delete();

    // set class index to dirty in order to be saved at exit time
    this->dirty = true;

    return true;
}

//------------------------------------------------------------------------------
void 
nEntityClassServer::RemoveEntityClassPost(const char * className)
{
    n_assert(className);

    // find class in index
    int index = this->classIndex.FindIndex(className);
    if ( -1 == index || !this->classIndex[index].isEntityClass)
    {
        return;
    }

    n_assert(this->classIndex[index].IsDeleted());

    // remove the file
    nString path = this->GetEntityClassFilename(className);
    kernelServer->GetFileServer()->DeleteFile(path.Get());

    // remove the class from the class index
    this->classIndex.Erase(index);
}

//------------------------------------------------------------------------------
nEntityClass * 
nEntityClassServer::NewEntityClass(nEntityClass * neSuperClass, const char * name)
{
    nEntityClass * entClass = this->CreateEntityClass(neSuperClass, name);

    // call init instance now
    if (entClass)
    {
        entClass->InitInstance(nObject::NewInstance);
    }

    // set class index to dirty in order to be saved at exit time
    this->dirty = true;

    return entClass;
}

//------------------------------------------------------------------------------
nEntityClass * 
nEntityClassServer::CreateEntityClass(nEntityClass * neSuperClass, const char * name)
{
    n_assert(neSuperClass);
    n_assert(name);
    n_assert2( nEntityClassServer::ValidClassName(name), ("Tried to create entity class with invalid class name '%s'", name) );
    n_assert( !this->FindEntityClass(name) );

	nString objname(name);
	nString classname(name);
	classname += "class";
	nString fullName = neSuperClass->GetFullName();
	fullName += "/";
	fullName += objname;

	// Create the class nClass
	// derive from the class for the entity class
    nClassEntityClass * clazz = static_cast<nClassEntityClass *> (neSuperClass->GetClass());
    if (0 == clazz)
    {
        return 0;
    }
    nClassEntityClass * newClass = n_new(nClassEntityClass(classname.Get(), kernelServer, clazz->n_new_ptr, false));
    if (0 == newClass)
    {
        return 0;
    }
	// Create the class instance
	nEntityClass * entClass = static_cast<nEntityClass *> (neSuperClass->Clone(fullName.Get(), false));
    if (0 == entClass)
    {
        n_delete(newClass);
        return 0;
    }

    // initialize newClass
    newClass->properName.Set(classname.Get());
    newClass->SetInstanceSize(clazz->GetInstanceSize());
    nComponentIdList * idlSource = clazz->GetComponentIdList();
    nComponentIdList * idlDest = newClass->GetComponentIdList();
    *idlDest = *idlSource;
    kernelServer->AddClass(clazz->GetName(), newClass);
    
	// No native commands added
	newClass->BeginCmds();
	newClass->EndCmds();

	// initialize the nClass part
	entClass->nClassEntityObject::InitClass(objname.Get(),kernelServer,neSuperClass->n_new_ptr, false);
	entClass->properName.Set(objname.Get());
	entClass->SetInstanceSize(neSuperClass->nClass::GetInstanceSize());
    idlSource = neSuperClass->nClassEntityObject::GetComponentIdList();
    idlDest = entClass->nClassEntityObject::GetComponentIdList();
    *idlDest = *idlSource;
	kernelServer->AddClass(neSuperClass->nClass::GetName(), entClass);

	// No native commands added
	entClass->BeginCmds();
	entClass->EndCmds();

	// Now join the two
    entClass->ChangeClass(newClass);

    // append the class to the index
    this->AddClassToIndex(entClass, true);
/*
    // add scripting to the class
    nScriptClassServer * scriptClass = nScriptClassServer::Instance();
    if( scriptClass )
    {
        scriptClass->AddClass( nString( name ) );
    }
*/
#ifndef NGAME
    entClass->BindSignal( nObject::SignalObjectDirty, this, &nEntityClassServer::TriggerClassDirty,10 );
#endif
  
    return entClass;
}

//------------------------------------------------------------------------------
/**
    Creates a new subclass from a given nClass with a different name.
    The class creates objects of the same type of its super nClass, but its 
    nClass instance pointer points to the subclass created, the data structure 
    of the created objects is really the same.
    This can be used to overload scripting parameters for example.
    New classes can be created with different script implementations, 
    adding new scripting commands, etc.

    Note: this is to create any nClass not nEntityClasses.
*/
nClass * 
nEntityClassServer::CreateSubClass(nClass * superClass, const char * name)
{
    n_assert(superClass);
    n_assert(name);
    n_assert( nEntityClassServer::ValidClassName(name) );
    n_assert( nKernelServer::Instance()->IsClassLoaded(name) == 0 );

	// Create the class nClass
    nClass * newClass = n_new(nClass(name, kernelServer, superClass->n_new_ptr, false));
    if (0 == newClass)
    {
        return 0;
    }

    // initialize newClass
    newClass->properName.Set(name); 
    newClass->SetInstanceSize(superClass->GetInstanceSize());
    kernelServer->AddClass(superClass->GetName(), newClass);
    
	// No native commands added
	newClass->BeginCmds();
	newClass->EndCmds();

    // check if the class is already in the index
    int index = this->classIndex.FindIndex(name);
    if (-1 == index)
    {
        // store class info in the class index table
        nEntityClassInfo ci ;

        ci.className = name;
        int supIndex = this->classIndex.FindIndex(superClass->GetName());
        if ( supIndex != -1 )
        {
            ci.superClasses = this->classIndex[supIndex].superClasses;
        }
        ci.superClasses.Append( superClass->GetName() );
        ci.dirty = false;
        ci.SetEntityClass(0);
        ci.isEntityClass = false;
        ci.nclass = newClass;
        ci.classKey = this->currentClassKey;

        // add to class index (new class created by user)
        this->classIndex.Append(ci);
    }
    else
    {
        n_assert(!this->classIndex[index].isEntityClass);
        this->classIndex[index].nclass = newClass;
    }

    return newClass;
}

//------------------------------------------------------------------------------
/**
    Create a subclass with the name given and make it derive from the superClass.
*/
bool
nEntityClassServer::NewClass(const char * superClassName, const char * subClassName)
{
    n_assert(superClassName);
    n_assert(subClassName);

    nClass * superClass;

    superClass = nKernelServer::Instance()->FindClass(superClassName);
    if (0 == superClass)
    {
        return false;
    }

    // set class index to dirty in order to be saved at exit time
    this->dirty = true;

    nClass * subClass = this->CreateSubClass(superClass, subClassName);
    return (subClass != 0);
}

//------------------------------------------------------------------------------
/**
    Create a subclass with the name given and make it derive from the superClass.
*/
bool
nEntityClassServer::RemoveClass(const char * className)
{
    n_assert(className);
    nClass * cl;

    cl = nKernelServer::Instance()->FindClass(className); ///TODO: change by IsClassLoaded but also delete of classIndex
    if (0 == cl)
    {
        return false;
    }

    // not possible to remove a non-native class
    if (cl->IsNative())
    {
        return false;
    }

    // not possible to remove a class with subclasses
    if (cl->GetRefCount() > 1)
    {
        return false;
    }

    // not possible to remove a class with instances
    if (cl->GetNumInstances() >= 1)
    {
        return false;
    }

    // get the list with all the classes, and check one by one the parent
    const nHashList * classlist = nKernelServer::Instance()->GetClassList();
    n_assert( classlist );
    for ( nHashNode * node = classlist->GetHead(); node; node = node->GetSucc() )
    {
        if ( static_cast<nClass*>(node)->GetSuperClass() == cl )
        {
            return false;
        }
    }

    // check info available in entity server
    int index = this->classIndex.FindIndex(className);
    if (-1 == index)
    {
        return false;
    }

    // delete entry in entity server
    // remove the class from the class index    
    this->classIndex.Erase(index);

    // delete from kernel server
    n_delete(cl);

    // set class index to dirty in order to be saved at exit time
    this->dirty = true;

    return true;
}

//------------------------------------------------------------------------------
void
nEntityClassServer::SetEntityClassDirty(nEntityClass * neclass, bool dirty)
{
    n_assert(neclass);

    // check if class exists
    int index = this->classIndex.FindIndex(neclass->nClass::GetName());
    if (-1 != index)
    {
        // set dirty value
        this->classIndex[index].dirty = dirty;
    }

    // put dirty in class index too in order to simplify the check if
    // there is anything to write
    this->dirty |= dirty;
}

//------------------------------------------------------------------------------
bool
nEntityClassServer::GetEntityClassDirty(nEntityClass * neclass)
{
    n_assert(neclass);

    // check if class exists
    int index = this->classIndex.FindIndex(neclass->nClass::GetName());
    if (-1 != index)
    {
        // get dirty value
        return this->classIndex[index].dirty;
    }
    return false;
}

//------------------------------------------------------------------------------
void
nEntityClassServer::SetDirtyAllClasses()
{
    for(int i = 0;i < this->classIndex.Size();i++)
    {
        nEntityClassInfo& info = this->classIndex[i];
        if ( info.isEntityClass &&  ! info.IsDeleted() )
        {
            info.dirty = true;
#ifndef NGAME
            info.GetEntityClass()->SetObjectDirty( true );
#endif
        }
    }
    this->dirty = true;
}

//------------------------------------------------------------------------------
bool
nEntityClassServer::UnloadAllEntityClasses(void)
{
    nEntityClass * neclass = this->FindEntityClass("nentityobject");
    if (neclass) 
    {
        return this->UnloadEntitySubClasses(neclass);
    }
    return true;
}

//------------------------------------------------------------------------------
bool
nEntityClassServer::UnloadEntitySubClasses(nEntityClass * neclass)
{
    n_assert(neclass);
    bool failed = false;
    nArray<nEntityClass *> subclasses;

    this->GetLoadedEntitySubClasses(neclass, subclasses);
    for(int i = 0;i < subclasses.Size();i++)
    {
        if (! this->UnloadEntitySubClasses(subclasses[i]))
        {
            failed = true;
        }
        if (subclasses[i]->IsUserCreated())
        {
            if (! this->UnloadEntityClass(subclasses[i]))
            {
                failed = true;
            }
        }
    }

    return !failed;
}

//------------------------------------------------------------------------------
bool
nEntityClassServer::LoadAllEntityClasses(void)
{
    nEntityClass * neclass = this->FindEntityClass("nentityobject");
    n_assert(neclass);
    return this->LoadEntitySubClasses(neclass);
}

//------------------------------------------------------------------------------
bool
nEntityClassServer::LoadEntitySubClasses(nEntityClass * neclass)
{
    n_assert(neclass);
    bool failed = false;
    nArray<nString> subclasses;

    this->GetEntitySubClassNames(neclass->nClass::GetName(), subclasses);
    for(int i = 0;i < subclasses.Size();i++)
    {
        nEntityClass * neclass = this->GetEntityClass(subclasses[i].Get());
        if (neclass)
        {
            if (! this->LoadEntitySubClasses(neclass))
            {
                failed = true; 
            }
        }
        else
        {
            n_message("The entity class '%s' could not be loaded. \n"
                //"The class will be removed from the class index.\n"
                //"Some files might be orphaned, contact with programmers to remove them.\n"
                //"Please reexport it if you need it."
                , subclasses[i].Get());
            // remove the class from the class index
            //this->RemoveEntityClass(subclasses[i].Get());
            failed = true;
        }
    }

    return !failed;
}

//------------------------------------------------------------------------------
void 
nEntityClassServer::GetEntitySubClassNames(const char * name, nArray<nString> & subclasses) const
{
    n_assert(name);

    // clear the array of subclasses
    subclasses.Clear();

    // check loaded & native classes (classes not created by users on runtime)
    if (!strcmp(name, "nentityobject"))
    {
        for(int i = 0;i < this->classIndex.Size();i++)
        {
            nEntityClassInfo * info = &this->classIndex[i];
            if (0 == info->superClasses.Size() && (info->className != "nentityobject"))
            {
                subclasses.Append(info->className);
            }
        }    
    }
    else
    {
        for(int i = 0;i < this->classIndex.Size();i++)
        {
            nEntityClassInfo * info = &this->classIndex[i];
            int size = info->superClasses.Size();
            if (size > 0 && (info->superClasses[size-1] == name))
            {
                subclasses.Append(info->className);
            }
        }
    }
}

//------------------------------------------------------------------------------
void 
nEntityClassServer::GetLoadedEntitySubClasses(nEntityClass * neclass, nArray<nEntityClass *> & subclasses) const
{
    n_assert(neclass);

    // clear the array of subclasses
    subclasses.Clear();

    // get first child class
    nRoot * child = neclass->GetHead();
    while(child)
    {
        subclasses.Append(static_cast<nEntityClass *>(child));
        child = child->GetSucc();
    }
}

//------------------------------------------------------------------------------
bool 
nEntityClassServer::LoadEntityClassIndex(void)
{
    // clear the class index
    this->classIndex.Clear();

    // get filename where the class index is stored
    nString filename = this->GetEntityClassDir();
    filename = filename + ClassIndexFilename;

    // first store all entity object classes native
    nEntityClass * ecl = static_cast<nEntityClass *> (nKernelServer::Instance()->FindClass("nentityobject"));
    this->AddNativeClasses(ecl);

    // load the file
    kernelServer->PushCwd(this);
    nObject * obj = nKernelServer::Instance()->Load(filename.Get(), false);
    kernelServer->PopCwd();

    // create the non-native non-entity (plain) classes
    this->CreateNonNativePlainClasses();

    // class index has been loaded from disk -> not dirty
    this->dirty = false;

    return (obj != 0);
}

//------------------------------------------------------------------------------
void
nEntityClassServer::AddNativeClasses(nEntityClass * cl)
{
    if (!cl->IsUserCreated())
    {
        /// add this class
        this->AddClassToIndex(cl, false);

        /// add recursively all the subclasses
        nEntityClass * child = static_cast<nEntityClass *> (cl->nRoot::GetHead());
        while(child)
        {
            this->AddNativeClasses(child);
            child = static_cast<nEntityClass *> (child->nRoot::GetSucc());
        }
    }
}

//------------------------------------------------------------------------------
bool 
nEntityClassServer::SaveEntityClasses(void)
{
    // Empty the dependencyServer , example save the editable assets
    nDependencyServer::InitGlobal(); 

    // save all dirty classes
    for(int i = 0;i < this->classIndex.Size();i++)
    {
        nEntityClassInfo * info = &this->classIndex[i];
        if (info->IsDeleted())
        {
            this->RemoveEntityClassPost(info->className.Get());
            // for not increment the i, RemoveEntityClassPost remove this item from classIndex
            // Alternative is traverse the index from end to begin
            --i;
        } 
        else if (info->isEntityClass && info->dirty)
        {
            n_assert(info->GetEntityClass());
            this->SaveEntityClass(info->GetEntityClass());
            info->dirty = false;

            #ifndef NGAME // This method only defined in conjurer
            info->GetEntityClass()->SetObjectDirty( false );
            #endif      
        }
    }

    // save the class index
    this->SaveEntityClassIndex();

    // Empty  and flush the dependencyServer , example save the editable assets
    nDependencyServer::InitGlobal(); 
    return true;
}

//------------------------------------------------------------------------------
/**
    Scan all entity classes in the index, and those signaled as pending to be 
    deleted are removed from that state.
*/
bool 
nEntityClassServer::DiscardDeletedClasses(void)
{
    // save all dirty classes
    for(int i = 0;i < this->classIndex.Size();i++)
    {
        nEntityClassInfo * info = &this->classIndex[i];
        if (info->IsDeleted())
        {
            // remove the class from the class index
            this->classIndex.Erase(i);

            // for not increment the i, RemoveEntityClassPost remove this item from classIndex
            // Alternative is traverse the index from end to begin
            --i; 
        } 
    }

    return true;
}

//------------------------------------------------------------------------------
bool 
nEntityClassServer::SaveEntityClassIndex(void)
{
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    // get filename where the class index is stored
    nString filename = this->GetEntityClassDir();
    filename = filename + ClassIndexFilename;

    // get and sort the strings
    ArraySortedStrings strings;
    strings.SetFixedSize( 0x100 );

    for(int i = 0;i < this->classIndex.Size();i++)
    {
        nEntityClassInfo & info = this->classIndex[i];

        if ( !info.isEntityClass || !info.GetEntityClass() ||
            (info.GetEntityClass() && info.GetEntityClass()->IsUserCreated()))
        {
            // build classpath
            nString classpath;
            for(int i = 0;i < info.superClasses.Size();i++)
            {
                classpath.Append("/");
                classpath.Append(info.superClasses[i]);
            }
            classpath.Append("/");
            classpath.Append(info.className);

            int index = (info.classKey & nEntityObjectServer::IDHIGHMASK) >> nEntityObjectServer::IDLOWBITS;
            bool valid = ( index >= 0 ) && ( index < strings.Size() );
            n_assert( valid );
            if( valid )
            {
                strings[ index ].Append( n_new( nString( classpath ) ) );
            }
        }
    }

    // save configuration with cmd BeginNewObjectConfig cmd
    bool retval = false;
    nCmd * cmd = ps->GetCmd(this, 'THIS');
    if (ps->BeginObjectWithCmd(this, cmd, filename.Get())) 
    {
        for( int i = 0 ; i < strings.Size() ; ++i )
        {
            ps->Put(this, 'SCCK', (i << nEntityObjectServer::IDLOWBITS) );

            for( int j = 0 ; j < strings[ i ].Size() ; ++j )
            {
                // Persist cmd AddClassToIndex
                ps->Put(this, 'ACTI', strings[ i ][ j ]->Get() );
            }
        }
        ps->Put( this, 'SLCK' );
        ps->EndObject(true);
        retval = true;
    }

    // delete strings
    for( int i = 0 ; i < strings.Size() ; ++i )
    {
        for( int j = 0 ; j < strings[ i ].Size() ; ++j )
        {
            n_delete( strings[ i ][ j ] );
        }
    }
    strings.Reset();

    // class index has been loaded from disk -> not dirty
    this->dirty = false;

    return retval;    
}


//------------------------------------------------------------------------------
void
nEntityClassServer::AddClassToIndex(const char * classpath)
{
    // tokenize the classpath 
    nString cp(classpath);
    nArray<nString> tokens;
    cp.Tokenize("/", tokens);
    
    // build the classinfo
    nEntityClassInfo classinfo;
    for(int i = 0;i < tokens.Size() - 1;i++)
    {
        classinfo.superClasses.Append(tokens[i]);
    }
    classinfo.className = tokens[tokens.Size() - 1];
    classinfo.SetEntityClass(0);
    classinfo.dirty = false;
    classinfo.classKey = this->currentClassKey;

    // check if entity class or not by searching the native class in the entity server
    if (this->FindEntityClass( tokens[0].Get() ))
    {
        classinfo.isEntityClass = true;
    }
    else
    {
        classinfo.isEntityClass = false;
    }

    // check if the class is already in the index
    int index = this->classIndex.FindIndex(classinfo);
    if (-1 == index)
    {
        // add to the parent
        this->classIndex.Append(classinfo);
    }
}

//------------------------------------------------------------------------------
void
nEntityClassServer::AddClassToIndex(nEntityClass * neclass, bool dirty)
{
    // get nEntityObject
    nClass * clneo = nKernelServer::Instance()->FindClass("nentityobject");

    // build the classinfo (superclasses)
    nEntityClassInfo classinfo;
    if (neclass != clneo)
    {
        nClass * parent = neclass->superClass;
        while(parent != clneo)
        {
            nString parentClassName = parent->GetName();
            classinfo.superClasses.Insert(0, parentClassName);
            parent = parent->superClass;
        }
    }
    classinfo.className = neclass->nClass::GetName();
    classinfo.dirty = dirty;
    classinfo.isEntityClass = true;
    nEntityClassInfo * oldinfo = this->classIndex.Find(classinfo);
    if( oldinfo )
    {
        classinfo.classKey = oldinfo->classKey;
    }
    else
    {
        classinfo.classKey = this->currentClassKey;
    }

    // add to the parent
    this->classIndex.Append(classinfo);

    // set the entity class
    this->classIndex.Find(classinfo)->SetEntityClass(neclass);
}

//------------------------------------------------------------------------------
int 
nEntityClassServer::CmpEntityClassInfo(const nEntityClassInfo * elm0, const nEntityClassInfo * elm1)
{
    return strcmp(elm0->className.Get(), elm1->className.Get() );
}

//------------------------------------------------------------------------------
void
nEntityClassServer::SetClassKey( int number )
{
    this->currentClassKey = number;
}

//------------------------------------------------------------------------------
void
nEntityClassServer::SetLocalClassKey()
{
    this->currentClassKey = nEntityObjectServer::Instance()->GetHighId();
}

//------------------------------------------------------------------------------
/**
    Create the non-native (user created) plain (not entity classes) nClass.
    The list of this classes is stored for now in the class index structure
    in the entity server. It was the easiest place to put it because so far
    the entity server has already similar functionality to create classes
    in runtime, and has some nice data structures that can be reused.

    @todo decide if move non-native plain nClasses out of the entity server
*/
void
nEntityClassServer::CreateNonNativePlainClasses()
{
    for(int i = 0;i < this->classIndex.Size();i++)
    {
        nEntityClassInfo * info = &this->classIndex[i];
        // check if plain nClass
        if (!info->isEntityClass)
        {
            // create first all the parents
            const char * superClassName;
            nClass * superClass;
            const char * subClassName;
            nClass * subClass;

            n_assert(info->superClasses.Size() > 0);

            superClassName = info->superClasses[0].Get();
            superClass = nKernelServer::Instance()->IsClassLoaded(superClassName);
            if (superClass)
            {
                // first create all parent classes (if needed)
                for(int i = 1;i < info->superClasses.Size();i++)
                {
                    subClassName = info->superClasses[i].Get();
                    subClass = nKernelServer::Instance()->IsClassLoaded(subClassName);
                    // if not exists create it
                    if (!subClass)
                    {
                        subClass = this->CreateSubClass(superClass, subClassName);
                        n_assert(subClass);
                    }
                    superClassName = subClassName;
                    superClass = subClass;
                }

                // now create the class
                if (superClass)
                {
                    subClassName = info->className.Get();
                    subClass = nKernelServer::Instance()->IsClassLoaded(subClassName);
                    if (!subClass)
                    {
                        subClass = this->CreateSubClass(superClass, subClassName);
                        n_assert(subClass);
                        superClassName = subClassName;
                        superClass = subClass;
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
bool
nEntityClassServer::GetDirty()
{
    return this->dirty;
}

//------------------------------------------------------------------------------
/**
*/
int
nEntityClassServer::StringSorter( nString * const* elm0, nString * const* elm1)
{
    return strcmp( (*elm0)->Get(), (*elm1)->Get() );
}

//------------------------------------------------------------------------------
bool
nEntityClassServer::ValidClassName(const nString& name)
{
    const char* ptr = name.Get();
    
    if ( name.IsEmpty() )
    {
        return false;
    }

    // The first character is a upper character
    if ( ! isupper( *ptr ) )
    {
        return false;
    }
    ++ptr;

    
    bool valid = true;

    while ( 0 != *ptr && valid )
    {
        // lower character , number and '_'
        valid = islower( *ptr) || isdigit( *ptr) || ( *ptr == '_') ;
        ptr++;
    }
    /// Todo: check is a reserve name
    return valid;
}

//------------------------------------------------------------------------------
bool
nEntityClassServer::CheckClassName(const nString& name)
{
    return nEntityClassServer::ValidClassName(name);
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
nEntityClassServer::TriggerClassDirty(nObject* emitter)
{
    nEntityClass * neclass = static_cast<nEntityClass*>(emitter);
    SetEntityClassDirty(neclass, true);
}
#endif //NGAME

//------------------------------------------------------------------------------
/**
*/
void
nEntityClassServer::CallBackClassNotFound(const char* className)
{
    nEntityClassServer::Instance()->GetEntityClass(className);
}
