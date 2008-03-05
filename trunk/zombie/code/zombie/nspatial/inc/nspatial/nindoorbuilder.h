#ifndef N_INDOORBUILDER_H
#define N_INDOORBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nIndoorBuilder
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief nIndoorBuilder helps to build indoors.
    
    (C) 2004  Conjurer Services, S.A.   
*/
#include "entity/nobjectinstancer.h"
#include "mathlib/plane.h"

class nTransformNode;
class nMeshBuilder;
class nSceneNode;
class nSpatialIndoorCell;
class bbox3;

#define N_OBBOX_MARGIN 0.01f

class nIndoorBuilder
{
    struct PortalInfo
    {
        vector3 vertices[4];
        plane p;
    };

public:

    /// constructor 1
    nIndoorBuilder(nEntityClass *indoorClass);
    /// destructor
    ~nIndoorBuilder();

    /// save all the indoor information
    bool Save();
    /// create a cell from a given mesh that wraps it
    bool CreateNewCell(nMeshBuilder& meshBuilder, int id);
    /// determine the cell that contains the given point
    int GetContainerCellId(const vector3 &point);
    /// insert a new object in the space
    bool InsertNewObject(nEntityObject *indoorBrush, vector3 &point);
    /// insert a physics object
    bool InsertPhysicsObject(nObject *object, bbox3 &box);
    /// set the portals' class
    void SetPortalsClass(nEntityClass *portalClass);
    /// set the indoor's shell
    void SetIndoorShell(const nString &name);
    /// insert a new portal in the space
    bool InsertNewPortal(const vector3 portalVertices[4], 
                         int cellId1, 
                         int cellId2,
                         bool c1ToC2Active,
                         bool c2ToC1Active,
                         ncSpatialPortal *&portal1,
                         ncSpatialPortal *&portal2,
                         float deactDist1,
                         float deactDist2);

    /// get the indoor brushes instancer
    nObjectInstancer *GetBrushesInstancer(int cellId);
    /// get the portals instancer
    nObjectInstancer *GetPortalsInstancer(int cellId);
    /// get the physics instancer
    nObjectInstancer *GetPhysicsInstancer(int cellId);
    /// add indoor brush to the instancer
    void AddBrushToInstancer(int cellId, nObject *indoorBrush);
    /// add portal to the instancer
    void AddPortalToInstancer(int cellId, nObject *portal);
    /// remove portal from the corresponding instancer
    void RemovePortalFromInstancer(int cellId, nObject *portal);
    /// add physics object to the instancer
    void AddPhysicsObjectToInstancer(int cellId, nObject *physicsObject);
    /// add an indoor brushes instancer
    void AddBrushesInstancer(int cellId, nObjectInstancer *instancer);
    /// add a portals instancer
    void AddPortalsInstancer(int cellId, nObjectInstancer *instancer);
    /// add a physics objects instancer
    void AddPhysicsInstancer(int cellId, nObjectInstancer *instancer);

private:

    /// indoor entity class we are building
    nRef<nEntityClass> m_indoorClass;
    /// temporal indoor used to build the cells structure
    nRef<nEntityObject> m_tempIndoor;
    /// portals' class
    nRef<nEntityClass> m_portalsClass;
    /// create and insert a portal in a cell, pointing to another one
    ncSpatialPortal *InsertPortalInCell(//const vector3 &clipRectCenter,
                                        nSpatialIndoorCell *parentCell,
                                        bool active,
                                        float deactDist);
    /// create and insert a portal in the outdoor
    ncSpatialPortal *InsertPortalOutdoor(//const vector3 &clipRectCenter,
                                         bool active,
                                         float deactDist);

    /// reverse a list of portal vertices
    void ReverseVertices(const vector3 portalVertices[4], vector3 *reversedVertices);

    /// array of indoor brushes instancers
    nKeyArray<nObjectInstancer*> *m_cellBrushesInstancers;
    /// array of portals instancers
    nKeyArray<nObjectInstancer*> *m_cellPortalsInstancers;
    /// array of physics instancers
    nKeyArray<nObjectInstancer*> *m_cellPhysicsInstancers;
};

//------------------------------------------------------------------------------
/**
    get the indoor brushes instancer for the given cell id
*/
inline
nObjectInstancer *
nIndoorBuilder::GetBrushesInstancer(int cellId)
{
    nObjectInstancer *instancer = 0;
    this->m_cellBrushesInstancers->Find(cellId, instancer);

    return instancer;
}

//------------------------------------------------------------------------------
/**
    get the portals instancer for the given cell id
*/
inline
nObjectInstancer *
nIndoorBuilder::GetPortalsInstancer(int cellId)
{
    nObjectInstancer *instancer = 0;
    this->m_cellPortalsInstancers->Find(cellId, instancer);

    return instancer;
}

//------------------------------------------------------------------------------
/**
    get the physics instancer for the given cell id
*/
inline
nObjectInstancer *
nIndoorBuilder::GetPhysicsInstancer(int cellId)
{
    nObjectInstancer *instancer = 0;
    this->m_cellPhysicsInstancers->Find(cellId, instancer);

    return instancer;
}

//------------------------------------------------------------------------------
/**
    add indoor brush to the corresponding instancer
*/
inline
void 
nIndoorBuilder::AddBrushToInstancer(int cellId, nObject *indoorBrush)
{
    nObjectInstancer *instancer = 0;
    
    if (!this->m_cellBrushesInstancers->Find(cellId, instancer))
    {
        instancer = static_cast<nObjectInstancer*>(nKernelServer::Instance()->New("nobjectinstancer"));
        instancer->Init(64, 64);
        this->m_cellBrushesInstancers->Add(cellId, instancer);
    }

    instancer->Append(indoorBrush);
}

//------------------------------------------------------------------------------
/**
    add portal to the corresponding instancer
*/
inline
void 
nIndoorBuilder::AddPortalToInstancer(int cellId, nObject *portal)
{
    nObjectInstancer *instancer = 0;
    
    if (!this->m_cellPortalsInstancers->Find(cellId, instancer))
    {
        instancer = static_cast<nObjectInstancer*>(nKernelServer::Instance()->New("nobjectinstancer"));
        instancer->Init(64, 64);
        this->m_cellPortalsInstancers->Add(cellId, instancer);
    }

    instancer->Append(portal);
}

//------------------------------------------------------------------------------
/**
    remove portal from the corresponding instancer
*/
inline
void 
nIndoorBuilder::RemovePortalFromInstancer(int cellId, nObject *portal)
{
    nObjectInstancer *instancer = 0;
    
    if (this->m_cellPortalsInstancers->Find(cellId, instancer))
    {
        int index = instancer->FindIndex(portal);
        if (index == -1)
        {
            return;
        }

        instancer->Erase(index);
    }
}

//------------------------------------------------------------------------------
/**
    add physics object to the corresponding instancer
*/
inline
void 
nIndoorBuilder::AddPhysicsObjectToInstancer(int cellId, nObject *physicsObject)
{
    nObjectInstancer *instancer = 0;
    
    if (!this->m_cellPhysicsInstancers->Find(cellId, instancer))
    {
        instancer = static_cast<nObjectInstancer*>(nKernelServer::Instance()->New("nobjectinstancer"));
        instancer->Init(64, 64);
        this->m_cellPhysicsInstancers->Add(cellId, instancer);
    }

    instancer->Append(physicsObject);
}

//------------------------------------------------------------------------------
/**
    add an indoor brushes instancer
*/
inline
void 
nIndoorBuilder::AddBrushesInstancer(int cellId, nObjectInstancer *instancer)
{
    this->m_cellBrushesInstancers->Add(cellId, instancer);
}

//------------------------------------------------------------------------------
/**
    add a portals instancer
*/
inline
void 
nIndoorBuilder::AddPortalsInstancer(int cellId, nObjectInstancer *instancer)
{
    this->m_cellPortalsInstancers->Add(cellId, instancer);
}

//------------------------------------------------------------------------------
/**
    add a physics objects instancer
*/
inline
void 
nIndoorBuilder::AddPhysicsInstancer(int cellId, nObjectInstancer *instancer)
{
    this->m_cellPhysicsInstancers->Add(cellId, instancer);
}

#endif

