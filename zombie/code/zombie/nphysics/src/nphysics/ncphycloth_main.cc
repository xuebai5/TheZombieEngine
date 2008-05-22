#include "precompiled/pchnphysics.h"
//-----------------------------------------------------------------------------
//  ncphycloth_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "nphysics/ncphycloth.h"
#include "nphysics/nphygeomtrimesh.h"
#include "nphysics/nphygeomray.h"
#include "nphysics/nphygeomsphere.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhyCloth,ncPhySimpleObj);

//-----------------------------------------------------------------------------

phyreal ncPhyCloth::massCloth(phyreal(.1));

phyreal ncPhyCloth::airResistance(phyreal(0.6));

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 05-Jun-2005   Zombie         created
*/
ncPhyCloth::ncPhyCloth() : 
    nodesXAxis( 21 ), // Default values
    nodesZAxis( 21 ),
    separationXAxis( phyreal(.1) ),
    separationZAxis( phyreal(.1) ),
    numTotalNodes(0),
    bufferVertexes(0),
    mesh(0),
    bufferIndexes(0),
    velocity(0,0,0),
    forceAccumulated(0,0,0),
    nodesInfo(0),
    nodesCollisionPoints(0),
    numCollisionPoints(0),
    nodesHalfPoints(0),
    numHalfPoints(0),
    nodesRestPoints(0),
    numRestPoints(0)
{
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 05-Jun-2005   Zombie         created
*/
ncPhyCloth::~ncPhyCloth() 
{
    n_delete_array( this->bufferIndexes );
    n_delete_array( this->bufferVertexes );
    n_delete_array( this->nodesInfo );
    n_delete_array( this->nodesCollisionPoints );
    n_delete_array( this->nodesHalfPoints );
    n_delete_array( this->nodesRestPoints );
}

//-----------------------------------------------------------------------------
/**
    User init instance code

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 05-Jun-2005   Zombie         created
*/
void ncPhyCloth::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhySimpleObj::InitInstance( initType );
}

//-----------------------------------------------------------------------------
/**
    Creates the object

    @param world it's the world where the object will be come to existance

    history:
        - 05-Jun-2005   Zombie         created
*/
void ncPhyCloth::Create( nPhysicsWorld* world )
{
    n_assert2( world, "Null pointer." );

    n_assert2( this->nodesXAxis & 1, "The number of nodes has to be odd." );
    n_assert2( this->nodesZAxis & 1, "The number of nodes has to be odd." );

    this->mesh = 
        static_cast<nPhyGeomTriMesh*>(nKernelServer::Instance()->New( "nphygeomtrimesh" ) );

    n_assert2( this->mesh, "Failed to create a nPhyGeomTriMesh." );
    
    this->SetGeometry( this->mesh );

    this->BuildMesh();

    ncPhySimpleObj::Create( world );

    this->RegisterForPostProcess();

    this->nodesInfo[0].tied = true;
    this->nodesInfo[20].tied = true;
}

//-----------------------------------------------------------------------------
/**
    Builds the mesh representing the rectangular piece of cloth.

    history:
        - 05-Jun-2005   Zombie         created
*/
void ncPhyCloth::BuildMesh()
{
    /// creating structure to hold the mesh
    this->numTotalNodes = this->nodesXAxis * this->nodesZAxis;

    this->bufferVertexes = n_new_array( phyreal, this->numTotalNodes * 3 );

    n_assert2( this->bufferVertexes, "Run out of memory." );

    this->mesh->SetBufferVertexes( this->numTotalNodes, this->bufferVertexes );
    
    int numberOfIndexes( (this->nodesXAxis-1) * (this->nodesZAxis-1) * 6 );

    this->bufferIndexes = n_new_array( int, numberOfIndexes );

    n_assert2( this->bufferIndexes, "Run out of memory." );

    this->mesh->SetBufferIndexes( numberOfIndexes, this->bufferIndexes );

    /// filing with data the mesh
    
    // creating the vertexes data
    phyreal *ptrBuffer( this->bufferVertexes );
    
    for( unsigned indexZ(0); indexZ < this->nodesZAxis; ++indexZ )
    {
        vector3 position( 0,0, phyreal( indexZ ) * separationZAxis );

        for( unsigned indexX(0); indexX < this->nodesXAxis; ++indexX,++ptrBuffer )
        {
            position.x = indexX * separationXAxis;

            *ptrBuffer = position.x;
            *++ptrBuffer = position.y;
            *++ptrBuffer = position.z;
        }
    }

    // creating the indexes data
    int *ptrIndexes( this->bufferIndexes );

    for( unsigned indexZ(0); indexZ < this->nodesZAxis - 1; ++indexZ )
    {
        for( unsigned indexX(0); indexX < this->nodesXAxis - 1; ++indexX, ++ptrIndexes )
        {
            // top triangle
            *ptrIndexes = indexZ * this->nodesZAxis + indexX;
            *++ptrIndexes = indexZ * this->nodesZAxis + indexX + 1;
            *++ptrIndexes = (indexZ+1) * this->nodesZAxis + indexX + 1;

            // botton triangle
            *++ptrIndexes = indexZ * this->nodesZAxis + indexX;
            *++ptrIndexes = (indexZ+1) * this->nodesZAxis + indexX + 1;
            *++ptrIndexes = (indexZ+1) * this->nodesZAxis + indexX;
        }
    }

    this->mesh->Update();

    // info nodes
    this->nodesInfo = n_new_array( nodeInfo, this->numTotalNodes );

    // collision nodes
    this->numCollisionPoints = (( this->nodesXAxis + 1 ) / 2)*(( this->nodesZAxis + 1 ) / 2);

    this->nodesCollisionPoints = n_new_array( nodeInfo*, this->numCollisionPoints );

    // half point nodes
    this->numHalfPoints = (( this->nodesXAxis - 1 ) / 2) * (( this->nodesZAxis - 1 ) / 2);

    this->nodesHalfPoints = n_new_array( nodeInfo*, this->numHalfPoints );

    // rest point nodes
    this->numRestPoints = (((this->nodesXAxis - 1 ) / 2) * (this->nodesZAxis-1) / 2) +
        (((this->nodesXAxis+1) / 2) * (this->nodesZAxis+1) / 2) - 1;

    this->nodesRestPoints = n_new_array( nodeInfo*, this->numRestPoints );

    nodeInfo* node(this->nodesInfo);

    ptrBuffer = this->bufferVertexes;

    int counterCollisionPoints(0);
    int counterHalfPointsPoints(0);
    int counterRestPoints(0);

    for( int indexZ(0); indexZ < int(this->nodesZAxis); ++indexZ )
    {
        for( int indexX(0); indexX < int(this->nodesXAxis); ++indexX, ++node, ptrBuffer+=3 )
        {
            node->tied = false;

            node->numNodes = 0;
         
            // DANGEROUS:
            node->oldposition = reinterpret_cast<vector3*>(ptrBuffer);

            node->newposition = *node->oldposition;

            node->changed = false;

            if( !(indexZ & 1) && !(indexX & 1 ) )
            {
                // collision point

                // only needs the next two points in X and Z

                // first in horizontal
                node->nodes[0] = this->GetNode( indexX + 2, indexZ );

                if( node->nodes[0] )
                    ++node->numNodes;

                // then vertical
                node->nodes[1] = this->GetNode( indexX, indexZ + 2 );

                if( node->nodes[1] )
                    ++node->numNodes;

                this->nodesCollisionPoints[ counterCollisionPoints++ ] = node;

                continue;
            }

            if( ( indexZ & 1 ) && ( indexX & 1 ) )
            {
                // half - point
                
                // only needs the four points in x

                node->nodes[node->numNodes] = this->GetNode( indexX - 1, indexZ - 1 );
                if( node->nodes[node->numNodes] )
                    ++node->numNodes;
                node->nodes[node->numNodes] = this->GetNode( indexX + 1, indexZ - 1 );
                if( node->nodes[node->numNodes] )
                    ++node->numNodes;
                node->nodes[node->numNodes] = this->GetNode( indexX - 1, indexZ + 1 );
                if( node->nodes[node->numNodes] )
                    ++node->numNodes;
                node->nodes[node->numNodes] = this->GetNode( indexX + 1, indexZ + 1 );
                if( node->nodes[node->numNodes] )
                    ++node->numNodes;

                this->nodesHalfPoints[ counterHalfPointsPoints++ ] = node;

                continue;
            }


            // checking borders
            bool leftXBorder(false);
            bool rightXBorder(false);

            if( indexX - 1 < 0 )
            {
                leftXBorder = true;   
            }
            if( indexX + 1 >= int(this->nodesXAxis) )
            {
                rightXBorder = true;   
            }

            bool leftZBorder(false);
            bool rightZBorder(false);

            if( indexZ - 1 < 0 )
            {
                leftZBorder = true;   
            }
            if( indexZ + 1 >= int(this->nodesZAxis) )
            {
                rightZBorder = true;   
            }


            // the rest of the points
            if( !rightZBorder )
            {
                node->nodes[node->numNodes] = this->GetNode( indexX    , indexZ - 1 );
                if( node->nodes[node->numNodes] )
                    ++node->numNodes;
            }
            
            if( !leftZBorder )
            {
                node->nodes[node->numNodes] = this->GetNode( indexX    , indexZ + 1 );
                if( node->nodes[node->numNodes] )
                    ++node->numNodes;
            }

            if( !rightXBorder )
            {
                node->nodes[node->numNodes] = this->GetNode( indexX - 1, indexZ     );
                if( node->nodes[node->numNodes] )
                    ++node->numNodes;
            }

            if( !leftXBorder )
            {
                node->nodes[node->numNodes] = this->GetNode( indexX + 1, indexZ     );
                if( node->nodes[node->numNodes] )
                    ++node->numNodes;
            }
         
            this->nodesRestPoints[ counterRestPoints++ ] = node;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Adds a force.

    @param force adds a general force to the cloth piece

    history:
        - 05-Jun-2005   Zombie         created
*/
void ncPhyCloth::AddForce( const vector3& force )
{
    this->forceAccumulated += force;
}

//-----------------------------------------------------------------------------
/**
    Function to be process after running the simulation.

    history:
        - 05-Jun-2005   Zombie         created
*/
void ncPhyCloth::PostProcess()
{
    /// runs the simulation

    // pre-calculus
    nodeInfo* node(0);

    vector3 gravity(0,-.5f/60.f,0);

    for( int index(0); index < this->numCollisionPoints; ++index )
    {
        node = this->nodesCollisionPoints[ index ];

        node->force += this->forceAccumulated + gravity;
        node->normals = 0;
        node->normal = vector3(0,0,0);

        if( node->tied == false )
        {
            // computing every node velocity
            node->velocity *= airResistance;

            node->velocity += node->force / massCloth;

            if( node->velocity.lensquared() < phyreal(.001*.001) )
            {
                node->velocity = vector3(0,0,0);
            }

            node->force = vector3(0,0,0);
        }
    }

    this->Move();

    // reseting the accumulated forces
    this->forceAccumulated = vector3(0,0,0);
}

//-----------------------------------------------------------------------------
/**
    Un/sets tied point (not allowed to tie completely, max two sides).

    @param xNode coordinate x for a node
    @param zNode coordinate z for a node
    @param is specifies if the node is tied or not

    history:
        - 05-Jun-2005   Zombie         created
*/
void ncPhyCloth::SetTiedNode( const unsigned xNode, const unsigned zNode, bool is )
{
    n_assert2( this->GetWorld(), "This operation can't be performed until the object it's in the world." );

    n_assert2( xNode < this->nodesXAxis, "Index out of bounds." );
    n_assert2( zNode < this->nodesZAxis, "Index out of bounds." );

    this->nodesInfo[ zNode * this->nodesZAxis + xNode ].tied = is;
}

//-----------------------------------------------------------------------------
/**
    Moves the cloth in the world.

    history:
        - 05-Jun-2005   Zombie         created
*/
void ncPhyCloth::Move()
{
    // TODO: find the real one
    static phyreal timeFrame( phyreal(1) / phyreal(60) );

    nodeInfo* node(this->nodesInfo);

    for( int index(0); index < int(this->numTotalNodes); ++index, ++node )
    {
        node->normals = 0;
    }

    for( int index(0); index < this->numCollisionPoints; ++index )
    {
        node = this->nodesCollisionPoints[ index ];

        node->normal = vector3(0,0,0);

        node->changed = false;

        if( !node->tied )
        {
            node->newposition += node->velocity * timeFrame;
            if( node->velocity.lensquared() )
                node->changed = true;
        }
    }

    nodeInfo* nodeNext(0);

    static unsigned numIterations(5);
    /// applies the constrains between nodes
    for( unsigned iteration(0); iteration < numIterations; ++iteration )
    {

        // horizontal
        for( int index(0); index < this->numCollisionPoints; ++index )
        {
            node = this->nodesCollisionPoints[ index ];

            nodeNext = node->nodes[0];

            if( nodeNext )
            {

                vector3 direction( nodeNext->newposition - node->newposition );

                phyreal diference( direction.len() - (this->separationXAxis*2.f) );

                phyreal absdiference(fabs(diference));

                if( absdiference > phyreal(.0001) )
                {
                    direction.norm();

                    if( diference < 0 )
                        direction *= phyreal(-1);

                    direction *= absdiference;

                    if( node->tied )
                    {
                        if( !nodeNext->tied )
                        {
                            nodeNext->newposition -= direction;
                        }

                    }
                    else
                    {
                        if( !nodeNext->tied )
                        {
                            node->newposition += direction * phyreal(.5);
                            nodeNext->newposition += direction * phyreal(-.5);
                        }
                        else
                        {
                            node->newposition += direction;
                        }
                    }  
                }
                node->changed = true;
                nodeNext->changed = true;
            }
            
            // vertical

            nodeNext = node->nodes[1];

            if( !nodeNext )
                continue;

            vector3 direction( nodeNext->newposition - node->newposition );

            phyreal diference( direction.len() - (this->separationXAxis*2.f) );

            phyreal absdiference(fabs(diference));

            if( absdiference < phyreal(.0001) )
                continue;

            direction.norm();

            if( diference < 0 )
                direction *= phyreal(-1);

            direction *= absdiference;

            if( node->tied )
            {
                if( !nodeNext->tied )
                {
                    nodeNext->newposition -= direction;
                }

            }
            else
            {
                if( !nodeNext->tied )
                {
                    node->newposition += direction * phyreal(.5);
                    nodeNext->newposition += direction * phyreal(-.5);
                }
                else
                {
                    node->newposition += direction;
                }
            }  
            
            node->changed = true;
            nodeNext->changed = true;
        }

    }

    static nPhyGeomSphere* ray( static_cast<nPhyGeomSphere*>(nKernelServer::Instance()->New( "nphygeomsphere" )));
    ray->SetCategories(0);
    ray->SetCollidesWith(-1);

    static phyreal distance(phyreal(0.01));

    ray->SetRadius( distance );


    vector3 geomposition;

    this->GetGeometry()->GetPosition(geomposition);

    this->GetGeometry()->Disable();

    // nPhySpace* space( this->GetParentSpace() );
    /// check for collision

    for( int index(0); index < this->numCollisionPoints; ++index )
    {
        node = this->nodesCollisionPoints[ index ];

        if( node->tied )
            continue;

        ray->SetPosition( node->newposition + geomposition );

        nPhyCollide::nContact contact;

        if( nPhysicsServer::Instance()->Collide( ray, 1, &contact ) )
        //if( nPhyCollide::Collide( ray->Id(), space->Id(), 1, &contact ) )
        {       
            bool normalInverse(false);
            if( contact.GetGeometryA() == ray )
                normalInverse = true;

            vector3 normal;
            contact.GetContactPosition( node->newposition );
            contact.GetContactNormal( node->normal );

            if( normalInverse )
                node->normal *= phyreal(-1);

            node->normal *= -(distance + contact.GetContactPenetrationDepth());
            node->newposition += node->normal - geomposition;
            node->normals = 1;
            node->changed = true;
            node->normal.norm();
        }
    }

    this->GetGeometry()->Enable();

    node = nodesInfo;

    // calculating the half points
    for( int index(0); index < this->numHalfPoints; ++index )
    {
        node = this->nodesHalfPoints[ index ];

        node->newposition = vector3(0,0,0);

        int counter(0);

        for( int inner(0); inner < node->numNodes; ++inner )
        {
            if( node->nodes[ inner ] )
            {
                ++counter;
                node->newposition += node->nodes[ inner ]->newposition;
                
                if( node->nodes[inner]->normals )
                {
                    if( !node->normals )
                    {
                        node->normal = node->nodes[inner]->normal;
                        ++node->normals;
                        node->angle = 0;
                    }
                    else
                    {                                
                        ++node->normals;
                        node->angle += 1.f - fabs(node->normal.dot(node->nodes[inner]->normal)); 
                        node->normal += node->nodes[inner]->normal;
                        node->normal *= .5f;
                        node->angle *= .5f;
                    }
                }
            }                                        
        }

        if( counter )
        {
            node->newposition *= 1.f / phyreal( counter );
            if( node->normals > 0 )
            {
                static phyreal mult(1.f);
                if( node->normals > 1 )
                    node->newposition += node->normal * (distance * mult) * node->angle;
            }
        }

        node->changed = true;
    }

    node = nodesInfo;

    // calculating the rest of the points
    for( int index(0); index < this->numRestPoints; ++index )
    {
        node = this->nodesRestPoints[ index ];

        node->newposition = vector3(0,0,0);

        for( int inner(0); inner < node->numNodes; ++inner )
        {
            node->newposition += node->nodes[ inner ]->newposition;
        }

        if( node->numNodes )
        {
            node->newposition *= 1.f / phyreal( node->numNodes );
        }
        node->changed = true;
    }

    node = nodesInfo;
    // updating the positions
    for( unsigned index(0); index < this->numTotalNodes; ++index, ++node )
    {
        if( node->tied )
            continue;
        if( node->changed )
        {
            *node->oldposition = node->newposition;
        }
    }

    this->mesh->Update();
}

//-----------------------------------------------------------------------------
/**
    Function to be procesed during the collision check.

    @param numContacts maximun number of contacts expected
    @param contacts container of contacts

    @return if any collision was processed

    history:
        - 17-Jan-2005   Zombie         created
*/
bool ncPhyCloth::CollisionProcess( int /*numContacts*/, nPhyCollide::nContact* /*contacts*/ )
{
    return true;
}

//-----------------------------------------------------------------------------
/**
    Returns a node given coordinates.

    @param x x node coordinates
    @param z z node coordinates

    @return a node

    history:
        - 17-Jan-2005   Zombie         created
*/
ncPhyCloth::nodeInfo* ncPhyCloth::GetNode( int x, int z ) const
{
    if( x < 0 )
        return 0;
    if( z < 0 )
        return 0;

    if( x >= int(this->nodesXAxis) )
        return 0;

    if( z >= int(this->nodesZAxis) )
        return 0;

    return &this->nodesInfo[ x + z * this->nodesXAxis ];
}
//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
