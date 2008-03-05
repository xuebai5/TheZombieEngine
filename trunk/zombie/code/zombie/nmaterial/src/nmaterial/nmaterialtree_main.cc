#include "precompiled/pchnmaterial.h"
//------------------------------------------------------------------------------
//  nmaterialtree_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nmaterial/nmaterialtree.h"

nNebulaClass(nMaterialTree, "nroot");

//------------------------------------------------------------------------------
/**
*/
nMaterialTree::nMaterialTree() :
    stackDepth(0),
    numNodes(0),
    numNodeKeys(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMaterialTree::~nMaterialTree()
{
    this->UnloadResource();
}

//------------------------------------------------------------------------------
/**
    load shader resources for all assigned nodes.
*/
bool
nMaterialTree::LoadResource()
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
nMaterialTree::UnloadResource()
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
nMaterialTree::BeginNode(nVariable::Handle key, const nFourCC val)
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
    n_assert2(this->numNodes < 32, "ma.garcias- Exceeded max size of material tree.")
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
    this->nodeStack[this->stackDepth++] = newNode;
}

//------------------------------------------------------------------------------
/**
    Set shader filename to current node.
*/
void
nMaterialTree::SetShader(const char *shaderName)
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
nMaterialTree::SetShaderObject(nShader2* shader)
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
nMaterialTree::SetTechnique(const char* technique)
{
    n_assert(this->stackDepth > 0);
    Node *currentNode = this->nodeStack[this->stackDepth - 1];
    n_assert(currentNode);
    currentNode->technique = technique;
}

//------------------------------------------------------------------------------
/**
    Go up a level in the decision tree.
*/
void
nMaterialTree::EndNode()
{
    n_assert(this->stackDepth > 0);
    this->stackDepth--;
}

//------------------------------------------------------------------------------
/**
    Remove current decision node.
*/
void
nMaterialTree::Remove()
{
    n_assert(this->stackDepth > 0);
    Node *currentNode = this->nodeStack[this->stackDepth - 1];
    n_assert(currentNode);
    // @todo remove effectively, for now just invalidate 
    currentNode->key = nVariable::InvalidHandle;
    currentNode->value = 0;
}

//------------------------------------------------------------------------------
/**
    Get thennumber of nodes
*/
int
nMaterialTree::GetNumNodes()
{
    return this->numNodes;
}

//------------------------------------------------------------------------------
/**
    get shader for node at index
*/
nShader2*
nMaterialTree::GetShaderAt(int index)
{
    n_assert(index < this->numNodes);
    return this->nodeArray[index].GetShader();
}

//------------------------------------------------------------------------------
/**
    get technique for node at index
*/
const char*
nMaterialTree::GetShaderNameAt(int index)
{
    n_assert(index < this->numNodes);
    return this->nodeArray[index].GetShaderName();
}

//------------------------------------------------------------------------------
/**
    get technique for node at index
*/
const char*
nMaterialTree::GetTechniqueAt(int index)
{
    n_assert(index < this->numNodes);
    return this->nodeArray[index].GetTechnique();
}

//------------------------------------------------------------------------------
/**
    set shader index for the scene
*/
void
nMaterialTree::SetShaderIndexAt(int index, int shaderIndex)
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
nMaterialTree::IsParameterUsed(nShaderState::Param p)
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
void
nMaterialTree::SetSelectorType(nFourCC fourcc)
{
    if (this->FindNodeWithType(fourcc))
    {
        //HACK- there is a bug linking lights to brushes, this allows working
        if (this->numNodeKeys < MaxDepth - 1)
        //
        {
            NodeKey *nodeKey = &this->nodeKeys[this->numNodeKeys++];
            nodeKey->type = fourcc;
        }
    }
}

//------------------------------------------------------------------------------
/**
    select a shader from the decision tree
*/
nShader2*
nMaterialTree::SelectShader()
{
    Node* node = this->SelectNode();
    n_assert(node);
    return node->GetShader();
}

//------------------------------------------------------------------------------
/**
    select a shader from the decision tree, return scene index
*/
int
nMaterialTree::SelectShaderIndex()
{
    Node* node = this->SelectNode();
    n_assert(node);
    if (node->refShader.isvalid())
    {
        return node->shaderIndex;
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    select a shader from the decision tree, return scene index
*/
nMaterialTree::Node*
nMaterialTree::SelectNode()
{
    n_assert(this->numNodes > 0);
    Node *currentNode = this->GetNodeAt(0);
    
    // while the current node has children
    while (currentNode->first)
    {
        for (Node *child = currentNode->first; child; child = child->next)
        {
            if (this->FindNodeType(child->value))
            {
                currentNode = child;
                goto NextLevel;
            }
        }
        break;
    NextLevel: ;
    }
    
    this->numNodeKeys = 0;

    n_assert(currentNode);
    //n_assert(currentNode->refShader.isvalid());
    
    return currentNode;
}
