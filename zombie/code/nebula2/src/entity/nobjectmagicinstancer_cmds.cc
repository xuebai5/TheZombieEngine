//------------------------------------------------------------------------------
//  nobjectmagicinstancer_cmds.cc
//  (c) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/nobjectmagicinstancer.h"
#include "zombieentity/ncsubentity.h"

//------------------------------------------------------------------------------
/**
    @brief save object to persistent stream
*/
bool 
nObjectMagicInstancer::SaveCmds(nPersistServer* ps)
{
    bool retval = true;

    if (this->objs)
    {

        for ( unsigned int magicNumber = 0; magicNumber < ( 1 << nEntityObjectServer::IDHIGHBITS ) ; magicNumber++ )
        {
            // put magic number command
            nCmd * cmd = ps->GetCmd(this, 'JSMN');
            cmd->Rewind();
            cmd->In()->SetI( magicNumber );
            ps->PutCmd( cmd );

            // save all objects
            for(int i = 0;i < this->Size();i++)
            {
                // get object
                nObject * obj = this->At(i);
                n_assert( obj );

                // get object id
                if ( ! obj->IsA("nentityobject") )
                {
                    // Object which are not entity objects are saved in the magic 0 section
                    if ( magicNumber != 0 )
                    {
                        continue;
                    }
                }
                else
                {
                    nEntityObjectId objId = 0;
                    nEntityObject* entObj = static_cast<nEntityObject*>( obj );
                    ncSubentity* subEntComp = entObj->GetComponent<ncSubentity>();
                    if ( subEntComp )
                    {
                        // Subentities have special oid's
                        objId = subEntComp->GetPersistenceOID();
                    }
                    else
                    {
                        // Get normal entity oid
                        entObj->GetId();
                    }

                    nEntityObjectId objectMagic = ( objId & ( nEntityObjectServer::IDHIGHMASK >> nEntityObjectServer::IDHIGHBITS )) >> nEntityObjectServer::IDSUBENTITYBITS;
                    // save objects of current magic only
                    if ( objectMagic != magicNumber )
                    {
                        continue;
                    }
                }

                // get constructor cmd (begin new object)
                nCmd * cmd = ps->GetCmd(this, 'BENO');
                bool objNamed = false;

                // fill the cmd
                cmd->Rewind();
                cmd->In()->SetS(obj->GetClass()->GetName());
                if (obj->GetClass()->IsA("nroot"))
                {
                    objNamed = true;
                    nRoot * objroot = static_cast<nRoot *> (obj);
                    cmd->In()->SetS(objroot->GetFullName().Get());
                }
                else
                {
                    objNamed = false;
                    cmd->In()->SetS("");
                }

                // save the object
                cmd->Rewind();
                if (ps->BeginObjectWithCmd(obj, cmd))
                {
                    if (!obj->SaveCmds(ps))
                    {
                        retval = false;
                    }
                    ps->EndObject(objNamed);
                } 
            }
        }
    }

    return retval;
}

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nObjectMagicInstancer)
    NSCRIPT_ADDCMD('JSMN', void, SetMagicNumber, 1, (unsigned int), 0, ());
NSCRIPT_INITCMDS_END()
