#ifndef N_3DS_GRAPHICS_EXPORTER_H
#define N_3DS_GRAPHICS_EXPORTER_H

#include "util/nmaptabletypes.h"
#include "n3dsexportsettings/n3dsExportSettings.h"

//------------------------------------------------------------------------------
/**
    @class nRoot
    @ingroup n3dsMaxGraphics
    @brief The basic function is collect the information for graphics module.

    (C) 2005 Conjurer Services, S.A. 
*/
//------------------------------------------------------------------------------

class IGameNode;
class nEntityClass;
class neIndoorBrushClass;
class neIndoorClass;
class nePortalClass;
class nSceneNode;
class nRoot;
class neMirageClass;

//------------------------------------------------------------------------------
class n3dsGraphicsExport
{
public:
    /// defalut constructor
    n3dsGraphicsExport();
    /// default destructor
    ~n3dsGraphicsExport();
    void Export();
    /// export the brush asset
    void ExportBrushClass(nEntityClass* entityClass, nArray<n3dsAssetInfo> assetPathArray);
    /// init the indoor
    void InitExportIndoor(nEntityClass* indoorClass, nEntityClass* indoorBrushClass, nEntityClass* portalClass, nEntityClass* indoorFacadeClass);
    /// export the indoor
    void ExportIndoor(nEntityClass* indoorClass, nEntityClass* indoorBrushClass, nEntityClass* portalClass, nEntityClass* indoorFacadeClass,nArray<n3dsAssetInfo> assetPathArray );
    
    /// check if is blended node
    static bool IsBlendedNode( IGameNode* igame );
    /// export skinned character
    void ExportCharacterClass(nEntityClass* entityClass, nArray<n3dsAssetInfo> assetPathArray, nArray<n3dsAssetInfo> ragAssetPathArray );

protected:
    nSceneNode*              rootNode;
    nMapTable<nSceneNode*>*  mapCellIdToNohContainer;
    /// Save the sceneNode
    void SaveScene( nEntityClass* entityClass ) const;
    /// Save the sceneNode for asset
    void SaveSceneForAsset( const nString& assetPath ) const;
    /// Create the sceneNode for this entity class
    nSceneNode* CreateSceneNode(nEntityClass* entityClass);
    /// Create the sceneNode for this entity class
    nSceneNode* CreateSceneNodeForAsset(const nString& assetPath);
    /// Create graphic in portal
    void ExportIndoorBrush(IGameNode* igame, int CellId, nEntityClass* indoorBrushClass);
    void ExportIndoorPortal(IGameNode* igame, int portalId, nEntityClass* portalClass );
    void ExportIndoorFacade(IGameNode* igame, nEntityClass* indoorWrapClass , nArray<n3dsAssetInfo> assetPathArray );
    void SortScene(nRoot* node) const;
    
};

#endif
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------