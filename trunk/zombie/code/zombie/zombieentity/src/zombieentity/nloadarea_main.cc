#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  nloadarea_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/nloadarea.h"
#include "zombieentity/nloaderserver.h"

nNebulaScriptClass(nLoadArea, "nroot");

#ifndef NGAME
uint nLoadArea::uniqueAreaId = 0;
#endif

//------------------------------------------------------------------------------
/**
    constructor
*/
nLoadArea::nLoadArea() :
    isDirty(false),
    isLoaded(false),
    areaType(InvalidType)
{
    #ifndef NGAME
    this->areaId = uniqueAreaId++;
    this->statsValid = false;
    this->ResetStatsCounters();
    #endif
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nLoadArea::~nLoadArea()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nLoadArea::SetSpaceId(nEntityObjectId spaceId)
{
    this->refSpace.set(spaceId);
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
nEntityObjectId
nLoadArea::GetSpaceId() const
{
    return this->refSpace.getid();
}

//------------------------------------------------------------------------------
/**
*/
nEntityObject*
nLoadArea::GetSpaceEntity()
{
    return this->refSpace.isvalid() ? this->refSpace.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nLoadArea::BeginCells(int /*num*/)
{
    this->areaCells.Reset();
}

//------------------------------------------------------------------------------
/**
*/
void
nLoadArea::SetCellAt(int index, int cellId)
{
    this->areaCells.Set(index, cellId);
}

//------------------------------------------------------------------------------
/**
*/
int
nLoadArea::GetCellAt(int index)
{
    return this->areaCells.At(index);
}

//------------------------------------------------------------------------------
/**
*/
void
nLoadArea::EndCells()
{
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nLoadArea::AddCell(int cellId)
{
    this->areaCells.Append(cellId);
    this->isDirty = true;
    nLoaderServer::Instance()->UpdateAreas();
}

//------------------------------------------------------------------------------
/**
*/
void
nLoadArea::RemoveCell(int cellId)
{
    nArray<int>::iterator cellIter = this->areaCells.Find(cellId);
    if (cellIter)
    {
        this->areaCells.Erase(cellIter);
        this->isDirty = true;
        nLoaderServer::Instance()->UpdateAreas();
    }
}

//------------------------------------------------------------------------------
/**
*/
int
nLoadArea::GetNumCells() const
{
    return this->areaCells.Size();
}

//------------------------------------------------------------------------------
/**
    Subclasses should override this method to check distances to
    load or unload neighbor loading areas.
*/
void
nLoadArea::Trigger(const vector3& /*position*/, const int /*cameraCellId*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Subclasses must override this method to compute minimum linear distance
    from the camera position to any given set of control points in the area.
*/
float
nLoadArea::GetMinDistanceSq(const vector3& /*cameraPos*/)
{
    return 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
bool
nLoadArea::Load()
{
    this->isLoaded = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nLoadArea::Unload()
{
    this->isLoaded = false;
}


//------------------------------------------------------------------------------
/**
*/
bool
nLoadArea::ContainsCell(int cellId)
{
    return (this->areaCells.Find(cellId) != 0);
}
