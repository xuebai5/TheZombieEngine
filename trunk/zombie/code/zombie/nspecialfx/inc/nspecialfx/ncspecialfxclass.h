#ifndef NC_SPECIALFXCLASS_H
#define NC_SPECIALFXCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncSpecialFXClass
    @ingroup SpecialFX
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief ...

    (C) 2006 Conjurer Services, S.A.
*/
#include "entity/nentity.h"


//------------------------------------------------------------------------------
class ncSpecialFXClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncSpecialFXClass,nComponentClass);

public:
    enum FxType
    {
        FX_DECAL = 0,
        FX_SPAWN,
        
        NUMTYPES,
        INVALIDTYPE,
    };

    /// constructor
    ncSpecialFXClass();
    /// destructor
    ~ncSpecialFXClass();

    /// save component state
    bool SaveCmds(nPersistServer* ps);

    //Get type
    FxType GetType();

    /// set effect lifetime
    void SetLifeTime(float);
    /// get effect lifetime
    float GetLifeTime();
    /// set 
    void SetUnique(bool);
    /// get 
    bool GetUnique();
    /// ...
   // void Set...(...);

    /// set type
    void SetFxType(const char *);
    /// get type
    const char* GetFxType() const;

#ifndef NGAME
    /// set type
    void SetFxTypeEdition(int);
    /// get type
    int GetFxTypeEdition() const;
#endif

    /// set global
    void SetFXGlobal(bool);
    /// get global
    bool GetFXGlobal() const;

    /// set start random
    void SetStartRandom(bool);
    /// get start random
    bool GetStartRandom();

    /// convert type string to enum
    static FxType StringToType(const char* str);
    /// convert type enum to string
    static const char* TypeToString(FxType t);

protected:
    float lifetime;
    bool unique;
    FxType type;
    bool global;
    bool randomTimeOffset;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSpecialFXClass::SetLifeTime(float life)
{
    this->lifetime = life;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ncSpecialFXClass::GetLifeTime()
{
    return this->lifetime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSpecialFXClass::SetUnique(bool value)
{
    this->unique = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncSpecialFXClass::GetUnique()
{
    return this->unique;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSpecialFXClass::SetFxType(const char* type)
{
    this->type = StringToType(type);
}

//------------------------------------------------------------------------------
/**
*/
inline
ncSpecialFXClass::FxType
ncSpecialFXClass::GetType()
{
    return this->type;
}



//------------------------------------------------------------------------------
/**
*/
inline
const char*
ncSpecialFXClass::GetFxType() const
{
    return TypeToString(this->type);
}

//------------------------------------------------------------------------------
/**
*/
#ifndef NGAME
inline
void
ncSpecialFXClass::SetFxTypeEdition(int type)
{
    this->type = ( type >= 0 && type < NUMTYPES)  ? static_cast<FxType>(type) : INVALIDTYPE;
}
#endif

//------------------------------------------------------------------------------
/**
*/
#ifndef NGAME
inline
int
ncSpecialFXClass::GetFxTypeEdition() const
{
    return static_cast<int>(this->type);
}
#endif

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSpecialFXClass::SetFXGlobal(bool is)
{
    this->global = is;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
ncSpecialFXClass::GetFXGlobal() const
{
    return this->global;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
ncSpecialFXClass::SetStartRandom( bool random )
{
    this->randomTimeOffset = random;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
ncSpecialFXClass::GetStartRandom()
{
    return this->randomTimeOffset;
}
//------------------------------------------------------------------------------
#endif /*NC_SPECIALFXCLASS_H*/
