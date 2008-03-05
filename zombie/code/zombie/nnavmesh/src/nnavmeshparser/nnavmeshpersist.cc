#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  nnavmeshpersist.cc
//------------------------------------------------------------------------------

#include "nnavmeshparser/nnavmeshpersist.h"
#include "nnavmeshparser/nnavmeshfilereader.h"
#include "nnavmeshparser/nnavmeshfilewriter.h"
#include "nnavmeshparser/navrootparser.h"
#include "nnavmeshparser/navtag.h"
#include "nnavmeshparser/navparserfactory.h"

//------------------------------------------------------------------------------
// File formats are identified by these file name extensions
const char* nNavMeshPersist::AsciiFileExtension = "txt";
const char* nNavMeshPersist::BinaryFileExtension = "nav";
const char* nNavMeshPersist::CompressedFileExtension = "rle";

//------------------------------------------------------------------------------
/**
    Load
*/
bool nNavMeshPersist::Load(const char* filename, nNavMesh* mesh)
{
    n_assert(filename);

    // Use the correct reader according to file extension
    nNavMeshFileReader* reader = NULL;
    if ( nString(filename).CheckExtension(AsciiFileExtension) )
    {
        reader = n_new(nNavMeshAscReader);
    }
    else if ( nString(filename).CheckExtension(BinaryFileExtension) )
    {
        reader = n_new(nNavMeshBinReader);
    }
    else if ( nString(filename).CheckExtension(CompressedFileExtension) )
    {
        reader = n_new(nNavMeshRleReader);
    }
    else
    {
        n_error( "nNavMeshPersist::Load: filetype not supported!\n" );
        return false;
    }
    n_assert(reader);

    // Load the mesh from file
    NavLoaderFactory factory;
    NavRootParser* parser = static_cast<NavRootParser*>( factory.CreateBlockParser(NavTag::Root) );
    bool success = parser->Parse(filename, reader, mesh);

    // Clean up and return success or failure
    factory.DeleteBlockParser(parser);
    n_delete(reader);
    return success;
}

//------------------------------------------------------------------------------
/**
    Save
*/
bool nNavMeshPersist::Save(const char* filename, nNavMesh* mesh)
{
    n_assert(filename);

    // Use the correct writer according to file extension
    nNavMeshFileWriter* writer = NULL;
    if ( nString(filename).CheckExtension(AsciiFileExtension) )
    {
        writer = n_new(nNavMeshAscWriter);
    }
    else if ( nString(filename).CheckExtension(BinaryFileExtension) )
    {
        writer = n_new(nNavMeshBinWriter);
    }
    else if ( nString(filename).CheckExtension(CompressedFileExtension) )
    {
        writer = n_new(nNavMeshRleWriter);
    }
    else
    {
        n_error( "nNavMeshPersist::Save: filetype not supported!\n" );
    }
    n_assert(writer);

    // Save the mesh to file
    NavSaverFactory factory;
    NavRootParser* parser = static_cast<NavRootParser*>( factory.CreateBlockParser(NavTag::Root) );
    bool success = parser->Parse(filename, writer, mesh);

    // Clean up and return success or failure
    factory.DeleteBlockParser(parser);
    n_delete(writer);
    return success;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
