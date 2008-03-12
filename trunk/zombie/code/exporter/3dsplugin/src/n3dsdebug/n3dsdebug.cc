#include "precompiled/pchn3dsmaxexport.h"
#include "n3dsdebug/n3dsdebug.h"

const char* n3dsDebugINodeGetName( INode* node)
{
    const char* val="";
    if (node)
    {
        val = node->GetName();
    } 
    return val;
}

const char* n3dsDebugIGameNodeGetName( IGameNode* node)
{
    const char* val="";
    if (node)
    {
        val = node->GetName();
    } 
    return val;
}

const char* n3dsDebugMtlGetName( Mtl* mat )
{
    const char* val="";
    if (mat)
    {
        val = mat->GetName();
    } 
    return val;
}

