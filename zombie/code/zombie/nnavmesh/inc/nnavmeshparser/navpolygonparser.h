#ifndef N_NAVPOLYGONPARSER_H
#define N_NAVPOLYGONPARSER_H

//------------------------------------------------------------------------------
/**
    @file navpolygonparser.h
    @ingroup NebulaNavmeshSystem

    Parsers for a polygon block.

    Block file format:
    @verbatim
    --------------------------------------------------------
    polygon:
        version 1
        num_vertices <int>
        vertex <x (float)> <y (float)> <z (float)>
        ...
        vertex <x (float)> <y (float)> <z (float)>
    --------------------------------------------------------
    @endverbatim
*/

#include "nnavmeshparser/navblockparser.h"

class polygon;

//------------------------------------------------------------------------------
/**
    @class NavPolygonParser

    Class to parse a polygon block.
*/
class NavPolygonParser : public NavBlockParser
{
public:
    /// Constructor
    NavPolygonParser(NavParserFactory* factory);

    /// Parse a polygon, returning true if successfully parsed
    virtual bool Parse(nNavMeshFile* file, polygon* poly);

protected:
    /// Parse the block as having version 1 syntax
    virtual bool ParseVersion1();

    /// Parse the number of polygon's vertices
    virtual bool VerticesNumber() = 0;
    /// Parse the vertices list of the polygon
    virtual bool VerticesList();
    /// Parse a vertex of the polygon
    virtual bool Vertex(int vertex_index) = 0;

    /// Polygon being parsed
    polygon* poly;
};

//------------------------------------------------------------------------------
/**
    @class NavPolygonLoader

    Class to load a polygon block.
*/
class NavPolygonLoader : public NavPolygonParser
{
public:
    /// Constructor
    NavPolygonLoader(NavParserFactory* factory);

protected:
    /// Parse the number of polygon's vertices
    virtual bool VerticesNumber();
    /// Parse a vertex of the polygon
    virtual bool Vertex(int vertex_index);
};

//------------------------------------------------------------------------------
/**
    @class NavPolygonSaver

    Class to save the polygon block.
*/
class NavPolygonSaver : public NavPolygonParser
{
public:
    /// Constructor
    NavPolygonSaver(NavParserFactory* factory);

protected:
    /// Parse the number of polygon's vertices
    virtual bool VerticesNumber();
    /// Parse a vertex of the polygon
    virtual bool Vertex(int vertex_index);
};

//------------------------------------------------------------------------------
#endif
