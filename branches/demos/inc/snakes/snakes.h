#ifndef SNAKES_H
#define SNAKES_H

#include "mathlib/vector.h"
#include "util/nnode.h"
#include "util/nlist.h"
#include "util/narray.h"

struct SnakeJoint
{
    vector3 vPosition;
    vector3 vRotation;
};

struct PathNode
{
    nNode node;
    float fLifeTime;
    vector3 vPosition;
    vector3 vRotation;
};

struct Seed
{
    vector3 vPosition;
    int order;
};

class Snake;
struct SnakeState
{
    SnakeState() :
        pSnake(0), order(-1) {}

    Snake* pSnake;
    int order;
};

class SnakeBoard
{
public:

    //check that the point is contained in the board
    bool Contains( const vector3& vPosition );

    //find a seed closer to the point than the given distance
    Seed* FindSeed( const vector3& vPosition, float fDistance );

    //remove the given seed
    void RemSeed( Seed* );

    float fWidth;
    float fHeight;
    nArray<Seed> seeds;
};

/// Current state of a snake in the game
class Snake
{
public:

    ///constructor
    Snake();
    ///constructor
    ~Snake();

	///setup initial path and joints for initial position
	void Initialize();
	///destroy all joints and path nodes
	void Cleanup();
    ///add a new joint at the end of the list
    void AddJoint();

	///advance the whole snake using current position and speed
    void Tick( float fElapsedTime );

    ///freeze simulation
    void Freeze();
    ///is simulation frozen
    bool IsFrozen();
    ///freeze/unfreeze simulation
    void ToggleFreeze();

    ///create a new path node using the current position and rotation
    void NewPathNode();
    ///get head of current path
    PathNode* GetPath() const;
    ///get next node in path
    PathNode* GetNextNode( PathNode* ) const;

	///check for collision with another snake
    bool CheckCollision( Snake* );

	///set head position
	void SetPosition( const vector3& );
	///set head position
	const vector3& GetPosition() const;
	///set forward vector
	void SetForward( const vector3& );
	///get forward vector
	const vector3& GetForward() const;

	///set size in joints
	void SetNumJoints( const int );
	///set size in joints
	int GetNumJoints() const;
	///set joint radius
	void SetRadius( const float );
	///get joint radius
	float GetRadius() const;
	///set speed
    void SetSpeed( const float );

    ///get joint by index
    const SnakeJoint& GetJointAt( int );

private:

	//position and facing of snake head
	vector3 vPosition;
	vector3 vForward;

    //num joints building up the snake
    int nJoints;

    //radius of each joint
    float fRadius;

    //current speed of movement
    float fSpeed;

    //simulation is frozen
    bool bFrozen;

    //state of current path- new nodes are added to the head
    nList path;
    //state of joints in the snake- first is head
    nArray<SnakeJoint> joints;
};

//---------------------------------------------------------------------------

inline
void
Snake::SetPosition(const vector3 &pos)
{
	this->vPosition = pos;
}

//---------------------------------------------------------------------------

inline
const vector3&
Snake::GetPosition() const
{
	return this->vPosition;
}

//---------------------------------------------------------------------------

inline
void
Snake::SetForward(const vector3 &rot)
{
	this->vForward = rot;
    this->vForward.norm();
}

//---------------------------------------------------------------------------

inline
const vector3&
Snake::GetForward() const
{
	return this->vForward;
}

//---------------------------------------------------------------------------

inline
void
Snake::SetNumJoints(const int nJoints)
{
	this->nJoints = nJoints;
}

//---------------------------------------------------------------------------

inline
int
Snake::GetNumJoints() const
{
    return this->joints.Empty() ? this->nJoints : this->joints.Size();
}

//---------------------------------------------------------------------------

inline
const SnakeJoint&
Snake::GetJointAt( int jointIndex )
{
    return this->joints.At( jointIndex );
}

//---------------------------------------------------------------------------

inline
void
Snake::SetSpeed(const float speed)
{
	this->fSpeed = speed;
}

//---------------------------------------------------------------------------

inline
void
Snake::SetRadius(const float radius)
{
	this->fRadius = radius;
}

//---------------------------------------------------------------------------

inline
float
Snake::GetRadius() const
{
	return this->fRadius;
}

//---------------------------------------------------------------------------

inline
void
Snake::Freeze()
{
	this->bFrozen = true;
}

//---------------------------------------------------------------------------

inline
bool
Snake::IsFrozen()
{
	return this->bFrozen;
}

//---------------------------------------------------------------------------

inline
void
Snake::ToggleFreeze()
{
	this->bFrozen = !this->bFrozen;
}

#endif /*SNAKES_H*/
