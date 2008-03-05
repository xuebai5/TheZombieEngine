//-----------------------------------------------------------------------------
//  nactiondesc_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nactiondesc.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nActionDesc )
    NSCRIPT_ADDCMD('ESAC', void, SetActionClass, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('EGAC', const char*, GetActionClass , 0, (), 0, ());
    NSCRIPT_ADDCMD('ESBA', void, SetBoolArg, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD('EGBA', bool, GetBoolArg, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESFA', void, SetFloatArg, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD('EGFA', float, GetFloatArg, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESIA', void, SetIntArg, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('EGIA', int, GetIntArg, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESEA', void, SetEntityArg, 2, (int, nEntityObject*), 0, ());
    NSCRIPT_ADDCMD('ESEI', void, SetEntityArgById, 2, (int, nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('EGEA', nEntityObject*, GetEntityArg, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EGEI', nEntityObjectId, GetEntityArgAsId, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESST', void, SetStringArg, 2, (int, const char*), 0, ());
    NSCRIPT_ADDCMD('EGSA', const char*, GetStringArg, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESV3', void, SetVector3Arg, 2, (int, const vector3&), 0, ());
    NSCRIPT_ADDCMD('EGV3', void, GetVector3Arg, 1, (int), 1, (vector3&) );
    NSCRIPT_ADDCMD('SETT', void, SetActionType, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SA64', void, SetArgsInBase64, 1, (const char*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
nActionDesc::SaveCmds( nPersistServer* ps )
{
    // Class
    ps->Put( this, 'ESAC', this->actionClass.Get() );

    // Arguments
    for ( int i(1); i < this->actionArgs.Size(); ++i )
    {
        int index( i - 1 ); // First argument is reserved for the entity
        const nArg& arg( this->actionArgs[i] );
        switch ( arg.GetType() )
        {
            case nArg::Bool:
                ps->Put( this, 'ESBA', index, arg.GetB() );
                break;
            case nArg::Float:
                ps->Put( this, 'ESFA', index, arg.GetF() );
                break;
            case nArg::Int:
                ps->Put( this, 'ESIA', index, arg.GetI() );
                break;
            case nArg::Object:
                // Entities are saved from dynamic arguments
                {
                    ValueInfo* dynArg = this->FindDynamicArg(i);
                    if ( dynArg )
                    {
                        switch ( dynArg->GetValueType() )
                        {
                            case ValueInfo::ENTITY_ID:
                                ps->Put( this, 'ESEI', index, dynArg->GetEntityId() );
                                break;
                            case ValueInfo::ENTITY_NAME:
                                n_message( "carles.ros: There isn't support for persistence of entities by name yet" );
                                break;
                        }
                    }
                }
                break;
            case nArg::String:
                ps->Put( this, 'ESSA', index, arg.GetS() );
                break;
#ifndef NGAME
            default:
                nString msg( "carles.ros: The type of parameter " );
                msg += nString(i);
                msg += " in the init command of the action '";
                msg += this->actionClass;
                msg += "' has no support for persistence";
                n_error( msg.Get() );
                return false;
#endif
        }
    }

    return true;
}

//------------------------------------------------------------------------------
