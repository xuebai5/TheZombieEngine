//------------------------------------------------------------------------------
//  nsignalbindingpython.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnpythonserver.h"

#include "python/nsignalbindingpython.h"
#include "python/ncmdprotopython.h"

//------------------------------------------------------------------------------

extern "C" PyObject* _putOutSingleArg(nArg *arg);

//------------------------------------------------------------------------------
nArray<nSignalBindingPython*> nSignalBindingPython::pythonBindings;

//------------------------------------------------------------------------------
/**
    @param obj python object
    @param str method name in the object
    @param priority priority of the binding
*/
nSignalBindingPython::nSignalBindingPython( PyObject * obj, const nString & str, int priority, const nString & proto ):
    nSignalBinding(priority),
    pyRefObject(0),
    method(str)
{
    // check the python object
    n_assert( obj );

    if( obj )
    {
        // get params from proto def string
        nString params = proto;
        int index = -1;

        do
        {
            index = params.FindChar( '_', 0 );
            if( index != -1 )
            {
                params = params.ExtractRange( index + 1, params.Length() - ( index + 1 ) );
            }
        }while( index != -1 );

        // create the Proto Definition of the method
        this->protoDef.Format( "v_%s_%s", this->method.Get(), params.Get() );

        // create a weak reference to object
        this->pyRefObject = PyWeakref_NewRef( obj, 0 );
    }

    pythonBindings.Append( this );
}

//------------------------------------------------------------------------------
/**
*/
nSignalBindingPython::~nSignalBindingPython()
{
    pythonBindings.EraseQuick( pythonBindings.FindIndex( this ) );
}

//------------------------------------------------------------------------------
/**
*/
bool
nSignalBindingPython::Invoke( nCmd * cmd )
{
    // get info about parameters expected
    ProtoDefInfo info( this->protoDef.Get() );

    if( ! info.valid )
    {
        return false;
    }

    if( cmd->GetNumInArgs() != info.numInArgs )
    {
        return false;
    }

    this->CallPythonMethod( cmd );

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nSignalBindingPython::Invoke( va_list args )
{
    nCmdProtoPython cmdProto( this->protoDef.Get() );
    nCmd * cmd = cmdProto.NewCmd();
    n_assert(cmd);

    // set input command arguments
    cmd->CopyInArgsFrom( args );

    cmd->Rewind();

    this->CallPythonMethod( cmd );

    cmdProto.RelCmd( cmd );

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nSignalBindingPython::CallPythonMethod( nCmd * const cmd )
{
    ProtoDefInfo info( this->protoDef.Get() );

    // create a tuple for put the args in
    PyObject * arglist = PyTuple_New( cmd->GetNumInArgs() );

    if( ! arglist )
    {
        if( PyErr_Occurred() )
        {
            PyErr_Print();
        }
        return;
    }

    // copy the args in the tuple
    for( int i = 0; i < cmd->GetNumInArgs() ; ++i )
    {
        PyTuple_SetItem( arglist, i, _putOutSingleArg( cmd->In() ) );
    }

    if( this->pyRefObject )
    {
        PyObject * object = PyWeakref_GetObject( this->pyRefObject );

        // create the callable object using object + method
        PyObject * pyStr = PyString_FromFormat( this->method.Get() );
        n_assert( pyStr );
        if( pyStr )
        {
            // when callobject(pyMethod) is created a Ref to the object is addded
            PyObject * pyMethod = PyObject_GetAttr( object, pyStr );
            if( pyMethod )
            {
                PyObject * retval;

                retval = PyObject_CallObject( pyMethod, arglist );

                if( PyErr_Occurred() )
                {
                    PyErr_Print();
                }

                if( retval != 0 )
                {
                    Py_DECREF( retval );
                }

                Py_DECREF( pyMethod );
            }
            else
            {
                if( PyErr_Occurred() )
                {
                    PyErr_Print();
                }
            }

            Py_DECREF( pyStr );
        }
    }

    Py_DECREF( arglist );
}

//------------------------------------------------------------------------------
/**
    @returns the proto definition of binding
*/
const char *
nSignalBindingPython::GetProtoDef() const
{
    return this->protoDef.Get();
}

//------------------------------------------------------------------------------
/**
    @returns true if binding is valid
*/
bool
nSignalBindingPython::IsValid() const
{
    if( this->pyRefObject )
    {
        if( PyWeakref_GetObject( this->pyRefObject ) != Py_None )
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    @param obj the PyObject to found
    @returns the founded binding or 0
*/
nSignalBindingPython *
nSignalBindingPython::FindBinding( const PyObject * const obj )
{
    nSignalBindingPython * founded = 0;

    for( int i = 0 ; i < pythonBindings.Size() && ! founded ; ++i )
    {
        PyObject * object = PyWeakref_GetObject( pythonBindings[ i ]->pyRefObject );
        if( object != Py_None )
        {
            if( object == obj )
            {
                founded = pythonBindings[ i ];
            }
        }
    }

    return founded;
}

//------------------------------------------------------------------------------
