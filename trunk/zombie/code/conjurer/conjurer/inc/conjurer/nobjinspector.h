#ifndef N_OBJINSPECTOR_H
#define N_OBJINSPECTOR_H
//-------------------------------------------------------------------
/**
    @file nobjinspector.h
    @class nObjInspector
    @ingroup NebulaConjurerEditor

    @author Javier Ferrer Gomez

    @brief Nebula object inspector with scripting interface

    (C) 2005 Conjurer Services, S.A.
*/
#include "util/nlist.h"
#include "kernel/ncmdprotonativecpp.h"

class nPersistServer;

class nObjInspector: public nRoot
{
public:   
    /// Constructor
    nObjInspector();
    /// Delete the object
    ~nObjInspector();

    /// Extract to an array the commands that make the object state
    void InitCmdsState(nObject *);
    /// Extract to an array all the commands of the class
    void InitCmdsClass(const char *);
    /// Extract to an array all the signals of the class
    void InitSignalsClass(const char *);

    /// Return the number of commands
    int GetCmdsCount();
    /// Return the number of in arguments for a command
    int GetInArgsCount(int);
    /// Return the number of out arguments for a command
    int GetOutArgsCount(int);
    
    /// Return the protodef of a command
    const char * GetCmdProtoName(int);
    /// Return the name of a command
    const char * GetCmdName(int);

    /** 
    * Return command's argument type <br>
    * Return values:
    * <ul><li>1: Integer</li>
    * <li>2: Float</li>
    * <li>3: String</li>
    * <li>4: Boolean</li>
    * <li>5: Object</li></ul>
    */
    int GetType(int, int);

    /// Return the int argument value for a command
    int GetI(int, int);
    /// Return the float argument value for a command
    float GetF(int, int);
    /// Return the string argument value for a command
    const char * GetS(int, int);
    /// Return the bool argument value for a command
    bool GetB(int, int);
    /// Return the object argument value for a command
    void * GetO(int, int);

    /// Get the work mode. 0 = STATE, 1 = CLASS
    int GetState();

    /// Get the proper name of the specified class
    const char * GetClassProperName(const char *);

protected:

    nArray<nCmd*> SelectCommandList(int mode);

    enum {
        STATE=0,    // Works over the state commands
        CLASS,      // Works over the class commands
        SIGNALS    // Works over the signals commands
    };

    enum {
        INTEGER=1,
        FLOAT,
        STRING,
        BOOLEAN,
        OBJECT,
        LIST,
        FLOAT4,
        MATRIX44
    };

    static int           uniqueId;          // Allow multiple instances

    int                  mode;                 // Select the work mode

    nArray<nCmd*>        stateList;
    nArray<nCmd*>        classList;
    nArray<nCmd*>        signalList;

    nPersistServer*      ps;
    
};

inline
int
nObjInspector::GetState()
{
    return this->mode;
}

// Function for select the work list
inline
nArray<nCmd*>
nObjInspector::SelectCommandList(int mode) 
{
    if (mode == STATE)
    {
        return this->stateList;
    }
    else if (mode == CLASS)
    {
        return this->classList;
    }
    else 
    {
        return this->signalList;
    }
}
#endif