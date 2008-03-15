#ifndef N_CMDOPERATION_H
#define N_CMDOPERATION_H

//------------------------------------------------------------------------------
/**
    @class nCmdOperation
    @ingroup NebulaTriggerSystem

    Class that executes a command of an object.

    The class stores the id of the object (path for nRoot, id for nEntityObject
    and class name for nEntityClass). Then the object pointer is resolved when
    going to execute the command.

    nObject is not supported. Neither commands with object arguments. If you
    need an object argument, define an alternative command that receives the
    object id and then resolves the object pointer on execution time, like this
    class does.

    (C) 2006 Conjurer Services, S.A.
*/

#include "ntrigger/noperation.h"

//------------------------------------------------------------------------------
class nCmdOperation : public nOperation
{
public:
    /// Default constructor
    nCmdOperation();
    /// Destructor
    ~nCmdOperation();
    /// Get operation type
    OperationType GetType() const;
    /// Execute the operation
    void Execute( nEntityObject* trigger, nEntityObject* entity );

    /// Get operation type as string
    const char* GetTypeLabel () const;
    /// Set the object and command to be executed
    void SetObjectCommand(nObject*, const char*);
    /// Set the object and command to be executed
    void SetNRootCommand(const char*, const char*, const char*);
    /// Set the object and command to be executed
    void SetEntityObjectCommand(nEntityObjectId, const char*, const char*);
    /// Set the object and command to be executed
    void SetEntityClassCommand(const char*, const char*);
    /// Get the object onto which the command will be executed
    nObject* GetTargetObject () const;
    /// Get the command to be executed
    const char* GetTargetCommand () const;
    /// Set a boolean argument
    void SetBoolArg(int, bool);
    /// Get a boolean argument
    bool GetBoolArg(int) const;
    /// Set a float argument
    void SetFloatArg(int, float);
    /// Get a float argument
    float GetFloatArg(int) const;
    /// Set an integer argument
    void SetIntArg(int, int);
    /// Get an integer argument
    int GetIntArg(int) const;
    /// Set a string argument
    void SetStringArg(int, const char*);
    /// Get a string argument
    const char* GetStringArg(int) const;

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

private:
    /// Init the action command arguments to default values
    void InitArgs( const char* className );
    /// Validate that an argument index and type are valid
    bool ValidateArg( int index, nArg::Type type ) const;
    /// Fill the values of the dynamic arguments in the action arguments list
    void FillDynamicArgs( nEntityObject* entity );

    /// Id of the object onto which the command will be executed
    union
    {
        nEntityObjectId entityId;
        const char* entityClassName;
        const char* nrootPath;
    } objectId;
    /// Buffer to store the object id when the id is a string (in this case objectId points to this string)
    nString objectIdStr;
    /// Type of the object onto which the command will be executed
    enum
    {
        UNDEFINED,
        NROOT,
        ENTITY_OBJECT,
        ENTITY_CLASS
    } objectType;
    /// Command to execute
    nString cmdName;
    /// Command arguments
    nArray<nArg> cmdArgs;
};

//------------------------------------------------------------------------------
#endif // N_CMDOPERATION_H
