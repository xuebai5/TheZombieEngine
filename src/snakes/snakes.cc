
#include "snakes/snakes.h"

//--------------------------------------------------------------------------------
/**
*/
bool
SnakeBoard::Contains( const vector3& vPosition )
{
    if ( vPosition.x < 0.f || vPosition.x > this->fWidth )
        return false;
    if ( vPosition.z < 0.f || vPosition.z > this->fHeight )
        return false;

    return true;
}

//--------------------------------------------------------------------------------
/**
*/
Seed*
SnakeBoard::FindSeed( const vector3& vPosition, float fDistance )
{
    for ( int index = 0; index < this->seeds.Size(); ++index )
    {
        if ( vector3::distance( this->seeds[index].vPosition, vPosition ) < fDistance )
        {
            return &this->seeds[index];
        }
    }
    return 0;
}

//--------------------------------------------------------------------------------
/**
*/
void
SnakeBoard::RemSeed( Seed* pSeed )
{
    for ( int index = 0; index < this->seeds.Size(); ++index )
    {
        if ( &this->seeds[index] == pSeed )
        {
            this->seeds.Erase( index );
        }
    }
}

//--------------------------------------------------------------------------------
/**
*/
Snake::Snake() :
    vForward(0.f, 0.f, 1.f),
    bFrozen(false)
{
    
}

//--------------------------------------------------------------------------------
/**
*/
Snake::~Snake()
{
    this->Cleanup();
}

//--------------------------------------------------------------------------------
/**
*/
void
Snake::Initialize()
{
	//cleanup everything if it is already initialized
	this->Cleanup();
    if ( this->IsFrozen() ) this->ToggleFreeze();

	n_assert(this->nJoints > 0);
	vector3 curPosition = this->vPosition;
	for ( int iJoint = 0; iJoint < this->nJoints; ++iJoint )
	{
		SnakeJoint& newJoint = this->joints.PushBack( SnakeJoint() );
		newJoint.vPosition = curPosition;
		newJoint.vRotation = this->vForward;
	}
}

//--------------------------------------------------------------------------------
/**
*/
void
Snake::AddJoint()
{
    if ( !this->joints.Empty() )
    {
        this->joints.Append( SnakeJoint() );
    }
}

//--------------------------------------------------------------------------------
/**
*/
void
Snake::Cleanup()
{
	this->joints.Clear();
    while ( nNode* curNode = this->path.GetHead() )
    {
        curNode->Remove();
        PathNode* pathNode = (PathNode*) curNode->GetPtr();
        n_delete( pathNode );
    }
}

//--------------------------------------------------------------------------------
/**
*/
void
Snake::Tick( float fElapsedTime )
{
    n_assert(!this->joints.Empty());

    //advance current position
    if ( !this->bFrozen )
    {
        this->vPosition += this->vForward * this->fSpeed * fElapsedTime;
    }

    //compute position of all joints following the current path
    vector3 curPosition( this->vPosition );
    vector3 curRotation( this->vForward );
    PathNode* curPathNode = this->GetPath();
    for ( int index = 0; index < this->joints.Size(); ++index )
    {
        SnakeJoint& curJoint = this->joints.At(index);
        curJoint.vPosition = curPosition;
        curJoint.vRotation = curRotation;
        
        //compute position of next joint
        if ( curPathNode )
        {
            //if there is a path node closer than [radius] check if the distance to it is less than radius
            float fDistance = vector3::distance( curPosition, curPathNode->vPosition );
            if ( fDistance < this->fRadius )
            {
                float fRemaining = fRadius - fDistance;
                curPosition = curPathNode->vPosition - ( curPathNode->vRotation * fRemaining );
                curRotation = curPathNode->vRotation;
                curPathNode = this->GetNextNode( curPathNode );
                continue;
            }
        }
        
        //the next joint is just [radius] units behind the current one, and rotation remains
        curPosition = curPosition - ( curRotation * this->fRadius );
    }
}

//--------------------------------------------------------------------------------
/**
*/
bool
Snake::CheckCollision( Snake* pOtherSnake )
{
    //check for collision with all joints of the other snake
    for ( int iJoint = 0; iJoint < this->joints.Size(); ++iJoint )
    {
        SnakeJoint& curJoint = this->joints.At( iJoint );

        for ( int index = 0; index < pOtherSnake->GetNumJoints(); ++index )
        {
            if ( pOtherSnake == this && ( index - iJoint ) < 2 ) continue;

            const SnakeJoint& otherJoint = pOtherSnake->GetJointAt(index);
            float fDistance = vector3::distance( curJoint.vPosition, otherJoint.vPosition );
            if ( fDistance < this->fRadius )
            {
                return true;
            }
        }
    }

    return false;
}

//--------------------------------------------------------------------------------
/**
*/
void
Snake::NewPathNode()
{
    //do not create a new path node at a distance from the previous one less than radius
    PathNode* pathNode = this->GetPath();
    if ( pathNode && vector3::distance( pathNode->vPosition, this->vPosition ) < this->fRadius )
        return;

    PathNode* newNode = n_new( PathNode );
    newNode->node.SetPtr( newNode );
    newNode->vPosition = this->vPosition;
    newNode->vRotation = this->vForward;

    this->path.AddHead( &newNode->node );
}

//--------------------------------------------------------------------------------
/**
*/
PathNode*
Snake::GetPath() const
{
    if ( nNode* node = this->path.GetHead() )
    {
        return (PathNode*) node->GetPtr();
    }
    return 0;
}

//--------------------------------------------------------------------------------
/**
*/
PathNode*
Snake::GetNextNode( PathNode* curNode ) const
{
    if ( curNode && curNode->node.GetSucc() )
    {
        return (PathNode*) curNode->node.GetSucc()->GetPtr();
    }
    return 0;
}
