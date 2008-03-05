#include "precompiled/pchgameplay.h"
//------------------------------------------------------------------------------
//  npathfinder.cc
//------------------------------------------------------------------------------

#include "npathfinder/npathfinder.h"
#include "nastar/nastar.h"
#include "util/nrandomlogic.h"
#include "nnavmesh/nnavutils.h"
#include "variable/nvariableserver.h"

#ifndef __ZOMBIE_EXPORTER__
#include "nnavmesh/nnavmesh.h"
#include "ncnavmesh/ncnavmeshnode.h"
#include "ncaistate/ncaistate.h"
#include "ncgameplayliving/ncgameplaylivingclass.h"
#endif

#define TEST_TRIAL 15.f

nNebulaClass(nPathFinder, "nobject");

nPathFinder* nPathFinder::Singleton = 0;

#ifndef NGAME
bool nPathFinder::clearPath( true );
bool nPathFinder::roundPath( true );
bool nPathFinder::singleSmoothPath( true );
#endif

//------------------------------------------------------------------------------
/**
	Constructor
*/
nPathFinder::nPathFinder() :
	width (4.f),
	height(2.f)
{
	n_assert(Singleton==0);
	Singleton = this;

	// Create the A* search
	this->pathFinder = static_cast<nAStar*>(nKernelServer::Instance()->New("nastar"));
}

//------------------------------------------------------------------------------
/**
	Destructor
*/
nPathFinder::~nPathFinder()
{
	Singleton = 0;

	if ( this->pathFinder )
	{
		this->pathFinder->Release();
		this->pathFinder = 0;
	}
}

//------------------------------------------------------------------------------
/**
	SetMesh
*/
void
nPathFinder::SetMesh (nNavMesh* mesh)
{
	n_assert(pathFinder);

	if ( pathFinder )
	{
		pathFinder->SetMesh (mesh);
	}
}

//------------------------------------------------------------------------------
/**
	GetMesh
*/
nNavMesh*
nPathFinder::GetMesh() const
{
	n_assert(pathFinder);
	nNavMesh* mesh = 0;

	if ( pathFinder )
	{
		mesh = pathFinder->GetMesh();
	}
	
	return mesh;
}

//------------------------------------------------------------------------------ 
/**
	FindPathStraight
*/
bool 
nPathFinder::FindPathStraight (const vector3& start, 
                               const vector3& goal, 
                               nArray<vector3>* path, 
                               nEntityObject* entity)
{
	n_assert(this->pathFinder);
	n_assert(path);

	bool valid = false;

	if ( path && this->pathFinder )
	{
		valid = this->pathFinder->FindPath (start, goal, &this->path, &this->smoothable);
	
		if ( valid )
		{
			this->SmoothPath (entity, PATH_STRAIGHT);

			// Now, copy the final path
			path->Clear();
			for ( int i=0; i<this->path.Size(); i++ )
			{
				path->Append (this->path[i]);
			}
		}
	}

	return valid;
}

//------------------------------------------------------------------------------ 
/**
	FindPathZigZag
*/
bool 
nPathFinder::FindPathZigZag (const vector3& start, 
                             const vector3& goal, 
                             nArray<vector3>* path, 
                             nArray<bool> &dirs,
                             nEntityObject* entity)
{
	n_assert(this->pathFinder);
	n_assert(path);

	bool valid = false;

	if ( path && this->pathFinder )
	{
        valid = this->pathFinder->FindPath (start, goal, &this->path, &this->smoothable);
	
		if ( valid )
		{
			this->SmoothPath (entity, PATH_ZIGZAG);
			this->BuildZigZag (entity, dirs);
			
			// Now, copy the final path
			path->Clear();
			for ( int i=0; i<this->path.Size(); i++ )
			{
				path->Append (this->path[i]);
			}
		}
	}

	return valid;
}

//------------------------------------------------------------------------------
/**
	SmoothPath

	@brief Beautify the path
*/
void
nPathFinder::SmoothPath (nEntityObject* entity, ePathStyle style)
{
	nArray<vector3> intermediatePath;
	static const int maxTicks = 10;

	// First, get the path 
	for ( int i=0; i<this->path.Size(); i++ )
	{
		vector3 position = this->path[i];

//		if ( !this->ExistPoint(position, intermediatePath) )
		{
			intermediatePath.Append (position);
		}
	}

#ifndef NGAME
    if ( this->clearPath )
#endif
    {
        // Second approximation. Check the path between the START and the END, and after, the nearest to the END and so on.    
        this->ClearPath (intermediatePath, this->smoothable);
    }

	float size = 0.4f;

#ifndef __ZOMBIE_EXPORTER__
	ncGameplayLivingClass* gpClass = 0;

	if ( entity )
	{
		gpClass = entity->GetEntityClass()->GetComponent <ncGameplayLivingClass>();
		n_assert(gpClass);
	}

	if ( gpClass )
	{
		size = gpClass->GetSize();                 
	}
#endif

#ifndef NGAME
    if ( this->singleSmoothPath )
#endif
    {
	    this->SingleSmoothPath (intermediatePath, size);
    }

	// Copy the intermediate path into the final path
	nArray<vector3> finalPath;

	for ( int i=0; i<intermediatePath.Size(); i++ )
	{
		finalPath.Append (intermediatePath[i]);
	}

	if ( style == PATH_STRAIGHT N_IFNDEF_NGAME( && this->roundPath ) )
	{
		this->RoundPath (finalPath, entity);
	}
	else
	{
		this->CopyPath (finalPath);
	}
}

//------------------------------------------------------------------------------
/**
	CopyPath
*/
void
nPathFinder::CopyPath (const nArray<vector3>& source)
{
	this->path.Clear();

	for ( int i=0; i<source.Size(); i++ )
	{
		this->path.Append (source[i]);
	}
}

//------------------------------------------------------------------------------
/**
	ClearPath
*/
void
nPathFinder::ClearPath (nArray<vector3>& path, nArray<bool>& smoothable)
{
    const float MaxAllowedTime = nVariableServer::Instance()->GetGlobalVariable("MaxPathSmoothingTime")->GetFloat() / 3;
    nTime maxAllowedTime = nTimeServer::Instance()->GetTime() + MaxAllowedTime;

    if ( path.Size() > 0 && path.Size() == smoothable.Size() )
	{
		nArray<vector3> aux;
        nArray<bool> auxSmooth;

		aux.Append (path[0]);
        auxSmooth.Append (smoothable[0]);

        bool allowedTimeExpired = false;
		int i;
		for ( i=0; i<path.Size() -1 ; )
		{
            int j( path.Size() - 1 );
			for ( ; j>i+1; j-- )
			{
                if ( nTimeServer::Instance()->GetTime() > maxAllowedTime )
                {
                    // Allowed process time expired -> Copy remaining path and finish
                    for ( int j = i + 1; j < path.Size(); ++j )
                    {
                        aux.Append( path[j] );
                        auxSmooth.Append( smoothable[j] );
                    }
                    allowedTimeExpired = true;
                    break;
                }

				vector3 start = path[i];
				vector3 end   = path[j];

                if ( this->IsWalkable (start, end) )
				{
                    break;
				}
			}
            if ( allowedTimeExpired )
            {
                break;
            }
			aux.Append (path[j]);
            auxSmooth.Append (smoothable[j]);
			i = j;
		}

		// Copy the aux path into the path
		path.Clear();
        smoothable.Clear();

		for ( i=0; i<aux.Size(); i++ )
		{
			path.Append (aux[i]);
            smoothable.Append (auxSmooth[i]);
		}
	}
}

//------------------------------------------------------------------------------
/**
	IsWalkable

    @todo Optimize, optimize and optimize, and when finished optimize even more
*/
bool
nPathFinder::IsWalkable (const vector3& start, const vector3& end) const
{
    // Do an approximated walkable check, mainly for waypoint navigation nodes
    nNavMesh* navMesh( this->pathFinder->GetMesh() );
    n_assert( navMesh );
    if ( navMesh->GetWaypointNode( start ) && navMesh->GetWaypointNode( end ) )
    {
        if ( nNavUtils::IsWalkable( start, end, NULL ) )
        {
            return true;
        }
    }

    // -- Check for walkable surface on the navigation mesh

    ncNavNode* current( this->pathFinder->GetMesh()->GetNode( start ) );

    if ( !current )
    {
        return false;
    }

    // This goes faster and more robust than previous implementation which checked for a
    // walkable surface each 5 cm, but it's still very slow so it should be optimized as well.
    vector3 crossPoint;
    vector3 crossNormal;
    return !this->pathFinder->GetMesh()->GetBoundaryPoint( line3(start, end), crossPoint, crossNormal );
}

//------------------------------------------------------------------------------
/**
	RoundPath

	@brief Calculate the turn points if there is a entity defined
*/
void
nPathFinder::RoundPath 
#ifdef __ZOMBIE_EXPORTER__
(nArray<vector3>&, nEntityObject*) {
#else
(nArray<vector3>& intermediatePath, nEntityObject* entity)
{
    const float MaxAllowedTime = nVariableServer::Instance()->GetGlobalVariable("MaxPathSmoothingTime")->GetFloat() / 3;
    nTime maxAllowedTime = nTimeServer::Instance()->GetTime() + MaxAllowedTime;

	this->path.Clear();
	if ( entity )
	{
        // Calc min allowed distance between two consecutive points of the final path
        // @todo Somewhat get the expected max frame time, with some tolerance
        // @todo Use the entity's max speed (or current speed?)
        float maxFrameTime( 0.1f );
        float maxSpeed( 1.0f );
        const float minSecurityDistanceSquared( maxSpeed * maxFrameTime * maxSpeed * maxFrameTime );

		ncGameplayLivingClass* gpClass = 0;

		if ( entity )
		{
			gpClass = entity->GetEntityClass()->GetComponent <ncGameplayLivingClass>();
			n_assert(gpClass);
		}
		n_assert(gpClass);
		float turnRadius; 

		this->path.Append (intermediatePath[0]);

		for ( int i=1; i<intermediatePath.Size()-1; i++ )
		{
            if ( nTimeServer::Instance()->GetTime() > maxAllowedTime )
            {
                // Allowed process time expired -> Copy remaining path and finish
                for ( int j = i; j < intermediatePath.Size() - 1; ++j )
                {
                    this->path.Append( intermediatePath[j] );
                }
                break;
            }

			vector3 p1, p3;
			vector3 p2 = intermediatePath[i];
			float distance1, distance3;
            nArray<vector3> turnPath;

		    if ( gpClass )
		    {
			    turnRadius = gpClass->GetTurnRadius();
		    }
		    else
		    {
			    turnRadius = 2;
		    }

            // Calc a turn and check if it's a valid turn, otherwise keep reducing the turn radius and trying again
            bool validTurn( false );
            int turnAttempts( 4 );
            float turnDecay( 0.5f );
            for ( ; !validTurn && turnAttempts > 0; --turnAttempts, turnRadius *= turnDecay )
            {
                turnPath.Clear();

			    // Calculate p1 and p3
			    p1 = intermediatePath[i-1] - p2;
			    p3 = intermediatePath[i+1] - p2; 

			    distance1 = p1.len();
			    distance3 = p3.len();

			    p1.norm();
			    p3.norm();

			    if ( distance1 > turnRadius && 
				    distance3 > turnRadius )
			    {
				    p1 *= turnRadius;
				    p3 *= turnRadius;
			    }
			    else
			    {
				    // We'll take the smallest value for the turning, really, this will determine whether is a valid point or not
				    if ( distance1 > distance3 )
				    {
					    p1 *= distance3;
					    p3 *= distance3;
				    }
				    else
				    {
					    p1 *= distance1;
					    p3 *= distance1;
				    }
			    }
                if ( i > 1 )
                {
                    p1 *= 0.5f;
                }
                if ( i < intermediatePath.Size() - 2 )
                {
                    p3 *= 0.5f;
                }

			    p1 += p2;
			    p3 += p2;

			    // Generate tree points for every joint
                validTurn = true;
                vector3 prev( this->path.Back() );
			    for ( float f = 0.f; f<1.0f; f+= 0.15f )
			    {
				    vector3 mid;

				    mid.x = p3.x*f*f + p2.x * (2*f - 2*f*f) + p1.x * (1-f)*(1-f);
				    mid.y = prev.y; //p3.y*f*f + p2.y * (2*f - 2*f*f) + p1.y * (1-f)*(1-f);
				    mid.z = p3.z*f*f + p2.z * (2*f - 2*f*f) + p1.z * (1-f)*(1-f);

                    if ( ( prev - mid ).lensquared() < minSecurityDistanceSquared )
                    {
                        continue;
                    }

                    if ( !this->IsWalkable( prev, mid ) )
                    {
                        validTurn = false;
                        break;
                    }

                    turnPath.Append( mid );
                    prev = mid;
			    }
		    }

            if ( !validTurn )
            {
                // No valid turn found -> Insert the intermediate goal
                if ( !ExistPoint( p2, this->path ) )
                {
                    this->path.Append( p2 );
                }
            }
            else
            {
                // Valid turn found -> Insert turn into path
                for ( int i(0); i < turnPath.Size(); ++i )
                {
				    if ( !ExistPoint (turnPath[i], this->path) )
				    {
					    this->path.Append (turnPath[i]);
				    }
                }
            }
        }

		this->path.Append (intermediatePath[intermediatePath.Size()-1]);
	}
	else
	{
		for ( int i=0; i<intermediatePath.Size(); i++ )
		{
			this->path.Append (intermediatePath[i]);
		}
	}
#endif
}

//------------------------------------------------------------------------------
/**
	SingleSmoothPath
*/
void
nPathFinder::SingleSmoothPath (nArray<vector3>& path, float size)
{
    const float MaxAllowedTime = nVariableServer::Instance()->GetGlobalVariable("MaxPathSmoothingTime")->GetFloat() / 3;
    nTime maxAllowedTime = nTimeServer::Instance()->GetTime() + MaxAllowedTime;

	for ( int i=1; i<path.Size() - 1; i++ )
	{
        if ( nTimeServer::Instance()->GetTime() > maxAllowedTime )
        {
            // Allowed process time expired -> Do not touch the remaining path and finish
            break;
        }

        if ( !this->smoothable[i] )
        {
            continue;
        }

		vector3 x0 = path[i-1];
		vector3 x1 = path[i];
		vector3 x2 = path[i+1];
		vector3 edge0 = x0 - x1;
		vector3 edge1 = x2 - x1;

		edge0.y = edge1.y = 0.f;

		float dot = edge0.dot (edge1);

		if ( dot > 0 )
		{
			edge0.norm();
			edge1.norm();

			bool done = false;

			for ( float f=1.f; f<TEST_TRIAL && !done; f+=1.f )
			{
				vector3 xf = edge0;

				xf *= size * (-f);
				xf += x0;

				if ( this->IsWalkable (xf, x2) )
				{
					path[i] = xf;
					done = true;
				}
			}

			if ( !done )
			{
				vector3 baseLine = x2 - x0;
				vector3 pullLine;

				baseLine *= 0.5f;
				pullLine = x1 - x0 + baseLine;
				pullLine.norm();

				for ( float f=1.f; f<TEST_TRIAL && !done; f+=1.f )
				{
					vector3 xf = x0 + baseLine;
					vector3 dist = pullLine;

					dist *= size * f;
					xf += dist;

					if ( this->IsWalkable (x0, xf) && this->IsWalkable (xf, x2) )
					{
						path[i] = xf;
						done = true;
					}
				}
			}

			if ( !done )
			{
				for ( float f=1.f; f<TEST_TRIAL && !done; f+=1.f )
				{
					vector3 xf1, xf2;

					xf1 = edge0;
					xf2 = edge1;

					xf1 *= size * (-f);
					xf2 *= size * (-f);

					xf1 += x0;
					xf2 += x2;

					if ( this->IsWalkable (xf1, xf2) )
					{
						path[i] = xf1;
						path.Insert (i+1, xf2);
                        this->smoothable.Insert (i+1, true);
						done = true;    
						i++;
					}
				}    
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
	ExistPoint
*/
bool
nPathFinder::ExistPoint (const vector3& point, const nArray<vector3>& path) const
{
	bool exist = false;

	for ( int i=0; i<path.Size() && !exist; i++ )
	{
		vector3 elem = path[i];
		exist = elem.isequal (point, FLT_EPSILON);
	}

	return exist;
}

//------------------------------------------------------------------------------
/**
	BuildZigZag

	@brief Get a zigzag path from the smoothed path
*/
void
nPathFinder::BuildZigZag (nEntityObject* /*entity*/, nArray<bool> &dirs)
{
	nArray<vector3> zigzag;
	vector3 previous;

    int i;
	for ( i = 0; i < this->path.Size() - 1; ++i)
	{
		vector3 p0 = this->path[i];
		vector3 p1 = this->path[i+1];
		vector3 direction = p1-p0;
		vector3 tg;
		int times = 0;
		float l;
		
		direction.y = 0;
		l = direction.len();
		direction.norm();

		tg.x = -direction.z;
		tg.z = direction.x;
		direction *= height;
		
		n_assert(this->height);

		if ( this->height )
		{
			times = int (l / this->height);
		}

		zigzag.Append (p0);
		previous = p0;

		for ( int j = 1; j < times; ++j )
		{
            float flj = static_cast<float>(j);
			vector3 p2 = direction * flj;
            int sign = n_rand_int(100) < 50 ? -1 : 1;
            dirs.Append(sign > 0);
            //j%2 == 0 ? -1 : 1;
			vector3 p3 = tg * float(sign);
			int threshold = n_rand_int (int (this->width * 10));
			float length = threshold * 0.1f; 

			// Select randomly the length of the zigzag
			p2 += p3*length + p0;

			while ( !this->IsWalkable (previous, p2) && length > -0.3f )
			{
				length -= 0.3f;
				p2 = direction * flj + p3*length + p0;
			}

			if ( !this->IsWalkable (previous, p2) )
			{
				p2 = direction * flj + p0;
			}

			zigzag.Append (p2);
			previous = p2;
		}
	}		

	if ( this->path.Size() > 1 )
	{
		zigzag.Append (this->path[this->path.Size()-1]);
	}

	// Now, we'll copy the zigzag into the final path
	this->path.Clear();

	for ( i=0; i<zigzag.Size(); i++ )
	{
		this->path.Append (zigzag[i]);
	}
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
	SetPathCleaning
*/
void nPathFinder::SetPathCleaning( bool enable )
{
    nPathFinder::clearPath = enable;
}

//------------------------------------------------------------------------------
/**
	GetPathCleaning
*/
bool nPathFinder::GetPathCleaning()
{
    return nPathFinder::clearPath;
}

//------------------------------------------------------------------------------
/**
	SetPathRounding
*/
void nPathFinder::SetPathRounding( bool enable )
{
    nPathFinder::roundPath = enable;
}

//------------------------------------------------------------------------------
/**
	GetPathRounding
*/
bool nPathFinder::GetPathRounding()
{
    return nPathFinder::roundPath;
}

//------------------------------------------------------------------------------
/**
	SetSingleSmoothing
*/
void nPathFinder::SetSingleSmoothing( bool enable )
{
    nPathFinder::singleSmoothPath = enable;
}

//------------------------------------------------------------------------------
/**
	GetSingleSmoothing
*/
bool nPathFinder::GetSingleSmoothing()
{
    return nPathFinder::singleSmoothPath;
}
#endif // !NGAME
