#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  nnavmeshfile.cc
//------------------------------------------------------------------------------

#include "nnavmeshparser/nnavmeshfile.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
/// Codes used to identify file format types
static const char* AsciiFourCC = "navigation";
static const int BinFourCC = 'NVX2';

//------------------------------------------------------------------------------
/**
    GetAsciiFourCC
*/
const char* nNavMeshFile::GetAsciiFourCC()
{
    return AsciiFourCC;
}

//------------------------------------------------------------------------------
/**
    GetAsciiFourCC
*/
const int nNavMeshFile::GetBinFourCC()
{
    return BinFourCC;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
