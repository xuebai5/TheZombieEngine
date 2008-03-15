#ifndef N_PATHFINDER_H
#define N_PATHFINDER_H

//------------------------------------------------------------------------------
/**
    @class nPathFinder

    (C) 2006 Conjurer Services, S.A.
*/

#include "kernel/nobject.h"
#include "util/narray.h"
#include "mathlib/vector.h"

class nAStar;
class nEntityObject;
class nNavMesh;

class nPathFinder : public nObject
{
public:
	enum ePathStyle
	{
		PATH_STRAIGHT = 0,
		PATH_ZIGZAG
	};

	nPathFinder();
	~nPathFinder();

	/// Set the mesh
	void SetMesh (nNavMesh* mesh);
	/// Get the mesh
	nNavMesh* GetMesh() const;

	/// Get the straight path
	bool FindPathStraight (const vector3& start, 
                           const vector3& goal, 
                           nArray<vector3>* path,
                           nEntityObject* entity = 0);
    /// Get the zig-zag path
	bool FindPathZigZag (const vector3& start, 
                         const vector3& goal, 
                         nArray<vector3>* path, 
                         nArray<bool> &dirs,
                         nEntityObject* entity = 0);

	/// Set the width of the zigzag
	void SetWidth (float width);
	/// Set the height of the zigzag
	void SetHeight (float height);

#ifndef NGAME
    /// Enable/disable path cleaning
    static void SetPathCleaning( bool enable );
    /// Tell if path cleaning is enabled
    static bool GetPathCleaning();
    /// Enable/disable path rounding
    static void SetPathRounding( bool enable );
    /// Tell if path rounding is enabled
    static bool GetPathRounding();
    /// Enable/disable single smooth path
    static void SetSingleSmoothing( bool enable );
    /// Tell if single smooth path is enabled
    static bool GetSingleSmoothing();
#endif
	
	/// Get the instance of the nAStar object
	static nPathFinder* Instance();

private:
	/// Smoothes the path
	void SmoothPath (nEntityObject* entity, ePathStyle style);
	/// Remove unnecessaries points within the path
	void ClearPath (nArray<vector3>& path, nArray<bool>& smoothable);
	/// Says if there is a walkable path between two points
	bool IsWalkable (const vector3& start, const vector3& end) const;
	/// Changes squares by round turnings
	void RoundPath (nArray<vector3>& path, nEntityObject* entity = 0);
	/// Single smooth path
	void SingleSmoothPath (nArray<vector3>& path, float size = 2.f);
	/// Copies a path
	void CopyPath (const nArray<vector3>& path);

	/// Says if a given point is at the path
	bool ExistPoint (const vector3& point, const nArray<vector3>& path) const;

	/// Build a zig zag path
	void BuildZigZag (nEntityObject* entity, nArray<bool> &dirs);

	nArray<vector3> path;
    nArray<bool> smoothable;
	nAStar*	pathFinder;
	static nPathFinder* Singleton;
	float width, height;

#ifndef NGAME
    /// Path cleaning enabled?
    static bool clearPath;
    /// Path rounding enabled?
    static bool roundPath;
    /// Single path smoothing enabled?
    static bool singleSmoothPath;
#endif
};

//------------------------------------------------------------------------------
/**
    Instance
*/
inline
nPathFinder*
nPathFinder::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
	SetWidth
*/
inline
void
nPathFinder::SetWidth (float width)
{
	this->width = width;
}

//------------------------------------------------------------------------------
/**
	SetHeight
*/
inline
void
nPathFinder::SetHeight (float height)
{
	this->height = height;
}

#endif