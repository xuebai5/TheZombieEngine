//------------------------------------------------------------------------------
//  nnetworkmanagermulti_actions.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkmanager/nnetworkmanagermulti.h"

#include "nnetworkmanager/nclientproxy.h"

#include "nnetworkenet/nnetclientenet.h"
#include "nnetworkenet/nnetserverenet.h"

#include "ngpactionmanager/ngpactionmanager.h"

#include "entity/nentityobjectserver.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
    @param action basic action name
    @param numarg number of input arguments of the basic action
    @param arglist input arguments
    @param foreground if the action is foreground action or background action
    @param queue if action need to be enqueue
*/
void
nNetworkManagerMulti::SendBasicAction( 
    const char * action, int numarg, nArg * arglist,  bool foreground, bool queue )
{
    n_assert2( numarg <= nNetworkManagerMulti::MAX_NUM_PARAMS, "Too much params in a Basic Action" );

    bool needToSend = false;
    bool clientOnly = nGPActionManager::Instance()->CheckExecuteFlags( action , nGPActionManager::ISONLY_CLIENT );
    bool serverOnly = nGPActionManager::Instance()->CheckExecuteFlags( action , nGPActionManager::ISONLY_SERVER );
    bool inClients  = nGPActionManager::Instance()->CheckExecuteFlags( action , nGPActionManager::IS_IN_CLIENTS );
    bool isAhead    = nGPActionManager::Instance()->CheckExecuteFlags( action , nGPActionManager::IS_AHEAD );
    bool initOK = true;

    // execute if neccesary
    if( isAhead || ( ( this->refNetServer.isvalid() || clientOnly ) && ! inClients ) )
    {
        if( queue )
        {
            nGPActionManager::Instance()->QueueAction( action, numarg, arglist, foreground );
        }
        else
        {
            initOK = nGPActionManager::Instance()->SetAction( action, numarg, arglist, foreground );
        }
    }

    // check if need to send basic action
    if( this->refNetServer.isvalid() )
    {
        // check if only need to execute in server
        needToSend = initOK && ! serverOnly;

        // check if only need to execute in client
        needToSend = needToSend && ! clientOnly;
    }
    else if( this->refNetClient.isvalid() )
    {
        needToSend = ! clientOnly;
    }

    // send if it is neccesary
    if( needToSend )
    {
        // find args for "init" function of basic action
        nCmd * cmd = this->GetBasicActionInit( action );
        n_assert( cmd );

        if( cmd )
        {
            NLOG( network, ( NLOGACTIONS | 2, "Server: Send basic action \"%s\"", action ) );

            // set buffer to send
            nNetUtils::MessageType sendType = nNetUtils::Ordered;
            char * actionBuffer = this->actionOrderedBuffer;
            int * actionIndex = &this->actionOrderedIndex;
            if( nGPActionManager::Instance()->CheckExecuteFlags( action , nGPActionManager::IS_UNRELIABLE ) )
            {   
                sendType = nNetUtils::Unreliable;
                actionBuffer = this->actionUnreliableBuffer;
                actionIndex = &this->actionUnreliableIndex;
            }

            // init pointers to buffers
            char * base = actionBuffer + *actionIndex;
            char * pointer = base;

            // create flags
            unsigned char flags = 0;
            flags |= foreground ? BAF_FOREGROUND : 0;
            flags |= queue ? BAF_QUEUE : 0;

            // pack number of args in flag parameter
            flags |= unsigned char( numarg & BAF_NUMARGS );

            // get first parameter, the entity
            cmd->Rewind();
            nArg * arg = cmd->In();

            nEntityObject* entity = (nEntityObject*) arglist[ 0 ].GetO();

            // pack in a buffer the action
            pointer += nNetUtils::PackUniqueTo( pointer, action );
            pointer += nNetUtils::PackTo( pointer, entity->GetId() );
            pointer += nNetUtils::PackTo( pointer, flags );
            for( int i = 1; i < numarg ; ++i )
            {
                arg = cmd->In();
                pointer += nNetUtils::PackTo( pointer, arglist[ i ], arg );
            }

            cmd->GetProto()->RelCmd( cmd );
            cmd = 0;

            // send with the adecuate network layer
            unsigned int bufferSize = unsigned int( pointer - base );
            *actionIndex += bufferSize;

            if( *actionIndex > MTU_SIZE )
            {
                this->SendActionBuffer( actionBuffer, *actionIndex, sendType );
            }

            // save statistics
            if( bufferSize > this->longestAction )
            {
                this->longestAction = bufferSize;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @remarks let the basic action arguments in the argsArray object variable
    @param message buffer with the basic action
    @param entity [OUT] entity that execute the basic action
    @param action [OUT] basic action name
    @param numargs [OUT] number of input arguments of the basic action
    @param foreground [OUT] if the action is foreground action or background action
    @returns size of the basic action in the buffer
*/
size_t
nNetworkManagerMulti::UnpackAction( const char * message, nEntityObject * &entity, const char * &action,
                              int & numargs, bool &foreground, bool &queue )
{
    // init pointer to data
    const char * pointer = message;

    // get basic action name
    pointer += nNetUtils::UnpackUniqueFrom( pointer, action );

    // find args for "init" function of basic action
    nCmd * cmd = this->GetBasicActionInit( action );
    n_assert( cmd );

    if( cmd )
    {
        // get entity
        unsigned int id;
        pointer += nNetUtils::UnpackFrom( pointer, id );
        entity = nEntityObjectServer::Instance()->GetEntityObject( id );

        if( ! entity )
        {
            NLOG( network, ( NLOGACTIONS | 1, "Client ERROR: No Entity %x for action \"\"", id, action ) );
        }

        // get first parameter, the entity
        cmd->Rewind();
        nArg * arg = cmd->In();

        // get foreground param
        char byte;
        pointer += nNetUtils::UnpackFrom( pointer, byte );
        foreground = (byte & BAF_FOREGROUND) != 0;

        // get queue param
        queue = (byte & BAF_QUEUE) != 0;

        // get basic action params
        numargs = ( byte & BAF_NUMARGS );

        n_assert2( numargs <= nNetworkManagerMulti::MAX_NUM_PARAMS, "Too much params in a Basic Action" );
        this->argsArray[ 0 ].Delete();
        this->argsArray[ 0 ].SetO( entity );
        for( int i = 1 ; i < numargs ; ++i )
        {
            this->argsArray[ i ].Delete();
            arg = cmd->In();
            pointer += nNetUtils::UnpackFrom( pointer, this->argsArray[ i ], arg );
        }

        cmd->GetProto()->RelCmd( cmd );
    }

    return (pointer - message);
}

//------------------------------------------------------------------------------
/**
    @param action name of the basic action
    @returns the command for the init
*/
nCmd*
nNetworkManagerMulti::GetBasicActionInit( const char * action )const
{
    nClass * actionClass = nKernelServer::Instance()->FindClass( action );
    n_assert( actionClass );

    if( actionClass )
    {
        nCmdProto * initCmd = actionClass->FindCmdByName( "init" );
        n_assert( initCmd );

        if( initCmd )
        {
            return initCmd->NewCmd();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param message basic action payload
    @param size basic action payload size
*/
void
nNetworkManagerMulti::ActionsFromServer( const char * message, const int size )
{
    // basic action params
    const char * action;
    bool foreground;
    bool queue;
    int numargs = 0;
    nEntityObject * entity;

    const char * pointer = message;
    while( ( pointer - message ) < size )
    {
        action = 0;
        entity = 0;
        pointer += this->UnpackAction( pointer, entity, action, numargs, foreground, queue );

        if( entity && action )
        {
            NLOG( network, ( NLOGACTIONS | 2, "Client: Basic Action \"%s\" from server", action ) );

            // execute the action
            if( queue )
            {
                nGPActionManager::Instance()->QueueAction( action, numargs, this->argsArray, foreground );
            }
            else
            {
                nGPActionManager::Instance()->SetAction( action, numargs, this->argsArray, foreground );
            }
        }
        else
        {
            NLOG( network, ( NLOGACTIONS | 1, "Client ERROR: Bad actions block" ) );
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param messagebasic action payload
    @param size basic action payload size
    @param client that send the basic action
*/
void
nNetworkManagerMulti::ActionsFromClient( const char * message, const int size )
{
    // basic action params
    const char * action;
    bool foreground;
    bool queue;
    int numargs = 0;
    nEntityObject * entity;

    const char * last = 0;
    const char * pointer = message;
    while( ( pointer - message ) < size )
    {
        action = 0;
        entity = 0;
        last = pointer;
        pointer += this->UnpackAction( pointer, entity, action, numargs, foreground, queue );

        if( entity && action )
        {
            NLOG( network, ( NLOGACTIONS | 2, "Server: Basic action %x:\"%s\" ", 
                entity->GetId(), action ) );

            bool canSendAction = true;

            // execute the action if needed
            if( ! nGPActionManager::Instance()->CheckExecuteFlags( action , nGPActionManager::IS_IN_CLIENTS ) )
            {
                if( queue )
                {
                    nGPActionManager::Instance()->QueueAction( action, numargs, this->argsArray, foreground );
                }
                else
                {
                    canSendAction = nGPActionManager::Instance()->SetAction( action, numargs, this->argsArray, foreground );
                }
            }

            // check that only execute on server
            canSendAction = canSendAction && 
                ! nGPActionManager::Instance()->CheckExecuteFlags( action , nGPActionManager::ISONLY_SERVER );

            if( canSendAction )
            {
                NLOG( network, ( NLOGACTIONS | 2, "Send basic action \"%s\"", action ) );

                // set buffer to send
                nNetUtils::MessageType sendType = nNetUtils::Ordered;
                char * actionBuffer = this->actionOrderedBuffer;
                int * actionIndex = &this->actionOrderedIndex;
                if( nGPActionManager::Instance()->CheckExecuteFlags( action , nGPActionManager::IS_UNRELIABLE ) )
                {   
                    sendType = nNetUtils::Unreliable;
                    actionBuffer = this->actionUnreliableBuffer;
                    actionIndex = &this->actionUnreliableIndex;
                }

                // put in the buffer for send
                unsigned int bufferSize = unsigned int( pointer - last );
                memcpy( actionBuffer + *actionIndex, last, bufferSize );
                *actionIndex += bufferSize;

                if( *actionIndex > MTU_SIZE )
                {
                    this->SendActionBuffer( actionBuffer, *actionIndex, sendType );
                }

                // save statistics
                if( bufferSize > this->longestAction )
                {
                    this->longestAction = bufferSize;
                }
            }
        }
        else
        {
            NLOG( network, ( NLOGACTIONS | 1, "Server ERROR: Bad actions block" ) );
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManagerMulti::SendActionBuffer( const char * buffer, int & size, const nNetUtils::MessageType type )
{
    NLOG( network, ( NLOGACTIONS | 2, "Send action buffer ( %d ) of %d bytes", int( type ), size ) );

    if( this->refNetServer.isvalid() )
    {
        this->refNetServer->SendMessageAll( buffer, size, type );
    }
    else if( this->refNetClient.isvalid() )
    {
        this->refNetClient->SendMessage( buffer, size, type );
    }

    // save statistics
    this->actionsSended.lastBytes += size;
    ++this->actionsSended.lastNum;

    // restore buffer initial state
    size = int( nNetUtils::PackTo( buffer, char( nNetworkManager::MESSAGE_ACTION ) ) );
}

//------------------------------------------------------------------------------
