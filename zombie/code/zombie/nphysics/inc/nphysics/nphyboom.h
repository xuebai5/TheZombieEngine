#ifndef N_PHYBOOM_H
#define N_PHYBOOM_H

//-----------------------------------------------------------------------------
/**
    @class nPhyBoom
    @ingroup NebulaPhysicsSystem
    @brief Representation of a explosion or blast.

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Explosion
    
    @cppclass nPhyBoom
    
    @superclass nObject

    @classinfo Representation of a explosion or blast.
*/    

//-----------------------------------------------------------------------------

#include "kernel/nobject.h"
#include "nphysics/nphysicsconfig.h"

//-----------------------------------------------------------------------------
class nPhysicsWorld;
class nPhyRigidBody;
//-----------------------------------------------------------------------------

class nPhyBoom : public nObject {
    public:
        /// types of booms
        typedef enum {
            explosion, // radial force
            blast
        } type;

        /// default constructor
        nPhyBoom();

        /// destructor
        ~nPhyBoom();

        /// sets the type of boom
        void SetType( type typeboom );

        /// sets the length of the boom
        void SetLength( phyreal length );

        /// sets the force of the booooom
        void SetForce( phyreal force );

        /// boom
        void GoBoomAt( const vector3& position );

        /// sets the world for this explosion
        void SetWorld( nPhysicsWorld* world );

        /// assignament operator (deep copy)
        nPhyBoom& operator = ( const nPhyBoom& boom );

        /// returns the world
        nPhysicsWorld* GetWorld() const;

        /// returns the type
        type Type() const;

        /// returns the length
        phyreal GetLength() const;

        /// returns the force
        phyreal GetForce() const;

        /// get last position
        const vector3& At() const;

        /// checks if affected
        bool IsAffected( const nPhyRigidBody* body ) const;

        /// returns the radius
        phyreal GetRadius() const;

        /// sets the radius
        void SetRadius( phyreal radius );

        /// returns the direction of the explosion
        const vector3& GetDirection() const;

        /// sets the direction of the explosion
        void SetDirection( const vector3& direction );
        
    private:
        /// copy constructor (not allowed)
        nPhyBoom( const nPhyBoom& );

        /// explosion force in newtows
        phyreal boomForce;

        /// boom type
        type boomType;

        /// length of boom
        phyreal boomLength;

        /// world
        nPhysicsWorld* world;

        /// last bomb position
        vector3 at;

        /// radius
        phyreal boomRadius;

        /// direction
        vector3 boomDirection;

        /// checks for a blast
        bool IsBlastAffected( const vector3& point ) const;
};

//-----------------------------------------------------------------------------
/**
    Returns the world.

    @return world  

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
nPhysicsWorld* nPhyBoom::GetWorld() const
{
    return this->world;
}

//-----------------------------------------------------------------------------
/**
    Returns the type.

    @return type    explosion type  

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
nPhyBoom::type nPhyBoom::Type() const
{
    return this->boomType;
}

//-----------------------------------------------------------------------------
/**
    Returns the length.

    @return length

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
phyreal nPhyBoom::GetLength() const
{
    return this->boomLength;
}

//-----------------------------------------------------------------------------
/**
    Returns the force.

    @return force

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
phyreal nPhyBoom::GetForce() const
{
    return this->boomForce;
}

//-----------------------------------------------------------------------------
/**
    Get last position.

    @return last registered explosion position

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
const vector3& nPhyBoom::At() const
{
    return this->at;
}

//-----------------------------------------------------------------------------
/**
    Returns the radius.

    @return explosion radius

    history:
        - 02-Dec-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
phyreal nPhyBoom::GetRadius() const
{
    return this->boomRadius;
}

//-----------------------------------------------------------------------------
/**
    Sets the radius.

    @param radius   explosion radius

    history:
        - 02-Dec-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhyBoom::SetRadius( phyreal radius )
{
    this->boomRadius = radius;
}

//-----------------------------------------------------------------------------
/**
    Returns the direction of the explosion.

    @return direction

    history:
        - 02-Dec-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
const vector3& nPhyBoom::GetDirection() const
{
    return this->boomDirection;
}

//-----------------------------------------------------------------------------
/**
    Sets the direction of the explosion.

    @param direction explosion direction

    history:
        - 02-Dec-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhyBoom::SetDirection( const vector3& direction )
{
    this->boomDirection = direction;
}

//-----------------------------------------------------------------------------
/**
    Sets the type of boom.

    @param type boom type

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhyBoom::SetType( type typeboom )
{
    this->boomType = typeboom;
}

//-----------------------------------------------------------------------------
/**
    Sets the length of the boom.

    @param length distance

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhyBoom::SetLength( phyreal length )
{
    n_assert2( length > 0, "It cannot be a negative value" );

    this->boomLength = length;
}

//-----------------------------------------------------------------------------
/**
    Sets the force of the booooom.

    @param force force of the boom in newtons

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhyBoom::SetForce( phyreal force )
{
    n_assert2( force > 0, "It cannot be a negative value" );

    this->boomForce = force;
}

//-----------------------------------------------------------------------------
/**
    Sets the world for this explosion.

    @param world world where this explosion will live

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhyBoom::SetWorld( nPhysicsWorld* world )
{
    this->world = world;
}

#endif
