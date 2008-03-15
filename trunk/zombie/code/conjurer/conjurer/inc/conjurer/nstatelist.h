#ifndef N_STATELIST_H
#define N_STATELIST_H
//-------------------------------------------------------------------
/**
    @file ncmdsaver.cc
    @ingroup NebulaConjurerEditor
    
    @author Javier Ferrer Gomez
    
    @brief This class is called by the persist server for get the list
    of all commands that compose the state.
    
    (C) 2005 Conjurer Services, S.A.
*/

#include "kernel/nscriptserver.h"

class nStateList: public nScriptServer
{
public:
    nStateList();
    virtual ~nStateList();

    virtual bool BeginWrite(nFile *, nObject*, nCmd *);
    virtual bool WriteBeginNewObject(nFile *, nObject *, nObject *);
    virtual bool WriteBeginNewObjectCmd(nFile *, nObject *, nObject *, nCmd *);
    /// Translate and write a cmd message
    virtual bool WriteCmd(nFile *file, nCmd *cmd);
    virtual bool WriteEndObject(nFile*, nObject *, nObject *);
    virtual bool EndWrite(nFile*);

    nArray<nCmd*> GetStateList();

protected:    
    nArray<nCmd*>        stateList;
    int                  numNestedObjects;

};

inline
nArray<nCmd*>
nStateList::GetStateList()
{    
    return this->stateList;    
}
#endif
