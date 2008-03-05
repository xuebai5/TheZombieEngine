#ifndef N_CLASSEDITORSTATE_H
#define N_CLASSEDITORSTATE_H
//------------------------------------------------------------------------------
/**
    @class nClassEditorState
    @ingroup NebulaConjurerEditor

    Editor state for class manipulation.

    (C) 2004 Conjurer Services, S.A.
*/
#include "conjurer/neditorstate.h"

//------------------------------------------------------------------------------
class nClassEditorState : public nEditorState
{
public:
    /// constructor
    nClassEditorState();
    /// destructor
    virtual ~nClassEditorState();

    /// build impostor level for given class
    void BuildImpostorLevel(const char *);

};

#endif /*N_CLASSEDITORSTATE_H*/
