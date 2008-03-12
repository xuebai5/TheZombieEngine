#include "precompiled/pchn3dsmaxexport.h"

#pragma warning( push, 3 )
#include "Max.h"
#include "istdplug.h"
#include "iparamb2.h"
#pragma warning( pop )
//------------------------------------------------------------------------------
#include "n3dsmaterial/n3dsmateriallist.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dsscenelist.h"
#include "n3dsexporters/n3dsfileserver.h"
#include "n3dsexporters/n3dsbitmaps.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsanimator/nanimatorbuilderserver.h"
#include "n3dsanimationexport/n3dsskeleton.h"
//------------------------------------------------------------------------------
#include "nmaterial/nmaterialnode.h"
#include "kernel/nfileserver2.h"
#include "nmaterial/nmaterialserver.h"

const char pathMaterialTypes[] =  "/usr/material/materialTypes";
const char pathListMaterial[]  =  "/usr/material/listmaterial";
const char pathArtistTextureRepo[] =  "e:/renaissance/trunk/texturas/";
const char* pathShaderTypeLibrary = "wc:libs/gfxmaterials/";

//------------------------------------------------------------------------------
/**
*/
n3dsMaterialList::n3dsMaterialList() : 

    mtlToIndex(71) ,
    index2MaterialExport(64,64)
{
    animBuilder = n_new(nAnimatorBuilderServer);
    n_assert(animBuilder);
    //InitializedData();
}

//------------------------------------------------------------------------------
/**
*/
n3dsMaterialList::~n3dsMaterialList()
{
    n_delete(animBuilder);

    // Clean listMaterialType
    listMatTypeLibrary.Begin();
    nMatTypePropBuilder** ptr = listMatTypeLibrary.Next();
    while ( ptr )
    {
        nMatTypePropBuilder* matType(*ptr);
        if ( matType)
        {
            n_delete( matType );

        }
        ptr = listMatTypeLibrary.Next();
    }

}

//------------------------------------------------------------------------------
/**
    return the id of material, 
    @param material the 3ds max material
    @return the id of n3dsMaterial ( interface for material)
*/
int
n3dsMaterialList::GetMaterialId3ds(Mtl* mtl)
{
    int* index = mtlToIndex[mtl];
    if (index )
    {
        return *index;
    } else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    return the id of material, if material is a SHELL_MATERIAL return a  baked material
    @param material the igame material
    @return the id of n3dsMaterial ( interface for material)
*/
int
n3dsMaterialList::GetMaterialId(IGameMaterial *material)
{
    if ( 0 == material ) //return the default material
    {
        return 0;
    }

    Mtl* mtl = material->GetMaxMaterial();
    mtl = SkeepShellMaterial( mtl );
    n_assert( 0 == mtl ||  FALSE == mtl->IsMultiMtl() );
    return GetMaterialId3ds( mtl );
}

//------------------------------------------------------------------------------
/**
    return the id of material, 
    if material is a SHELL_MATERIAL return a  baked material

    example: When in the 3ds launch a lightmap create a shell_material
    the shell_material has a original material and baked material
    the baked material has a lightmap

    @param material the 3ds max material
    @param num the number of submaterial 0..n
    @return the id of n3dsMaterial ( interface for material)
*/
int 
n3dsMaterialList::GetSubMaterialId(IGameMaterial *material,int num)
{
    if ( 0 == material ) //return the default material
    {
        return 0;
    }

    Mtl* mtl = material->GetMaxMaterial();
    mtl = SkeepShellMaterial( mtl );

    if ( 0 == mtl || FALSE == mtl->IsMultiMtl())
    {
        return 0;
        //@todo log message
    } else
    {
        mtl = mtl->GetSubMtl(num);
        return GetMaterialId3ds( mtl );
    }

}

//------------------------------------------------------------------------------
/**
    Get a material root of 3dsMax scene. 
    And create all n3dsMateial interface for each 3dsmax material
*/
void 
n3dsMaterialList::InitializedData(/*const nString& className*/)
{
    nString className = "trick";
    IGameScene *pIgame=n3dsExportServer::Instance()->GetIGameScene();
    n_assert(pIgame);

    // Initialize the animator Builder
    GetAnimatorBuilder()->Init( className );

    // Create a root nodes for materials
    rootMaterialNode = (nRoot*) nKernelServer::ks->New("nroot", pathListMaterial);
    rootMaterialType = (nRoot*) nKernelServer::ks->New("nroot", pathMaterialTypes);

    // Set in builder the path on generate the materials
    this->builder.pathSurfaceOut = pathListMaterial;
    this->builder.pathMatTypeOut = pathMaterialTypes;
    

    // Create a default material
    {
        int position = 0;
        n3dsMaterial nebMaterial;
        nebMaterial.SetMaterial( 0 );  // The "error" material
        mtlToIndex.Add( 0, &position );
        index2MaterialExport.Append( nebMaterial );
    }

    // For each graphic object append material
    {
        n3dsSceneList list = n3dsExportServer::Instance()->GetMaxIdOrderScene();
        n3dsSceneList::iterator index( list.Begin( n3dsObject::graphics ) );
        index.ShowProgressBar( "Export Materials: "); 
        for ( ; index != list.End() ; ++index)
        {
            IGameNode* node = (*index).GetNode();
            if ( !( node->IsNodeHidden()  || n3dsSkeleton::IsBoneNode( node ) ) )
            {
                nString uniqueName;
                uniqueName.Format("%.8X", node->GetNodeID() );
                node->GetIGameObject()->InitializeData();
                bool valid = Append( node->GetNodeMaterial() , uniqueName);
                N3DSERRORCOND( /*name*/  materialExport, 
                             /*COND*/  !valid,
                             /*LOG*/( 0, "ERROR: use by \"%s\" " , node->GetName()  )
                            );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @return a n3dsmaterial by id
*/
const n3dsMaterial &
n3dsMaterialList::Getn3dsMaterial(int idx)
{
    return index2MaterialExport[idx];
}


//------------------------------------------------------------------------------
/**
*/
void
n3dsMaterialList::Preparen3dsMaterial(int idx)
{
    index2MaterialExport[idx].PrepareAndExport();
}

//------------------------------------------------------------------------------
/**
    Assign material to shape
    @param shape the shape to assign material
    @param idMaterial id of n3dsMaterial
    @param shapeToMaterial  shape of properties asigned to material, example skinned
*/
void 
n3dsMaterialList::SetMaterial(nGeometryNode* shape, int idMaterial, nMatTypePropBuilder* shapeToMaterial)
{
    n3dsMaterial& material = this->index2MaterialExport[idMaterial];
    //Call this method only if the material is exported.
    material.PrepareAndExport();

    nAbstractPropBuilder& matShape   = material.GetMatShape(); 
    nAbstractPropBuilder& matSurface = material.GetMatSurface();

    //shape->SetSurface( material.getName( ) );
    builder.SetMaterialToShape (shape, &matSurface, &matShape , shapeToMaterial, material.GetUniqueName() );

}

//------------------------------------------------------------------------------
/**
    Assign material to shape
    @param shape the shape to assign material
    @param shapeToMaterial  shape of properties asigned to material, example skinned
*/
void 
n3dsMaterialList::SetShadowMaterial(nGeometryNode* shape, nMatTypePropBuilder* shapeToMaterial)
{
    nAbstractPropBuilder matSurface;
    nMatTypePropBuilder matType;
    nString name("shadowvolume_");
    matType.AddParam("mat", "shadowvolume");
    matSurface.SetMaterial( matType);
    if ( shapeToMaterial )
    {
        name += shapeToMaterial->GetUniqueString();
    }
    builder.SetMaterialToShape (shape, &matSurface, 0, shapeToMaterial, name);
}



//------------------------------------------------------------------------------
/**
*/
nMatTypePropBuilder*
n3dsMaterialList::GetMatTypePropBuilder(const char* materialTypeName)
{
    nMatTypePropBuilder** ptrData = this->listMatTypeLibrary[materialTypeName];
    nMatTypePropBuilder* matLibrary(0);
    if ( ptrData )
    {
        matLibrary =  *ptrData;
    } else
    {
        
        nString path(pathShaderTypeLibrary);
        path+= materialTypeName;
        path +=".n2";
        nMaterial* matType = nMaterialServer::Instance()->LoadMaterialFromFile( path.Get() );

        if ( matType)
        {
            matLibrary = n_new( nMatTypePropBuilder );
            matLibrary->GetFrom(*matType);
        } else
        {
            matLibrary = 0;
        }

        this->listMatTypeLibrary.Add( materialTypeName ,&matLibrary );
    }

    return matLibrary;
}


//------------------------------------------------------------------------------
/**
    if the material is shell material return if backed material is multimaterial.
    Important the shell material with a simple material return false,
    This function is used for split a mesh
    @param material IGame material
    @return if the material is a multimaterial
*/
bool
n3dsMaterialList::IsMultitype(IGameMaterial *material)
{
    Mtl* mat;
    mat=material->GetMaxMaterial();
    if (!mat) return false;

    while ((mat) && (mat->ClassID() == Class_ID(BAKE_SHELL_CLASS_ID, 0)))
    {
        mat=mat->GetSubMtl(1);
    }

    N3DSERRORCOND( /*name*/ materialExport,
                /*COND*/  material && ( mat == 0),
                /*LOG*/( 0, "ERROR: The shell material has invalid original material ,  \"%s\" " , material->GetMaterialName() )
                );
    return ( mat && TRUE == mat->IsMultiMtl());
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsMaterialList::Append( IGameMaterial* mtl , const nString& uniqueName )
{
    if ( 0 == mtl) 
    {
        return false;
    } else
    {
        return Append( mtl->GetMaxMaterial() , uniqueName );
    }
}


//------------------------------------------------------------------------------
/**
    Recursive append materials 
*/
bool 
n3dsMaterialList::Append( Mtl* material , const nString& uniqueName)
{ 
    bool result = true;
    if ( 0 == material)
    {
        result = false;
    } else
    {
        // if is baked shell material only export the original material
        if ( material->ClassID() == Class_ID(BAKE_SHELL_CLASS_ID, 0)  )
        {
            result = Append( material->GetSubMtl(1) , uniqueName );

        } else if ( TRUE == material->IsMultiMtl() ) // Multimaterial
        {
            for (int i=0; i< material->NumSubMtls(); i++) 
            {   
                //fix the problem with arbitray slot number
                if ( material->GetSubMtl(i) )
                {
                    bool valid;
                    valid = Append( material->GetSubMtl(i) , uniqueName  + "_" + i);
                    result &= valid;
                    N3DSERRORCOND( /*name*/  materialExport, 
                                /*COND*/  !valid,
                                /*LOG*/( 0, "ERROR: Invalid submaterial number  \"%d\" " , i+1  )
                                );
                }
            }


        } else
        {
            int* index = mtlToIndex[material];
            if ( 0 == index) // if not found
            {
                int position = index2MaterialExport.Size();
                n3dsMaterial nebMaterial;
                nebMaterial.SetMaterial( material );
                nebMaterial.SetUniqueName( uniqueName );
             
                mtlToIndex.Add( material, &position );
                index2MaterialExport.Append( nebMaterial );
                result = nebMaterial.IsValid();
            }
        }   
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Skeep all shel material, and get the backet material
    example the lightmap is a shell has two submaterials
    the original material
    the backed material, it's has the lightmap 
    @param mtl the max material
    @return if the backed material
*/
Mtl* 
n3dsMaterialList::SkeepShellMaterial(Mtl *mtl)
{
    while ( mtl && mtl->ClassID() == Class_ID(BAKE_SHELL_CLASS_ID, 0) )
    {
        mtl = mtl->GetSubMtl(1);
    }
    //@todo log message if mtl = 0
    return mtl;
}

//------------------------------------------------------------------------------
/**
    Append to list of bumMaps fopr create a normalMaps for only generates only one
    @param 
    @return the new name or empty string if file not exist
*/
nString 
n3dsMaterialList::AppendBumpMapTexture( const nString& fileName  )
{
    nString newName;
    BumpMap bumpMap;
    nString repo(pathArtistTextureRepo);
 //   const bool& collectTextures = n3dsExportServer::Instance()->GetSettings().collectTextures;
    bumpMap.name = fileName;
    bumpMap.name.ToLower();
    bumpMap.name.ConvertBackslashes();

    nString* tmp = listBumpMaps[bumpMap];

    if (tmp)
    {
        newName = *tmp;
    } else
    {
        nFile *file = nFileServer2::Instance()->NewFileObject();
        bool fileValid = (file->Open( fileName.Get() , "r" ) ) && ( file->GetSize() > 0 );
        file->Release();

        if ( fileValid )
        {
            int index = bumpMap.name.IndexOf( repo, 0 ); // use the lower string
            fileValid = 0 == index;
            if ( fileValid )
            {
                newName = bumpMap.name.ReplaceBegin( repo,  "tmpwc:export/generatedtextures/");
                newName = newName.ExtractToLastSlash();
                newName += "nor100";
                newName+= "_";
                newName+= bumpMap.name.ExtractFileName();
                newName.StripExtension();
                newName+=".dds";
            }
        } 
        
        
        if ( !fileValid )
        {
            newName.Clear();
        }

        listBumpMaps.Add( bumpMap, &newName);       
    }
    newName = newName.ReplaceBegin( "tmpwc:" , "wc:" );
    return newName;
}

//------------------------------------------------------------------------------
/** 
    Enque in file server the textures for copy
    @param 
    @return the new name of texture or emty if file not existed
*/
nString 
n3dsMaterialList::AppendTexture( const nString& fileName )
{
    const bool& gameLibrary = n3dsExportServer::Instance()->GetSettings().gameLibrary;
    const bool& collectTextures = n3dsExportServer::Instance()->GetSettings().collectTextures;
    nString name= fileName;
    name.ToLower();
    name.ConvertBackslashes();
    nString newName(name);

    nFile *file = nFileServer2::Instance()->NewFileObject();
    bool valid = (file->Open( fileName.Get() , "r" ) ) && ( file->GetSize() > 0 );
    file->Release();

    if ( valid)
    {
        if ( gameLibrary || collectTextures )
        {
            // Replace the begin of path by wctextutres if the texture is not in 
            nString repo(pathArtistTextureRepo);
            int index = name.IndexOf( repo, 0 ); // use the lower string
            valid = 0 == index;
            if ( valid )
            {
                newName = name.ExtractRange( repo.Length() , name.Length() - repo.Length() );
                newName = "wc:export/textures/" + newName;
                if ( collectTextures )
                {
                    n3dsExportServer::Instance()->GetQueueFileServer()->QueueCopyFile( fileName, newName , gameLibrary );
                }
                newName = name.ReplaceBegin( repo , "wctextures:") ;
            } else
            {
                int index = name.IndexOf( "home:", 0); //Special case use system textures
                valid = 0 == index;
            }
            //@todo enque file Save
        }
    }
    
    
    if (!valid)
    {
        newName.Clear();
    }



    return newName;
}

//------------------------------------------------------------------------------
/**
    @param 
    @return
*/
void 
n3dsMaterialList::CreateBumpMaps()
{
    BumpMap bumpMap;
    nString* newName;

    int  size = listBumpMaps.GetCount();
    float count = 0;

    n3dsExportServer::Instance()->ProgressBarStart( "Create normalmaps " );

    for ( listBumpMaps.Begin(), listBumpMaps.Next( bumpMap, newName ) ; 0 != newName ; listBumpMaps.Next( bumpMap, newName ) )
    {
        n3dsExportServer::Instance()->ProgressUpdate( count / size , *newName );
        if (! newName->IsEmpty() )
        {
            n3dsBitmaps::BuildNormText( bumpMap.name , *newName );
        }
        count++;
    }

    n3dsExportServer::Instance()->ProgressBarEnd();
}


//------------------------------------------------------------------------------
/** 
    Create a normal map from bumpMap
    @param fileIn
*/
void 
n3dsMaterialList::SaveResourceFile()
{
    this->CreateBumpMaps();
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
