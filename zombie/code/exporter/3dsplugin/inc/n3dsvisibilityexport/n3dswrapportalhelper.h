#ifndef N_3DS_PORTAL_HELPER_H
#define N_3DS_PORTAL_HELPER_H
#include "mathlib/vector.h"
//------------------------------------------------------------------------------
/**
    @class n3dsPortalDummy
    @ingroup n3dsMaxVisibility
    @brief Is a wrapper for maxscript portal dummy helper( maxscript class)

    (C) 2005 Conjurer Services, S.A. 
*/

#define PORTAL_HELPER_CLASS_ID Class_ID(0x52b9041a, 0x25af6af3)

class INode;
class IGameNode;


class n3dsWrapPortalHelper
{
public:
    /// default constructor
    n3dsWrapPortalHelper();
    /// default destructor
    ~n3dsWrapPortalHelper();
    /// extract info from max scene node
    bool ExtractFrom(INode* node);
    /// extract info from igame scen node
    bool ExtractFrom(IGameNode* node);
    /// get max node 1
    INode* GetCell1() const;
    /// get max node 2
    INode* GetCell2() const;
    /// get Box size
    const vector3& GetBoxSize() const;
    /// return if c1 is visible from c2
    bool GetC1ToC2Active() const;
    /// return if c1 is visible from c2
    bool GetC2ToC1Active() const;
    /// return the portal's deactivation distance 1
    float GetDeactDist1() const;
    /// return the portal's deactivation distance 2
    float GetDeactDist2() const;
private:
    INode* cell1;
    INode* cell2;
    bool c1ToC2Active;
    bool c2ToC1Active;
    vector3 boxSize;
    float deactDist1;
    float deactDist2;
};

//------------------------------------------------------------------------------
/**
*/
inline
n3dsWrapPortalHelper::n3dsWrapPortalHelper() : 
    cell1(0) , 
    cell2(0),
    c1ToC2Active(false),
    c2ToC1Active(false),
    deactDist1(FLT_MAX),
    deactDist2(FLT_MAX)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
n3dsWrapPortalHelper::~n3dsWrapPortalHelper()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
inline
INode* 
n3dsWrapPortalHelper::GetCell1() const
{
    return cell1;
}

//------------------------------------------------------------------------------
/**
*/
inline
INode* 
n3dsWrapPortalHelper::GetCell2() const
{
    return cell2;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
n3dsWrapPortalHelper::GetBoxSize() const
{
    return boxSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
n3dsWrapPortalHelper::GetC1ToC2Active() const
{
    return c1ToC2Active;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
n3dsWrapPortalHelper::GetC2ToC1Active() const
{
    return c2ToC1Active;
}

//------------------------------------------------------------------------------
/**
    return the portal's deactivation distance 1
*/
inline
float
n3dsWrapPortalHelper::GetDeactDist1() const
{
    return this->deactDist1;
}

//------------------------------------------------------------------------------
/**
    return the portal's deactivation distance 2
*/
inline
float
n3dsWrapPortalHelper::GetDeactDist2() const
{
    return this->deactDist2;
}

#endif
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------