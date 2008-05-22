#ifndef NC_PHYCLOTH_H
#define NC_PHYCLOTH_H
//-----------------------------------------------------------------------------
/**
    @class ncPhyCloth
    @ingroup NebulaPhysicsSystem
    @brief Specializated object that represents a rectangular piece of cloth.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Cloth
    
    @cppclass ncPhyCloth
    
    @superclass ncPhySimpleObj

    @classinfo Specializated object that represents a rectangular piece of cloth.
*/

//-----------------------------------------------------------------------------

#include "nphysics/ncphysimpleobj.h"

//-----------------------------------------------------------------------------
class nPhyGeomTriMesh;
//-----------------------------------------------------------------------------
class ncPhyCloth : public ncPhySimpleObj
{

    NCOMPONENT_DECLARE(ncPhyCloth,ncPhySimpleObj);

public:

    /// constructor
    ncPhyCloth();

    /// destructor
    ~ncPhyCloth();

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// sets the number of nodes in the x-axis 
    void SetNodesXAxis( const unsigned number );

    /// sets the number of nodes in the Z-axis 
    void SetNodesZAxis( const unsigned number );

    /// separation between nodes in the x-axis
    void SetSeparationXAxis( const phyreal separation );

    /// separation between nodes in the z-axis
    void SetSeparationZAxis( const phyreal separation );

    /// creates the object
    void Create( nPhysicsWorld* world );

    /// object persistency
    bool SaveCmds(nPersistServer* ps);

    /// adds a force
    void AddForce( const vector3& force );

    /// function to be process after running the simulation
    void PostProcess();

    /// un/sets tied point (not allowed to tie completely, max two sides)
    void SetTiedNode( const unsigned xNode, const unsigned zNode, bool is );

    /// function to be procesed during the collision check.
    bool CollisionProcess( int numContacts, nPhyCollide::nContact* contacts );

private:

    /// stores the nodes in the x-axis
    unsigned nodesXAxis;
    /// stores the nodes in the y-axis
    unsigned nodesZAxis;

    /// stores the separation of x-axis
    phyreal separationXAxis;

    /// stores the separation of z-axis
    phyreal separationZAxis;

    /// stores the total number of nodes
    unsigned numTotalNodes;

    /// builds the mesh representing the rectangular piece of cloth
    void BuildMesh();

    /// stores the vertexs buffer
    phyreal* bufferVertexes;

    /// holds a reference to the geometry
    nPhyGeomTriMesh* mesh;

    /// stores the indexes buffer
    int* bufferIndexes;

    /// stores the default value of the mass
    static phyreal massCloth;

    /// stores the default value of the air-resistance
    static phyreal airResistance;

    /// stores the velocity
    vector3 velocity;

    /// stores the accumulated force
    vector3 forceAccumulated;

    /// forces per node and tied info
    struct nodeInfo {
        vector3 velocity;

        vector3 force;

        vector3 newposition;

        bool tied : 1;

        bool changed : 1;

        int normals;

        vector3 normal;

        vector3* oldposition;

        int numNodes;

        nodeInfo* nodes[4];

        phyreal angle;
    };

    /// stores nodes info
    nodeInfo* nodesInfo;

    /// stores the collision points
    nodeInfo** nodesCollisionPoints;

    /// stores the number of collision points
    int numCollisionPoints;

    /// stores the half points
    nodeInfo** nodesHalfPoints;

    /// stores the number of half points
    int numHalfPoints;

    /// stores the rest points
    nodeInfo** nodesRestPoints;

    /// stores the number of rest points
    int numRestPoints;

    /// moves the cloth in the world
    void Move();

    /// returns a node given coordinates
    nodeInfo* GetNode( int x, int z ) const;

};

//-----------------------------------------------------------------------------
/**
    Sets the number of nodes in the x-axis.

    @param number it's the number of nodes in the x-axis.

    history:
        - 05-Jun-2005   Zombie         created
*/
inline
void ncPhyCloth::SetNodesXAxis( const unsigned number )
{
    n_assert2( number > 1, "Number of nodes has to bigger than 1." );

    this->nodesXAxis = number;
}

//-----------------------------------------------------------------------------
/**
    Sets the number of nodes in the z-axis.

    @param number it's the number of nodes in the z-axis.

    history:
        - 05-Jun-2005   Zombie         created
*/
inline
void ncPhyCloth::SetNodesZAxis( const unsigned number )
{
    n_assert2( number > 1, "Number of nodes has to bigger than 1." );

    this->nodesZAxis = number;
}

//-----------------------------------------------------------------------------
/**
    Separation between nodes in the x-axis.

    @param separation it's the separation between nodes in the x-axis

    history:
        - 05-Jun-2005   Zombie         created
*/
inline
void ncPhyCloth::SetSeparationXAxis( const phyreal separation )
{
    n_assert2( separation > 0, "Number of nodes has to bigger than 0." );

    this->separationXAxis = separation;
}

//-----------------------------------------------------------------------------
/**
    Separation between nodes in the z-axis.

    @param separation it's the separation between nodes in the z-axis

    history:
        - 05-Jun-2005   Zombie         created
*/
inline
void ncPhyCloth::SetSeparationZAxis( const phyreal separation )
{
    n_assert2( separation > 0, "Number of nodes has to bigger than 0." );

    this->separationZAxis = separation;
}

#endif