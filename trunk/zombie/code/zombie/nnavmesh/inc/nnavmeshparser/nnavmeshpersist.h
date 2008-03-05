#ifndef N_NAVMESHPERSIST_H
#define N_NAVMESHPERSIST_H

//------------------------------------------------------------------------------
/**
    @class nNavMeshPersist
    @ingroup NebulaNavmeshSystem

    Navigation mesh loader and saver for all supported file formats and versions.
*/

class nNavMesh;

//------------------------------------------------------------------------------
struct nNavMeshPersist
{
    /// Load a navigation mesh from file, returning true if successfully loaded
    static bool Load(const char* filename, nNavMesh* mesh);

    /// Save a navigation mesh to file, returning true if successfully saved
    static bool Save(const char* filename, nNavMesh* mesh);

    /// File extension for ascii navigation mesh files;
    static const char* AsciiFileExtension;
    /// File extension for binary navigation mesh files
    static const char* BinaryFileExtension;
    /// File extension for binary compressed navigation mesh files
    static const char* CompressedFileExtension;
};

//------------------------------------------------------------------------------
#endif
