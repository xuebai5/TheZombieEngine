#ifndef NC_DICTIONARYCLASS_H
#define NC_DICTIONARYCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncDictionaryClass
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Entity component class to hold the entity data dictionary.
    TODO: For now it has no members, but it could be used to determine
    which variables are required to be created in the dictionary object.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "variable/nvariableserver.h"

//------------------------------------------------------------------------------
class ncDictionaryClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncDictionaryClass,nComponentClass);

public:
    /// constructor
    ncDictionaryClass();
    /// destructor
    ~ncDictionaryClass();
    /// object persistence
    bool SaveCmds(nPersistServer * ps);
    /// get variable context (used to init instances)
    nVariableContext& VariableContext();

    /// check if variable is valid
    bool IsVariableValid(const char *);
    /// set bool variable
    void SetBoolVariable(const char *, bool);
    /// get bool variable
    bool GetBoolVariable(const char *);
    /// set integer variable
    void SetIntVariable(const char *, int);
    /// get integer variable
    int GetIntVariable(const char *);
    /// set float variable
    void SetFloatVariable(const char *, float);
    /// get float variable
    float GetFloatVariable(const char *);
    /// set vector variable
    void SetVectorVariable(const char *, vector4);
    /// get vector variable
    vector4 GetVectorVariable(const char *);
    /// set string variable
    void SetStringVariable(const char *, const char *);
    /// get string variable
    const char * GetStringVariable(const char *);

private:
    nVariableContext varContext;    ///< default variable values for runtime entities

};

//------------------------------------------------------------------------------
/**
*/
inline
nVariableContext&
ncDictionaryClass::VariableContext()
{
    return this->varContext;
}

//------------------------------------------------------------------------------
#endif
