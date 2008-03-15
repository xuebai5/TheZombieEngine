#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  navrootparser.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "nnavmeshparser/navrootparser.h"
#include "nnavmeshparser/navtag.h"
#include "nnavmeshparser/navparserfactory.h"
#include "nnavmeshparser/nnavmeshfile.h"

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavRootParser::NavRootParser(NavParserFactory* factory)
    : NavBlockParser(NavTag::Root, factory)
{
    ADD_PARSE_VERSION(NavRootParser, 1);
    ADD_PARSE_VERSION(NavRootParser, 2);
}

//------------------------------------------------------------------------------
/**
    Open and parse a navigation mesh file, returning true if success
*/
bool
NavRootParser::Parse(const char* filename, nNavMeshFile* file, nNavMesh* mesh)
{
    // Open the file to parse it
    if ( !file->OpenFile(filename) )
    {
        return false;
    }

    // Parse the file
    bool parse_success = NavBlockParser::Parse(file, mesh);

    // Close the file and do any final validation
    bool close_success = file->CloseFile();

    return parse_success && close_success;
}

//------------------------------------------------------------------------------
/**
    Parse the beginning of the block

    Needed to override the parent method because this block is special in
    that it has a field previous to the version number (the file format type)
    and it doesn't have any special block beginning/ending.
*/
bool
NavRootParser::BeginBlock()
{
    return this->file->ParseFourCC();
}

//------------------------------------------------------------------------------
/**
    Parse the ending of the block

    Needed to override the parent method because this block doesn't have any
    special block beginning/ending.
*/
bool
NavRootParser::EndBlock()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Parse the block as having version 1 syntax
*/
bool
NavRootParser::ParseVersion1()
{
    if ( !this->ParseBlock(NavTag::Mesh) )
    {
        return false;
    }
    if ( !this->ParseBlock(NavTag::Graph) )
    {
        return false;
    }
    if ( !this->ParseBlock(NavTag::Obstacles) )
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Parse the block as having version 2 syntax
*/
bool
NavRootParser::ParseVersion2()
{
    if ( !this->ParseBlock(NavTag::Mesh) )
    {
        return false;
    }
    if ( !this->ParseBlock(NavTag::Obstacles) )
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
