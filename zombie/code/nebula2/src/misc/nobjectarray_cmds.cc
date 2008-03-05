//------------------------------------------------------------------------------
/**
    @file nobjectarray_cmds.cc
    @author Mateu Batle

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"

#include "misc/nobjectarray.h"

//------------------------------------------------------------------------------
bool 
nObjectArray::SaveCmds(nPersistServer* ps)
{
    bool retval = true;

    if (this->objs)
    {
        ps->Put(this, 'INIT', this->Size(), 64);

        // save all objects
        for(int i = 0;i < this->Size();i++)
        {
            // get constructor cmd (begin new object)
            nCmd * cmd = ps->GetCmd(this, 'BENO');
            bool objNamed = false;

            // get object
            nObject * obj = this->At(i);

            // fill the cmd
            cmd->Rewind();
            cmd->In()->SetS(obj->GetClass()->GetName());
            nString objName = this->GetObjectNameAtIndex(i);
            objNamed = false;
            if (objName.IsEmpty())
            {
                if (obj->GetClass()->IsA("nroot"))
                {
                    objNamed = true;
                    nRoot * objroot = static_cast<nRoot *> (obj);
                    objName = objroot->GetFullName();
                }
            }
            cmd->In()->SetS(objName.Get());

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

    return retval;
}

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nObjectArray)
    NSCRIPT_ADDCMD('INIT', void, Init, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('CLEA', void, Clear, 0, (), 0, ());
    NSCRIPT_ADDCMD('SIZE', int, Size, 0, (), 0, ());
    NSCRIPT_ADDCMD('EMPT', bool, Empty, 0, (), 0, ());
    NSCRIPT_ADDCMD('APPE', void, Append, 1, (nObject *), 0, ());
    NSCRIPT_ADDCMD('APWN', void, AppendWithName, 2, (nObject *, const char *), 0, ());
    NSCRIPT_ADDCMD('SETS', void, Set, 2, (int, nObject *), 0, ());
    NSCRIPT_ADDCMD('SEWN', void, SetWithName, 3, (int, nObject *, const char *), 0, ());
    NSCRIPT_ADDCMD('ERAS', void, Erase, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ATAT', nObject *, At, 1, (int), 0, ());
    NSCRIPT_ADDCMD('FIIN', int, FindIndex, 1, (nObject *), 0, ());
    NSCRIPT_ADDCMD('GONA', const char *, GetObjectNameAtIndex, 1, (int), 0, ());    
NSCRIPT_INITCMDS_END()
