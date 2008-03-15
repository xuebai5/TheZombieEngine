//------------------------------------------------------------------------------
//  ninput_logfile.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "input/ninputserver.h"
#include "input/ninputlogevent.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
//------------------------------------------------------------------------------
// table with the names of nInputType
const struct{
    char * name;
    nInputType type;
} LogFileTypeNames[] = {
    {"NONE",           N_INPUT_NONE},
    {"KEY_DOWN",       N_INPUT_KEY_DOWN},           
    {"KEY_UP",         N_INPUT_KEY_UP},             
    {"KEY_CHAR",       N_INPUT_KEY_CHAR},
    {"MOUSE_MOVE",     N_INPUT_MOUSE_MOVE},         
    {"AXIS_MOVE",      N_INPUT_AXIS_MOVE},          
    {"BUTTON_DOWN",    N_INPUT_BUTTON_DOWN},        
    {"BUTTON_UP",      N_INPUT_BUTTON_UP},          
    {"BUTTON_DBLCLCK", N_INPUT_BUTTON_DBLCLCK},
    {0,N_INPUT_NONE}
};

// max number of events to load from a file in memory
const int LOGFILE_MAXNUMEVENTS( 100 );
const float LOG_EPSILON( 0.0001f );

//------------------------------------------------------------------------------
/**
    @param type type to get the name
    @returns a strign with the name of the type
*/
const char *
nInputServer::InputTypeToString( const nInputType type )
{
    int i = 0;
    while( LogFileTypeNames[i].name && LogFileTypeNames[i].type != type )
    {
        ++i;
    }
    return LogFileTypeNames[i].name;
}

//------------------------------------------------------------------------------
/**
    @param name name of the nInputType
    @returns the nInputType
*/
const nInputType
nInputServer::StringToInputType( const nString & name )
{
    int i = 0;
    while( LogFileTypeNames[i].name && name != LogFileTypeNames[i].name )
    {
        ++i;
    }
    return LogFileTypeNames[i].type;
}

//------------------------------------------------------------------------------
/**
    @param num number of events to get from file
*/
void
nInputServer::LogFileGetEvents( const int num )
{
    n_assert2( num > 0 , "luis.cabellos: Log File get events from file bad number" );

    int counter = num;

    char cadena[N_MAXPATH];
    nInputLogEvent *log_event;
    while( this->logFileObject->GetS( cadena, N_MAXPATH ) && counter-- )
    {
        // all events are in a line of the file and the line begin with '+' simbol
        if( cadena[0] == '+' )
        {
            // get the parameters from the line
            nString input( cadena + 1 );
            nArray<nString> tokens;
            N_IFDEF_ASSERTS(int numTokens =) input.Tokenize( " ", tokens );

            // create the event and get the time of execution
            int index = 0;
            log_event = n_new( nInputLogEvent );
            log_event->time = tokens[index++].AsFloat();
            log_event->event = this->NewEvent();
            n_assert2( log_event->event, "luis.cabellos:No memory" );

            // get the type of the event
            nInputType type = this->StringToInputType( tokens[index++] );
            n_assert2( type != N_INPUT_NONE, "luis.cabellos:Log File Type Event incorrect" );

            // fill the event with the parameters
            log_event->event->SetType( type );
            log_event->event->SetDeviceId( tokens[index++].AsInt() );
            switch( type )
            {
                case N_INPUT_KEY_CHAR:
                    log_event->event->SetChar( static_cast<char>( tokens[index++].AsInt() ) );

                    break;

                case N_INPUT_KEY_UP:
                case N_INPUT_KEY_DOWN:
                    log_event->event->SetKey( static_cast<nKey>( tokens[index++].AsInt() ) );
                    
                    break;

                case N_INPUT_MOUSE_MOVE:
                    log_event->event->SetAbsPos( tokens[index].AsInt(), tokens[index+1].AsInt() );
                    index += 2;
                    log_event->event->SetRelPos( tokens[index].AsFloat(), tokens[index+1].AsFloat() );
                    index += 2;

                    break;

                case N_INPUT_AXIS_MOVE:
                    log_event->event->SetAxis( tokens[index++].AsInt() );
                    log_event->event->SetAxisValue( tokens[index++].AsFloat() );

                    break;

                case N_INPUT_BUTTON_UP:
                case N_INPUT_BUTTON_DOWN:
                case N_INPUT_BUTTON_DBLCLCK:
                    log_event->event->SetButton( tokens[index++].AsInt() );
                    log_event->event->SetAbsPos( tokens[index].AsInt(), tokens[index+1].AsInt() );
                    index += 2;
                    log_event->event->SetRelPos( tokens[index].AsFloat(), tokens[index+1].AsFloat() );
                    index += 2;

                    break;

                default:
                    n_assert_always();
            }
            n_assert2( numTokens == index, "luis.cabellos:Log File event bad formatted" );
            // add event to list of events to execute
            this->log_file_events.AddTail( log_event );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param filename file where save the events
*/
void
nInputServer::StartLogToFile( nString * filename )
{
    // end actual logging
    if( this->IsLogFileRunning() )
    {
        this->StopLogFile();
    }

    // save actual time in initial log time
    this->log_init_time = this->timeStamp;

    // open Out file
    n_assert2( ! this->logFileObject, "luis.cabellos:Log File is not close" );
    nFileServer2 * fs = nKernelServer::ks->GetFileServer();
    this->logFileObject = fs->NewFileObject();

    this->logFileObject->Open( filename->Get() , "w" );

    this->log_file = N_LOG_OUT;
}

//------------------------------------------------------------------------------
/**
    @param filename file with the events to load
*/
void
nInputServer::StartLogFromFile( nString * filename )
{
    // end actual logging
    if( this->IsLogFileRunning() )
    {
        this->StopLogFile();
    }

    // open In file
    n_assert2( ! this->logFileObject, "luis.cabellos:Log File is not close" );
    nFileServer2 * fs = nKernelServer::ks->GetFileServer();
    
    if( ! fs->FileExists( filename->Get() ) )
    {
        return;
    }

    this->logFileObject = fs->NewFileObject();
    this->logFileObject->Open( filename->Get(), "r" );

    // get first block of events
    this->LogFileGetEvents( LOGFILE_MAXNUMEVENTS );

    // save actual time in initial log time
    this->log_init_time = this->timeStamp;

    // read events from file

    this->log_file = N_LOG_IN;
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::StopLogFile()
{
    this->LogFileClean();

    this->log_file = N_LOG_NONE;
}

//------------------------------------------------------------------------------
/**
    @retval true if is log to/from file
*/
bool
nInputServer::IsLogFileRunning()
{
    return ( this->log_file != N_LOG_NONE );
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::LogFileEvents()
{
    n_assert2( this->log_file != N_LOG_NONE, "luis.cabellos:Log File called with incorrect log type" );

    if( this->log_file == N_LOG_IN )
    {
        this->LogFileEventsFromFile();
    }
    else
    {
        this->LogFileEventsToFile();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::LogFileClean()
{
    // delete list of events
    nInputLogEvent *logevent;
    while( 0 != (logevent = static_cast<nInputLogEvent*>( this->log_file_events.RemHead() ) ) )
    {
        n_delete( logevent->event );
        n_delete( logevent );
    }

    // delete list of old events
    nInputEvent *event;
    while( 0 != (event = static_cast<nInputEvent*>( this->log_file_oldEvents.RemHead() ) ) )
    {
        n_delete( event );
    }

    // close file
    if( this->logFileObject )
    {
        this->logFileObject->Close();
        this->logFileObject->Release();
        this->logFileObject = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::LogFileEventsFromFile()
{
    this->FlushEvents();

    // if there aren't more events, end log from file
    if( this->log_file_events.IsEmpty() )
    {
        this->StopLogFile();
    }
    else
    {
        // running flag is true will there are events to execute
        bool running = true;
        nInputLogEvent *log = 0;
        while( running && !this->log_file_events.IsEmpty() )
        {
            log = static_cast<nInputLogEvent*>( this->log_file_events.GetHead() );
            // if it's time to execute the event
            if( log->time <= (this->timeStamp - this->log_init_time) )
            {
                // send event to input server
                this->LinkEvent( log->event );

                // erase from log list
                this->log_file_events.RemHead();
                n_delete( log );
                log = 0;

                // if there aren't more events in memory
                if( this->log_file_events.IsEmpty() )
                {
                    // load next block of events
                    this->LogFileGetEvents( LOGFILE_MAXNUMEVENTS );
                }
            }
            else
            {
                running = false;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::LogFileEventsToFile()
{
    nInputEvent *ie;
    //nInputLogEvent *log_event;
    const char * type;
    for (ie = static_cast<nInputEvent *>( this->events.GetHead() );
         ie;
         ie = static_cast<nInputEvent *>( ie->GetSucc() ) )
    {
        // look event repeat
        nInputEvent * oldEvent = 0;
        switch( ie->GetType() )
        {
            // look for repeated AXIS events
            case N_INPUT_AXIS_MOVE:
                for( oldEvent = static_cast<nInputEvent*>( this->log_file_oldEvents.GetHead() );
                    oldEvent;
                    oldEvent = static_cast<nInputEvent*>( oldEvent->GetSucc() ) )
                {
                    if( oldEvent->GetType() == ie->GetType() &&
                        oldEvent->GetDeviceId() == ie->GetDeviceId() &&
                        oldEvent->GetButton() == ie->GetButton() )
                    {
                        break;
                    }
                }

                if( oldEvent )
                {
                    // if event is the same
                    if( fabs( oldEvent->GetAxisValue() - ie->GetAxisValue() ) < LOG_EPSILON )
                    {
                        // go to next event
                        continue;
                    }

                    // actualize old event
                    oldEvent->SetAxisValue( ie->GetAxisValue() );
                }
                else
                {
                    // if axis event isn't save save it in old events list
                    oldEvent = this->NewEvent();
                    n_assert2( oldEvent, "luis.cabellos:No memory" );

                    oldEvent->SetType( ie->GetType() );
                    oldEvent->SetDeviceId( ie->GetDeviceId() );
                    oldEvent->SetButton( ie->GetButton() );
                    oldEvent->SetAxisValue( ie->GetAxisValue() );
                    this->log_file_oldEvents.AddTail( oldEvent );
                }

                break;
        }

        // write event to file
        type = InputTypeToString( ie->GetType() );
        n_assert( type );
        this->logFileObject->PutChar('+');
        this->logFileObject->PutS( nString( static_cast<float>( this->timeStamp - this->log_init_time ) ).Get() );
        this->logFileObject->PutChar(' ');
        this->logFileObject->PutS( type );
        this->logFileObject->PutChar(' ');
        this->logFileObject->PutS( nString( ie->GetDeviceId() ).Get() );
        this->logFileObject->PutChar(' ');

        switch (ie->GetType()) 
        {
            case N_INPUT_KEY_CHAR:
                this->logFileObject->PutS( nString( static_cast<int>( ie->GetChar() ) ).Get() );
                break;

            case N_INPUT_KEY_UP:
            case N_INPUT_KEY_DOWN:
                this->logFileObject->PutS( nString( static_cast<int>( ie->GetKey() ) ).Get() );
                break;

            case N_INPUT_AXIS_MOVE:
                this->logFileObject->PutS( nString( ie->GetAxis() ).Get() );
                this->logFileObject->PutChar(' ');
                this->logFileObject->PutS( nString( ie->GetAxisValue() ).Get() );
                break;

            case N_INPUT_BUTTON_UP:
            case N_INPUT_BUTTON_DOWN:
            case N_INPUT_BUTTON_DBLCLCK:
                this->logFileObject->PutS( nString( ie->GetButton() ).Get() );
                this->logFileObject->PutChar(' ');
                // no break because need the next case to complete
                // break;

            case N_INPUT_MOUSE_MOVE:
                this->logFileObject->PutS( nString( ie->GetAbsXPos() ).Get() );
                this->logFileObject->PutChar(' ');
                this->logFileObject->PutS( nString( ie->GetAbsYPos() ).Get() );
                this->logFileObject->PutChar(' ');
                this->logFileObject->PutS( nString( ie->GetRelXPos() ).Get() );
                this->logFileObject->PutChar(' ');
                this->logFileObject->PutS( nString( ie->GetRelYPos() ).Get() );
                break;

            default:
                break;
        }
        this->logFileObject->PutS( "\n" );
    }
}
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
