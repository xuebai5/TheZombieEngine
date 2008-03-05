#ifndef N_NAVOBSTACLESPARSER_H
#define N_NAVOBSTACLESPARSER_H

//------------------------------------------------------------------------------
/**
    @file navobstaclesparser.h
    @ingroup NebulaNavmeshSystem

    Parsers for the navigation obstacles block.

    Block file format:
    @verbatim
    --------------------------------------------------------
    obstacles:
        version 1
        num_obstacles <int>
        {polygon block}
        ...
        {polygon block}
    --------------------------------------------------------
    @endverbatim
*/

#include "nnavmeshparser/navblockparser.h"

//------------------------------------------------------------------------------
/**
    @class NavObstaclesParser

    Class to parse the navigation obstacles block.
*/
class NavObstaclesParser : public NavBlockParser
{
public:
    /// Constructor
    NavObstaclesParser(NavParserFactory* factory);

protected:
    /// Parse the block as having version 1 syntax
    virtual bool ParseVersion1();

    /// Parse the number of obstacles
    virtual bool ObstaclesNumber() = 0;
    /// Parse the obstacles list
    virtual bool ObstaclesList();
};

//------------------------------------------------------------------------------
/**
    @class NavObstaclesLoader

    Class to load the navigation obstacles block.
*/
class NavObstaclesLoader : public NavObstaclesParser
{
public:
    /// Constructor
    NavObstaclesLoader(NavParserFactory* factory);

protected:
    /// Parse the number of obstacles
    virtual bool ObstaclesNumber();
};

//------------------------------------------------------------------------------
/**
    @class NavObstaclesSaver

    Class to save the navigation obstacles block.
*/
class NavObstaclesSaver : public NavObstaclesParser
{
public:
    /// Constructor
    NavObstaclesSaver(NavParserFactory* factory);

protected:
    /// Parse the number of obstacles
    virtual bool ObstaclesNumber();
};

//------------------------------------------------------------------------------
#endif
