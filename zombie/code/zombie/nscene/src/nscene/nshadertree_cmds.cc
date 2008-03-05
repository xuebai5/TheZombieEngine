#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nshadertree_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nshadertree.h"

NSCRIPT_INITCMDS_BEGIN(nShaderTree)
    NSCRIPT_ADDCMD('BGNN', void, BeginNode, 2, (const char *, const char *), 0, ());
    NSCRIPT_ADDCMD('ENDN', void, EndNode, 0, (), 0, ());
    NSCRIPT_ADDCMD('SSHD', void, SetShader, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('STEC', void, SetTechnique, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('SSEQ', void, SetSequence, 1, (const char *), 0, ());
    //for backwards compatibility with persisted materials:
    NSCRIPT_ADDCMD('ENDP', void, EndPass, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
void
nShaderTree::BeginNode(const char *name, const char *value)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nFourCC fourcc = nVariableServer::StringToFourCC(value);
    this->BeginNode(handle, fourcc);
}

//------------------------------------------------------------------------------
/**
    keep this for backwards compatibility only with persisted materials
*/
void
nShaderTree::EndPass()
{
    this->EndObject();
}

//------------------------------------------------------------------------------
/**
    Persist the material shader internal decision tree.
*/
bool
nShaderTree::SaveNode(nPersistServer* ps, nShaderTree::Node *currentNode)
{
    n_assert(currentNode);
    nCmd* cmd;

    // --- setshader ---
    cmd = ps->GetCmd(this, 'SSHD');
    cmd->In()->SetS(currentNode->shaderName.Get());
    ps->PutCmd(cmd);

    if (currentNode->GetTechnique())
    {
        // --- settechnique ---
        cmd = ps->GetCmd(this, 'STEC');
        cmd->In()->SetS(currentNode->GetTechnique());
        ps->PutCmd(cmd);
    }

    if (currentNode->GetSequence())
    {
        // --- setsequence ---
        cmd = ps->GetCmd(this, 'SSEQ');
        cmd->In()->SetS(currentNode->GetSequence());
        ps->PutCmd(cmd);
    }

    nShaderTree::Node *child;
    for (child = currentNode->first; child; child = child->next)
    {
        // skip an invalid branch of the tree (due to invalid shader)
        if (child->key != nVariable::InvalidHandle)
        {
            // --- beginnode ---
            cmd = ps->GetCmd(this, 'BGNN');
            char buf[5];
            cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(child->key));
            cmd->In()->SetS(nVariableServer::FourCCToString(child->value, buf, sizeof(buf)));
            ps->PutCmd(cmd);
            
            this->SaveNode(ps, child);
            
            // --- endnode ---
            cmd = ps->GetCmd(this, 'ENDN');
            ps->PutCmd(cmd);
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShaderTree::SaveCmds(nPersistServer* ps)
{
    if (this->GetNumNodes() > 0)
    {
        this->SaveNode(ps, this->GetNodeAt(0));
    }

    return true;
}
