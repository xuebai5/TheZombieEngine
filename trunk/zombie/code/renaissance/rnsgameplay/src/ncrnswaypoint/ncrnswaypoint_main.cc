#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncrnswaypoint_main.cc
//------------------------------------------------------------------------------

#include "ncrnswaypoint/ncrnswaypoint.h"
#include "zombieentity/nctransform.h"
#include "entity/nobjectinstancer.h"
#include "zombieentity/ncsubentity.h"
#include "nspatial/nspatialserver.h"
#include "ncnavmesh/ncnavmeshnode.h"
#include "ncnavmesh/ncnavmesh.h"
#include "nnavmesh/nnavmesh.h"
#include "nnavmesh/nnavutils.h"
#include "mathlib/polygon.h"

//------------------------------------------------------------------------------
#ifndef NGAME
    const char* ncRnsWaypoint::attributeLabels[] = {
        "Walk",
        "Crouch",
        "Prone",
        "Run",
        "Jump",
        "Jump over",
        "Climb up",
        "Climb down",
        "Wall",
        "Swim",
        "Swim in",
        "Swim out",
        "Fly",
        "Fly in",
        "Fly out",
        "Cover low",
        "Cover hight"
    };
#endif

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncRnsWaypoint::ncRnsWaypoint() : attributes(0), radius(2)
{ 
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncRnsWaypoint::~ncRnsWaypoint()
{
    // empty   
}

//------------------------------------------------------------------------------
/**
    Initialize the component
*/
void 
ncRnsWaypoint::InitInstance(nObject::InitInstanceMsg initType)
{
    if (initType != nObject::ReloadedInstance)
    {
        // bind signals
        this->entityObject->BindSignal( ncSubentity::SignalUpdateEntityReferences, this, &ncRnsWaypoint::UpdateEntityLinks, 0 );
    }

    // Bind the waypoint with any overlapping nav mesh node, losing any current external link
    this->GenerateExternalLinks();
}

//------------------------------------------------------------------------------
/**
    Update links to other entities. Called by a signal from ncSubentity
*/
void
ncRnsWaypoint::UpdateEntityLinks()
{
    for (int link = 0; link < this->links.Size(); link++)
    {
        nEntityObjectId oid = this->links[ link ];
        this->links[ link ] = this->entityObject->FromPersistenceOID( oid );
    }
}

//------------------------------------------------------------------------------
/**
    Bind the waypoint with any overlapping nav mesh node, losing any current external link
*/
void
ncRnsWaypoint::GenerateExternalLinks()
{
    // Remove old external links to make sure that no invalid links remain (valid ones will be recreated)
    this->ClearExternalLinks( REMOVE_SELF_FROM_TARGET );

    // Get the node containing the center of this waypoint
    nEntityObject* outdoor = nSpatialServer::Instance()->GetOutdoorEntity();
    if ( outdoor )
    {
        const vector3& waypointPos( this->GetComponentSafe<ncTransform>()->GetPosition() );
        nNavMesh* navMesh( outdoor->GetComponentSafe<ncNavMesh>()->GetNavMesh() );
        ncNavNode* meshNode( navMesh->GetNode( waypointPos ) );
        if ( meshNode )
        {
            this->AddExternalLink( meshNode );
            meshNode->AddExternalLink( this );
        }
    }
    // @todo: Remove all the following code (included the one past this return) when the above code works for
    //        binding waypoints to any navmesh
/*    return;

    // Give more priority to indoor cells when indoor and outdoor overlap
    // Basically, check first in indoors, and only if no mesh node has been found check later in outdoor
    // In outdoor space brush nodes are also given more priority over terrain ones

    // -- Indoors

    // Get all the navnodes in the same spatial cell as the waypoint
    nArray< nEntityObject* > nodes;
    const vector3& waypointPos( this->GetComponentSafe<ncTransform>()->GetPosition() );
    nSpatialServer::Instance()->GetEntitiesCategory( waypointPos, nSpatialTypes::CAT_NAVMESHNODES,
        nSpatialTypes::SPF_ALL_INDOORS | nSpatialTypes::SPF_ALL_CELLS |
        nSpatialTypes::SPF_CONTAINING | nSpatialTypes::SPF_USE_BBOX | nSpatialTypes::SPF_USE_CELLS_BBOX, nodes );

    // Connect those mesh nodes that contain the waypoint node
    bool overlappingNodeFound( false );
    for ( int i(0); i < nodes.Size(); ++i )
    {
        ncNavMeshNode* meshNode( nodes[i]->GetComponent<ncNavMeshNode>() );
        if ( meshNode )
        {
            // Check if the waypoint node is contained in the mesh node within some tolerance in height
            const float LinkTolerance( 0.5f );
            polygon* nodePoly( meshNode->GetZone() );
            n_assert( nodePoly );
            if ( nodePoly->IsPointInside( waypointPos.x, waypointPos.z ) &&
                waypointPos.y > nodePoly->GetVertex(0).y - LinkTolerance &&
                waypointPos.y < nodePoly->GetVertex(0).y + LinkTolerance )
            {
                // Both nodes overlap -> connect each other
                this->AddExternalLink( meshNode );
                meshNode->AddExternalLink( this );
//                        NLOG_REL( navbuild, (NLOGUSER | 0, "Linked nodes %d and %d",
//                            waypoint->GetEntityObject()->GetId(), meshNode->GetEntityObject()->GetId()) );
                overlappingNodeFound = true;
            }
        }
    }

    // If the waypoint has been connected to an indoor, just finish for this node
    if ( overlappingNodeFound )
    {
        return;
    }

    // -- Outdoor

    // Get all the navnodes in the same spatial cell as the waypoint
    nodes.Clear();
    nSpatialServer::Instance()->GetEntitiesCategory( waypointPos, nSpatialTypes::CAT_NAVMESHNODES,
        nSpatialTypes::SPF_OUTDOORS | nSpatialTypes::SPF_ALL_CELLS |
        nSpatialTypes::SPF_CONTAINING | nSpatialTypes::SPF_USE_BBOX | nSpatialTypes::SPF_USE_CELLS_BBOX, nodes );

    // Connect those mesh nodes that contain the waypoint node
    for ( int i(0); i < nodes.Size(); ++i )
    {
        ncNavMeshNode* meshNode( nodes[i]->GetComponent<ncNavMeshNode>() );
        if ( meshNode )
        {
            // Check if the waypoint node is contained in the mesh node, considering both at the same height
            polygon* nodePoly( meshNode->GetZone() );
            if ( nodePoly->IsPointInside( waypointPos.x, waypointPos.z ) )
            {
                // Both nodes overlap -> connect each other
                this->AddExternalLink( meshNode );
                meshNode->AddExternalLink( this );
//                        NLOG_REL( navbuild, (NLOGUSER | 0, "Linked nodes %d and %d",
//                            waypoint->GetEntityObject()->GetId(), meshNode->GetEntityObject()->GetId()) );
            }
        }
    }*/
}

//------------------------------------------------------------------------------
/**
    Add a local link by id (only for persistency)
*/
void 
ncRnsWaypoint::AddLocalLinkById( nEntityObjectId entityId )
{
    if ( this->links.FindIndex( entityId ) == -1 )
    {
        this->links.Append( entityId );
    }
}

//------------------------------------------------------------------------------
/**
    GetMidPoint
*/
void
ncRnsWaypoint::GetMidpoint( vector3& midpoint ) const
{
    midpoint = this->GetComponentSafe<ncTransform>()->GetPosition();
}

//------------------------------------------------------------------------------
/**
    IsPointInside
*/
bool
ncRnsWaypoint::IsPointInside (const vector3& point) const
{
    vector3 center;
    this->GetMidpoint( center );
    return point.isequal( center, /*this->radius +*/ TINY );
}

//------------------------------------------------------------------------------
/**
    IsPointInside
*/
bool
ncRnsWaypoint::IsPointInside (float x, float z) const
{
    vector3 center;
    this->GetMidpoint( center );
    center.y = 0;
    return vector3(x,0,z).isequal( center, /*this->radius +*/ TINY );
}

//------------------------------------------------------------------------------
/**
    Get the node's closest navigable point to the given point

    The navigable area is that area comprised by all the points with a LoS to
    the waypoint's position, but only the LoS for the current point is tested
    due to an obvious performance issue. If there's no LoS to the given point,
    then the waypoint's position is returned as the closest point.
*/
void
ncRnsWaypoint::GetClosestNavigablePoint( const vector3& point, vector3& closestPoint ) const
{
    // Build LoS
    vector3 waypoint;
    this->GetMidpoint( waypoint );

    // Test for LoS
    if ( nNavUtils::IsWalkable( point, waypoint, NULL ) )
    {
        // Take as the closest point the given point if it has LoS to the waypoint,...
        closestPoint = point;
    }
    else
    {
        // ...otherwise take the waypoint's position.
        closestPoint = waypoint;
    }

    nNavUtils::GetGroundHeight( closestPoint, closestPoint.y );
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    GetNumAttributes
*/
int
ncRnsWaypoint::GetNumAttributes() const
{
    return this->numAttributes;
}

//------------------------------------------------------------------------------
/**
    GetAttributeLabel
*/
const char*
ncRnsWaypoint::GetAttributeLabel( int index ) const
{
    n_assert( index >= 0 && index < this->numAttributes );
    return this->attributeLabels[index];
}

//------------------------------------------------------------------------------
/**
    GetAttributeState
*/
bool
ncRnsWaypoint::GetAttributeState( int index ) const
{
    return this->ExistFlag( 1<<index );
}

//------------------------------------------------------------------------------
/**
    SetAttributeState
*/
void
ncRnsWaypoint::SetAttributeState( int index, bool value )
{
    this->ToggleFlag( 1<<index, value );
}
#endif//!NGAME
