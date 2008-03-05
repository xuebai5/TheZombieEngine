#ifndef N_NBLENDTARGETKEY_H
#define N_NBLENDTARGETKEY_H
//------------------------------------------------------------------------------
/**
    @class nnBlendTargetKey
    @ingroup NebulaAnimationSystem
    
    @brief
    
    (C) 2005 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
class nBlendTargetKey
{
public:
    /// default constructor
    nBlendTargetKey();
    /// constructor
    nBlendTargetKey( int kI, int cI, int tI);
    /// destructor
    ~nBlendTargetKey();
    /// get time
    int GetKeyIndex();
    /// set time
    void SetKeyIndex(int keyIndex);
    /// get curve index
    int GetCurveIndex();
    /// set curve index
    void SetCurveIndex(int curveIndex);
    /// get target index
    int GetTargetIndex();
    /// set target index
    void SetTargetIndex(int targetIndex);

private:
    int keyIndex;
    int curveIndex;
    int targetIndex;
};

//------------------------------------------------------------------------------
/**
*/
inline
nBlendTargetKey::nBlendTargetKey() :
    keyIndex(0),
    curveIndex(-1),
    targetIndex(-1)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nBlendTargetKey::nBlendTargetKey( int kI, int cI, int tI) :
    keyIndex(kI),
    curveIndex(cI),
    targetIndex(tI)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nBlendTargetKey::~nBlendTargetKey()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBlendTargetKey::GetKeyIndex()
{
    return this->keyIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendTargetKey::SetKeyIndex(int kI)
{
    this->keyIndex = kI;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBlendTargetKey::GetCurveIndex()
{
    return this->curveIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendTargetKey::SetCurveIndex(int curveIndex)
{
    this->curveIndex = curveIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBlendTargetKey::GetTargetIndex()
{
    return this->targetIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendTargetKey::SetTargetIndex(int targetIndex)
{
    this->targetIndex = targetIndex;
}

//------------------------------------------------------------------------------
#endif
