#ifndef N_LOGOUTPUTOBJECT_H
#define N_LOGOUTPUTOBJECT_H
//------------------------------------------------------------------------------
/**
    @class LogOutputObject
    @ingroup Kernel

    Interface for logging to an output object.

    An object that implements this interface can be given to the log server to
    receive all the generated logs.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
class nLogOutputObject
{
public:
    /// constructor
    nLogOutputObject();

    /// enable the log output object
    void SetEnabled(bool enabled);
    /// disable the log output object
    bool GetEnabled();

    virtual void AddLine( const char* log ) = 0;

protected:
    bool enabled;

};

//------------------------------------------------------------------------------
inline
nLogOutputObject::nLogOutputObject() :
    enabled(true)
{
    // empty
}

//------------------------------------------------------------------------------
inline
void 
nLogOutputObject::SetEnabled(bool enabled)
{
    this->enabled = enabled;
}

//------------------------------------------------------------------------------
inline
bool 
nLogOutputObject::GetEnabled()
{
    return this->enabled;
}

//------------------------------------------------------------------------------
#endif
