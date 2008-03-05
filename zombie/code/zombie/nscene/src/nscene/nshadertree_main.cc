#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nshadertree_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nshadertree.h"

nNebulaScriptClass(nShaderTree, "nobject");

//------------------------------------------------------------------------------
/**
*/
nShaderTree::nShaderTree() :
    stackDepth(0),
    numNodes(0),
    currentNode(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nShaderTree::~nShaderTree()
{
    this->UnloadResource();
}

//------------------------------------------------------------------------------
/**
    load shader resources for all assigned nodes.
*/
bool
nShaderTree::LoadResource()
{
    for (int i = 0; i < this->numNodes; i++)
    {
        if (!this->nodeArray[i].refShader.isvalid())
        {
            if (!this->nodeArray[i].shaderName.IsEmpty())
            {
                nShader2 *shader = nGfxServer2::Instance()->NewShader(this->nodeArray[i].GetShaderName());
                if (!shader->IsLoaded())
                {
                    shader->SetFilename(this->nodeArray[i].GetShaderName());
                }
                this->nodeArray[i].refShader = shader;
            }
            // @todo ma.garcias set an error material for invalid material cases
            //this->nodeArray[i].refShader = ...
            //...
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    unload all owned shaders.
*/
void
nShaderTree::UnloadResource()
{
    for (int i = 0; i < this->numNodes; i++)
    {
        if (this->nodeArray[i].refShader.isvalid() &&
            this->nodeArray[i].refShader->IsValid())
        {
            this->nodeArray[i].refShader->Release();
            this->nodeArray[i].refShader.invalidate();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Push a node into the decision tree.
    Find a child node with the specifed key value, or create it if not found.
*/
void
nShaderTree::BeginNode(nVariable::Handle key, const nFourCC val)
{
    // get node at the top of the stack
    Node *currentNode = 0;
    if (this->stackDepth > 0)
    {
        currentNode = this->nodeStack[this->stackDepth - 1];
    }
    
    // if there is a current node, find if any of its children has the key
    if (currentNode)
    {
        Node *child;
        for (child = currentNode->first; child; child = child->next)
        {
            if (child->key == key && child->value == val)
            {
                this->nodeStack[this->stackDepth++] = child;
                return;
            }
        }
    }
    
    // if there is no current node, or none of its children has the key
    n_assert2(this->numNodes < MaxNodes, "ma.garcias- Exceeded max size of material tree.")
    Node *newNode = &this->nodeArray[this->numNodes++];
    memset(newNode, 0, sizeof(Node));
    
    if (currentNode)
    {
        newNode->parent = currentNode;
        Node *node = currentNode->first;
        if (node)
        {
            while (node->next)
            {
                node = node->next;
            }
            node->next = newNode;
        }
        else
        {
            currentNode->first = newNode;
        }
    }
    
    newNode->key = key;
    newNode->value = val;
    newNode->shaderIndex = -1;
    this->nodeStack[this->stackDepth++] = newNode;

    // initialize current node to root
    this->currentNode = this->GetNodeAt(0);
}

//------------------------------------------------------------------------------
/**
    Set shader filename to current node.
*/
void
nShaderTree::SetShader(const char *shaderName)
{
    n_assert(this->stackDepth > 0);
    Node *currentNode = this->nodeStack[this->stackDepth - 1];
    n_assert(currentNode);
    currentNode->shaderName = shaderName;
}

//------------------------------------------------------------------------------
/**
    Set shader filename to current node.
*/
void
nShaderTree::SetShaderObject(nShader2* shader)
{
    n_assert(this->stackDepth > 0);
    Node *currentNode = this->nodeStack[this->stackDepth - 1];
    n_assert(currentNode);
    currentNode->refShader = shader;
}

//------------------------------------------------------------------------------
/**
    Set shader technique to current node.
*/
void
nShaderTree::SetTechnique(const char* technique)
{
    n_assert(this->stackDepth > 0);
    Node *currentNode = this->nodeStack[this->stackDepth - 1];
    n_assert(currentNode);
    currentNode->technique = technique;
}

//------------------------------------------------------------------------------
/**
    Set shader technique to current node.
*/
void
nShaderTree::SetSequence(const char* sequence)
{
    n_assert(this->stackDepth > 0);
    Node *currentNode = this->nodeStack[this->stackDepth - 1];
    n_assert(currentNode);
    currentNode->sequence = sequence;
}

//------------------------------------------------------------------------------
/**
    Go up a level in the decision tree.
*/
void
nShaderTree::EndNode()
{
    n_assert(this->stackDepth > 0);
    this->stackDepth--;
}

//------------------------------------------------------------------------------
/**
    Remove current decision node.
*/
void
nShaderTree::Remove()
{
    n_assert(this->stackDepth > 0);
    Node *currentNode = this->nodeStack[this->stackDepth - 1];
    n_assert(currentNode);
    // @todo ma.garcias -remove effectively, for now just invalidate 
    currentNode->key = nVariable::InvalidHandle;
    currentNode->value = 0;
}

//------------------------------------------------------------------------------
/**
    Get thennumber of nodes
*/
int
nShaderTree::GetNumNodes()
{
    return this->numNodes;
}

//------------------------------------------------------------------------------
/**
    get shader for node at index
*/
nShader2*
nShaderTree::GetShaderAt(int index)
{
    n_assert(index < this->numNodes);
    return this->nodeArray[index].GetShader();
}

//------------------------------------------------------------------------------
/**
    get shader name for node at index
*/
const char*
nShaderTree::GetShaderNameAt(int index)
{
    n_assert(index < this->numNodes);
    return this->nodeArray[index].GetShaderName();
}

//------------------------------------------------------------------------------
/**
    get technique for node at index
*/
const char*
nShaderTree::GetTechniqueAt(int index)
{
    n_assert(index < this->numNodes);
    return this->nodeArray[index].GetTechnique();
}

//------------------------------------------------------------------------------
/**
    get sequence for node at index
*/
const char*
nShaderTree::GetSequenceAt(int index)
{
    n_assert(index < this->numNodes);
    return this->nodeArray[index].GetSequence();
}

//------------------------------------------------------------------------------
/**
    set shader index for the scene
*/
void
nShaderTree::SetShaderIndexAt(int index, int shaderIndex)
{
    n_assert(index < this->numNodes);
    this->nodeArray[index].shaderIndex = shaderIndex;
}

//------------------------------------------------------------------------------
/**
    Check if parameter is used as a key, or else check if parameter is
    used in any of owned (and loaded) shaders.
*/
bool
nShaderTree::IsParameterUsed(nShaderState::Param p)
{
    for (int i = 0; i < this->numNodes; i++)
    {
        if (this->nodeArray[ i ].refShader.isvalid() &&
            this->nodeArray[ i ].refShader->IsValid())
        {
            if (this->nodeArray[ i ].refShader->IsParameterUsed(p))
            {
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShaderTree::SetSelectorType(nFourCC selectorType)
{
    // find if there is a child of current node with this selector type
    n_assert_return( this->currentNode, false );

    // while the current node has children
    if (this->currentNode->first)
    {
        for (Node *child = this->currentNode->first; child; child = child->next)
        {
            if (child->value == selectorType)
            {
                this->currentNode = child;

                return true;
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    select a shader from the decision tree, return scene index
*/
int
nShaderTree::SelectShaderIndex()
{
    n_assert_return( this->currentNode, -1 );

    int shaderIndex = this->currentNode->shaderIndex;

    this->currentNode = this->GetNodeAt(0);

    return shaderIndex;
}

//------------------------------------------------------------------------------
/**
*/
int
nShaderTree::GetCurrentShaderIndex()
{
    n_assert_return( this->currentNode, -1 );
    
    return this->currentNode->shaderIndex;
}

//------------------------------------------------------------------------------
/**
*/
void
nShaderTree::InvalidateCurrent()
{
    if (this->currentNode->parent)
    {
        Node* toRemove = this->currentNode;

        this->currentNode = this->currentNode->parent;

        if (this->currentNode->first == toRemove)
        {
            this->currentNode->first = toRemove->next;
        }
        else
        {
            for (Node* curNode = this->currentNode->first; curNode; curNode = curNode->next)
            {
                if (curNode->next == toRemove)
                {
                    curNode->next = toRemove->next;
                    break;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nShaderTree::ResetCurrentNode()
{
    this->currentNode = this->GetNodeAt(0);
}
