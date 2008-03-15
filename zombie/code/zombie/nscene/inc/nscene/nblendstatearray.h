#ifndef N_BLENDSTATEARRAY_H
#define N_BLENDSTATEARRAY_H
//------------------------------------------------------------------------------
/**
    @class nBlendStateArray
    @ingroup NebulaAnimationSystem

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/nblendstate.h"
#include "util/narray.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
class nBlendStateArray
{
public:
    /// constructor
    nBlendStateArray();
    /// destructor
    ~nBlendStateArray();

    /// begin adding anim states
    void Begin(int num);
    /// set a new anim state
    void SetState(int index, const nBlendState& state);
    /// finish adding anim states
    void End();
    /// append a new anim state
    void AddState(const nBlendState& state);
    /// get number of animation states
    int GetNumStates() const;
    /// get an anim state object at given index
    nBlendState& GetStateAt(int index) const;
    /// find state by name
    nBlendState* FindState(const nString& n) const;
    /// find a state index by name
    int FindStateIndex(const nString& n) const;

    /// set number of curves
    void SetNumCurves( int numCurves );
    /// get number of curves
    int GetNumCurves() const;

    /// remove state
    void RemoveState(int stateIndex);

private:
    nArray<nBlendState> stateArray;
    int numCurves; /// maximum number of curves of the different animations
};

//------------------------------------------------------------------------------
/**
*/
inline
nBlendStateArray::nBlendStateArray() :
    stateArray(0, 0),
    numCurves(0)
{
    // empty
}
//------------------------------------------------------------------------------
/**
*/
inline
nBlendStateArray::~nBlendStateArray()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Begin adding states to the blend state array.
*/
inline
void
nBlendStateArray::Begin(int num)
{
    this->stateArray.SetFixedSize(num);
}

//------------------------------------------------------------------------------
/**
    Assign state to blend state array
*/
inline
void
nBlendStateArray::SetState(int index, const nBlendState& state)
{
    this->stateArray[index] = state;
}

//------------------------------------------------------------------------------
/**
    End adding states to the blend state array.
*/
inline
void
nBlendStateArray::End()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendStateArray::AddState(const nBlendState& state)
{
    // copy the array into a temporary array
    nArray<nBlendState> prevStates;
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
nBlendStateArray::GetNumStates() const
{
    return this->stateArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nBlendState&
nBlendStateArray::GetStateAt(int index) const
{
    return this->stateArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
nBlendState*
nBlendStateArray::FindState(const nString& n) const
{
    int i;
    int num = this->stateArray.Size();
    for (i = 0; i < num; i++)
    {
        nBlendState& state = this->stateArray[i];
        if (state.GetName() == n)
        {
            return &state;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Finds a state index by name. Returns -1 if not found.
*/
inline
int
nBlendStateArray::FindStateIndex(const nString& n) const
{
    int i;
    int num = this->stateArray.Size();
    for (i = 0; i < num; i++)
    {
        nBlendState& state = this->stateArray[i];
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
nBlendStateArray::RemoveState( int stateIndex)
{
    if( this->stateArray.Size() > stateIndex)
    {
        //remove file
        nString pathName = this->stateArray[stateIndex].GetAnimFile();
        nFileServer2* fileServer =  nFileServer2::Instance();
        if(fileServer->FileExists( pathName.Get() ))
        {
            fileServer->DeleteFile( pathName.Get() );
        }

        this->stateArray.Erase(stateIndex);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nBlendStateArray::SetNumCurves( int numCurves )
{
    if(numCurves > this->numCurves )
    {
        this->numCurves = numCurves;
    }
}
//------------------------------------------------------------------------------
/**
*/
inline
int
nBlendStateArray::GetNumCurves() const 
{
    return this->numCurves;
}

//------------------------------------------------------------------------------
#endif
