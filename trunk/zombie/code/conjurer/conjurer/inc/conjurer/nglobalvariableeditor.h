#ifndef N_GLOBALVARIABLEEDITOR_H
#define N_GLOBALVARIABLEEDITOR_H
//------------------------------------------------------------------------------
/**
    @file nglobalvariableeditor.h
    @class nGlobalVariableEditor
    @ingroup Conjurer
    @author MA Garcias <ma.garcias@yahoo.es>
    
    @brief A helper class to provide a way to edit global variables.
    
    (C) 2006 Conjurer Services, S.A.
*/
#include "kernel/nroot.h"

class nVariableServer;
//------------------------------------------------------------------------------
class nGlobalVariableEditor : public nRoot
{
public:
    /// constructor
    nGlobalVariableEditor();
    /// destructor
    virtual ~nGlobalVariableEditor();

    /// load global variables
    void LoadGlobalVariables();

    /// save global variables
    void SaveGlobalVariables();

    /// is global variable context dirty
    bool IsDirty();

    /// FadeMinDistance, FadeMaxDistance
    void SetFadeDistances(float, float);
    void GetFadeDistances(float&, float&);    
    /// MaxPathFindingTime
    void SetMaxPathFindingTime(float);
    float GetMaxPathFindingTime();
    /// MaxPathSmoothingTime
    void SetMaxPathSmoothingTime(float);
    float GetMaxPathSmoothingTime();
    /// MinAgentsSpeed
    void SetMinAgentsSpeed(float);
    float GetMinAgentsSpeed();
    /// GroundSeekStartDistance, GroundSeekEndDistance
    void SetGroundSeekDistances(float, float);
    void GetGroundSeekDistances(float&, float&);

    //...add property methods for editable global variables

    /// notify when a global variable has changed
    void GlobalVariableChanged();

private:
    nAutoRef<nVariableServer> refVarServer;
    bool isDirty;
};

//------------------------------------------------------------------------------
#endif //N_GLOBALVARIABLEEDITOR_H
