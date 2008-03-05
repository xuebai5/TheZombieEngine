#include "precompiled/pchnnavmesh.h"
//------------------------------------------------------------------------------
//  navmeshparser.cc
//------------------------------------------------------------------------------

#include "nnavmeshparser/navmeshparser.h"
#include "nnavmeshparser/navtag.h"
#include "nnavmeshparser/navparserfactory.h"
#include "nnavmeshparser/nnavmeshfile.h"
#include "nnavmeshparser/navmeshnodeparser.h"
#include "nnavmesh/nnavmesh.h"
#include "ncnavmesh/ncnavmeshnode.h"
#include "mathlib/polygon.h"
#include "entity/nentityobjectserver.h"

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavMeshParser::NavMeshParser(NavParserFactory* factory)
    : NavBlockParser(NavTag::Mesh, factory)
{
    ADD_PARSE_VERSION(NavMeshParser, 1);
}

//------------------------------------------------------------------------------
/**
    Parse the block as having version 1 syntax
*/
bool
NavMeshParser::ParseVersion1()
{
    if ( !this->NodesNumber() )
    {
        return false;
    }
    if ( !this->NodesList() )
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Parse the nodes list of the navigation mesh
*/
bool
NavMeshParser::NodesList()
{
    n_assert(this->parserFactory);
    n_assert(this->mesh);

    NavMeshNodeParser* parser = static_cast<NavMeshNodeParser*>( this->parserFactory->CreateBlockParser(NavTag::MeshNode) );
    bool success = true;
    for ( int i = 0; i < this->mesh->GetNumNodes(); ++i )
    {
        if ( !parser->Parse(this->file, this->mesh, i) )
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
NavMeshLoader::NavMeshLoader(NavParserFactory* factory)
    : NavMeshParser(factory)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Parse the number of nodes within the navigation mesh
*/
bool
NavMeshLoader::NodesNumber()
{
    n_assert(this->file);
    n_assert(this->mesh);

    int num_nodes;
    if ( !this->file->ParseInt16(NavTag::MeshNodesNb, num_nodes) )
    {
        return false;
    }

    nEntityObject* newNode = 0;
    ncNavMeshNode* meshNodeComp = 0;
    // Create the needed navigation mesh nodes and their polygons
    for ( int i = 0; i < num_nodes; ++i )
    {
        polygon* poly = n_new(polygon);
        newNode = nEntityObjectServer::Instance()->NewLocalEntityObject("nenavmeshnode");
        meshNodeComp = newNode->GetComponentSafe<ncNavMeshNode>();
        meshNodeComp->CreateNode(poly);
        this->mesh->InsertNode(meshNodeComp);
        //this->mesh->InsertNode( n_new(nNavMeshNode)(poly) );
        n_delete(poly); // The mesh duplicates the polygon, so delete it
    }
    return mesh->GetNumNodes() == num_nodes;
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
NavMeshSaver::NavMeshSaver(NavParserFactory* factory)
    : NavMeshParser(factory)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Parse the number of nodes within the navigation mesh
*/
bool
NavMeshSaver::NodesNumber()
{
    n_assert(this->file);
    n_assert(this->mesh);

    int num_nodes = mesh->GetNumNodes();
    return this->file->ParseInt16(NavTag::MeshNodesNb, num_nodes);
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
