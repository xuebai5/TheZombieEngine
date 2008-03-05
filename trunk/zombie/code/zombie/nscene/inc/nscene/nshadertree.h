#ifndef N_SHADERTREE_H
#define N_SHADERTREE_H
//------------------------------------------------------------------------------
/**
    @class nShaderTree
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>
    
    @brief A composite of shaders for environment-dependent surfaces.
    
    (C) 2006 Conjurer Services, S.A.
*/

#include "kernel/nobject.h"

class nShader2;

//------------------------------------------------------------------------------
class nShaderTree : public nObject
{
public:
    /// constructor
    nShaderTree();
    /// destructor
    virtual ~nShaderTree();
    /// object persistency
    virtual bool SaveCmds(nPersistServer* ps);
    
    /// push a new node
    void BeginNode(nVariable::Handle h, const nFourCC val);
    /// push a new node (script only)
    void BeginNode(const char *name, const char *value);
    /// assign shader filename to the current node in the decision tree
    void SetShader(const char *shaderName);
    /// assign shader object to the current node in the decision tree
    void SetShaderObject(nShader2* shader);
    /// set shader technique to the current node in the decision tree
    void SetTechnique(const char *technique);
    /// set sequence to the current node in the decision tree
    void SetSequence(const char *sequence);
    /// go up a level in the decision
    void EndNode();
    /// cancel and remove current branch
    void Remove();

    /// set name
    void SetName(const char *);
    /// get name
    const char *GetName();

    /// get number of nodes
    int GetNumNodes();
    /// get shader for node at index
    nShader2* GetShaderAt(int index);
    /// get shader name for node at index
    const char* GetShaderNameAt(int index);
    /// get technique for node at index
    const char* GetTechniqueAt(int index);
    /// get sequence for node at index
    const char* GetSequenceAt(int index);
    /// set shader index for node at index
    void SetShaderIndexAt(int index, int shaderIndex);

    /// load the shader resource file
    virtual bool LoadResource();
    /// unload shader resources
    virtual void UnloadResource();
    
    /// check if shader parameter is used by any effect
    bool IsParameterUsed(nShaderState::Param param);

    /// set type value
    bool SetSelectorType(nFourCC fourcc);
    /// select a shader for the current set of parameters
    int SelectShaderIndex();
    /// get shader index for current decision node
    int GetCurrentShaderIndex();
    /// remove current node from the tree
    void InvalidateCurrent();
    /// reset current node to the root node
    void ResetCurrentNode();

    /// end current tree (to keep backwards compatibility with nmaterial)
    void EndPass();
    
protected:

    /// the decision tree:
    struct Node
    {
        /// get shader filename
        const char* GetShaderName();
        /// get shader technique
        const char* GetTechnique();
        /// get shader technique
        const char* GetSequence();
        /// get shader object
        nShader2* GetShader();

        Node *parent;           // parent node
        Node *first;            // first child node
        Node *next;             // next child of same parent node
        nVariable::Handle key;
        nFourCC value;
        nString shaderName;
        nString technique;
        nString sequence;
        nRef<nShader2> refShader;
        int shaderIndex;        // index in the scene shader database
    };
    
    /// helper method to recursively persist a node
    bool SaveNode(nPersistServer* ps, nShaderTree::Node *currentNode);

    /// get node by index
    Node *GetNodeAt(const int index);

    enum
    {
        MaxNodes = 32,
        MaxDepth = 8,
    };

    Node* nodeStack[MaxDepth];
    int stackDepth;
    Node nodeArray[MaxNodes];
    int numNodes;

    Node* currentNode;

    nString treeName;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderTree::SetName(const char* name)
{
    this->treeName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nShaderTree::GetName()
{
    return this->treeName.IsEmpty() ? 0 : this->treeName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nShaderTree::Node::GetShaderName()
{
    return this->shaderName.IsEmpty() ? 0 : this->shaderName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nShaderTree::Node::GetTechnique()
{
    return this->technique.IsEmpty() ? 0 : this->technique.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nShaderTree::Node::GetSequence()
{
    return this->sequence.IsEmpty() ? 0 : this->sequence.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nShaderTree::Node::GetShader()
{
    return this->refShader.isvalid() ? this->refShader.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderTree::Node *
nShaderTree::GetNodeAt(const int index)
{
    return &this->nodeArray[index];
}

//------------------------------------------------------------------------------
#endif /*N_SHADERTREE_H*/
