#ifndef N_LAYER_H
#define N_LAYER_H

#include "kernel/nroot.h"
#include "nphysics/nphysicsserver.h"

//------------------------------------------------------------------------------
/**
    @class nLayer
    @ingroup Framework

    @brief nLayer groups elements.
    
    (C) 2005  Conjurer Services, S.A.

    Author: Miquel Angel Rujula
*/

#include "util/nstring.h"
#include "entity/nentityobject.h"
#include "util/nstream.h"
#include "util/nbase64.h"

#ifdef NGAME
    #error This header only is for a conjurer
#endif

class nLayer : public nRoot
{
public:

    /// constructor 1
    nLayer();
    /// constructor 2
    nLayer(const nString &name);
    /// destructor
    ~nLayer();

    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);

    /// lock the layer
    void Lock();
    /// unlock the layer
    void Unlock();
    /// says if the layer is locked
    bool IsLocked();
    /// set active flag
    void SetActive(bool flag);
    /// get active flag
    bool IsActive() const;
    /// set the layer's name
    void SetLayerName(const nString &name);
    /// get the layer's name
    const char *GetLayerName();
    /// set layer's id
    void SetId(int id);
    /// get layer's id
    int GetId();
    /// set locked flag
    void SetLocked(bool flag);
    /// set layer user key
    void SetLayerKey(nEntityObjectId);
    /// get layer user key
    nEntityObjectId GetLayerKey();
    /// set layer password
    void SetPassword(const char *);
    /// set layer password
    void SetPassword(const char *, bool encrypt);
    /// return true if password is set
    bool HasPassword() const;
    /// clear the password (set it to 0)
    void ClearPassword();
    /// return true if supplied password is OK
    bool CheckPassword(const char * password);
    /// return the given password string in encrypted form
    nString EncryptPasswordString(const char * password);
    /// return the password
    const char * GetPassword();

    // Signals
    NSIGNAL_DECLARE('YLLK', void, LayerLocked, 1, (bool), 0, ());

private:

    // it can be modified or not
    bool m_locked;
    // it's active or not (visible/invisible)
    bool m_active;
    // layer name
    nString m_name;
    // layer id
    int m_id;
    // layer password(in Base64)
    nString m_password;


    // user key (for persistence only)
    nEntityObjectId layerKey;

};

//------------------------------------------------------------------------------
/**
    Locks the layer

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
inline
void
nLayer::Lock()
{
    this->SetLocked(true);
}

//------------------------------------------------------------------------------
/**
    Unlocks the layer

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
inline
void
nLayer::Unlock()
{
    this->SetLocked(false);
}

//------------------------------------------------------------------------------
/**
    Says if the layer is locked

    history:
        - 18-Feb-2005   Miquel Angel Rujula created
*/
inline
bool 
nLayer::IsLocked()
{
    return this->m_locked;
}

//------------------------------------------------------------------------------
/**
    Activates the layer

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
inline
void
nLayer::SetActive(bool flag)
{
    this->m_active = flag;
    nPhysicsServer::Instance()->LayersUpdate();
}

//------------------------------------------------------------------------------
/**
    Says if the layer is active or not

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
inline
bool
nLayer::IsActive() const
{
    return this->m_active;
}

//-----------------------------------------------------------------------------
/**
    Set the layer's id

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
inline
void 
nLayer::SetId(int id)
{
    this->m_id = id;
}

//-----------------------------------------------------------------------------
/**
    Get the layer's id

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
inline
int
nLayer::GetId()
{
    return this->m_id;
}

//-----------------------------------------------------------------------------
/**
    Sets the layer's name

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
inline
void 
nLayer::SetLayerName(const nString &name)
{
    this->m_name = name;
}

//-----------------------------------------------------------------------------
/**
    Gets the layer's name

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
inline
const char *
nLayer::GetLayerName()
{
    return this->m_name.Get();
}

//-----------------------------------------------------------------------------
/**
*/
inline
void 
nLayer::SetLayerKey(nEntityObjectId key)
{
    this->layerKey = key;
}

//-----------------------------------------------------------------------------
/**
*/
inline
nEntityObjectId
nLayer::GetLayerKey()
{
    return this->layerKey;
}

//------------------------------------------------------------------------------
/**
    Sets the locked value
*/
inline
void
nLayer::SetLocked(bool flag)
{
    this->m_locked = flag;
    this->SignalLayerLocked(this, flag);
}

//------------------------------------------------------------------------------
/**
    Sets the password
*/
inline
void
nLayer::SetPassword(const char * password)
{
    this->SetPassword(password, true);
}

//------------------------------------------------------------------------------
/**
    Sets the password
*/
inline
void
nLayer::SetPassword(const char * password, bool encrypt)
{
    if (password == 0)
    {
        this->m_password = 0;
    }
    else
    {
        if (encrypt)
        {
            this->m_password = this->EncryptPasswordString(password);
        }
        else
        {
            this->m_password = password;
        }
    }
}


//------------------------------------------------------------------------------
/**
    Returns true if this layer has a password set
*/
inline
bool
nLayer::HasPassword() const
{
    return this->m_password != 0;
}

//------------------------------------------------------------------------------
/**
    Clear the password (set it to 0)
*/
inline
void
nLayer::ClearPassword()
{
    this->SetPassword(0);
}

//------------------------------------------------------------------------------
/**
    Returns true if the password supplied is OK
*/
inline
bool
nLayer::CheckPassword(const char * password)
{
    if ( this->HasPassword() )
    {
        return this->m_password == this->EncryptPasswordString(password);
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------
/**
    Returns true if the password supplied is OK
*/
inline
nString
nLayer::EncryptPasswordString(const char * password)
{
    if (password == 0)
    {
        return 0;
    }
    int size = static_cast<int>( strlen(password) );
    char * data;
    nBase64::Base256To64( size, password, data );
    nString encryptedPassword( data );
    n_free( data );
    return encryptedPassword; 
}

//------------------------------------------------------------------------------
/**
    Returns the password
*/
inline
const char *
nLayer::GetPassword()
{
    return this->m_password.Get();
}


//-----------------------------------------------------------------------------
#endif // N_LAYER_MANAGER

