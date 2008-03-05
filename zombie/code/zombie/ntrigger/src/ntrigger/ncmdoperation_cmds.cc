//-----------------------------------------------------------------------------
//  ncmdoperation_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ncmdoperation.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nCmdOperation )
    NSCRIPT_ADDCMD('EGTL', const char*, GetTypeLabel , 0, (), 0, ());
    NSCRIPT_ADDCMD('ESOC', void, SetObjectCommand, 2, (nObject*, const char*), 0, ());
    NSCRIPT_ADDCMD('ESRC', void, SetNRootCommand, 3, (const char*, const char*, const char*), 0, ());
    NSCRIPT_ADDCMD('ESEC', void, SetEntityObjectCommand, 3, (nEntityObjectId, const char*, const char*), 0, ());
    NSCRIPT_ADDCMD('ESCC', void, SetEntityClassCommand, 2, (const char*, const char*), 0, ());
    NSCRIPT_ADDCMD('EGTO', nObject*, GetTargetObject , 0, (), 0, ());
    NSCRIPT_ADDCMD('EGTC', const char*, GetTargetCommand , 0, (), 0, ());
    NSCRIPT_ADDCMD('ESBA', void, SetBoolArg, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD('EGBA', bool, GetBoolArg, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESFA', void, SetFloatArg, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD('EGFA', float, GetFloatArg, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESIA', void, SetIntArg, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('EGIA', int, GetIntArg, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESST', void, SetStringArg, 2, (int, const char*), 0, ());
    NSCRIPT_ADDCMD('EGSA', const char*, GetStringArg, 1, (int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
nCmdOperation::SaveCmds( nPersistServer* ps )
{
    nObject* object = this->GetTargetObject();
    if ( !object )
    {
        return false;
    }

    // Object and command
    const char* className = object->GetClass()->GetName();
    switch ( this->objectType )
    {
        case NROOT:
            ps->Put( this, 'ESRC', this->objectId.nrootPath, className, this->cmdName.Get() );
            break;
        case ENTITY_OBJECT:
            ps->Put( this, 'ESEC', this->objectId.entityId, className, this->cmdName.Get() );
            break;
        case ENTITY_CLASS:
            ps->Put( this, 'ESCC', this->objectId.entityClassName, className, this->cmdName.Get() );
            break;
#ifndef NGAME
        default:
            {
                nString errorMsg( "carles.ros: Unknown object type for object class '" );
                errorMsg += className;
                errorMsg += "'.";
                n_error( errorMsg.Get() );
                return false;
            }
#endif
    }

    // Arguments
    for ( int i(0); i < this->cmdArgs.Size(); ++i )
    {
        const nArg& arg( this->cmdArgs[i] );
        switch ( arg.GetType() )
        {
            case nArg::Bool:
                ps->Put( this, 'ESBA', i, arg.GetB() );
                break;
            case nArg::Float:
                ps->Put( this, 'ESFA', i, arg.GetF() );
                break;
            case nArg::Int:
                ps->Put( this, 'ESIA', i, arg.GetI() );
                break;
            case nArg::String:
                ps->Put( this, 'ESST', i, arg.GetS() );
                break;
            case nArg::Object:
                // Supported type, but value is dynamically generated, so nothing to save
                break;
#ifndef NGAME
            default:
                nString msg( "carles.ros: Parameter " );
                msg += nString(i);
                msg += " in the command ";
                msg += className;
                msg += ".";
                msg += this->cmdName;
                msg += " has an unsupported type.";
                n_error( msg.Get() );
#endif
        }
    }

    return true;
}
