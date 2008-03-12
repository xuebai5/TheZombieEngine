#ifndef N_MATERIAL_BUILDER_EXPORTER_H
#define N_MATERIAL_BUILDER_EXPORTER_H

#include "nabstractpropbuilder.h"
#include "util/nstring.h"
#include "util/nhashtable.h"

//------------------------------------------------------------------------------
/**
    @class nMaterialBuilderServer
    @ingroup n3dsMaxMaterial
    @brief is a server create a unique nMaterialNode, nSurfaceNode and nMaterial by properties.
    Assign the material to shape
*/
class nGeometryNode;
class nMaterialNode;

class nMaterialBuilderServer
{
public:
    /// in this path, the nMaterialNode and nSurfaceNode were created 
    nString pathSurfaceOut; 
    nString pathSurfaceIn;
    /// in this path, the nMaterial were created 
    nString pathMatTypeOut;
    nString pathMatTypeIn;

    nMaterialBuilderServer();
    ~nMaterialBuilderServer();

    /// Assign the material to shape
    void SetMaterialToShape(nGeometryNode*            shape, 
                            nAbstractPropBuilder*  matToSurface ,
                            nAbstractPropBuilder*  matToShape   = 0,
                            nMatTypePropBuilder*   shapeToMaterial = 0,
                            const nString&         Name = "");

    

private:

    class nStrPath  : public nStrNode
    {
        public:
        nString path;
        int id;
    };

    

    nHashTable surfaceHash;
    nHashTable materialHash;

    nStrPath* AppendSurface  (nAbstractPropBuilder& Surface, const nString& Name );
    nStrPath* AppendMaterial (nMatTypePropBuilder&  Material, const nString& Name );
    void SetMaterial(nMaterialNode* MaterialNode,nAbstractPropBuilder& Surface,const nString& Name);

    int surfaceCount;
    int matTypeCount;

};
inline
nMaterialBuilderServer::nMaterialBuilderServer() :
    surfaceHash(71),
    materialHash(29),
    surfaceCount(0),
    matTypeCount(0)
{

}
inline
nMaterialBuilderServer::~nMaterialBuilderServer()
{
    surfaceHash.removeAll();       
    materialHash.removeAll();
}

#endif