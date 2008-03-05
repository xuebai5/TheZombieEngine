#ifndef NC_DICTIONARY_H
#define NC_DICTIONARY_H
//------------------------------------------------------------------------------
/**
    @class ncDictionary
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Entity component to hold a variable context as a data dictionary.
    It serves as a general-purpose data container.
    
    Additionally to regular variables, the dictionary components provides
    as well an interface to store loval variables. The usage is very similar, 
    but they are not persisted along with the rest of them. Instead, they are
    runtime containers of generic data. To improve the access to data in local
    variables, the setter methods return an integer index to the local variable
    that can be used to speed up the access when retrieving its value.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "variable/nvariablecontext.h"

//------------------------------------------------------------------------------
class ncScene;

//------------------------------------------------------------------------------
class ncDictionary : public nComponentObject
{
    
    NCOMPONENT_DECLARE(ncDictionary, nComponentObject);

public:
    /// constructor
    ncDictionary();
    /// destructor
    virtual ~ncDictionary();
    /// component persistency
    bool SaveCmds(nPersistServer* ps);
    /// initialize component pointers from entity object
    void InitInstance(nObject::InitInstanceMsg initType);

    /// get variable context
    nVariableContext& VarContext();
    /// add a variable to the context
    void AddVariable(const nVariable& var);
    /// get a variable object by its handle
    nVariable* GetVariable(nVariable::Handle handle) const;

    /// appends a new var to localVarArray, returns the index
    int AddLocalVar(const nVariable& value);
    /// returns local variable at given index
    nVariable& GetLocalVar(int index);
    /// clear local variables
    void ClearLocalVars();
    /// find local variable by variable handle
    nVariable* FindLocalVar(nVariable::Handle handle);
    /// find local variable by variable handle and return its index
    nVariable* FindLocalVar(nVariable::Handle handle, int& index);

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
    void SetVectorVariable(const char *, const vector4&);
    /// get vector variable
    const vector4& GetVectorVariable(const char *);
    /// set string variable
    void SetStringVariable(const char *, const char *);
    /// get string variable
    const char * GetStringVariable(const char *);

    /// set local integer variable
    int SetLocalIntVariable(const char *, int);
    /// get local integer variable by name
    int GetLocalIntVariable(const char *);
    /// set local float variable
    int SetLocalFloatVariable(const char *, float);
    /// get local float variable by name
    float GetLocalFloatVariable(const char *);
    /// set local vector variable
    int SetLocalVectorVariable(const char *, const vector4&);
    /// get local vector variable by name
    const vector4& GetLocalVectorVariable(const char *);
    /// set local string variable
    int SetLocalStringVariable(const char *, const char *);
    /// get local string variable by name
    const char * GetLocalStringVariable(const char *);
    /// set local object variable
    int SetLocalObjectVariable(const char *, nObject*);
    /// get local string variable by name
    nObject* GetLocalObjectVariable(const char *);

private:
    nVariableContext varContext;
    nArray<nVariable> localVarArray;

    void UpdateRenderContext(); ///< temporary member to synch with render context
    ncScene *sceneComp;         ///< temporary member to synch with render context

};

//------------------------------------------------------------------------------
/**
*/
inline
nVariableContext&
ncDictionary::VarContext()
{
    return this->varContext;
}

//------------------------------------------------------------------------------
/**
    Add a variable to the dictionary. If a variable with the same handle
    already exists, copy the value from the new variable.
*/
inline
void
ncDictionary::AddVariable(const nVariable& var)
{
    nVariable* currentVar = this->varContext.GetVariable(var.GetHandle());
    if (currentVar)
    {
        *currentVar = var;
    }
    else
    {
        this->varContext.AddVariable(var);
    }
}

//------------------------------------------------------------------------------
/**
    @param  h       the variable handle
    @return         pointer to variable object, or 0 if variable doesn't exist
                    in this context
*/
inline
nVariable*
ncDictionary::GetVariable(nVariable::Handle h) const
{
    return this->varContext.GetVariable(h);
}

//------------------------------------------------------------------------------
/**    
*/
inline
int
ncDictionary::AddLocalVar(const nVariable& value)
{
    int index;
    nVariable* var = this->FindLocalVar(  value.GetHandle() , index );
    if ( var )
    {
        *var = value;
    } else
    {
        index =  this->localVarArray.Size();
        this->localVarArray.Append(value);
    }
    return index;

}

//------------------------------------------------------------------------------
/**    
*/
inline
nVariable&
ncDictionary::GetLocalVar(int index)
{
    return this->localVarArray.At(index);
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable*
ncDictionary::FindLocalVar(nVariable::Handle handle, int& index)
{
    index = -1;
    int i;
    int num = this->localVarArray.Size();
    for (i = 0; i < num; i++)
    {
        if (this->localVarArray[i].GetHandle() == handle)
        {
            index = i;
            return &(this->localVarArray[i]);
        }
    }
    // fallthrough: not found
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable*
ncDictionary::FindLocalVar(nVariable::Handle handle)
{
    int index;
    return this->FindLocalVar(handle, index);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncDictionary::ClearLocalVars()
{
    this->localVarArray.Clear();
}

//------------------------------------------------------------------------------
#endif
