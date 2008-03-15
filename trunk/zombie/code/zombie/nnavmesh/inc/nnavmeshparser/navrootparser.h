#ifndef N_NAVROOTPARSER_H
#define N_NAVROOTPARSER_H

//------------------------------------------------------------------------------
/**
    @class NavRootParser
    @ingroup NebulaNavmeshSystem

    Class to parse the navigation root block.

    Block file format:
    @verbatim
    --------------------------------------------------------
    type navigation
    version 1
    {navigation mesh block}
    {navigation graph block}
    {obstacles block}
    --------------------------------------------------------
    type navigation
    version 2
    {navigation mesh block}
    {obstacles block}
    --------------------------------------------------------
    @endverbatim

    (C) 2005 Conjurer Services, S.A.
*/

#include "nnavmeshparser/navblockparser.h"

//------------------------------------------------------------------------------
class NavRootParser : public NavBlockParser
{
public:
    /// Constructor
    NavRootParser(NavParserFactory* factory);

    /// Open and parse a navigation mesh file, returning true if success
    virtual bool Parse(const char* filename, nNavMeshFile* file, nNavMesh* mesh);

protected:
    /// Parse the beginning of the block
    virtual bool BeginBlock();
    /// Parse the ending of the block
    virtual bool EndBlock();

    /// Parse the block as having version 1 syntax
    virtual bool ParseVersion1();
    /// Parse the block as having version 2 syntax
    virtual bool ParseVersion2();
};

//------------------------------------------------------------------------------
#endif
