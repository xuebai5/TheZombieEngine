#include "precompiled/pchn3dsmaxexport.h"
#pragma warning( push, 3 )
#include "Max.h"
#pragma warning( pop )
#include "n3dsmaterial/nabstractpropbuilder.h"
#include "kernel/nkernelserver.h"
#include "nscene/nscenenode.h"
#include "nmaterial/nmaterialnode.h"

nClass* nAbstractPropBuilder::nmaterialnode = 0;
nClass* nAbstractPropBuilder::nsurfacenode  = 0;


nAbstractPropBuilder::nAbstractPropBuilder() :
    thisType(nAbstractPropBuilder::NONE),
    stringValid(false),
    matTypeIsValid(false)
{
    n_assert(nKernelServer::ks);
    nmaterialnode = nKernelServer::ks->FindClass("nmaterialnode");
    n_assert(nmaterialnode);
    nsurfacenode  = nKernelServer::ks->FindClass("nsurfacenode");
    n_assert(nsurfacenode);
}


int
__cdecl
nAbstractPropBuilder::TextureSorter(const varTexture* elm0, const varTexture* elm1)
{    
    return strcmp(elm0->varName.Get(), elm1->varName.Get() );
}

int
__cdecl
nAbstractPropBuilder::AnimSorter( const nString* elm0, const nString* elm1)
{
    n_assert(elm0);
    n_assert(elm1);
    return strcmp( elm0->Get(), elm1->Get() );
}

int
__cdecl
nAbstractPropBuilder::FloatSorter(const varFloat* elm0, const varFloat* elm1)
{    
    return strcmp(elm0->varName.Get(), elm1->varName.Get() );
} 

int
__cdecl
nAbstractPropBuilder::IntSorter(const varInt* elm0, const varInt* elm1)
{    
    return strcmp(elm0->varName.Get(), elm1->varName.Get() );
}


int
__cdecl
nAbstractPropBuilder::VectorSorter(const varVector* elm0, const varVector* elm1)
{    
    return strcmp(elm0->varName.Get(), elm1->varName.Get() );
}

  
int
__cdecl
nAbstractPropBuilder::ShaderSorter(const varShader* elm0, const varShader* elm1)
{    
    return strcmp(elm0->varName.Get(), elm1->varName.Get() );
}



void 
nAbstractPropBuilder::SetTexture ( const varTexture& var)
{
    this->stringValid = false;
    varTexture var2(var);
    var2.texName.ToLower();
    this->textures.Append ( var );
}

const nAbstractPropBuilder::nSortedTextures& 
nAbstractPropBuilder::GetTextureArray() const
{
    return this->textures;
}

nAbstractPropBuilder::nSortedTextures& 
nAbstractPropBuilder::GetTextureArray()
{
    return this->textures;
}

void
nAbstractPropBuilder::SetInt     ( const varInt&     var)
{
    this->stringValid = false;
    this->integers.Append( var );
}

void
nAbstractPropBuilder::SetFloat   ( const varFloat&   var)
{
    this->stringValid = false;
    this->floats.Append( var );
}

void
nAbstractPropBuilder::SetVector  ( const varVector&  var)
{
    this->stringValid = false;
    this->vectors.Append( var );
}

void 
nAbstractPropBuilder::SetAnim    ( const nString& pathAnim)
{
    this->stringValid = false;
    this->anims.Append(pathAnim);
}

void
nAbstractPropBuilder::SetShader  ( const varShader&  var)
{
    n_assert( this->thisType == NONE || this->thisType  == NSURFACE);

    this->stringValid = false;
    this->thisType = NSURFACE;
    this->shaders.Append( var );

}

void
nAbstractPropBuilder::SetMaterial( const nMatTypePropBuilder&    val)
{
    n_assert( this->thisType == NONE || this->thisType == NMATERIAL );
    this->stringValid = false;
    this->thisType = NMATERIAL;
    this->matType = val;
    this->matTypeIsValid = true;
}

//------------------------------------------------------------------------------
/**
    Set type Libaray.
    @parama val. The original material type for future conversion
*/
void 
nAbstractPropBuilder::SetMatType( const nMatTypePropBuilder&   val)
{
    n_assert( this->thisType == NONE || this->thisType == NMATERIAL  || this->thisType == SHADERTYPELIBRARY  );
    this->stringValid = false;
    this->matType = val;
    this->matTypeIsValid = true;
}

//------------------------------------------------------------------------------
/**
    Set type Libaray.
    @param name The name's material in Library
    @parama val. The original material type for future conversion
*/
void 
nAbstractPropBuilder::SetMaterialTypeFromLibrary( const char* name , const nMatTypePropBuilder&   val)
{
    n_assert( this->thisType == NONE || this->thisType == SHADERTYPELIBRARY );
    this->stringValid = false;
    this->thisType = SHADERTYPELIBRARY;
    this->matType = val;
    this->matTypeIsValid = true;
    this->materialTypeNameInLibrary = name;
}

//------------------------------------------------------------------------------
/**
    Set type Libaray.
    @param name The name's material in Library
    @parama val. The original material type for future conversion
*/
void 
nAbstractPropBuilder::SetMaterialTypeFromLibrary( const char* name)
{
    n_assert( this->thisType == NONE || this->thisType == SHADERTYPELIBRARY );
    this->stringValid = false;
    this->thisType = SHADERTYPELIBRARY;
    this->materialTypeNameInLibrary = name;
}

//------------------------------------------------------------------------------
/**
*/
const char* 
nAbstractPropBuilder::GetMaterialTypeName() const
{
    return this->materialTypeNameInLibrary.Get();
}

//------------------------------------------------------------------------------
/**
*/
bool 
nAbstractPropBuilder::IsInvalidShader() const
{
    n_assert ( this->thisType == SHADERTYPELIBRARY );
    return (this->materialTypeNameInLibrary.IsEmpty()) || (this->materialTypeNameInLibrary == "invalid" );
}

//------------------------------------------------------------------------------
/**
*/
bool 
nAbstractPropBuilder::IsCustomShader() const
{
    return (this->materialTypeNameInLibrary == "custom" );
}

const char* 
nAbstractPropBuilder::GetUniqueString()
{
    int idx;

    if (! this->stringValid )
    {
        char buf[255];
        stringKey="";

        switch (thisType)
        {
            case NONE:
                stringKey+="NONE#";
                break;
            case NMATERIAL:
                stringKey+="NMATERIAL#";//+material+"/";
                if (this->matTypeIsValid)
                {
                    stringKey += this->matType.GetUniqueString();
                    stringKey +="/";
                }
                break;

            case NSURFACE:
                stringKey+="NSURFACE#";
                for (idx = 0; idx < shaders.Size() ; idx ++)
                {
                    varShader& shader = shaders[idx];
                    stringKey+= shader.varName + "." + shader.val+"/";
                }

                break;
            case SHADERTYPELIBRARY:
                stringKey+="SHADERLIBRARY#";
                stringKey+=this->materialTypeNameInLibrary;
                // If the material is custom get the matType properties
                if (this->matTypeIsValid && (this->IsInvalidShader() || this->IsCustomShader() ) )
                {
                    stringKey += this->matType.GetUniqueString();
                    stringKey +="/";
                }
        }

        stringKey+="TEX#";
        for (idx=0; idx < textures.Size() ; idx++)
        {
            varTexture& texture = textures[idx];
            stringKey += texture.varName +"." + texture.texName + "/";
        }

        stringKey+="INT#";
        for ( idx=0; idx < integers.Size(); idx++)
        {
            varInt& integer = integers[idx];
            sprintf(buf,".%i#", integer.val);
            stringKey += integer.varName + buf;
        }

        stringKey+="FLOAT#";
        for (idx=0; idx < floats.Size(); idx ++)
        {
            varFloat& var = floats[idx];
            sprintf(buf,".%f#", var.val );
            stringKey += var.varName  + buf;
        }

        stringKey+="VECTOR#";
        for (idx = 0; idx< vectors.Size();  idx ++)
        {
            varVector& var = vectors[idx];
            sprintf(buf,".%f_%f_%f_%f#", var.val.x, var.val.y, var.val.z, var.val.w);
            stringKey += var.varName + buf;
        }

        stringKey+="ANIMS$";
        for (idx = 0 ; idx  < anims.Size() ; idx ++)
        {
            stringKey += anims[idx] + "$";
     
        }

       this->stringValid = true;
    }

    return this->stringKey.Get();
}


void 
nAbstractPropBuilder::SetTo( nAbstractShaderNode* node)
{
    n_assert(nKernelServer::ks);
    
    int idx;
    

        switch (thisType)
        {
            case NONE:
                break;
            case NMATERIAL:

                if (  node->IsA(nmaterialnode))
                {
                    //nMaterialNode* MaterialNode = (nMaterialNode*) node;
                    //MaterialNode->SetMaterial( this->material.Get() );
                }
                break;
               
            case NSURFACE:
                if ( node->IsA(nsurfacenode) )
                {
                    nSurfaceNode*  SurfaceNode = (nSurfaceNode*) node;
                    
                    for (idx = 0; idx < shaders.Size() ; idx ++)
                    {
                        varShader& var = shaders[idx];
                        SurfaceNode->SetShader( nVariableServer::StringToFourCC(var.varName.Get()) , var.val.Get() );
                    }
                }
                
                break;
            case SHADERTYPELIBRARY:
                break;
        }

        for (idx = 0; idx < textures.Size() ; idx ++)
        {
            varTexture& var = textures[idx];
            node->SetTexture( nShaderState::StringToParam( var.varName.Get()),
                               var.texName.Get() );
        }

        for (idx = 0; idx < floats.Size(); idx ++)
        {
            varFloat& var = floats[idx];
            node->SetFloat( nShaderState::StringToParam( var.varName.Get()),
                            var.val);
        }

        for (idx = 0; idx < integers.Size(); idx++)
        {
            varInt& var = integers[idx];
            node->SetInt( nShaderState::StringToParam( var.varName.Get())
                          , var.val);
        }

        for (idx = 0; idx < vectors.Size(); idx++)
        {
            varVector& var = vectors[idx];
            node->SetVector( nShaderState::StringToParam( var.varName.Get() ), 
                             var.val);
        }

        for (idx = 0 ; idx < anims.Size() ; idx ++)
        {
            node->AddAnimator( anims[idx].Get() );
        }

}

void 
nAbstractPropBuilder::operator +=(const nAbstractPropBuilder& rhs)
{
    switch (rhs.thisType)
    {
        case NONE:
            break;
        case NMATERIAL:
            if (this->thisType == NONE || this->thisType == NMATERIAL)
            {
                this->thisType = NMATERIAL;
                if (rhs.matTypeIsValid)
                {
                    this->matTypeIsValid = true;
                    this->matType += rhs.matType;
                }
            }

            break;

        case NSURFACE:
            if (this->thisType == NONE || this->thisType == NSURFACE)
            {
                this->thisType = NSURFACE;
                this->shaders += rhs.shaders;
            }

            break;
        case SHADERTYPELIBRARY:
            if (this->thisType == NONE || this->thisType == SHADERTYPELIBRARY)
            {
                this->thisType = SHADERTYPELIBRARY;
                this->matTypeIsValid = true;
                this->matType += rhs.matType;
                if ( this->thisType == NONE )
                {
                    this->materialTypeNameInLibrary = rhs.materialTypeNameInLibrary;
                }
            }
    }

    this->textures+= rhs.textures;
    this->integers+= rhs.integers;
    this->floats+= rhs.floats;
    this->vectors+= rhs.vectors;

    this->stringValid = false;

}


const char*  
nAbstractPropBuilder::GetNameClass()
{
    switch (thisType )
    {
        case nAbstractPropBuilder::NONE:
            return  "nabstractshadernode";
            break;

        case nAbstractPropBuilder::NMATERIAL :
            return "nmaterialnode";
            break;

        case nAbstractPropBuilder::NSURFACE :
            return "nsurfacenode";
            break;

        case nAbstractPropBuilder::SHADERTYPELIBRARY :
            return "nmaterialnode";
            break;
    }
    return "";
}

void 
nAbstractPropBuilder::Reduce()
{
    //@ todo remove unnecesary variables, textures , etc.... by material properties
}