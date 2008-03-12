#include "precompiled/pchn3dsmaxexport.h"

#include "n3dsexporters/n3dslog.h"
#include "n3dsanimator/ntexanimatorbuilder.h"
#include "kernel/nfileserver2.h"
#include "nscene/ntextureanimator.h"

const char * 
nAnimatorBuilder::GetUniqueString()
{
    return "default";
}
const char * 
nAnimatorBuilder::GetClass()
{
    return "nanimator";
}

void        
nAnimatorBuilder::SetToAnimator(nAnimator* /*anim*/)
{
    // Empty
}


const char * 
nTexAnimatorBuilder::GetUniqueString()
{
    if (!this->stringValid)
    {
        stringKey  = "TextAnimator_";
        stringKey +=  texture + "_"; 
        stringKey +=  extension + "_";
        stringKey +=  param +     "_";
        stringKey +=  nAnimLoopType::ToString(loopType);
        if ( sequence.Start() )
        {
            stringKey += sequence.GetPlainSequence();
        }
    }


    return this->stringKey.Get();
}
void 
nTexAnimatorBuilder::SetTexture(nString &text)
{
    this->stringValid = false;
    this->texture = text.Get();
    this->extension = texture.GetExtension();

    this->texture.StripExtension();
    this->texture.StripLastNumbers();



}

void 
nTexAnimatorBuilder::SetFrequency( int freq)
{
    this->stringValid = false;
    this->freq = freq;
}

void 
nTexAnimatorBuilder::SetShaderParam( const nString &param)
{
    this->stringValid = false;
    this->param = param;
}

void 
nTexAnimatorBuilder::SetLoopType(nAnimLoopType::Type loopType)
{
    this->stringValid = false;
    this->loopType = loopType;
}


void 
nTexAnimatorBuilder::SetLoopType(nString loopType)
{
    this->loopType = nAnimLoopType::FromString( loopType.Get() );
}

void 
nTexAnimatorBuilder::SetToAnimator(nAnimator * anim)
{
    nString currentTexture;

    nTextureAnimator * animator = (nTextureAnimator *) anim;
    animator->SetFrequency( ushort( this->freq ) );
    animator->SetShaderParam( this->param.Get() );
    animator->SetLoopType( this->loopType );
    animator->SetChannel("time");

    
    InitSequence();
    currentTexture = GetNextTexture();
    while ( ! currentTexture.IsEmpty() )
    {
        animator->AddTexture( currentTexture.Get() );
        currentTexture = GetNextTexture(); 
    }
}

void 
nTexAnimatorBuilder::SetSequence(nString sequence)
{
    this->stringValid = false;
    this->sequence.Set(sequence);   
}

bool 
nTexAnimatorBuilder::InitSequence()
{
    return this->sequence.Start();
}

nString 
nTexAnimatorBuilder::GetNextTexture()
{
   nString path;
   nString tmp = sequence.GetNext();

   if ( !tmp.IsEmpty() )
   {
       path = texture;
       path += tmp;
       path += ".";
       path += extension;
   }

   return path;
}

bool 
nTexAnimatorBuilder::CheckFilesOfAnimator()
{
    nFileServer2 *fileServer = nKernelServer::ks->GetFileServer();
    nFile *file = fileServer->NewFileObject();
    bool fileValid = true;

    this->InitSequence();
    nString Path = this->GetNextTexture();

    // Set this for invalid sequence
    fileValid = ! Path.IsEmpty();

    N3DSERRORCOND( materialExport , ! fileValid ,  (1, "ERROR:  invalid sequence for animator \"%s\" , \"%s\" ", texture.Get() , sequence.Get() ) );


    while ( ! Path.IsEmpty() )
    {
        fileValid = file->Open( Path.Get() , "r" );
        if ( fileValid)
        {
            fileValid = file->GetSize() > 0;
            file->Close();
        }

        N3DSERRORCOND( materialExport , ! fileValid ,  (1, "ERROR: animator invalid file \"%s\" ", Path.Get() ) );
        Path = this->GetNextTexture();
    }
    file->Release();
    return fileValid;
}

bool 
nTexAnimatorBuilder::CollectTextureAnimator(const nString &dst)
{
    bool  copied;
    nFileServer2 *fileServer;
    nString Path;
    nString newPath;

    fileServer = nKernelServer::ks->GetFileServer();

    newPath = this->GetTexture();
    newPath = newPath.ExtractFileName();
    newPath = dst + newPath;
    
    if (  fileServer->DirectoryExists( newPath.Get() ) || // Directory Valid 
            ! fileServer->MakePath( newPath.Get()) ) //Create a directory with name of texture
    {
        return false;
    } else
    {
        newPath += "/." + this->GetExtension();
        nTexAnimatorBuilder srcAnimator = *this;
        this->SetTexture(newPath);

        this->InitSequence();
        srcAnimator.InitSequence();

        newPath = this->GetNextTexture();
        Path   = srcAnimator.GetNextTexture();
        copied = true;

        while ( ! Path.IsEmpty() && copied  )
        {
            copied = fileServer->CopyFile( Path.Get() , newPath.Get() );
            newPath = this->GetNextTexture();
            Path   = srcAnimator.GetNextTexture();
        }
        return copied;
    }

}

const nString& 
nTexAnimatorBuilder::GetShaderParam() const
{
    return this->param;
}