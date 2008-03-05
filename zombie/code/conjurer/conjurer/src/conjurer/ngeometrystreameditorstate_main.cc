#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ngeometrystreameditorstate_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ngeometrystreameditorstate.h"
#include "conjurer/ngeometrystreameditor.h"

nNebulaScriptClass(nGeometryStreamEditorState, "neditorstate");

namespace
{
    const char* StreamsPath("/usr/streams/");
    const char* FileStreamsPath("wc:libs/streams/");
    const char* DefaultStreamName("defaultstream");
}

//------------------------------------------------------------------------------
/**
*/
nGeometryStreamEditorState::nGeometryStreamEditorState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGeometryStreamEditorState::~nGeometryStreamEditorState()
{
    if (this->refStreamsRoot.isvalid())
    {
        this->refStreamsRoot->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditorState::OnStateEnter(const nString& prevState)
{
    nEditorState::OnStateEnter(prevState);

    // load all streams in the library
    this->LoadStreamLibrary();
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditorState::OnStateLeave(const nString& nextState)
{
    nEditorState::OnStateLeave(nextState);

    //TEMPORARY- instead of losing the streams, for now we'll keep them
    // clear the stream library
    //if (this->refStreamsRoot.isvalid())
    //{
    //    this->refStreamsRoot->Release();
    //}

    // clear all temporary structures
    //nAutoRef<nRoot> refTmpStream("/tmp/streambuilder");
    //if (refTmpStream.isvalid())
    //{
    //    refTmpStream->Release();
    //}
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditorState::LoadStreamLibrary()
{
    nFileServer2* fileServer = kernelServer->GetFileServer();
    nArray<nString> files(fileServer->ListFiles(FileStreamsPath));

    if (!this->refStreamsRoot.isvalid())
    {
        this->refStreamsRoot = kernelServer->New("nroot", StreamsPath);
    }

    // clear the current library in case we want to reload
    while (this->refStreamsRoot->GetHead())
    {
        this->refStreamsRoot->GetHead()->Release();
    }

    kernelServer->PushCwd(this->refStreamsRoot);

    for (int index = 0; index < files.Size(); ++index)
    {
        nGeometryStreamEditor* streamEditor;
        streamEditor = static_cast<nGeometryStreamEditor*>(kernelServer->Load(files[index].Get()));
        n_assert(streamEditor);
        streamEditor->SetDirty(false);
    }

    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
nRoot*
nGeometryStreamEditorState::CreateStream(const char *streamName)
{
    n_assert(streamName);
    n_assert(this->refStreamsRoot.get());
    
    if (this->refStreamsRoot->Find(streamName))
    {
        return 0;
    }

    kernelServer->PushCwd(this->refStreamsRoot.get());
    nRoot* newStream = kernelServer->New("ngeometrystreameditor", streamName);
    n_assert(newStream);
    kernelServer->PopCwd();

    return newStream;
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditorState::DeleteStream(const char *streamName)
{
    n_assert(streamName);
    n_assert(this->refStreamsRoot.get());
    
    nRoot* stream = this->refStreamsRoot->Find(streamName);
    if (stream)
    {
        static_cast<nGeometryStreamEditor*>(stream)->DeleteStream();
        stream->Release();
    }    
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditorState::SaveStreamLibrary()
{
    n_assert(this->refStreamsRoot.get());
    
    nFileServer2 *fileServer = kernelServer->GetFileServer();
    if (!fileServer->DirectoryExists(FileStreamsPath))
    {
        fileServer->MakePath(FileStreamsPath);
    }
    
    nRoot *curChild;
    for (curChild = this->refStreamsRoot->GetHead(); 
         curChild;
         curChild = curChild->GetSucc())
    {
        nString fileName(FileStreamsPath);
        fileName.Append(curChild->GetName());
        fileName.Append(".n2");
        curChild->SaveAs(fileName.Get());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditorState::BuildAllStreams()
{
    n_assert(this->refStreamsRoot.get());

    this->SaveStreamLibrary();

    nRoot *curChild;
    for (curChild = this->refStreamsRoot->GetHead(); 
         curChild;
         curChild = curChild->GetSucc())
    {
        if (static_cast<nGeometryStreamEditor*>(curChild)->IsDirty())
        {
            static_cast<nGeometryStreamEditor*>(curChild)->BuildStream();
        }
    }
}
