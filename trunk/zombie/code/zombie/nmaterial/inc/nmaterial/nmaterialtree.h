#ifndef N_MATERIALSHADER_H
#define N_MATERIALSHADER_H
//------------------------------------------------------------------------------
/**
    @class nMaterialTree
    @ingroup NebulaMaterialSystem

    @brief A shader composite for environment-dependent materials

    (C) 2004 Conjurer Services, S.A.
*/

class nShader2;
//------------------------------------------------------------------------------
class nMaterialTree : public nRoot
{
public:
    /// constructor
    nMaterialTree();
    /// destructor
    virtual ~nMaterialTree();
    
    /// push a new node in the decision tree
    void BeginNode(nVariable::Handle h, const nFourCC val);
    /// assign shader filename to the current node in the decision tree
    void SetShader(const char *shaderName);
    /// assign shader object to the current node in the decision tree
    void SetShaderObject(nShader2* shader);
    /// set shader technique to the current node in the decision tree
    void SetTechnique(const char *technique);
    /// go up a level in the decision
    void EndNode();
    /// cancel and remove current branch
    void Remove();

    /// get number of nodes
    int GetNumNodes();
    /// get shader for node at index
    nShader2* GetShaderAt(int index);
    /// get shader name for node at index
    const char* GetShaderNameAt(int index);
    /// get technique for node at index
    const char* GetTechniqueAt(int index);
    /// set shader index for node at index
    void SetShaderIndexAt(int index, int shaderIndex);

    /// load the shader resource file
    virtual bool LoadResource();
    /// unload shader resources
    virtual void UnloadResource();
    
    /// is parameter used by effect?
    bool IsParameterUsed(nShaderState::Param p);
    /// set type value
    void SetSelectorType(nFourCC fourcc);

    /// select a shader for the current set of parameters
    int SelectShaderIndex();
    /// select a shader for the current set of parameters
    nShader2 *SelectShader();
    
protected:
    friend class nMaterial;

    // the decision tree:
    struct Node
    {
        const char* GetShaderName();
        const char* GetTechnique();
        nShader2* GetShader();

        Node *parent;           // parent node
        Node *first;            // first child node
        Node *next;             // next child of same parent node
        nVariable::Handle key;
        nFourCC value;
        nString shaderName;
        nString technique;
        nRef<nShader2> refShader;
        int shaderIndex;        // index in the scene shader database
    };
    
    struct NodeKey
    {
        nFourCC type;
    };
    
    /// get node by index
    Node *GetNodeAt(const int index);
    /// find a case node with given code
    bool FindNodeWithType(nFourCC fourcc);
    /// find and invalidate selector node
    bool FindNodeType(nFourCC fourcc);
    /// select the branch for the current set of parameters
    Node* SelectNode();
    
    enum
    {
        MaxNodes = 32,
        MaxDepth = 8,
    };
    Node *nodeStack[MaxDepth];
    int stackDepth;
    Node nodeArray[MaxNodes];
    int numNodes;
    NodeKey nodeKeys[MaxDepth];
    int numNodeKeys;
};

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nMaterialTree::Node::GetShaderName()
{
    return this->shaderName.IsEmpty() ? 0 : this->shaderName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nMaterialTree::Node::GetTechnique()
{
    return this->technique.IsEmpty() ? 0 : this->technique.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nMaterialTree::Node::GetShader()
{
    return this->refShader.isvalid() ? this->refShader.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMaterialTree::Node *
nMaterialTree::GetNodeAt(const int index)
{
    return &this->nodeArray[index];
}

//------------------------------------------------------------------------------
/**
    is there a NodeKey with a given key?
*/
inline
bool
nMaterialTree::FindNodeWithType(nFourCC type)
{
    for (int i = 0; i < this->numNodes; i++)
    {
        if (this->nodeArray[i].value == type)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    is there a NodeKey with a given key and value? 
    if so, return true and invalidate it to avoid using it again
    in the same selection process.
*/
inline
bool
nMaterialTree::FindNodeType(nFourCC fourcc)
{
    n_assert(this->numNodeKeys < MaxDepth);
    for (int i = 0; i < this->numNodeKeys; i++)
    {
        if (this->nodeKeys[i].type == fourcc)
        {
            this->nodeKeys[i].type = 0;
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
#endif // N_MATERIALSHADER_H
