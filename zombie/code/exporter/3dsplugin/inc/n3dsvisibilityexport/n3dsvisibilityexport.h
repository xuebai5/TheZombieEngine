#ifndef N_3DS_VISIBILITY_EXPORTER_H
#define N_3DS_VISIBILITY_EXPORTER_H

#pragma warning( push, 3 )
#include "max.h"
#include "igame/igame.h"
#pragma warning( pop )

#include "util/narray.h"
#include "mathlib/vector.h"
#include "mathlib/bbox.h"
#include "mathlib/obbox.h"

#include <map>

class INode;
class nRoot;
class nSceneNode;
class nMeshBuilder;
class nIndoorBuilder;
class n3dsWrapPortalHelper;
class nString;
class neIndoorClass;
class nePortalClass;
class nePortal;
class neIndoorBrushClass;
class neIndoorBrush;
class ncSpatialPortal;
class nEntityClass;
class nEntityObject;

//------------------------------------------------------------------------------
/**
    @class n3dsVisibilityExport
    @ingroup n3dsMaxVisibility
    @brief Visibility export is a server for use other ecport modules, example graphics.
    The basic function is collect the information for visibility module.
    First step is find in the scene the visibility cell , and create it in indoorbuilder.
    Second step is find the portals and conect with cells.
    (C) 2005 Conjurer Services, S.A. 
*/

class n3dsVisibilityExport
{
public:
    class Portal
    {
    public:
        Portal() :  ePortal1(0), ePortal2(0) {};
        ~Portal(){/*Empty*/}; //need for nArray
        // entity portal 1
        ncSpatialPortal * ePortal1;
        // entity portal 2
        ncSpatialPortal * ePortal2;
        /// oriented bounding box container
        obbox3       container;
        /// the  cell id1
        int id1;
        /// the cell id2
        int id2;
        /// C2 is visible from c1
        bool c1ToC2Active;
        /// C1 is visivle form c2
        bool c2ToC1Active;
        //// clip points
        vector3 points[4];
        /// deactivation distances
        float deactDist1;
        float deactDist2;
    };


    /// default constructor
    n3dsVisibilityExport();
    /// default destructor
    ~n3dsVisibilityExport();
    /// Return is a portal
    bool IsInPortal( vector3 const& pos, int& id);
    /// return a portal info;
    const Portal& GetPortal(int idx); 
    /// return is in a cell
    bool IsInCell(vector3 const& pos, int& id);
    /// Create a scenenode in corresponding noh 
    nSceneNode* CreateRootInScene(nString const& className, vector3 position, nString const& name);
    /// collect info from scene a create nrootNode
    void Init(nEntityClass * indoorClass , nEntityClass * indoorBrushClass, nEntityClass * portalClass);
    /// return the numbers of portals
    int GetNumPortals();
    /// return the numbers of cells
    int GetNumCells();
    ///  insert the brush in the indoor
    bool InsertIndoorBrush(nEntityObject *indoorBrush, vector3 &point);
    ///
    bool Save();
    /// Export ocluder and save it
    bool ExportOccluders(nEntityClass* entityClass);
    /// get the indoor builder
    nIndoorBuilder * GetIndoorBuilder();

protected:
    /// the std::Map the key is max node and value is the integer
    typedef std::map< INode*, int> INodeToInt;
    /// the number of cells
    int countCells;
    /// the number o portals
    int countPortals;
    /// the std::Map the key is max node and value is the Cell's ID
    INodeToInt       mapMaxToCell;
    /// The position in array is the portal's ID
    nArray<Portal>   mapIdPortalToRoot;
    /// The indor builder
    nIndoorBuilder *indoorBuilder;
    /// Collect info from cells and create's it
    void CreateCells(void);
    /// Create Portals  portals
    void CreatePortals(void);
    /// create a meshbuilder from igameNode;
    nMeshBuilder* CreateMeshBuilder(IGameNode* node);
    ///  Set To portal cell if from portal helper wrapper
    void SetCellIdFromPortalHelper(const n3dsWrapPortalHelper& wrap, Portal& portal);
    ///  Set portal's deactivation distance from portal helper wrapper
    void SetDeactDistFromPortalHelper(const n3dsWrapPortalHelper& wrap, Portal& portal);
    ///  Set To portal cell if from portal helper wrapper
    void SetOBBoxFromPortalHelper(IGameNode* node, const n3dsWrapPortalHelper& wrap, Portal& portal);
    /// Return the quad from 
    void GetQuad(IGameNode* node, vector3 points[4] );
    /// Return double sided from occluder
    bool ExtractDoubleSidedFromOccluder(INode* node);

};

#endif