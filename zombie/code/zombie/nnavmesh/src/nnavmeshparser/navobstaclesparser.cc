#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  navobstaclesparser.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "nnavmeshparser/navobstaclesparser.h"
#include "nnavmeshparser/navtag.h"
#include "nnavmeshparser/navparserfactory.h"
#include "nnavmeshparser/nnavmeshfile.h"
#include "nnavmeshparser/navpolygonparser.h"
#include "nnavmesh/nnavmesh.h"
#include "mathlib/polygon.h"

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavObstaclesParser::NavObstaclesParser(NavParserFactory* factory)
    : NavBlockParser(NavTag::Obstacles, factory)
{
    ADD_PARSE_VERSION(NavObstaclesParser, 1);
}

//------------------------------------------------------------------------------
/**
    Parse the block as having version 1 syntax
*/
bool
NavObstaclesParser::ParseVersion1()
{
    if ( !this->ObstaclesNumber() )
    {
        return false;
    }
    if ( !this->ObstaclesList() )
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Parse the obstacles list
*/
bool
NavObstaclesParser::ObstaclesList()
{
    n_assert(this->parserFactory);
    n_assert(this->mesh);

    NavPolygonParser* parser = static_cast<NavPolygonParser*>( this->parserFactory->CreateBlockParser(NavTag::Polygon) );
    bool success = true;
    for ( int i = 0; i < this->mesh->GetNumObstacles(); ++i )
    {
        if ( !parser->Parse(this->file, this->mesh->GetObstaclePolygon(i)) )
        {
            success = false;
            break;
        }
    }
    this->parserFactory->DeleteBlockParser(parser);
    return success;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavObstaclesLoader::NavObstaclesLoader(NavParserFactory* factory)
    : NavObstaclesParser(factory)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Parse the number of obstacles
*/
bool
NavObstaclesLoader::ObstaclesNumber()
{
    n_assert(this->file);
    n_assert(this->mesh);

    int num_obstacles;
    if ( !this->file->ParseInt16(NavTag::ObstaclesNb, num_obstacles) )
    {
        return false;
    }

    // Create the polygons for all obstacles
    for ( int i = 0; i < num_obstacles; ++i )
    {
        this->mesh->InsertObstacle( n_new(polygon) );
    }
    return this->mesh->GetNumObstacles() == num_obstacles;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavObstaclesSaver::NavObstaclesSaver(NavParserFactory* factory)
    : NavObstaclesParser(factory)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Parse the number of nodes within the navigation mesh
*/
bool
NavObstaclesSaver::ObstaclesNumber()
{
    n_assert(this->file);
    n_assert(this->mesh);

    int num_obstacles( this->mesh->GetNumObstacles() );
    return this->file->ParseInt16(NavTag::ObstaclesNb, num_obstacles);
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
