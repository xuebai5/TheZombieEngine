#ifndef N_ANIMSTATEARRAY_H
#define N_ANIMSTATEARRAY_H
//------------------------------------------------------------------------------
/**
    @class nAnimStateArray
    @ingroup Anim2

    @brief An anim state array holds several mutually exclusive nAnimState
    objects and allows switching between them.
    
    (C) 2003 RadonLabs GmbH
*/
#include "nanimation/nanimstate.h"
#include "util/narray.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
class nAnimStateArray
{
public:
    /// constructor
    nAnimStateArray();
    /// destructor
    ~nAnimStateArray();
    /// begin adding anim states
    void Begin(int num);
    /// set a new anim state
    void SetState(int index, const nAnimState& state);
    /// finish adding anim states
    void End();
    /// append a new anim state
    void AddState(const nAnimState& state);
    /// get number of animation states
    int GetNumStates() const;
    /// get an anim state object at given index
    nAnimState& GetStateAt(int index) const;
    /// find state by name
    nAnimState* FindState(const nString& n) const;
    /// find a state index by name
    int FindStateIndex(const nString& n) const;

    /// remove state
    void RemoveState(int stateIndex);

    /// set state bounding box
    void SetStateBoundingBox(int stateIndex, vector3& center, vector3& extents);
    /// set state bounding box
    void GetStateBoundingBox(int stateIndex, bbox3& box);

    ///clear array
    bool ClearArray();

private:
    nArray<nAnimState> stateArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAnimStateArray::nAnimStateArray() :
    stateArray(0, 0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimStateArray::~nAnimStateArray()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Begin adding states to the anim state array.

    @param  num     number of anim states
*/
inline
void
nAnimStateArray::Begin(int num)
{
    this->stateArray.SetFixedSize(num);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimStateArray::SetState(int index, const nAnimState& state)
{
    this->stateArray[index] = state;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimStateArray::AddState(const nAnimState& state)
{
    // copy the array into a temporary array
    nArray<nAnimState> prevStates;
    for (int i = 0; i < this->GetNumStates(); i++)
    {
        prevStates.Append(this->GetStateAt(i));
    }

    // grow the local array and copy the temporary copy into it
    this->Begin(this->GetNumStates() + 1);
    for (int i = 0; i < prevStates.Size(); i++)
    {
        this->SetState(i, prevStates[i]);
    }
    this->SetState(this->GetNumStates() - 1, state);
    this->End();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimStateArray::GetNumStates() const
{
    return this->stateArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimState&
nAnimStateArray::GetStateAt(int index) const
{
    return this->stateArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimState*
nAnimStateArray::FindState(const nString& n) const
{
    int i;
    int num = this->stateArray.Size();
    for (i = 0; i < num; i++)
    {
        nAnimState& state = this->stateArray[i];
        if (state.GetName() == n)
        {
            return &state;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Finds a state index by name. Returns -1 if state not found.
*/
inline
int
nAnimStateArray::FindStateIndex(const nString& n) const
{
    int i;
    int num = this->stateArray.Size();
    for (i = 0; i < num; i++)
    {
        nAnimState& state = this->stateArray[i];
        if (state.GetName() == n)
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nAnimStateArray::SetStateBoundingBox(int stateIndex, vector3& center, vector3& extents)
{
    this->stateArray[stateIndex].SetBoundingBox(center, extents);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nAnimStateArray::GetStateBoundingBox(int stateIndex, bbox3 &box)
{
    this->stateArray[stateIndex].GetBoundingBox(box);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimStateArray::RemoveState(int stateIndex)
{
    if (this->stateArray.Size() > stateIndex)
    {
        //remove file
        nString pathName = this->stateArray[stateIndex].GetAnimFile();
        nFileServer2* fileServer =  nFileServer2::Instance();
        if (fileServer->FileExists(pathName))
        {
            fileServer->DeleteFile(pathName);
        }

        this->stateArray.Erase(stateIndex);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAnimStateArray::ClearArray()
{
    this->stateArray.Clear();
    return true;
}

//------------------------------------------------------------------------------
#endif
