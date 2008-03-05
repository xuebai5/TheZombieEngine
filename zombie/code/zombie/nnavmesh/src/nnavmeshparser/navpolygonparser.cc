#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  navpolygonparser.cc
//------------------------------------------------------------------------------

#include "nnavmeshparser/navpolygonparser.h"
#include "nnavmeshparser/navtag.h"
#include "nnavmeshparser/nnavmeshfile.h"
#include "mathlib/polygon.h"

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavPolygonParser::NavPolygonParser(NavParserFactory* factory)
    : NavBlockParser(NavTag::Polygon, factory), poly(NULL)
{
    ADD_PARSE_VERSION(NavPolygonParser, 1);
}

//------------------------------------------------------------------------------
/**
    Parse a polygon, returning true if successfully parsed
*/
bool
NavPolygonParser::Parse(nNavMeshFile* file, polygon* poly)
{
    this->poly = poly;
    return NavBlockParser::Parse(file, NULL);
}

//------------------------------------------------------------------------------
/**
    Parse the block as having version 1 syntax
*/
bool
NavPolygonParser::ParseVersion1()
{
    if ( !this->VerticesNumber() )
    {
        return false;
    }
    if ( !this->VerticesList() )
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Parse the vertices list of the polygon
*/
bool
NavPolygonParser::VerticesList()
{
    n_assert(poly);

    for ( int i = 0; i < poly->GetNumVertices(); ++i )
    {
        if ( !this->Vertex(i) )
        {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavPolygonLoader::NavPolygonLoader(NavParserFactory* factory)
    : NavPolygonParser(factory)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
   Parse the node index of the navigation mesh node
*/
bool
NavPolygonLoader::VerticesNumber()
{
    n_assert(this->file);
    n_assert(this->poly);

    int num_vertices;
    if ( !this->file->ParseInt8(NavTag::PolygonVerticesNb, num_vertices) )
    {
        return false;
    }

    // Resize polygon vertices number
    nArray<vector3> v;
    v.SetFixedSize(num_vertices);
    n_assert(v.Size() == num_vertices);
    this->poly->Set(v);
    return poly->GetNumVertices() == num_vertices;
}

//------------------------------------------------------------------------------
/**
    Parse a vertex of the polygon
*/
bool
NavPolygonLoader::Vertex(int vertex_index)
{
    n_assert(this->file);
    n_assert(this->poly);
    n_assert(vertex_index >= 0 && vertex_index < this->poly->GetNumVertices());

    vector3 vertex;
    if ( !this->file->ParseVector3(NavTag::PolygonVertex, vertex) )
    {
        return false;
    }
    this->poly->SetVertex(vertex_index, vertex);
    return true;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavPolygonSaver::NavPolygonSaver(NavParserFactory* factory)
    : NavPolygonParser(factory)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
   Parse the node index of the navigation mesh node
*/
bool
NavPolygonSaver::VerticesNumber()
{
    n_assert(this->file);
    n_assert(this->poly);

    int num_vertices( poly->GetNumVertices() );
    return this->file->ParseInt8(NavTag::PolygonVerticesNb, num_vertices);
}

//------------------------------------------------------------------------------
/**
    Parse a vertex of the polygon
*/
bool
NavPolygonSaver::Vertex(int vertex_index)
{
    n_assert(this->file);
    n_assert(this->poly);
    n_assert(vertex_index >= 0 && vertex_index < this->poly->GetNumVertices());

    vector3 vertex( this->poly->GetVertex(vertex_index) );
    return this->file->ParseVector3(NavTag::PolygonVertex, vertex);
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
