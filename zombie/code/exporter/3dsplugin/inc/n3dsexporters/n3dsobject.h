#ifndef N_3DS_OBJECT_H
#define N_3DS_OBJECT_H
//------------------------------------------------------------------------------
/**
    @class n3dsObject
    @ingroup n3dsMaxExporterKernel

    Encapsulate a Igame object with others properties to facilities export

    (C) 2005 Conjurer Services, S.A.
*/
class IGameNode;
class INode;

class n3dsObject
{
public:
    enum ObjectType
    {
        graphics = 0,
        physics,
        visibility,
        plugs,
        NumTypes,  // keep this always at the end!
        InvalidType
    };

    enum ObjectSubType
    {
        visCell  = 0,
        visPortal ,
        visOccluder,
        NumSubTypes,
        InvalidSubType
    };

    n3dsObject();
    n3dsObject(IGameNode* node);
    /// return IGameNode
    IGameNode* GetNode() const { return node; };
    /// return type
    ObjectType GetType() const { return type; };   
    /// return type
    ObjectSubType GetSubType() const { return subType; };
    /// return nebula object type from IGame node 
    static ObjectType GetType(IGameNode* const node);

    /// return nebula object type from Max node 
    static ObjectSubType GetSubType(IGameNode* const node, ObjectType type);
private:
    static ObjectSubType GetSubTypeVisibility(IGameNode* const node);
    /// return nebula object type from Max node 
    static ObjectType GetType(INode* const node);
    IGameNode*  node;
    ObjectType  type;
    ObjectSubType subType;
};

//------------------------------------------------------------------------------
/**
*/
inline
n3dsObject::n3dsObject() : node(0) , type(InvalidType) , subType(InvalidSubType)
{

}



#endif

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------