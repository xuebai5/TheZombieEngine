#ifndef NSIGNALBINDINGPYTHON_H
#define NSIGNALBINDINGPYTHON_H
//------------------------------------------------------------------------------
/**
    @class nSignalBindingPython
    @ingroup NebulaSignals

    Binding of a signal with a Python object+method.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "signals/nsignalbinding.h"
#include "python/npythonserver.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nSignalBindingPython : public nSignalBinding
{
public:
    /// constructor
    nSignalBindingPython( PyObject * obj, const nString & str, int priority, const nString & proto );
    /// destructor
    virtual ~nSignalBindingPython();

    /** @name Invocation
        Methods for invoking a signal binding. */
    //@{
    /// Invocation used from scripting side
    virtual bool Invoke(nCmd * cmd);
    /// Invocation used from native side
    virtual bool Invoke(va_list args);
    //@}

    /// Return prototype string
    virtual const char * GetProtoDef() const;

    /// Return true if the binding is valid
    virtual bool IsValid() const;

    /// find a python binding with a PyObject receiver
    static nSignalBindingPython * FindBinding( const PyObject * const obj );

private:
    static nArray<nSignalBindingPython*> pythonBindings;

    /// create a ref to the python method and call to it
    void CallPythonMethod( nCmd * const cmd );

    PyObject * pyRefObject;
    nString protoDef;
    nString method;
};

//------------------------------------------------------------------------------
#endif//NSIGNALBINDINGPYTHON_H
