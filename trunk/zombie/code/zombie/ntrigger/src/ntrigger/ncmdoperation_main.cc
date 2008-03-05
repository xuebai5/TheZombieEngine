//-----------------------------------------------------------------------------
//  ncmdoperation_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ncmdoperation.h"

nNebulaScriptClass(nCmdOperation, "nroot");

//-----------------------------------------------------------------------------
namespace
{
    const char* OperationTypeLabel = "Command";
}

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nCmdOperation::nCmdOperation() :
    objectType( UNDEFINED )
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
nCmdOperation::~nCmdOperation()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Get operation type
*/
nOperation::OperationType
nCmdOperation::GetType() const
{
    return nOperation::COMMAND;
}

//-----------------------------------------------------------------------------
/**
    Get operation type as string
*/
const char*
nCmdOperation::GetTypeLabel() const
{
    return OperationTypeLabel;
}

//-----------------------------------------------------------------------------
/**
    Execute the operation

    Execute the stored command
*/
void
nCmdOperation::Execute( nEntityObject* /*trigger*/, nEntityObject* entity )
{
    // Get target object
    nObject* object( this->GetTargetObject() );
    n_assert( object );

    // Execute the command
    if ( object )
    {
        this->FillDynamicArgs( entity );
        object->CallArgs( this->cmdName.Get(), 0, NULL, this->cmdArgs.Size(), this->cmdArgs.Begin() );
    }
}

//-----------------------------------------------------------------------------
/**
    Fill the values of the dynamic arguments in the action arguments list
*/
void
nCmdOperation::FillDynamicArgs( nEntityObject* entity )
{
    // Any object argument is extected to be the entity activating the trigger,
    // so replace all object argument by that entity
    for ( int i(0); i < this->cmdArgs.Size(); ++i )
    {
        nArg& arg( this->cmdArgs[i] );
        if ( arg.GetType() == nArg::Object )
        {
            arg.SetO( entity );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Set the object and command to be executed
*/
void
nCmdOperation::SetObjectCommand( nObject* object, const char* cmdName )
{
    n_assert( object );
    n_assert( cmdName );

    const char* className = object->GetClass()->GetName();

    if ( object->IsA("nentityclass") )
    {
        this->SetEntityClassCommand( className, cmdName );
    }
    else if ( object->IsA("nentityobject") )
    {
        this->SetEntityObjectCommand( static_cast<nEntityObject*>( object )->GetId(), className, cmdName );
    }
    else
    {
        n_assert( object->IsA("nroot") );
        this->SetNRootCommand( static_cast<nRoot*>( object )->GetFullName().Get(), className, cmdName );
    }
}

//-----------------------------------------------------------------------------
/**
    Set the object and command to be executed
*/
void
nCmdOperation::SetNRootCommand( const char* path, const char* className, const char* cmdName )
{
    this->objectType = NROOT;
    this->objectIdStr = path;
    this->objectId.nrootPath = this->objectIdStr.Get();
    this->cmdName = cmdName;
    this->InitArgs( className );
}

//-----------------------------------------------------------------------------
/**
    Set the object and command to be executed
*/
void
nCmdOperation::SetEntityObjectCommand( nEntityObjectId entityId, const char* className, const char* cmdName )
{
    this->objectType = ENTITY_OBJECT;
    this->objectId.entityId = entityId;
    this->cmdName = cmdName;
    this->InitArgs( className );
}

//-----------------------------------------------------------------------------
/**
    Set the object and command to be executed
*/
void
nCmdOperation::SetEntityClassCommand( const char* className, const char* cmdName )
{
    this->objectType = ENTITY_CLASS;
    this->objectIdStr = className;
    this->objectId.entityClassName = this->objectIdStr.Get();
    this->cmdName = cmdName;
    this->InitArgs( className );
}

//-----------------------------------------------------------------------------
/**
    Init the action command arguments to default values
*/
void
nCmdOperation::InitArgs( const char* className )
{
    // Get input arguments prototype
    nClass* clazz = nKernelServer::Instance()->FindClass( className );
#ifndef NGAME
    if ( !clazz )
    {
        NLOG( defaultLog, (0, "Error: Object class %s not found.", className) );
        return;
    }
#endif
    nCmdProto* proto = clazz->FindCmdByName( this->cmdName.Get() );
#ifndef NGAME
    if ( !proto )
    {
        NLOG( defaultLog, (0, "Error: Object class %s doesn't have the command %s.", className, this->cmdName.Get()) );
        return;
    }
#endif
    nString protoDef( proto->GetProtoDef() );
    int inArgsPos(0);
    for ( int i(0); i < 2; ++i )
    {
        inArgsPos = protoDef.IndexOf( "_", inArgsPos ) + 1;
    }
    nString protoIn( &protoDef.Get()[inArgsPos] );

    // Create the argument list with default values
    if ( protoIn[0] == 'v' )
    {
        this->cmdArgs.SetFixedSize( 0 );
    }
    else
    {
        this->cmdArgs.SetFixedSize( protoIn.Length() );
        for ( int i(0); i < protoIn.Length(); ++i )
        {
            switch ( protoIn[i] )
            {
                case 'b':
                    this->cmdArgs[i].SetB(false);
                    break;
                case 'f':
                    this->cmdArgs[i].SetF(0);
                    break;
                case 'i':
                    this->cmdArgs[i].SetI(0);
                    break;
                case 's':
                    this->cmdArgs[i].SetS("");
                    break;
                case 'o':
                    this->cmdArgs[i].SetO(NULL);
                    break;
#ifndef NGAME
                default:
                    NLOG( defaultLog, (0, "Error: Argument type %c in the command %s.%s is not supported for trigger commands.", protoIn[i], className, this->cmdName.Get()) );
                    break;
#endif
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Get the object onto which the command will be executed

    Do not use this method at load time since the target object may not have
    been loaded yet.
*/
nObject*
nCmdOperation::GetTargetObject() const
{
    nObject* object( NULL );

    switch ( this->objectType )
    {
        case NROOT:
            object = nKernelServer::Instance()->Lookup( this->objectId.nrootPath );
            break;
        case ENTITY_OBJECT:
            object = nEntityObjectServer::Instance()->GetEntityObject( this->objectId.entityId );
            break;
        case ENTITY_CLASS:
            object = nEntityClassServer::Instance()->GetEntityClass( this->objectId.entityClassName );
            break;
    }

    return object;
}

//-----------------------------------------------------------------------------
/**
    Get the command to be executed
*/
const char*
nCmdOperation::GetTargetCommand() const
{
    return this->cmdName.Get();
}

//-----------------------------------------------------------------------------
/**
    Validate that an argument index and type are valid
*/
bool
nCmdOperation::ValidateArg( int index, nArg::Type type ) const
{
    if ( index < 0 || index >= this->cmdArgs.Size() )
    {
        NLOG( defaultLog, (0, "Error: Parameter %d of the command %s is out of range.", index, this->cmdName.Get()) );
        return false;
    }
    if ( this->cmdArgs[index].GetType() != type )
    {
        NLOG( defaultLog, (0, "Error: Parameter %d of the command %s has an invalid type.", index, this->cmdName.Get()) );
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
    Set a boolean argument
*/
void
nCmdOperation::SetBoolArg( int index, bool value )
{
#ifndef NGAME
    if ( this->ValidateArg( index, nArg::Bool ) )
#endif
    {
        this->cmdArgs[index].SetB( value );
    }
}

//-----------------------------------------------------------------------------
/**
    Get a boolean argument
*/
bool
nCmdOperation::GetBoolArg( int index ) const
{
#ifndef NGAME
    if ( !this->ValidateArg( index, nArg::Bool ) )
    {
        return false;
    }
#endif
    return this->cmdArgs[index].GetB();
}

//-----------------------------------------------------------------------------
/**
    Set a float argument
*/
void
nCmdOperation::SetFloatArg( int index, float value )
{
#ifndef NGAME
    if ( this->ValidateArg( index, nArg::Float ) )
#endif
    {
        this->cmdArgs[index].SetF( value );
    }
}

//-----------------------------------------------------------------------------
/**
    Get a float argument
*/
float
nCmdOperation::GetFloatArg( int index ) const
{
#ifndef NGAME
    if ( !this->ValidateArg( index, nArg::Float ) )
    {
        return 0;
    }
#endif
    return this->cmdArgs[index].GetF();
}

//-----------------------------------------------------------------------------
/**
    Set an integer argument
*/
void
nCmdOperation::SetIntArg( int index, int value )
{
#ifndef NGAME
    if ( this->ValidateArg( index, nArg::Int ) )
#endif
    {
        this->cmdArgs[index].SetI( value );
    }
}

//-----------------------------------------------------------------------------
/**
    Get an integer argument
*/
int
nCmdOperation::GetIntArg( int index ) const
{
#ifndef NGAME
    if ( !this->ValidateArg( index, nArg::Int ) )
    {
        return 0;
    }
#endif
    return this->cmdArgs[index].GetI();
}

//-----------------------------------------------------------------------------
/**
    Set a string argument
*/
void
nCmdOperation::SetStringArg( int index, const char* value )
{
#ifndef NGAME
    if ( this->ValidateArg( index, nArg::String ) )
#endif
    {
        this->cmdArgs[index].SetS( value );
    }
}

//-----------------------------------------------------------------------------
/**
    Get a string argument
*/
const char*
nCmdOperation::GetStringArg( int index ) const
{
#ifndef NGAME
    if ( !this->ValidateArg( index, nArg::String ) )
    {
        return NULL;
    }
#endif
    return this->cmdArgs[index].GetS();
}
