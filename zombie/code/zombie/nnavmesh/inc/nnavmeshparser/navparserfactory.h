#ifndef N_NAVPARSERFACTORY_H
#define N_NAVPARSERFACTORY_H

//------------------------------------------------------------------------------
/**
    @file navparserfactory
    @ingroup NebulaNavmeshSystem

    Navigation parser factories.

    (C) 2005 Conjurer Services, S.A.
*/

class NavBlockParser;
class NavTag;
struct nNavMeshFile;
class nNavMesh;

//------------------------------------------------------------------------------
/**
    @class NavParserFactory

    Base class to create parsers for navigation blocks.
*/
class NavParserFactory
{
public:
    /// Create a navigation block parser
    virtual NavBlockParser* CreateBlockParser(NavTag blockId);
    /// Delete a navigation block parser
    virtual void DeleteBlockParser(NavBlockParser* parser);
};

//------------------------------------------------------------------------------
/**
    @class NavLoaderFactory

    Class used to create loaders for navigation blocks.
*/
class NavLoaderFactory : public NavParserFactory
{
public:
    /// Create a navigation block loader
    virtual NavBlockParser* CreateBlockParser(NavTag blockId);
};

//------------------------------------------------------------------------------
/**
    @class NavSaverFactory

    Class used to create savers for navigation blocks.
*/
class NavSaverFactory : public NavParserFactory
{
public:
    /// Create a navigation block saver
    virtual NavBlockParser* CreateBlockParser(NavTag blockId);
};

//------------------------------------------------------------------------------
#endif
