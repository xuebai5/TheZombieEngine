#ifndef N_GEOMETRYSTREAMEDITORSTATE_H
#define N_GEOMETRYSTREAMEDITORSTATE_H
//------------------------------------------------------------------------------
/**
    @file ngeometrystreameditorstate.h
    @class nGeometryStreamEditorState
    @ingroup Conjurer
    @author MA Garcias <ma.garcias@yahoo.es>
    
    @brief Describes the processes to create, edit and build
    geometry streams using nGeometryStreamEditor interfaces.
    
    (C) 2006 Conjurer Services, S.A.
*/
#include "conjurer/neditorstate.h"

//------------------------------------------------------------------------------
class nGeometryStreamEditorState : public nEditorState
{
public:
    /// constructor
    nGeometryStreamEditorState();
    /// destructor
    virtual ~nGeometryStreamEditorState();
    /// called when state is becoming active
    virtual void OnStateEnter(const nString& prevState);
    /// called when state is becoming inactive
    virtual void OnStateLeave(const nString& nextState);

    /// create a new stream in the library
    nRoot* CreateStream(const char *);
    /// delete a stream from the library
    void DeleteStream(const char *);
    /// load stream library
    void LoadStreamLibrary();
    /// save stream library
    void SaveStreamLibrary();
    /// build all streams
    void BuildAllStreams();

private:

    nRef<nRoot> refStreamsRoot;
};

//------------------------------------------------------------------------------
#endif // N_GEOMETRYSTREAMEDITORSTATE_H
