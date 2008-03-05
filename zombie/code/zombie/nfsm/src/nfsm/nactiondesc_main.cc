//-----------------------------------------------------------------------------
//  nactiondesc_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nactiondesc.h"
#include "nnetworkmanager/nnetworkmanager.h"
#include "entity/nentityobjectserver.h"
#include "nscriptclassserver/nscriptclassserver.h"
#include "util/nbase64.h" // Kept for backwards compatibility

nNebulaScriptClass(nActionDesc, "nroot");

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nActionDesc::nActionDesc()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Set the action class of this action description
*/
void
nActionDesc::SetActionClass( const char* className )
{
    this->actionClass = className;

    // Kept for backwards compatibility
#ifndef NGAME
    if ( this->actionClass == "gpactionscript" )
    {
        NLOG( defaultLog, (0, "Error: gpactionscript class is obsolete.") );
        this->actionArgs.SetFixedSize(1);
        this->actionArgs[0].SetO(NULL);
        return;
    }
#endif

    // Get input arguments prototype
    nClass* clazz = nKernelServer::Instance()->FindClass( this->actionClass.Get() );
#ifndef NGAME
    if ( !clazz )
    {
        NLOG( defaultLog, (0, "Error: Action class %s not found.", this->actionClass.Get()) );
        return;
    }
#endif
    nScriptClassServer::Instance()->AddClass( this->actionClass );
    nCmdProto* proto = clazz->FindCmdByName( "init" );
#ifndef NGAME
    if ( !proto )
    {
        nString msg( "carles.ros: Action class '" + this->actionClass + "' doesn't have the init command" );
        n_error( msg.Get() );
        return;
    }
#endif
    nString protoDef( proto->GetProtoDef() );
    n_assert( protoDef.MatchPattern("b_init_*") );
    nString protoIn( &protoDef.Get()[7] );

    // Create the argument list with default values
    this->actionArgs.SetFixedSize( protoIn.Length() );
    for ( int i(0); i < protoIn.Length(); ++i )
    {
        switch ( protoIn[i] )
        {
            case 'b':
                this->actionArgs[i].SetB(false);
                break;
            case 'f':
                this->actionArgs[i].SetF(0);
                break;
            case 'i':
                this->actionArgs[i].SetI(0);
                break;
            case 'o':
                this->actionArgs[i].SetO(NULL);
                break;
            case 's':
                this->actionArgs[i].SetS("");
                break;
#ifndef NGAME
            default:
                nString msg = "carles.ros: This error shouldn't have appeared, the argument type '";
                msg += nString(protoIn[i-1]) + "' has been missed.";
                n_error( msg.Get() );
                break;
#endif
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Get the action class of this action description
*/
const char*
nActionDesc::GetActionClass() const
{
    return this->actionClass.Get();
}

//-----------------------------------------------------------------------------
/**
    Validate that an argument index and type are valid
*/
bool
nActionDesc::ValidateArg( int index, nArg::Type type ) const
{
    if ( index < 0 || index >= this->actionArgs.Size() - 1 )
    {
        NLOG( defaultLog, (0, "Error: Parameter %d of the action %s is out of range. Maybe the action's init command has changed?", index, this->actionClass.Get()) );
        return false;
    }
    if ( this->actionArgs[index+1].GetType() != type )
    {
        NLOG( defaultLog, (0, "Error: Parameter %d of the action %s has an invalid type. Maybe the action's init command has changed?", index, this->actionClass.Get()) );
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
    Set a boolean argument
*/
void
nActionDesc::SetBoolArg( int index, bool value )
{
#ifndef NGAME
    if ( this->ValidateArg( index, nArg::Bool ) )
#endif
    {
        this->actionArgs[index+1].SetB( value );
    }
}

//-----------------------------------------------------------------------------
/**
    Get a boolean argument
*/
bool
nActionDesc::GetBoolArg( int index ) const
{
    n_assert( this->actionArgs[index+1].GetType() == nArg::Bool );
    return this->actionArgs[index+1].GetB();
}

//-----------------------------------------------------------------------------
/**
    Set a float argument
*/
void
nActionDesc::SetFloatArg( int index, float value )
{
#ifndef NGAME
    if ( this->ValidateArg( index, nArg::Float ) )
#endif
    {
        this->actionArgs[index+1].SetF( value );
    }
}

//-----------------------------------------------------------------------------
/**
    Get a float argument
*/
float
nActionDesc::GetFloatArg( int index ) const
{
    n_assert( this->actionArgs[index+1].GetType() == nArg::Float );
    return this->actionArgs[index+1].GetF();
}

//-----------------------------------------------------------------------------
/**
    Set an integer argument
*/
void
nActionDesc::SetIntArg( int index, int value )
{
#ifndef NGAME
    if ( this->ValidateArg( index, nArg::Int ) )
#endif
    {
        this->actionArgs[index+1].SetI( value );
    }
}

//-----------------------------------------------------------------------------
/**
    Get an integer argument
*/
int
nActionDesc::GetIntArg( int index ) const
{
    n_assert( this->actionArgs[index+1].GetType() == nArg::Int );
    return this->actionArgs[index+1].GetI();
}

//-----------------------------------------------------------------------------
/**
    Set an entity argument
*/
void
nActionDesc::SetEntityArg( int index, nEntityObject* entity )
{
    nEntityObjectId entityId;
    if ( !entity )
    {
        entityId = 0;
    }
    else
    {
        entityId = entity->GetId();
    }
    this->SetEntityArgById( index, entityId );
}

//-----------------------------------------------------------------------------
/**
    Set an entity argument by its id (used for persistence)
*/
void
nActionDesc::SetEntityArgById( int index, nEntityObjectId entityId )
{
#ifndef NGAME
    if ( this->ValidateArg( index, nArg::Object ) )
#endif
    {
        this->GetDynamicArg( index+1 ).SetEntityId( entityId );
    }
}

//-----------------------------------------------------------------------------
/**
    Get an entity argument
*/
nEntityObject*
nActionDesc::GetEntityArg( int index ) const
{
    n_assert( this->actionArgs[index+1].GetType() == nArg::Object );
    ValueInfo* dynArg = this->FindDynamicArg( index+1 );
    if ( !dynArg )
    {
        return NULL;
    }
    return dynArg->GetEntity();
}

//-----------------------------------------------------------------------------
/**
    Get an entity argument as an entity id (used for the editor)
*/
nEntityObjectId
nActionDesc::GetEntityArgAsId( int index ) const
{
    n_assert( this->actionArgs[index+1].GetType() == nArg::Object );
    ValueInfo* dynArg = this->FindDynamicArg( index+1 );
    if ( !dynArg )
    {
        return 0;
    }
    return dynArg->GetEntityId();
}

//-----------------------------------------------------------------------------
/**
    Set a string argument
*/
void
nActionDesc::SetStringArg( int index, const char* value )
{
#ifndef NGAME
    if ( this->ValidateArg( index, nArg::String ) )
#endif
    {
        this->actionArgs[index+1].SetS( value );
    }
}

//-----------------------------------------------------------------------------
/**
    Get a string argument
*/
const char*
nActionDesc::GetStringArg( int index ) const
{
    n_assert( this->actionArgs[index+1].GetType() == nArg::String );
    return this->actionArgs[index+1].GetS();
}

//-----------------------------------------------------------------------------
/**
    Set a 3D vector argument
*/
void
nActionDesc::SetVector3Arg( int index, const vector3& value )
{
    for ( int i(0); i < 3; ++i )
    {
    #ifndef NGAME
        if ( this->ValidateArg( index+i, nArg::Float ) )
    #endif
        {
            this->actionArgs[index+i+1].SetF( reinterpret_cast<const float*>(&value)[i] );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Get a 3D vector argument
*/
void
nActionDesc::GetVector3Arg( int index, vector3& value ) const
{
    for ( int i(0); i < 3; ++i )
    {
        n_assert( this->actionArgs[index+i+1].GetType() == nArg::Float );
        reinterpret_cast<float*>(&value)[i] = this->actionArgs[index+i+1].GetF();
    }
}

//-----------------------------------------------------------------------------
/**
    Set the action type id of this action description (kept for backwards compatibility)
*/
void
nActionDesc::SetActionType( int actionType )
{
    switch ( actionType )
    {
        case 0:
            this->SetActionClass("ngpmovement");
            break;
        case 8:
            this->SetActionClass("ngpsearchtarget");
            break;
        case 9:
            this->SetActionClass("ngpforgettarget");
            break;
        case 10:
            this->SetActionClass("ngpshoot");
            break;
        case 11:
            this->SetActionClass("ngpmelee");
            break;
        case 999:
            this->SetActionClass("gpactionscript");
            break;
        default:
            n_error( "carles.ros: This error shouldn't have appeared, some action type has been missed while keeping backwards compatibility." );
            break;
    }
}

//-----------------------------------------------------------------------------
/**
    Set action arguments, given in base64 (kept for backwards compatibility)
*/
void
nActionDesc::SetArgsInBase64( const char* args )
{
    // Fill old nstream type used to store arguments
    int size;
    char* data;
    nBase64::Base64To256( args, size, data );
    nstream streamArgs;
    streamArgs.SetBuffer( size, data );
    n_free( data );

    // Extract the arguments and fill the argument list
    streamArgs.SetWrite( false );
    if ( this->actionClass == "gpactionscript" )
    {
        char scriptName[50];
        streamArgs.UpdateString( scriptName );
        this->SetActionClass( scriptName );
    }
    else
    {
        for ( int i(1); i < this->actionArgs.Size(); ++i )
        {
            int index( i - 1 ); // First argument is reserved for the entity
            switch ( this->actionArgs[i].GetType() )
            {
                case nArg::Bool:
                    {
                        bool value;
                        streamArgs.UpdateBool( value );
                        this->SetBoolArg( index, value );
                    }
                    break;
                case nArg::Float:
                    {
                        float value;
                        streamArgs.UpdateFloat( value );
                        this->SetFloatArg( index, value );
                    }
                    break;
                case nArg::Int:
                    {
                        int value;
                        streamArgs.UpdateInt( value );
                        this->SetIntArg( index, value );
                    }
                    break;
                case nArg::String:
                    {
                        char value[50];
                        streamArgs.UpdateString( value );
                        this->SetStringArg( index, value );
                    }
                    break;
                default:
                    n_error( "carles.ros: This error shouldn't have appeared, some argument type has been missed while keeping backwards compatibility." );
                    break;
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Tell the action manager to apply this action to the given entity
*/
void
nActionDesc::ApplyAction (nEntityObject* entity, bool foreground)
{
#ifndef NGAME
    // Verify that the action class exists
    if ( !nKernelServer::Instance()->FindClass( this->actionClass.Get() ) )
    {
        nString msg( "carles.ros: Action class '" + this->actionClass + "' not found" );
        n_error( msg.Get() );
        return;
    }
#endif

    // Update the dynamic arguments
    this->FillDynamicArgs( entity );

    // Finally apply the action
    nNetworkManager::Instance()->SendAction( this->actionClass.Get(), this->actionArgs.Size(), this->actionArgs.Begin(), foreground );
}

//-----------------------------------------------------------------------------
/**
    Get the dynamic argument for an action argument index (create it if needed)
*/
nActionDesc::ValueInfo&
nActionDesc::GetDynamicArg( int index )
{
    // Find the dynamic argument whose action index matches the given one
    for ( int i(0); i < this->dynamicArgs.Size(); ++i )
    {
        if ( this->dynamicArgs[i].GetArgIndex() == index )
        {
            return this->dynamicArgs[i];
        }
    }

    // Dynamic argument not found, so create it
    this->dynamicArgs.Append( ValueInfo() );
    this->dynamicArgs.Back().SetArgIndex( index );
    return this->dynamicArgs.Back();
}

//-----------------------------------------------------------------------------
/**
    Get the dynamic argument for an action argument index or NULL if it doesn't exist
*/
nActionDesc::ValueInfo*
nActionDesc::FindDynamicArg( int index ) const
{
    // Find the dynamic argument whose action index matches the given one
    for ( int i(0); i < this->dynamicArgs.Size(); ++i )
    {
        if ( this->dynamicArgs[i].GetArgIndex() == index )
        {
            return &this->dynamicArgs[i];
        }
    }

    // Dynamic argument not found
    return NULL;
}

//-----------------------------------------------------------------------------
/**
    Get and fill the values of the dynamic arguments in the action arguments list
*/
void
nActionDesc::FillDynamicArgs( nEntityObject* entity )
{
    // The first argument is always the entity to who the action is going to be applied
    this->actionArgs[0].SetO( entity );

    // Fill other dynamic arguments
    for ( int i(0); i < this->dynamicArgs.Size(); ++i )
    {
        ValueInfo& dynArg = this->dynamicArgs[i];
        switch ( dynArg.GetValueType() )
        {
            case ValueInfo::ENTITY_ID:
            case ValueInfo::ENTITY_NAME:
                this->actionArgs[ dynArg.GetArgIndex() ].SetO( dynArg.GetEntity() );
                break;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Set the action argument index
*/
void
nActionDesc::ValueInfo::SetArgIndex( int index )
{
    this->argIndex = index;
}

//-----------------------------------------------------------------------------
/**
    Set a value as an entity id
*/
void
nActionDesc::ValueInfo::SetEntityId( nEntityObjectId id )
{
    this->valueType = ENTITY_ID;
    this->value.entityId = id;
}

//-----------------------------------------------------------------------------
/**
    Set a value as an entity name
*/
void
nActionDesc::ValueInfo::SetEntityName( const char* name )
{
    this->valueType = ENTITY_NAME;
    this->strValue = name;
    this->value.entityName = this->strValue.Get();
}

//-----------------------------------------------------------------------------
/**
    Get the value as an entity
*/
nEntityObject*
nActionDesc::ValueInfo::GetEntity() const
{
    switch ( this->valueType )
    {
        case ValueInfo::ENTITY_ID:
            if ( this->value.entityId == 0 )
            {
                return NULL;
            }
            else
            {
                return nEntityObjectServer::Instance()->GetEntityObject( this->value.entityId );
            }
            break;
        case ValueInfo::ENTITY_NAME:
            n_message( "carles.ros: TODO: Get entity argument from its name" );
//            return nWorldInterface::Instance()->GetEntity( this->value.entityName );
            break;
    }

    n_error( "carles.ros: Shouldn't reach here, a call to GetEntity has been erroneously made." );
    return NULL;
}

//-----------------------------------------------------------------------------
