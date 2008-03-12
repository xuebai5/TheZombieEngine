#include "precompiled/pchn3dsmaxexport.h"
#pragma warning( push, 3 )
#include "Max.h"
#pragma warning( pop )
#include "n3dsmaterial/nmaterialbuilderserver.h"
#include "nscene/nshapenode.h"
#include "nmaterial/nmaterialnode.h"
#include "nmaterial/nmaterialserver.h"

const char* SHADER_TYPE_LIBRARY = "wc:libs/gfxmaterials/";

void 
nMaterialBuilderServer::SetMaterialToShape(nGeometryNode*            shape,
                                           nAbstractPropBuilder*  matToSurface ,
                                           nAbstractPropBuilder*  matToShape   ,
                                           nMatTypePropBuilder*   shapeToMaterial,
                                           const nString&         Name )
{

    n_assert(matToSurface)
    nAbstractPropBuilder mainSurface = *matToSurface;

    if (matToShape && matToShape->matTypeIsValid)
    {
        mainSurface.matType += matToShape->matType;
    }


    if (shapeToMaterial &&   
        ( mainSurface.GetAbstractType() == nAbstractPropBuilder::NMATERIAL  ||
          mainSurface.GetAbstractType() == nAbstractPropBuilder::SHADERTYPELIBRARY  // Custom material
        )
       )
    {
        mainSurface.matType += *shapeToMaterial;
    }

    if (matToShape)
    {
        matToShape->SetTo( shape );
    }
 
    nStrPath* node;
    node =   (nStrPath*) (this->surfaceHash.Find( mainSurface.GetUniqueString() ));
    if (!node)
    {
       node = AppendSurface( mainSurface , Name);
       n_assert(node);
    }

    nString path(this->pathSurfaceOut);
    path += "/";
    path += node->path;
    shape->SetSurface( path.Get() );
}


nMaterialBuilderServer::nStrPath * 
nMaterialBuilderServer::AppendSurface (nAbstractPropBuilder& Surface, const nString& Name)
{
    n_assert(nKernelServer::ks);
    nStrPath* hashNode = n_new(nStrPath);
    n_assert(hashNode);
 
    surfaceCount++;
    hashNode->SetName( Surface.GetUniqueString() );
    hashNode->path = "Sur_";
    hashNode->path += Name;
    nString path = this->pathSurfaceOut + "/";
    path += hashNode->path;

    nAbstractShaderNode* node;
    node = (nAbstractShaderNode*) nKernelServer::ks->New(Surface.GetNameClass() , path.Get() );
    n_assert(node);
//    node->SetComment( Surface.getUniqueString() );
    hashNode->SetPtr((void*)node);
    this->surfaceHash.Add(hashNode);
    Surface.SetTo( node );
    if (Surface.thisType == nAbstractPropBuilder::NMATERIAL ) //Set the  nMaterialNode
    {
        this->SetMaterial( static_cast<nMaterialNode*>(node) ,Surface , Name );
        node->SetName(hashNode->path.Get());

    } else if (Surface.thisType == nAbstractPropBuilder::SHADERTYPELIBRARY )
    {
        nMaterialNode* MaterialNode = static_cast<nMaterialNode*>(node);
        nString path(SHADER_TYPE_LIBRARY);
        if ( ! Surface.IsCustomShader() )
        {
            path+= Surface.GetMaterialTypeName();
            path +=".n2";
            nMaterial* matType = nMaterialServer::Instance()->LoadMaterialFromFile( path.Get() );
            if ( matType)
            {   
                MaterialNode->SetMaterial( matType->GetFullName().Get() );
                MaterialNode->SetName(hashNode->path.Get());
            } else
            {
                //N3DSWARN( materialExport , (3, "WARNING: %s : has not a valid shader type %s", hashNode->path.Get() , Surface.GetMaterialTypeName() ) );
                // Export as custom
                this->SetMaterial( MaterialNode ,Surface , Name );
            }
        } else
        {
            // Export as custom
            this->SetMaterial( MaterialNode ,Surface , Name );
        }
        node->SetName(hashNode->path.Get());  
    }
    return hashNode;
}



//------------------------------------------------------------------------------
/**
    Set the nmaterial to nmaterialnode
    @param  MaterialNode
    @parama surface. The nAbstractPropBuilder what contains the  material type
    @param Name , the unique name of material.
*/
void
nMaterialBuilderServer::SetMaterial(nMaterialNode* MaterialNode,nAbstractPropBuilder& Surface,const nString& Name)
{
    nMatTypePropBuilder matType = Surface.GetMatType();
    nStrPath* nodeMat;
    nodeMat =   (nStrPath*) (this->materialHash.Find( matType.GetUniqueString() ));

    if (!nodeMat)
    {
        nodeMat = AppendMaterial( matType , Name );
        n_assert(nodeMat);
    }       
    MaterialNode->SetMaterial( nodeMat->path.Get());
}

nMaterialBuilderServer::nStrPath* 
nMaterialBuilderServer::AppendMaterial (nMatTypePropBuilder& Material, const nString& Name )
{
    n_assert(nKernelServer::ks);
    nStrPath* hashNode = n_new(nStrPath);
    n_assert(hashNode);

    hashNode->id = this->matTypeCount;
    this->matTypeCount++;

    hashNode->path = "Mat_";
    hashNode->path += Name;
    hashNode->SetName( Material.GetUniqueString() );

    nString path = this->pathMatTypeOut + "/";
    path += hashNode->path;
    hashNode->path = path;

    nMaterial* node;
    node = (nMaterial*) nKernelServer::ks->New("nmaterial" , path.Get() );
    hashNode->SetPtr((void*)node);
    this->materialHash.Add( hashNode);
    Material.SetTo(*node);
    return hashNode;
}