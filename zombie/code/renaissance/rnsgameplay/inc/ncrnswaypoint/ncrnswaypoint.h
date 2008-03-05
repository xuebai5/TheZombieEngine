//------------------------------------------------------------------------------
#ifndef NCRNSWAYPOINT_H
#define NCRNSWAYPOINT_H

//------------------------------------------------------------------------------
#include "ncnavnode/ncnavnode.h"

//------------------------------------------------------------------------------
class ncRnsWaypoint : public ncNavNode
{

    NCOMPONENT_DECLARE(ncRnsWaypoint,ncNavNode);

    enum 
    {
        // If you change this enum, update also the editor specific variables:
        //   - Number of attributes (numAttributes)
        //   - Attributes labels (attributeLabels)
        MOVE_WALK       = 1<<0,     // Normal movement while standing
        MOVE_CROUCH     = 1<<1,     // Movement while crouching
        MOVE_PRONE      = 1<<2,     // Movement while been prone
        MOVE_RUN        = 1<<3,     // Running while standing
        MOVE_JUMP       = 1<<4,     // Normal jump
        MOVE_JUMPOVER   = 1<<5,     // Jump over fixed-height obstacles like fences, crates...
        MOVE_CLIMBUP    = 1<<6,     // Climp up a ladder, trunk, pipe or something similar
        MOVE_CLIMBDOWN  = 1<<7,     // Climp down a ladder, trunk, pipe or something similar
        MOVE_WALL       = 1<<8,     // Movement in the special wall navigation system
        MOVE_SWIM       = 1<<9,     // Swimming in the special swimming navigation system
        MOVE_SWIMIN     = 1<<10,    // Insertion point to the swimming navigation system
        MOVE_SWIMOUT    = 1<<11,    // Exit point from the swimming navigation system
        MOVE_SWIM_INOUT = MOVE_SWIMIN | MOVE_SWIMOUT,
        MOVE_FLY        = 1<<12,    // Flying around in the special flying navigation system
        MOVE_FLYIN      = 1<<13,    // Insertion point to the flying navigation system
        MOVE_FLYOUT     = 1<<14,    // Exit point from the flying navigation system
        MOVE_FLY_INOUT  = MOVE_FLYIN | MOVE_FLYOUT,
        COVER_LOW       = 1<<15,    // Take low cover
        COVER_HIGH      = 1<<16,    // Take full body cover
    };

#ifndef NGAME
    /// Number of attributes available to a waypoint
    static const int numAttributes = 17;
    /// Label displayed to the user for each waypoint attribute
    static const char* attributeLabels[numAttributes];
#endif

public:
    /// Constructor
    ncRnsWaypoint();

    /// Destructor
    ~ncRnsWaypoint();

    /// Init instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// Component persistency
    bool SaveCmds (nPersistServer* ps);

    /// Says if a point is inside the node
    virtual bool IsPointInside( const vector3& point ) const;
    /// Says if a 2D point is inside the 2D projection of the node
    virtual bool IsPointInside( float x, float z ) const;
    /// Get the midpoint of the node
    virtual void GetMidpoint( vector3& midpoint ) const;
    /// Get the node's closest navigable point to the given point
    void GetClosestNavigablePoint( const vector3& point, vector3& closestPoint ) const;

    /// Bind the waypoint with any overlapping nav mesh node, losing any current external link
    void GenerateExternalLinks();

        /// Add a local link by id (only for persistency)
        void AddLocalLinkById(nEntityObjectId);
        /// Set/get the attributes (only for persistency)
        void SetAttributes(unsigned int);
        unsigned int GetAttributes() const;
        /// Set/get walk attribute
        void SetWalkAttribute(bool);
        bool IsWalkAttribute() const;
        /// Set/get crouch attribute
        void SetCrouchAttribute(bool);
        bool IsCrouchAttribute() const;
        /// Set/get prone attribute
        void SetProneAttribute(bool);
        bool IsProneAttribute() const;
        /// Set/get run attribute
        void SetRunAttribute(bool);
        bool IsRunAttribute() const;
        /// Set/get jump attribute
        void SetJumpAttribute(bool);
        bool IsJumpAttribute() const;
        /// Set/get jump over attribute
        void SetJumpoverAttribute(bool);
        bool IsJumpoverAttribute() const;
        /// Set/get hop attribute
        void SetHopAttribute(bool);
        bool IsHopAttribute() const;
        /// Set/get leap attribute
        void SetLeapAttribute(bool);
        bool IsLeapAttribute() const;
        void SetLeapInAttribute(bool);
        bool IsLeapInAttribute() const;
        void SetLeapOutAttribute(bool);
        bool IsLeapOutAttribute() const;
        void SetLeapInOutAttribute(bool);
        /// Set/get burrow attribute
        void SetBurrowAttribute(bool);
        bool IsBurrowAttribute() const;
        void SetBurrowInAttribute(bool);
        bool IsBurrowInAttribute() const;
        void SetBurrowOutAttribute(bool);
        bool IsBurrowOutAttribute() const;
        void SetBurrowInOutAttribute(bool);
        /// Set/get roll attribute
        void SetRollAttribute(bool);
        bool IsRollAttribute() const;
        /// Set/get climb up attribute
        void SetClimbupAttribute(bool);
        bool IsClimbupAttribute() const;
        /// Set/get climb down attribute
        void SetClimbdownAttribute(bool);
        bool IsClimbdownAttribute() const;
        /// Set/get swim attribute
        void SetSwimAttribute(bool);
        bool IsSwimAttribute() const;
        void SetSwimInAttribute(bool);
        bool IsSwimInAttribute() const;
        void SetSwimOutAttribute(bool);
        bool IsSwimOutAttribute() const;
        void SetSwimInOutAttribute(bool);
        /// Set/get fly attribute
        void SetFlyAttribute(bool);
        bool IsFlyAttribute() const;
        void SetFlyInAttribute(bool);
        bool IsFlyInAttribute() const;
        void SetFlyOutAttribute(bool);
        bool IsFlyOutAttribute() const;
        void SetFlyInOutAttribute(bool);
        /// Set/get wall attribute
        void SetWallAttribute(bool);
        bool IsWallAttribute() const;
        /// Set/get cover low attribute
        void SetCoverLowAttribute(bool);
        bool IsCoverLowAttribute() const;
        /// Set/get cover high attribute
        void SetCoverHighAttribute(bool);
        bool IsCoverHighAttribute() const;
#ifndef NGAME
        // Editor needed commands
        /// Get the total number of attributes available to a waypoint
        int GetNumAttributes() const;
        /// Get the label of an attribute by its index
        const char* GetAttributeLabel(int) const;
        /// Get the state of an attribute by its index
        bool GetAttributeState(int) const;
        /// Set the state of an attribute by its index
        void SetAttributeState(int, bool);
#endif    

#ifndef NGAME
#endif

    /// update links to other entities
    void UpdateEntityLinks();

private:
    // Auxiliary methods to manage flags
    void ToggleFlag (unsigned int flag, bool activate);
    bool ExistFlag (unsigned int flag) const;

    unsigned int attributes;

    float radius;

};

//------------------------------------------------------------------------------
/**
    SetAttributes
*/
inline
void
ncRnsWaypoint::SetAttributes (unsigned int attributes)
{
    this->attributes = attributes;
}

//------------------------------------------------------------------------------
/**
    GetAttributes
*/
inline
unsigned int
ncRnsWaypoint::GetAttributes() const
{
    return this->attributes;
}

//------------------------------------------------------------------------------
/**
    ToggleFlag
*/
inline
void
ncRnsWaypoint::ToggleFlag (unsigned int flag, bool activate)
{
    if ( activate )
    {
        this->attributes |= flag;
    }
    else
    {
        this->attributes &= ~flag;
    }
}

//------------------------------------------------------------------------------
/**
    ExistFlag
*/
inline
bool
ncRnsWaypoint::ExistFlag (unsigned int flag) const
{
    return (this->attributes&flag) != 0;
}

//------------------------------------------------------------------------------
/**
    SetWalkAttribute
*/
inline
void
ncRnsWaypoint::SetWalkAttribute (bool walk)
{
    this->ToggleFlag (MOVE_WALK, walk);
}

//------------------------------------------------------------------------------
/**
    IsWalkAttribute
*/
inline
bool
ncRnsWaypoint::IsWalkAttribute() const
{
    return this->ExistFlag (MOVE_WALK);
}

//------------------------------------------------------------------------------
/**
    SetCrouchAttribute
*/
inline
void
ncRnsWaypoint::SetCrouchAttribute (bool crouch)
{
    this->ToggleFlag (MOVE_CROUCH, crouch);
}

//------------------------------------------------------------------------------
/**
    IsCrouchAttribute
*/
inline
bool
ncRnsWaypoint::IsCrouchAttribute() const
{
    return this->ExistFlag (MOVE_CROUCH);
}

//------------------------------------------------------------------------------
/**
    SetProneAttribute
*/
inline
void
ncRnsWaypoint::SetProneAttribute (bool prone)
{
    this->ToggleFlag (MOVE_PRONE, prone);
}

//------------------------------------------------------------------------------
/**
    IsProneAttribute
*/
inline
bool
ncRnsWaypoint::IsProneAttribute() const
{
    return this->ExistFlag (MOVE_PRONE);
}

//------------------------------------------------------------------------------
/**
    SetRunAttribute
*/
inline
void
ncRnsWaypoint::SetRunAttribute (bool run)
{
    this->ToggleFlag (MOVE_RUN, run);
}

//------------------------------------------------------------------------------
/**
    IsRunAttribute
*/
inline
bool
ncRnsWaypoint::IsRunAttribute() const
{
    return this->ExistFlag (MOVE_RUN);
}

//------------------------------------------------------------------------------
/**
    SetJumpAttribute
*/
inline
void
ncRnsWaypoint::SetJumpAttribute (bool jump)
{
    this->ToggleFlag (MOVE_JUMP, jump);
}

//------------------------------------------------------------------------------
/**
    IsJumpAttribute
*/
inline
bool
ncRnsWaypoint::IsJumpAttribute() const
{
    return this->ExistFlag (MOVE_JUMP);
}

//------------------------------------------------------------------------------
/**
    SetJumpoverAttribute
*/
inline
void
ncRnsWaypoint::SetJumpoverAttribute (bool jumpOver)
{
    this->ToggleFlag (MOVE_JUMPOVER, jumpOver);
}

//------------------------------------------------------------------------------
/**
    IsJumpoverAttribute
*/
inline
bool
ncRnsWaypoint::IsJumpoverAttribute() const
{
    return this->ExistFlag (MOVE_JUMPOVER);
}

//------------------------------------------------------------------------------
/**
    SetClimbupAttribute
*/
inline
void
ncRnsWaypoint::SetClimbupAttribute (bool climbup)
{
    this->ToggleFlag (MOVE_CLIMBUP, climbup);
}

//------------------------------------------------------------------------------
/**
    IsClimbupAttribute
*/
inline
bool
ncRnsWaypoint::IsClimbupAttribute() const
{
    return this->ExistFlag (MOVE_CLIMBUP);
}

//------------------------------------------------------------------------------
/**
    SetClimbdownAttribute
*/
inline
void
ncRnsWaypoint::SetClimbdownAttribute (bool climbdown)
{
    this->ToggleFlag (MOVE_CLIMBDOWN, climbdown);
}

//------------------------------------------------------------------------------
/**
    IsClimbDownAttribute
*/
inline
bool
ncRnsWaypoint::IsClimbdownAttribute() const
{
    return this->ExistFlag (MOVE_CLIMBDOWN);
}

//------------------------------------------------------------------------------
/**
    SetSwimAttribute
*/
inline
void
ncRnsWaypoint::SetSwimAttribute (bool swim)
{
    this->ToggleFlag (MOVE_SWIM, swim);
}

//------------------------------------------------------------------------------
/**
    IsSwimAttribute
*/
inline
bool
ncRnsWaypoint::IsSwimAttribute() const
{
    return this->ExistFlag (MOVE_SWIM);
}

//------------------------------------------------------------------------------
/**
    SetSwimInAttribute
*/
inline
void
ncRnsWaypoint::SetSwimInAttribute (bool swim)
{
    this->ToggleFlag (MOVE_SWIMIN, swim);
}

//------------------------------------------------------------------------------
/**
    IsSwimInAttribute
*/
inline
bool
ncRnsWaypoint::IsSwimInAttribute() const
{
    return this->ExistFlag (MOVE_SWIMIN);
}

//------------------------------------------------------------------------------
/**
    SetSwimOutAttribute
*/
inline
void
ncRnsWaypoint::SetSwimOutAttribute (bool swim)
{
    this->ToggleFlag (MOVE_SWIMOUT, swim);
}

//------------------------------------------------------------------------------
/**
    IsSwimOutAttribute
*/
inline
bool
ncRnsWaypoint::IsSwimOutAttribute() const
{
    return this->ExistFlag (MOVE_SWIMOUT);
}

//------------------------------------------------------------------------------
/**
    SetSwimInOutAttribute
*/
inline
void
ncRnsWaypoint::SetSwimInOutAttribute (bool swim)
{
    this->ToggleFlag (MOVE_SWIM_INOUT, swim);
}

//------------------------------------------------------------------------------
/**
    SetFlyAttribute
*/
inline
void
ncRnsWaypoint::SetFlyAttribute (bool fly)
{
    this->ToggleFlag (MOVE_FLY, fly);
}

//------------------------------------------------------------------------------
/**
    IsFlyAttribute
*/
inline
bool
ncRnsWaypoint::IsFlyAttribute() const
{
    return this->ExistFlag (MOVE_FLY);
}

//------------------------------------------------------------------------------
/**
    SetFlyInAttribute
*/
inline
void
ncRnsWaypoint::SetFlyInAttribute (bool fly)
{
    this->ToggleFlag (MOVE_FLYIN, fly);
}

//------------------------------------------------------------------------------
/**
    IsFlyAttribute
*/
inline
bool
ncRnsWaypoint::IsFlyInAttribute() const
{
    return this->ExistFlag (MOVE_FLYIN);
}

//------------------------------------------------------------------------------
/**
    SetFlyOutAttribute
*/
inline
void
ncRnsWaypoint::SetFlyOutAttribute (bool fly)
{
    this->ToggleFlag (MOVE_FLYOUT, fly);
}

//------------------------------------------------------------------------------
/**
    IsFlyAttribute
*/
inline
bool
ncRnsWaypoint::IsFlyOutAttribute() const
{
    return this->ExistFlag (MOVE_FLYOUT);
}   

//------------------------------------------------------------------------------
/**
    SetFlyInOutAttribute
*/
inline
void
ncRnsWaypoint::SetFlyInOutAttribute (bool fly)
{
    this->ToggleFlag (MOVE_FLY_INOUT, fly);
}

//------------------------------------------------------------------------------
/**
    SetWallAttribute
*/
inline
void
ncRnsWaypoint::SetWallAttribute (bool wallAttribute)
{
    this->ToggleFlag (MOVE_WALL, wallAttribute);
}

//------------------------------------------------------------------------------
/**
    IsWallAttribute
*/
inline
bool
ncRnsWaypoint::IsWallAttribute() const
{
    return this->ExistFlag (MOVE_WALL);
}

//------------------------------------------------------------------------------
/**
    SetCoverLowAttribute
*/
inline
void
ncRnsWaypoint::SetCoverLowAttribute (bool coverLow)
{
    this->ToggleFlag (COVER_LOW, coverLow);
}

//------------------------------------------------------------------------------
/**
    IsCoverLowAttribute
*/
inline
bool
ncRnsWaypoint::IsCoverLowAttribute() const
{
    return this->ExistFlag (COVER_LOW);
}

//------------------------------------------------------------------------------
/**
    SetCoverHighAttribute
*/
inline
void
ncRnsWaypoint::SetCoverHighAttribute (bool coverHigh)
{
    this->ToggleFlag (COVER_HIGH, coverHigh);
}

//------------------------------------------------------------------------------
/**
    IsCoverHighAttribute
*/
inline
bool
ncRnsWaypoint::IsCoverHighAttribute() const
{
    return this->ExistFlag (COVER_HIGH);
}

#endif 