//------------------------------------------------------------------------------
/**
    @file nentityclass_main.cc
    @author Mateu Batle

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nentity.h"
#include "entity/nentityclass.h"
#include "entity/nentityclassserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/nsystem.h"
#include "kernel/ndependencyserver.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
nNebulaEntityClass(nEntityClass,"nroot");

//------------------------------------------------------------------------------
nEntityClass::nEntityClass() :
    compIndexer(0)
{
    // do not save children
    this->SetSaveModeFlags(N_FLAG_SAVESHALLOW);
}

//------------------------------------------------------------------------------
nEntityClass::~nEntityClass()
{
    int i = this->comps.Size();
    while (i > 0)
    {
        nComponentClass * cc = this->GetComponentByIndex(--i);
        n_assert(cc);
        n_delete(cc);
        this->comps.EraseQuick(i);
    }

    nClass * cl = this->GetClass();
    cl->Release();
}

//------------------------------------------------------------------------------
void 
nEntityClass::InitInstance(nObject::InitInstanceMsg initType)
{
    nRoot::InitInstance(initType);

    int i(0);
    while (i < this->comps.Size())
    {
        nComponentClass * cc = this->GetComponentByIndex(i++);
        n_assert(cc);
        cc->InitInstance(initType);
    }
}

//------------------------------------------------------------------------------
/**
    Copy a component class from another entity class. It basically executes the 
    commands in SaveCmds from the component on itself.
*/
bool 
nEntityClass::CopyComponentFrom(nComponentClass * comp)
{
    bool retval = false;

    if (comp)
    {
        nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
        n_assert(ps);

        nPersistServer::nSaveMode oldMode = ps->GetSaveMode();
        ps->SetSaveMode(nPersistServer::SAVEMODE_CLONE);
        nObject * oldClone = ps->GetClone();
        ps->SetClone(this);

        nKernelServer::Instance()->PushCwd(this);
        if (comp->SaveCmds(ps))
        {
            retval = true;
        }
        nKernelServer::Instance()->PopCwd();

        ps->SetClone(oldClone);
        ps->SetSaveMode(oldMode);
    }

    return retval;
}

//------------------------------------------------------------------------------
bool 
nEntityClass::IsUserCreated(void) const
{
    return !this->native;
}

//------------------------------------------------------------------------------
nEntityClass *
nEntityClass::BeginNewObjectEntityClass(const char* superClass, const char* name)
{
    n_assert(superClass);
    n_assert(name);

    // get the super entity class
    nEntityClass * superecl = nEntityClassServer::Instance()->GetEntityClass(superClass);
    n_assert(superecl);

    // create new subclass
    nEntityClass * ecl = nEntityClassServer::Instance()->CreateEntityClass(superecl, name);
    n_assert(ecl);

    // set dirty to false since it was loaded from disk
    nEntityClassServer::Instance()->SetEntityClassDirty(ecl, false);

    // report this to persist server
    if (ecl)
    {
        nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(ecl, nObject::NoInit);
    }
    else
    {
        n_error("nObject::BeginNewObjectEntityclass(%s,%s):failed!\n", superClass, name);
    }

    return ecl;
}

//------------------------------------------------------------------------------
/**
*/
void
nEntityClass::ChangeClass(nClass* newClass)
{
    //this->GetClass()->Release();
	this->nRoot::SetClass(newClass);
    n_assert(!this->native);
    newClass->AddRef();
}

//------------------------------------------------------------------------------
/**
*/
bool 
nEntityClass::SaveFile(nFile* file)
{
    n_assert(file);
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    // if created natively it should not be saved
    if (this->native)
    {
        return false;
    }

    // check that no file is saving now
    if( ! ps->IsSaving() )
    {
        // save the name in the dependency server
        nDependencyServer::PushParentFileObject( this, file->GetFilename() );
    }

    bool retval = false;
    nCmd * cmd = ps->GetCmd(this, 'BNEC');
    cmd->In()->SetS(this->parent->GetName());
    cmd->In()->SetS(this->GetProperName());
    if (ps->BeginObjectWithCmd(this, cmd, file)) 
    {
        // ...the usual behaviour...
        nRoot *c;
        if (this->saveModeFlags & N_FLAG_SAVEUPSIDEDOWN) 
        {
            // upsidedown: save children first, then own status
            if (!(this->saveModeFlags & N_FLAG_SAVESHALLOW)) 
            {
                for (c = this->GetHead(); c; c = c->GetSucc()) 
                {
                    c->Save();
                }
            }
            retval = this->SaveCmds(ps);
        } 
        else 
        {
            // normal: save own status first, then children
            retval = this->SaveCmds(ps);
            if (!(this->saveModeFlags & N_FLAG_SAVESHALLOW)) 
            {
                for (c = this->GetHead(); c; c = c->GetSucc()) 
                {
                    c->Save();
                }
            }
        }
        ps->EndObject(true);
    } 
    else 
    {
        n_error("nEntityClass::SaveAs(): BeginObjectWithCmd() failed!");
    }

    // check that no file is saving now
    if( ! ps->IsSaving() )
    {
        nDependencyServer::PopParentFileObject();

        // save dependencies of object
        nDependencyServer::SaveDependencies();
    }

    return retval;
}

//------------------------------------------------------------------------------
bool 
nEntityClass::CopyCommonComponentsFrom(nEntityClass * entc)
{
    bool ret = true;

    int i(0);
    while (ret && i < this->comps.Size())
    {
        const nComponentId & compId = this->GetComponentIdByIndex(i++);
        if ( entc->HasComponentById( compId ) )
        {
            nComponentClass * comp = entc->GetComponent(compId);
            ret &= this->CopyComponentFrom(comp);
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
bool 
nEntityClass::HasComponent(const char * componentName) const
{
    if (!componentName)
    {
        return false;
    }

    return this->HasComponentById( nComponentId(componentName) );
}

//------------------------------------------------------------------------------
/**
    set component indexer for the entity class
*/
void 
nEntityClass::SetComponentIdList(nComponentIdList * ci)
{
    n_assert(ci);
    this->compIndexer = ci;
}

//------------------------------------------------------------------------------
/**
    Add a component unsafely (not called by user)
*/
bool
nEntityClass::AddComponentUnsafe(nComponentClass * comp)
{
    n_assert(comp);
    if (comp->AttachToEntity(this))
    {
        this->comps.Append(comp);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
