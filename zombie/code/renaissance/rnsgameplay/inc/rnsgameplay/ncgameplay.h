#ifndef NCGAMEPLAY_H
#define NCGAMEPLAY_H

//------------------------------------------------------------------------------
/**
    @class ncGameplay
    @ingroup Entities

    @brief Component Object that represent the game play.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"

//------------------------------------------------------------------------------
#ifndef NGAME
class nGfxServer2;
#endif//!NGAME

class nGPBasicAction;
class nPersistServer;
class ncAIMovEngine;

//------------------------------------------------------------------------------
class ncGameplay : public nComponentObject
{

    NCOMPONENT_DECLARE(ncGameplay,nComponentObject);

public:

	enum
	{
		BA_FOREGROUND = 0x01,
		BA_BACKGROUND = 0x02,
		BA_CLEARQUEUE = 0x04,
	};

    /// constructor
    ncGameplay();
    /// destructor
    virtual ~ncGameplay();

    /// Component persistency
    bool SaveCmds (nPersistServer* ps);

    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// runs the gameplay logic 
    virtual void Run ( const float deltaTime );
    /// check when the entity is already valid
    virtual bool IsValid() const;

    /// set the entity valid value
    void SetValid (bool value = true);

    /// Set a basic action for execution
    void SetBasicAction (nGPBasicAction* basicAction, UINT flags = BA_FOREGROUND | BA_CLEARQUEUE);
    /// Abort the current running basic action
    void AbortCurrentAction (UINT flags = BA_FOREGROUND);
    /// Notify the FSM when the action finishes
    void SignalActionDone ();
    /// check for one action in the entity
    bool HasAction (const nString& actionName, UINT flags = BA_FOREGROUND) const;
	/// says if there is a basic action running at this moment
	bool IsRunningAction() const;

    /// Gameplay properties for entities
    // -- General purpose attributes
    /// Sets/gets the name
    void SetName(const nString&);
    const nString& GetName() const;
    /// Sets/gets the group
    void SetGroup(const nString&);
    const nString& GetGroup() const;
	/// Add a basic action to the queue
    void AddBasicAction(const char*, int, nArg*, bool);
    void PopBasicAction();

    // -- Interface to send actions over the net
    /// Begin a new action to be sent
    void BeginAction(const char*) const;
    /// Append an integer argument to the action to be sent
    void AddIntArg(int) const;
    /// Append a float argument to the action to be sent
    void AddFloatArg(float) const;
    /// Append a string argument to the action to be sent
    void AddStringArg(const char*) const;
    /// Append a boolean argument to the action to be sent
	void AddBoolArg(bool) const;
    /// Send the action started with the last BeginAction
    void EndAction() const;

#ifndef NGAME
    /// debug draw of entity
    virtual void DebugDraw( nGfxServer2 * const gfxServer );
#endif//!NGAME

private:

	struct actionsSpec 
	{
		char* name;
		int params;
		nArg* args;
		bool foreground;
	};

	void ClearQueue();
	void GetQueuedAction();
	actionsSpec* GetTopQueue() const;

    /// Destroyes the basic action associated to the entity
    void DestroyBasicAction (UINT flags = BA_FOREGROUND);

    /// Basic actions
    nGPBasicAction* foregroundAction;
	nGPBasicAction* backgroundAction;
	nArray<actionsSpec*> queueActions;
    /// Says if is valid
    bool valid;

    /// Gameplay properties
    // -- general purpose properties
    nString name;    
    nString group;

    // Other entity components
    ncAIMovEngine* movEngine;

};

//------------------------------------------------------------------------------
/**
    IsValid
*/
inline
bool
ncGameplay::IsValid() const
{
    return this->valid;
}

//------------------------------------------------------------------------------
/**
    SetValid
*/
inline
void
ncGameplay::SetValid (bool value)
{
    this->valid = value;
}

//------------------------------------------------------------------------------
/**
    SetName
*/
inline
void
ncGameplay::SetName (const nString& name)
{
    this->name = name;
}

//------------------------------------------------------------------------------
/**
    GetName
*/
inline
const nString&
ncGameplay::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
    SetGroup
*/
inline
void
ncGameplay::SetGroup (const nString& group)
{
    this->group = group;
}
//------------------------------------------------------------------------------
/**
    GetGroup
*/
inline
const nString&
ncGameplay::GetGroup() const
{
    return this->group;
}

//------------------------------------------------------------------------------
/**
	IsRunningAction
*/
inline
bool
ncGameplay::IsRunningAction() const
{
	return bool(foregroundAction != 0);
}

//------------------------------------------------------------------------------
#endif//NCGAMEPLAY_H
