#ifndef N_ACTIONDESC_H
#define N_ACTIONDESC_H

//------------------------------------------------------------------------------
/**
    @class nActionDesc
    @ingroup NebulaFSMSystem

    Class to hold info about which action to create and with what parameters.

    The order and type of the action arguments are unknown by the FSM, they are
    set externally (by the editor/script) and used by the action manager.

    (C) 2006 Conjurer Services, S.A.
*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "entity/nentityobject.h"
#include "util/nstream.h" // Kept for backwards compatibility

//------------------------------------------------------------------------------
class nActionDesc : public nRoot
{
public:
    // Default constructor
    nActionDesc();
    /// Tell the action manager to apply this action to the given entity
    void ApplyAction (nEntityObject* entity, bool foreground);

    /// Set the action class of this action description
    void SetActionClass(const char*);
    /// Get the action class of this action description
    const char* GetActionClass () const;
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
    /// Set an entity argument
    void SetEntityArg(int, nEntityObject*);
    /// Set an entity argument by its id (used for persistence)
    void SetEntityArgById(int, nEntityObjectId);
    /// Get an entity argument
    nEntityObject* GetEntityArg(int) const;
    /// Get an entity argument as an entity id (used for the editor)
    nEntityObjectId GetEntityArgAsId(int) const;
    /// Set a string argument
    void SetStringArg(int, const char*);
    /// Get a string argument
    const char* GetStringArg(int) const;
    /// Set a 3D vector argument
    void SetVector3Arg(int, const vector3&);
    /// Get a 3D vector argument
    void GetVector3Arg(int, vector3&) const;

    /// Set the action type id of this action description (kept for backwards compatibility)
    void SetActionType(int);
    /// Set action arguments, given in base64 (kept for backwards compatibility)
    void SetArgsInBase64(const char*);

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

private:
    /// Action class
    nString actionClass;
    /// Arguments used when initializing the action
    nArray<nArg> actionArgs;
    /// Info about dynamic argument values (values that cannot be known or are unsafe to get at load time)
    class ValueInfo
    {
    public:
        /// Value type (not the argument type, but what the value means)
        enum ValueType
        {
            ENTITY_ID,
            ENTITY_NAME
        };

        /// Set the action argument index
        void SetArgIndex( int index );
        /// Get the action argument index
        int GetArgIndex() const;
        /// Get the type of this value
        ValueType GetValueType() const;
        /// Set a value as an entity id
        void SetEntityId( nEntityObjectId id );
        /// Get the value as an entity id
        nEntityObjectId GetEntityId() const;
        /// Set a value as an entity name
        void SetEntityName( const char* name );
        /// Get the value as an entity name
        const char* GetEntityName() const;
        /// Get the value as an entity
        nEntityObject* GetEntity() const;

    private:
        /// Argument index
        int argIndex;

        /// Value type
        ValueType valueType;

        /// Value
        union Value
        {
            nEntityObjectId entityId;
            const char* entityName;
        } value;

        /// Holder of a string value, used for easy and safe allocation/deallocation of strings
        nString strValue;
    };
    nArray<ValueInfo> dynamicArgs;

    /// Validate that an argument index and type are valid
    bool ValidateArg( int index, nArg::Type type ) const;
    /// Get the dynamic argument for an action argument index (create it if needed)
    ValueInfo& GetDynamicArg( int index );
    /// Get the dynamic argument for an action argument index or NULL if it doesn't exist
    ValueInfo* FindDynamicArg( int index ) const;
    /// Get and fill the values of the dynamic arguments in the action arguments list
    void FillDynamicArgs( nEntityObject* entity );

};

//-----------------------------------------------------------------------------
/**
    Get the action argument index
*/
inline
int
nActionDesc::ValueInfo::GetArgIndex() const
{
    return this->argIndex;
}

//-----------------------------------------------------------------------------
/**
    Get the type of this value
*/
inline
nActionDesc::ValueInfo::ValueType
nActionDesc::ValueInfo::GetValueType() const
{
    return this->valueType;
}

//-----------------------------------------------------------------------------
/**
    Get the value as an entity id
*/
inline
nEntityObjectId
nActionDesc::ValueInfo::GetEntityId() const
{
    n_assert( this->valueType == ENTITY_ID );
    return this->value.entityId;
}

//-----------------------------------------------------------------------------
/**
    Get the value as an entity name
*/
inline
const char*
nActionDesc::ValueInfo::GetEntityName() const
{
    n_assert( this->valueType == ENTITY_NAME );
    return this->value.entityName;
}

//------------------------------------------------------------------------------

#endif // N_ACTIONDESC_H
