#include "precompiled/pchgameplay.h"
//------------------------------------------------------------------------------
//  npathfinder.cc
//------------------------------------------------------------------------------

#include "nastar/nastar.h"
#include "nnavmesh/nnavmesh.h"
#include "ncnavmesh/ncnavmeshnode.h"
#include "nnavmesh/nnavutils.h"
#include "entity/nentity.h"
#include "ncaistate/ncaistate.h"
#include "variable/nvariableserver.h"

#include "mathlib/polygon.h"
#include "gfx2/ngfxserver2.h"

nNebulaClass(nAStar, "nobject");

nAStar* nAStar::Singleton = 0;

//------------------------------------------------------------------------------
/**
    Constructor
*/
nAStar::nAStar()
{
    n_assert(0==Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nAStar::~nAStar()
{
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    InitSearch
*/
void
nAStar::InitSearch (const vector3& start, const vector3& goal, ncNavNode* startNode, ncNavNode *goalNode)
{
    this->start = start;
    nNavUtils::GetGroundHeight( this->start, this->start.y );
    n_assert( goalNode );
    goalNode->GetClosestNavigablePoint( goal, this->goal );
	this->startNode = startNode;
	this->goalNode = goalNode;
    this->openList.Clear();
    this->closedList.Clear();
    this->pathNode.Clear();
    this->path.Clear();
}

//------------------------------------------------------------------------------
/**
    FindPath
*/
bool
nAStar::FindPath (const vector3& start, const vector3& goal, nArray<vector3>* path, nArray<bool>* smoothable) // nEntityObject* entity)
{
    ncNavNode* startNode = this->mesh->GetClosestNode( start );
    ncNavNode* goalNode  = this->mesh->GetClosestNode( goal );
    bool existPath = startNode != 0 && goalNode != 0;

    if ( existPath )
    {
        this->InitSearch (start, goal, startNode, goalNode);
        existPath = this->CalculatePath (startNode, goalNode);

		if ( existPath && path )
		{
			path->Clear();
            smoothable->Clear();

			for ( int i=this->path.Size()-1; i>=0; i-- )
			{
				path->Append (this->path[i]);
                smoothable->Append (this->smoothable[i]);
			}
	    }
    }
//    else
//    {
        // @todo Test if going straight to the goal is valid for those cases where
        //       no nodes are found for some of the given points
//    }

    return existPath;
}

//------------------------------------------------------------------------------
/**	
    CalculatePath
*/
bool
nAStar::CalculatePath (ncNavNode* start, ncNavNode* goal)
{
    n_assert(start&&goal);

    const float MaxAllowedTime = nVariableServer::Instance()->GetGlobalVariable("MaxPathFindingTime")->GetFloat();
    nTime maxAllowedTime = nTimeServer::Instance()->GetTime() + MaxAllowedTime;
    bool existPath = false;
    AStarValues values;

    // Let's search!
    start->SetParent( NULL );
    this->CalculateValues (start, start, goal, &values);
    this->InsertAtOpenList(start);

    start->SetG (values.g);
    start->SetH (values.h);
    start->SetF (values.f);

    ncNavNode* current;

    do
    {
        current = this->GetBestF();

        if ( current && current != goal )
        {
			this->MoveToClosedList (current);

            // For all links of the current node
            for ( int i=0; i<current->GetLinksNumber(); i++ )
            {
                ncNavNode* neightbour = current->GetLink(i);
                n_assert(neightbour);

                if ( neightbour && !this->IsClosed (neightbour) )
                {
                    this->CalculateValues (current, neightbour, goal, &values);

					if ( !this->IsOpen (neightbour) )
					{
						this->InsertAtOpenList (neightbour);
						neightbour->SetParent (current);
						neightbour->SetG (values.g);
						neightbour->SetH (values.h);
						neightbour->SetF (values.f);
					}
					else
					if ( neightbour->GetG() > values.g )
					{
						neightbour->SetParent (current);
						neightbour->SetG (values.g);
						neightbour->SetH (values.h);
						neightbour->SetF (values.f);
					}
				}
            }
        }


    } while ( current != goal && this->openList.Size() > 0 &&
        nTimeServer::Instance()->GetTime() < maxAllowedTime );

    // Now, we must build the path
    if ( current == goal )
    {
        this->BuildFinalPath();
        existPath = true;
    }

    return existPath;
}

//------------------------------------------------------------------------------
/**
    InsertAtOpenList
*/
void
nAStar::InsertAtOpenList (ncNavNode* node)
{
    n_assert(node);

    if ( node && this->openList.FindIndex(node) == -1 )
    {
        this->openList.Append (node);
    }
}

//------------------------------------------------------------------------------
/**
    MoveToClosedList
*/
void
nAStar::MoveToClosedList (ncNavNode* node)
{
    if ( this->closedList.FindIndex (node) == -1 )
    {
        this->closedList.Append (node);

		// delete from the open list
		int index = this->openList.FindIndex (node);
		this->openList.Erase (index);
    }
}

//------------------------------------------------------------------------------
/**
    IsConsidered
*/
bool
nAStar::IsConsidered (ncNavNode* node) const
{
    n_assert(node);

    bool exist = false;

    if ( node )
    {
        exist = this->openList.FindIndex (node) != -1 || 
                this->closedList.FindIndex(node) != -1; 
    }

    return exist;
}

//------------------------------------------------------------------------------
/**
	IsClosed
*/
bool
nAStar::IsClosed (ncNavNode* node) const
{
	n_assert(node);

	bool closed = false;

	if ( node )
	{
		closed = this->closedList.FindIndex (node) > -1;
	}

	return closed;
}

//------------------------------------------------------------------------------
/**
	IsOpen
*/
bool
nAStar::IsOpen (ncNavNode* node) const
{
	n_assert(node);

	bool open = false;

	if ( node )
	{
		open = this->openList.FindIndex (node) > -1;
	}

	return open;
}

//------------------------------------------------------------------------------
/**
    CalculateValues
*/
void
nAStar::CalculateValues (ncNavNode* current, ncNavNode* candidate, ncNavNode* goal, AStarValues* values)
{
    n_assert(current&&candidate&&goal&&values);

    if ( current && candidate && goal && values )
    {
        vector3 posCandidate;
        candidate->GetMidpoint( posCandidate );
        vector3 posCurrent;
        current->GetMidpoint( posCurrent );
        vector3 posGoal;
        goal->GetMidpoint( posGoal );

        values->g = (int)(posCurrent - posCandidate).lensquared();
        values->h = (int)(posGoal - posCandidate).lensquared();
        values->f = values->g + values->h;
    }
}

//------------------------------------------------------------------------------
/**
    GetBestF
*/
ncNavNode*
nAStar::GetBestF() const
{
    ncNavNode* bestNode =0 ;
    int bestF = INT_MAX;

    for ( int i=0; i<this->openList.Size(); i++ )
    {
        ncNavNode* element = this->openList[i];
        n_assert(element);

        if ( element )
        {
            int f = element->GetF();

            if ( f < bestF )
            {
                bestNode = element;
                bestF = f;                
            }
        }
    }

    return bestNode;
}

//------------------------------------------------------------------------------
/**
    IsValidGoal
*/
bool
nAStar::IsValidGoal (const vector3& goal, nEntityObject* entity) const
{
    n_assert(this->mesh);

    return this->IsValidPosition (goal, entity);
}

//------------------------------------------------------------------------------
/**
    IsValidPosition
*/
bool
nAStar::IsValidPosition (const vector3& position, nEntityObject* /*entity*/) const
{
    n_assert(this->mesh);

    // Project given point on ground
    vector3 groundPosition( position );
    if ( !nNavUtils::GetGroundHeight( position, groundPosition.y ) )
    {
        // No ground found, so it's not a valid position
        return false;
    }

    // Position is valid if there's a node that allows to reach the given position
    return this->mesh->GetNavNode( groundPosition ) != NULL;

    // @todo Check if the agent physically fits in the given position depending on its height and size
}

//------------------------------------------------------------------------------
/**
	BuildFinalPath
*/
void
nAStar::BuildFinalPath()
{
	ncNavNode* node;
	this->path.Clear();
    this->smoothable.Clear();

	this->path.Append (this->goal);
    this->smoothable.Append (false);

	for (node = this->goalNode; node && node != this->startNode; node = node->GetParent() )
	{
		n_assert(node);

		if ( node )
		{
            vector3 subgoal;
            node->GetMidpoint( subgoal );
            nNavUtils::GetGroundHeight( subgoal, subgoal.y );
			this->path.Append( subgoal );
            this->smoothable.Append( node->GetComponent<ncNavMeshNode>() != NULL );
		}
	}

	this->path.Append (this->start);
    this->smoothable.Append (false);
}
