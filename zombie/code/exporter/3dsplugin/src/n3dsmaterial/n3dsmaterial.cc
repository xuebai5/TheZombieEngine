#include "precompiled/pchn3dsmaxexport.h"
//------------------------------------------------------------------------------
#include "n3dsmaterial/n3dsmaterial.h"
#include "n3dsmaterial/n3dsmateriallist.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsanimator/nanimatorbuilderserver.h"
//------------------------------------------------------------------------------
#include "nmaterial/nmaterialnode.h"
#include "kernel/nfileserver2.h"
#include "tools/nmeshbuilder.h"
#include "nmaterial/nmaterialserver.h"
#include <sys/stat.h>


const char defaultNameMaterial[] = "none";
const char*  pathTextureSystem   = "wc:libs/system/textures/";

//------------------------------------------------------------------------------
/**
*/
void
n3dsMaterial::CreateNode()
{
    if (mtl)
    {
        nameMaterial = this->mtl->GetName();
    } else
    {
        nameMaterial = defaultNameMaterial;
    }

    nameMaterial.ConvertSpace();
    if (!notExport)
    {
        this->propMatSurface.SetAbstractType(nAbstractPropBuilder::NMATERIAL);
        nMatTypePropBuilder matType;
        matType.AddParam("env", "ambientlighting");
        matType.AddParam("env", "diffuselighting" );

        if (this->propFogDisable)
        {
            matType.AddParam("env","fogdisable");
        }

        if (this->propBillBoard)
        {
            matType.AddParam("deform", "billboard");
        } 

        if (this->propSwing)
        {
            matType.AddParam("deform", "swinging");
        } 

        if (this->ClipMap)
        {
            propMatSurface.SetTexture( "clipMap", texClip);
            matType.AddParam( "map", "clipmap" );
        } 

        if (this->DiffMap)
        {
            matType.AddParam( "map", "colormap" );
            propMatSurface.SetTexture("diffMap", texDiffuse );
            propMatSurface.SetVector("matDiffuse", vector4(1.0,1.0,1.0,1.0));
            propMatSurface.SetVector("matAmbient", vector4(1.0,1.0,1.0,1.0));

        } else
        {
            propMatSurface.SetVector("matDiffuse", matDiffuse);
            propMatSurface.SetVector("matAmbient", matDiffuse);
        }

        if (this->hasSpec)
        {
            matType.AddParam("env", "specularlighting");
            propMatSurface.SetFloat("matShininess", this->SpecularPow);
            propMatSurface.SetFloat("matLevel", this->SpecularLevel);
            if (this->SpecMap)
            {  
                matType.AddParam( "map", "levelmap" );
                propMatSurface.SetTexture("levelMap", this->texSpecular );
                propMatSurface.SetVector("matSpecular", vector4(1.0,1.0,1.0,1.0));

            } else
            {
                propMatSurface.SetVector("matSpecular", this->matSpecular);
            }
        }


        if (this->IlumMap)
        {
            matType.AddParam( "map", "illuminationmap" );
            propMatSurface.SetTexture("illuminationMap", texIlum );
        }

        if (this->LightMap)
        {
            matType.AddParam( "map", "lightmap" );
            propMatShape.SetTexture("lightMap", texLight );
        } 
        propMatSurface.SetMaterial(matType);
    } else
    {
        SetErrorMaterial();
    }
}


//------------------------------------------------------------------------------
/**
*/
void 
n3dsMaterial::GetMap(int map,bool &valid,nString &path)
{

    struct _stat fileStatus;
    bool notExport = false;

    //ID_DI is the map diffuse
    Texmap* subTex;
    subTex = mtl->GetSubTexmap(map);
    valid = (  (((StdMat*)mtl)->MapEnabled(map)) && // is enable map diffuse
        (subTex) && 
        (subTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00))); // IS bitmap

    if  (valid ) //MAPDIFFUSE
    {
        path=((BitmapTex *)subTex)->GetMapName();
        notExport= (0!=_stat(path.Get(),&fileStatus));// if not exist
        if (notExport)
        {
            n_printf("no exporter %s",this->mtl->GetFullName());
        }
        path.ConvertBackslashes();

    } else
    {
        notExport = TRUE == ((StdMat*)mtl)->MapEnabled(map);
    }


    this->notExport = this->notExport || notExport;
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsMaterial::SetMaterial(Mtl* mat )
{
    this->mtl=mat;
    Class_ID val;
    if (mtl)
    {
        val=mtl->ClassID();
    }
    if ((mtl!=0) && (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))) // StdMat
    {
        notExport=false;

        GetMap(ID_DI, this->DiffMap, this->texDiffuse); //Diffuse
        GetMap(ID_OP, this->ClipMap, this->texClip);    //opacity
        GetMap(ID_SI, this->IlumMap, this->texIlum);    //self-ilumination
        GetMap(ID_SS, this->LightMap, this->texLight);//LightMap in glossiness map ID_SS
        GetMap(ID_SH, this->SpecMap, this->texSpecular);


        Color color;
        color       = mtl->GetAmbient();
        matAmbient  = vector4(color.r, color.g, color.b, 1.0f);

        color       = mtl->GetDiffuse();
        matDiffuse  = vector4(color.r, color.g, color.b, 1.0f);

        color       = mtl->GetSpecular();
        matSpecular = vector4(color.r, color.g, color.b, 1.0f);



        this->SpecularLevel = mtl->GetShinStr();
        this->SpecularPow   = mtl->GetShininess();

        this->SpecularPow = pow ( 2.0f ,SpecularPow*10.0f);

        this->hasSpec = (this->SpecularLevel != 0) ;


        GetNameProperties();
        this->CreateNode();

    }else if ((mtl!=0) && ( mtl->ClassID() == ZOMBIE_MTL_CLASS_ID ))
    {
        GetNameProperties();
        ExportZombieMaterial(this->animators);
    }else
    {
        notExport=true;
        SetErrorMaterial();
    }
}


//------------------------------------------------------------------------------
/**
*/
n3dsMaterial::n3dsMaterial():
    notExport(false),
    propBillBoard(false),
    propSphereNormal(false),
    propSwing(false),
    propFogDisable(false),
    bumpMapIsNormalMap(false),
    shadowEmitter(false),
    isPrepared(false),
    meshComponents(0)
{
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsMaterial::GetNameProperties()
{
    nString name= mtl->GetName();
    nString Property;
    Property=name.GetExtension();
    name.StripExtension();

    while ( !Property.IsEmpty())
    {
        Property.ToLower();

        if ( Property== "bill")
        {
            this->propBillBoard =true;

        }else  if ( Property == "normal")
        {
            this->propSphereNormal =true ;

        } else  if ( Property == "swing")
        {
            this->propSwing =true ;

        } else if ( Property == "fogdisable")
        {
            this->propFogDisable = true;
        }


        Property=name.GetExtension();
        name.StripExtension();
    }

}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsMaterial::IsBillBoard() const
{
    return this->propBillBoard;
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsMaterial::IsEmitterShadow() const
{
    return this->shadowEmitter;
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsMaterial::IsSphereNormal() const
{
    return this->propSphereNormal;
}

//------------------------------------------------------------------------------
/**
*/
const char* 
n3dsMaterial::GetName() const
{
    return nameMaterial.Get();
}


//------------------------------------------------------------------------------
/**
*/
bool 
n3dsMaterial::IsValid() const
{
    return  ! this->notExport ;
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsMaterial::IsInValidShaderType() const
{
    /// @todo: remove this method , is a temporal solution for show a warning
    return (this->propMatSurface.GetAbstractType() == nAbstractPropBuilder::SHADERTYPELIBRARY ) &&
           (this->propMatSurface.IsInvalidShader() );
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsMaterial::ExportZombieTexture(Texmap *map,  nAbstractPropBuilder* surface, nString &name, nArray<nTexAnim>& animators)
{
    nString Path;
    bool Anim = false;

    nTexAnim animator;
    int numBlock =map->NumParamBlocks();
    int idxBlock;

    //First get all parameters
    for ( idxBlock = 0 ; idxBlock<numBlock  ; idxBlock++)
    {
        IParamBlock2* pb2 = map->GetParamBlock(idxBlock);
        ParamBlockDesc2* pdc= pb2->GetDesc();

        int i;
        for ( i = 0; i< pdc->count ; i++)
        {
            ParamDef pD = pdc->paramdefs[i];
            nString nameParam = pD.int_name;
            if (nameParam == "filename")
            {
                //PBBitmap* bitmap = pb2->GetBitmap( pD.ID );
                Path = pb2->GetStr( pD.ID );
                Path.ConvertBackslashes();
                animator.SetTexture(Path);
                
            } else if (nameParam == "animator" )
            {
                Anim = 0 != pb2->GetInt( pD.ID ) ;

            } else if (nameParam == "z_freq" )
            {
                int val = pb2->GetInt ( pD.ID );
                animator.SetFrequency ( val );

            } else if (nameParam == "z_loop" )
            {
                int val = pb2->GetInt ( pD.ID );
                switch (val)
                {
                    case 1: animator.SetLoopType(nAnimLoopType::Loop);
                        break;
                    case 3: animator.SetLoopType(nAnimLoopType::PingPong);
                        break;
                    default:
                        animator.SetLoopType(nAnimLoopType::Clamp);
                }
            } else if (nameParam == "z_seq" )
            {
                animator.SetSequence( pb2->GetStr( pD.ID ) );
            }
        }
    }

    if ( Path.IsEmpty() )
    {
        N3DSERROR( materialExport , (1, "ERROR:  %s : not has a bitmap", name.Get() ) );
        surface->SetTexture( name , Path); // Set empty texture, after if texture not existed show log
    } else
    {
        if (!Anim)
        {
            surface->SetTexture( name , Path);
        } else
        {
            animator.SetShaderParam ( name );
            animator.isToShape = ( (&this->propMatShape) == surface );
            animators.Append( animator );
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsMaterial::ExportZombieMaterial(nArray<nTexAnim>& animators)
{
    nMatTypePropBuilder matType;
    this->propMatSurface.SetAbstractType(nAbstractPropBuilder::SHADERTYPELIBRARY);
    nString nameInLibrary("invalid");


    Mtl *scp = this->mtl;

    int numBlock =scp->NumParamBlocks();
    int idxBlock;

    for ( idxBlock = 0 ; idxBlock<numBlock ; idxBlock++)
    {
        IParamBlock2* pb2 = scp->GetParamBlock(idxBlock);
        ParamBlockDesc2* pdc= pb2->GetDesc();
        int i;
        for ( i = 0; i< pdc->count ; i++)
        {
            ParamDef pD = pdc->paramdefs[i];
            nString name = pD.int_name;
            nString token;
            token = name.GetToken('_');

            if ( token == "z" )
            {
                token = name.GetToken('_');
                if  (token == "Param")
                {
                    int val = pb2->GetInt(pD.ID);
                    if (val)
                    {
                        token = name.GetToken('_');
                        name  = name.GetToken('_');
                        matType.AddParam(token.Get(), name.Get() );
                        if ( name == "billboard") this->propBillBoard = true;
                    }
                } else 
                {
                    nAbstractPropBuilder* Surface = 0;
                    if      ( token == "Sur") Surface = &this->propMatSurface;
                    else if ( token == "Sha") Surface = &this->propMatShape;

                    token = name.GetToken('_');
                    name  = name.GetToken('_');
                    if (Surface)
                    {
                        if  ( token == "Float")
                        {
                           Surface->SetFloat(name , pb2->GetFloat( pD.ID ) );
                        } else if  ( token == "Texture" )
                        {
                            Texmap * texture = pb2->GetTexmap( pD.ID );
                            if ( texture)
                            {                                
                                this->ExportZombieTexture( texture,Surface , name, animators);                                
                            } 
                        } else if  ( token == "Vector" )
                        {
                            Point4 vec1= pb2->GetPoint4( pD.ID );
                            vector4 vec(vec1.x, vec1.y, vec1.z, vec1.w);
                            Surface->SetVector( name , vec);
                        }
                        
                        else if ( token == "int" )
                        {
                            int val = pb2->GetInt( pD.ID );
                            Surface->SetInt( name , val );
                        }
                    } // close  if(surface)
                }
            } else if ( token == "bumpMapIsNormalMap")
            {
                bumpMapIsNormalMap = 0 != pb2->GetInt(pD.ID);
            } else if ( token == "shadowEmitter") 
            {
                shadowEmitter = 0 != pb2->GetInt(pD.ID);
            }  else if ( token == "shaderType")
            {
                nameInLibrary = pb2->GetStr( pD.ID );
            }
        }  // for paramdescriptor
    } // for paramblocks

    nameMaterial = this->mtl->GetName();
    nameMaterial.ConvertSpace();

    if ( matType.HasParam("deform","wave") ) // If the shader has this param then need the float
    {
        propMatSurface.SetInt( "isSkinned" , 3);
    }

    if ( !notExport)
    {
        propMatSurface.SetMaterialTypeFromLibrary(nameInLibrary.Get(), matType);
    } else
    {
        SetErrorMaterial();
    }
    
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsMaterial::ReduceAndCheck(nArray<nTexAnim>& /*animators*/)
{
    return true;
#if 0
    const nMaterialDb& matDb= nMaterialServer::Instance()->GetMaterialDb();
    nFixedArray<bool> shaderParamsUse(nShaderState::NumParameters);
    nAbstractPropBuilder::nSortedTextures textures;
    
    //First get the material properties, create a temporal material
    nMaterial* mat = static_cast<nMaterial*>(nKernelServer::ks->New("nmaterial", "/tmp3dsPluginMaterial")); //Create it in root
    propMatSurface.GetMatType().SetTo(*mat);
    matDb.GetShaderParams( mat, shaderParamsUse);
    this->meshComponents = matDb.GetVertexFlags(mat);
    mat->Release();


    //DElete unnecesary textures in shape
    nAbstractPropBuilder::nSortedTextures& texturesShape = propMatShape.GetTextureArray();
    for( int idx = texturesShape.Size() - 1 ; idx>= 0 ; --idx)
    {
        nAbstractPropBuilder::varTexture& texture = texturesShape[idx];
        int shaderParam = nShaderState::StringToParam(texture.varName.Get());
        if ( !shaderParamsUse[shaderParam] )
        {
            texturesShape.Erase(idx);
        }
    }

    //DElete unnecesary textures in material
    nAbstractPropBuilder::nSortedTextures& texturesSurface = propMatSurface.GetTextureArray();
    for( int idx = texturesSurface.Size() - 1 ; idx>= 0 ; --idx)
    {
        nAbstractPropBuilder::varTexture& texture = texturesSurface[idx];
        int shaderParam = nShaderState::StringToParam(texture.varName.Get());
        if ( !shaderParamsUse[shaderParam] )
        {
            texturesSurface.Erase(idx);
        }
    }

    //DElete unnecesary textures animators
    for ( int  idx=animators.Size()-1; idx>=0; --idx)
    {
        nTexAnim& animator=animators[idx];
        int shaderParam = nShaderState::StringToParam(animator.GetShaderParam().Get() );
        if ( !shaderParamsUse[shaderParam] )
        {
            animators.Erase(idx);
        }
    }

    //@todo: delete unnecesary float , vectors , ...

    return true;
#endif
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsMaterial::CollectTextures(nArray<nTexAnim>& animators)
{
    bool valid = true;
    n3dsMaterialList*  mtlList = n3dsExportServer::Instance()->GetMtlList();
    nAbstractPropBuilder::nSortedTextures textures;

    textures = propMatSurface.GetTextureArray();
    for ( int i = 0; i < textures.Size(); ++i)
    {
        nAbstractPropBuilder::varTexture& texture = textures[i];
        nString newName;
        if ( !texture.texName.IsEmpty() )
        {
            if ( texture.varName == "bumpMap")
            {
                if (!bumpMapIsNormalMap)
                {
                    newName = mtlList->AppendBumpMapTexture( texture.texName );
                } else
                {
                    newName = mtlList->AppendTexture( texture.texName );
                }
            } else
            {
                newName = mtlList->AppendTexture( texture.texName );
            }
            // Replace the name in material
            propMatSurface.SetTexture( texture.varName , newName);
        }
        valid = valid && !newName.IsEmpty();
        N3DSERRORCOND( materialExport , newName.IsEmpty(), (2, "ERROR: material \"%s\" : Invalid texture \"%s\"", nameMaterial.Get(), texture.texName.Get() ) )
    }

    textures = propMatShape.GetTextureArray();
    for ( int i = 0; i < textures.Size(); ++i)
    {
        nAbstractPropBuilder::varTexture& texture = textures[i];
        if ( !texture.texName.IsEmpty() )
        {
            nString newName = mtlList->AppendTexture( texture.texName );
            propMatShape.SetTexture( texture.varName , newName);
            valid =  valid && !newName.IsEmpty();
            N3DSERRORCOND( materialExport , newName.IsEmpty(), (2, "ERROR: material \"%s\" : Invalid texture \"%s\"", nameMaterial.Get(), texture.texName.Get() ) )
        } else
        {
            valid = false;
            N3DSERROR( materialExport , (2, "ERROR: material \"%s\" : Invalid texture", nameMaterial.Get() ) )
        }
        
    }

    for ( int  i=0; i< animators.Size() ; ++i)
    {
        nTexAnim& animator=animators[i];
        bool validAnim = animator.InitSequence();
        N3DSERRORCOND( materialExport , !validAnim, (2, "ERROR: material \"%s\" : Invalid sequence animator \"%s\"", nameMaterial.Get(), animator.GetShaderParam().Get() ) )
        if ( validAnim )
        {
            nString currentTexture = animator.GetNextTexture();
            nString newPath;

            if ( animator.GetShaderParam() == "bumpMap" )
            {
                newPath = mtlList->AppendBumpMapTexture( currentTexture  );
                while ( !currentTexture.IsEmpty() )
                {
                    nString tmpName;
                    if (!bumpMapIsNormalMap)
                    {
                         tmpName = mtlList->AppendBumpMapTexture( currentTexture  );
                    } else
                    {
                        tmpName = mtlList->AppendTexture( currentTexture);
                    }
                    N3DSERRORCOND( materialExport , tmpName.IsEmpty() , (2, "ERROR: material \"%s\" : Invalid sequence animator \"%s\" \"%s\"", nameMaterial.Get(), animator.GetShaderParam().Get(), currentTexture.Get() ) )
                    validAnim = validAnim && (! tmpName.IsEmpty());
                    currentTexture = animator.GetNextTexture(); 
                } 
            } else
            {
                newPath = mtlList->AppendTexture( currentTexture);
                while ( !currentTexture.IsEmpty() )
                {
                    nString tmpName = mtlList->AppendTexture( currentTexture);
                    N3DSERRORCOND( materialExport , tmpName.IsEmpty() , (2, "ERROR: material \"%s\" : Invalid sequence animator \"%s\" \"%s\"", nameMaterial.Get(), animator.GetShaderParam().Get(), currentTexture.Get() ) )
                    validAnim = validAnim && (! tmpName.IsEmpty());
                    currentTexture = animator.GetNextTexture(); 
                }
            }
            animator.SetTexture( newPath);
        }

        valid = valid && validAnim;
    }
    return valid;
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsMaterial::CreateAnimators(nArray<nTexAnim>& animators)
{
    nAnimatorBuilderServer* Server = n3dsExportServer::Instance()->GetMtlList()->GetAnimatorBuilder();
    for ( int i = 0; i < animators.Size() ; i++ )
    {
        nString pathAnim = Server->GetPath(& animators[i] );
        //animators[i].surface->SetAnim( pathAnim );
        if ( animators[i].isToShape )
        {
            this->propMatShape.SetAnim( pathAnim );
            
        } else
        {
            this->propMatSurface.SetAnim( pathAnim );
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsMaterial::SetErrorMaterial()
{
    nAbstractPropBuilder propMatSurface;
    propMatSurface.SetAbstractType(nAbstractPropBuilder::NSURFACE);
    propMatSurface.SetShader("dept", "shaders:error.fx");
    propMatSurface.SetTexture("diffMap", "wc:libs/system/textures/error.tga");
    this->propMatSurface = propMatSurface;
    this->notExport = true;
}


//------------------------------------------------------------------------------
/**
    Validate the material, check if the material type exists
    And convert the material. Call this method is necesary before ask the mesh components 
    remove unnecesary textures and values
*/
void 
n3dsMaterial::PrepareAndExport()
{
    if ( !isPrepared)
    {
        isPrepared = true;
        //apply conversion from change the material shader
        this->ConvertMatType(); 

        bool valid;
        valid =  this->ReduceAndCheck(animators);

        //This part is export
        valid = valid && this->CollectTextures(animators);
        this->CreateAnimators(animators);
        if (!valid)
        {
            notExport=true;
            this->SetErrorMaterial();
        }
    }
}


//------------------------------------------------------------------------------
/**
    Validate the material, check if the material type exists
    remove unnecesary textures and values
*/
void 
n3dsMaterial::ConvertMatType()
{
    bool valid(true);
    //nMatTypePropBuilder
    if ( this->propMatSurface.GetAbstractType() == nAbstractPropBuilder::SHADERTYPELIBRARY )
    {
        if ( (!this->propMatSurface.IsInvalidShader() ) &&  (! this->propMatSurface.IsCustomShader()) )
        {
            n3dsMaterialList*  mtlList = n3dsExportServer::Instance()->GetMtlList();
            nMatTypePropBuilder* matLibrary = mtlList->GetMatTypePropBuilder( this->propMatSurface.GetMaterialTypeName() );

            // If the original does not have a any property then put a default texture
            if ( matLibrary )
            {
                for ( int index = 0 ; index < matLibrary->GetList().Size() ; ++index )
                {
                    valid = valid && this->PutDefaultValuesForParam(index , matLibrary);
                }
                // put the new properties
                this->propMatSurface.SetMatType( *matLibrary);
                this->notExport =  this->notExport || (!valid );
                N3DSWARNCOND( materialExport, !valid , (2, "WARNING: %s : Invalid conversion to  material type \"%s\" ", this->GetName() , this->propMatSurface.GetMaterialTypeName() ) );
            } else
            {
                N3DSWARN( materialExport, (2, "WARNING:  %s : the material type \"%s\" not found in library", this->GetName() , this->propMatSurface.GetMaterialTypeName() ) );
                this->notExport = true;
            }
        }

        /* uncomment this after adaption period
        if ( this->propMatSurface.IsInvalidShader() )
        {
            this->notExport = true; // For show log
        }
        */

    }
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsMaterial::SetUniqueName(const nString unique)
{
    this->uniqueName = unique;
}


//------------------------------------------------------------------------------
/**
*/
const nString& 
n3dsMaterial::GetUniqueName()
{
    return  this->uniqueName;
}

//------------------------------------------------------------------------------
/**
*/
int 
n3dsMaterial::GetMeshComponents() const
{
    int component = nMeshBuilder::Vertex::Component::COORD    |                
                    nMeshBuilder::Vertex::Component::NORMAL   |
                    nMeshBuilder::Vertex::Component::UV0      |
                    nMeshBuilder::Vertex::Component::UV1      |
                    nMeshBuilder::Vertex::Component::UV2      |
                    nMeshBuilder::Vertex::Component::UV3      ;
    if (  (propMatSurface.GetAbstractType() == nAbstractPropBuilder::NMATERIAL) ||
          (propMatSurface.GetAbstractType() == nAbstractPropBuilder::SHADERTYPELIBRARY)
       )
    {

        if (propMatSurface.GetMatType().HasParam( "env", "vcolorblend" ) ||
            propMatSurface.GetMatType().HasParam( "env", "vcolormatambient" ) ||
            propMatSurface.GetMatType().HasParam( "deform", "swinging" ) ||
            propMatSurface.GetMatType().HasParam( "deform","wave" )
            )
        {
            component |= nMeshBuilder::Vertex::Component::COLOR;
        }  
    }
    return component;
}

//------------------------------------------------------------------------------
/**
    Put values and texture for simulate
    @return if it is valid conversion
*/
bool 
n3dsMaterial::PutDefaultValuesForParam(int index, nMatTypePropBuilder* matLibrary )
{
    static const nString  path(pathTextureSystem);
    const nMatTypePropBuilder& myMat = this->propMatSurface.GetMatType();
    const nMatTypePropBuilder::Param& param = matLibrary->GetList()[index];

    if ( myMat.HasParam(param) )
    {
        // If the max material has the same param then not put default texture
        return true;
    }
    
    // for dont'care texture use the magenta texture

    if (param.name == "hasalpha" && param.value == "true" )
    {
        N3DSWARN( materialExport , (2, "WARNING: %s : the shader use \"alpha\" but material doesn't  use it", this->GetName() ) );
    }
    else if (param.name == "alphatwoside" && param.value == "true" )
    {
        N3DSWARN( materialExport , (2, "WARNING: %s : the shader use \"alpha two side\" but material doesn't  use it", this->GetName() ) );
    }
    else if (param.name == "env" && param.value == "vcolormatambient" )
    {
        N3DSWARN( materialExport , (2, "WARNING: %s : the shader use vertex color but material doesn't  use it", this->GetName() ) );
        return false;
    }
    else if (param.name == "env" && param.value == "vcolorblend" )
    {
        N3DSWARN( materialExport , (2, "WARNING: %s : the shader use vertex color but material doesn't  use it", this->GetName() ) );
        return false;
    }
    else if (param.name == "deform" && param.value == "billboard" )
    {
        this->propBillBoard = true; // For generate a valid info
        N3DSWARN( materialExport , (2, "WARNING: %s : the shader use a billboard but material doesn't  use a billboard", this->GetName() ) );
        return false;
    }
    else if (param.name == "deform" && param.value == "swinging" )
    {
        N3DSWARN( materialExport , (2, "WARNING: %s : the shader use swing but material doesn't use it, paint the vertex color and mark this property in the material", this->GetName() ) );
        return false;
    }
    else if (param.name == "pro" && param.value == "notculling" )
    {
        // Empty
    }
    else if (param.name == "pro" && param.value == "depthbias" )
    {
        // Empty , the depthbias values is fixed in the shader , change this when depthbias is a variable
    }
    else if (param.name == "map" && param.value == "colormap" )
    {
        this->propMatSurface.SetTexture( "diffMap", path + "white.dds" );
    }
    else if (param.name == "map" && param.value == "clipmap" )
    {
        this->propMatSurface.SetTexture( "clipMap", path + "white.dds" );
    }
    else if (param.name == "map" && param.value == "bumpmap" )
    {
        this->propMatSurface.SetTexture( "bumpMap", path + "nobump.dds" );
        this->bumpMapIsNormalMap = true;

        if ( myMat.HasParam( "map" , "normalmap" ) )
        {
            N3DSWARN( materialExport , (2, "WARNING: %s : the shader use a bumpmap but material use a normalmap ", this->GetName() ) );
            return false;
        }
    }
    else if (param.name == "map" && param.value == "lightmap" )
    {
        //this shader not has static light, but micky needs see it
        this->propMatSurface.SetTexture( "lightMap", path + "white.dds" );
    }
    else if (param.name == "map" && param.value == "levelmap" )
    {
        this->propMatSurface.SetTexture( "levelMap", path + "white.dds" );
        //Because this is modulate by matSpecular
    }
    else if (param.name == "map" && param.value == "controlmap" )
    {
        // The white select the first texture
        this->propMatSurface.SetTexture( "controlMap", path + "white.dds" );
        this->propMatSurface.SetTexture( "diffMap2", path + "magenta.dds" );
    }
    else if (param.name == "map" && param.value == "illuminationmap" )
    {
        this->propMatSurface.SetTexture( "illuminationMap", path + "black.dds" );
    }
    else if (param.name == "map" && param.value == "normalmap" )
    {
        this->propMatSurface.SetTexture( "BumpMap3", path + "nobump.dds" );

        if ( myMat.HasParam( "map" , "bumpmap" ) )
        {
            N3DSWARN( materialExport , (2, "WARNING: %s : the shader use a normalmap but material use a bumpmap ", this->GetName() ) );
            return false;
        }
    }
    else if (param.name == "map" && param.value == "environmentmap2d" )
    {
        // No reflect
        if  ( matLibrary->HasParam("map" , "levelmapasenvmaskmap" ) )
        {
            //this->propMatSurface.SetTexture( "levelMap", path + "black.dds" );
            //Not modify the levelMap because object not has a specular then use a white levelMap
        } else
        {
            this->propMatSurface.SetTexture( "envMaskMap", path + "black.dds" );
        }
        this->propMatSurface.SetFloat( "envMaskFactor" , 0.0f);
        // don't care texture
        this->propMatSurface.SetTexture( "AmbientMap0", path + "magenta.dds" );
    }
    else if (param.name == "map" && param.value == "environmentmap3d" )
    {
        // No reflect
        this->propMatSurface.SetTexture( "AmbientMap0", path + "magenta.dds" );
        if  ( matLibrary->HasParam("map" , "levelmapasenvmaskmap" ) )
        {
            //this->propMatSurface.SetTexture( "levelMap", path + "black.dds" );
            //Not modify the levelMap because object not has a specular then use a white levelMap
        } else
        {
            this->propMatSurface.SetTexture( "envMaskMap", path + "black.dds" );
        }
        this->propMatSurface.SetFloat( "envMaskFactor" , 0.0f);
        // don't care texture
        this->propMatSurface.SetTexture( "CubeMap0", path + "magenta.dds" );
    }
    else if (param.name == "map" && param.value == "levelmapasenvmaskmap" )
    {
        if ( myMat.HasParam( "map" , "levelmap" ) )
        {
            // In this case is not posible simulate the same effect, because is the different texture
            N3DSWARN( materialExport , (2, "WARNING: %s : the shader use levelmap as enviroment mask but material doesn't use it and it use a level map", this->GetName() ) );
            return false;
        } else
        {
            // empty, the code  is in "environmentmap2d" and "environmentmap3d" cases.
        }
    }
    else if (param.name == "map" && param.value == "parallax" )
    {
        // The grey 127.5 is the 0 offset
        this->propMatSurface.SetTexture( "parallaxMap", path + "grey_127.dds" );
    }
    else if (param.name == "env" && param.value == "ambientlighting" )
    {
        //matAmbient
        this->propMatSurface.SetVector( "matAmbient", vector4( 0.f, 0.f, 0.f, 0.f) );

    }
    else if (param.name == "env" && param.value == "diffuselighting" )
    {
        this->propMatSurface.SetVector( "matDiffuse", vector4( 0.f, 0.f, 0.f, 0.f) );
    }
    else if (param.name == "env" && param.value == "fogdisable" )
    {
        N3DSWARN( materialExport , (2, "WARNING: %s : the shader disable the fog  but material respond to fog", this->GetName() ) );
        return false;
    }
    else if (param.name == "env" && param.value == "specularlighting" )
    {
        this->propMatSurface.SetVector( "matSpecular", vector4( 0.f, 0.f, 0.f, 0.f) );
        this->propMatSurface.SetFloat( "matLevel", 0.f );
        this->propMatSurface.SetFloat( "matShininess", 0.f );
    }
    else if (param.name == "env" && param.value == "illuminationcolor" )
    {
        this->propMatSurface.SetVector( "illuminationColor", vector4( 0.f, 0.f, 0.f, 0.f) );
    }
    else if (param.name == "deform" && param.value == "wave" )
    {
        N3DSWARN( materialExport , (2, "WARNING: %s : the shader use a  wave but material not use a wave", this->GetName() ) );
        return false;
    }
    else if (param.name == "pro" && param.value == "fresnel" )
    {
        N3DSWARN( materialExport , (2, "WARNING: %s : the shader use a  fresnel but material not use a fresnel", this->GetName() ) );
        return false;
    }
   
    return true;
}


//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------