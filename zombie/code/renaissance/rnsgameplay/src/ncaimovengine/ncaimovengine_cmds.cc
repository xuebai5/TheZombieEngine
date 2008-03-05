#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncaimovengine_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncaimovengine/ncaimovengine.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncAIMovEngine)
    NSCRIPT_ADDCMD_COMPOBJECT('IMVF', bool, MoveToFacing, 2, (const vector3&, const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IMV2', bool, MoveTo, 1, (const vector3&), 0, ());
    /*NSCRIPT_ADDCMD_COMPOBJECT('RMTZ', bool, MoveToZigZag, 4, (const vector3&, const vector3&, bool, nArray<bool> &), 0, ());*/
    NSCRIPT_ADDCMD_COMPOBJECT('IMOV', bool, Move, 2, (const vector3&, const vector3&), 0, ());
    /*NSCRIPT_ADDCMD_COMPOBJECT('RMZZ', bool, MoveZigZag, 3, (const vector3&, const vector3&, nArray<bool> &), 0, ());*/
    NSCRIPT_ADDCMD_COMPOBJECT('IJMP', bool, Jump, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGJP', vector3, GetJumpDistance, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGOJ', vector3, GetOriginalJumpDistance, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSJF', void, SetJumpFactor, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGJF', vector3, GetJumpFactor , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('R_DT', bool, DropTo, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISTP', void, Stop, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IFC2', bool, FaceTo, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IFCE', bool, Face, 1, (const quaternion&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('S_SM', void, SetStyle, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('R_GM', int, GetStyle, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSMD', void, SetMovDirection, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSMM', void, SetMovMode, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGMM', int, GetMovMode, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISES', void, SetMovEngineState, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGMS', int, GetMovEngineState , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIAC', bool, IsActioned, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIMV', bool, IsMoving, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IITN', bool, IsTurning, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIFG', bool, IsFacing, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIST', bool, IsStopped, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIJP', bool, IsJumping, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIDP', bool, IsDropping, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGES', float, GetEntitySpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGSP', float, GetSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISSP', void, SetSpeed, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGJR', float, GetJumpRange, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISPS', void, SetPause, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIPS', bool, IsPaused, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIAP', bool, IsApproaching, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IATF', bool, IsAtFloor, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGGL', const vector3&, GetGoal, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISGL', void, SetGoal, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISSR', void, SetStart, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGMI', float, GetMaxAvoidanceTurnInc, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESMI', void, SetMaxAvoidanceTurnInc, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGMA', float, GetMaxAvoidanceTurnAccel, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESMA', void, SetMaxAvoidanceTurnAccel, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGAT', float, GetAvoidanceTime, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESAT', void, SetAvoidanceTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGMT', float, GetMaxAvoidanceThrottleAccel, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESMT', void, SetMaxAvoidanceThrottleAccel, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FIES', bool, IsAtEndStretch, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncAIMovEngine::SaveCmds (nPersistServer* ps)
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // -- state
        int state = static_cast<int>(this->state);
        ps->Put (this->entityObject, 'ISES', state);

        // -- pause
        bool paused = this->IsPaused();
        //ps->Put (this->entityObject, 'IIPS', paused);
        ps->Put (this->entityObject, 'ISPS', paused);

        // -- start point
        ps->Put (this->entityObject, 'ISSR', this->start.x, this->start.y, this->start.z);

        // -- goal point
        ps->Put (this->entityObject, 'ISGL', this->goal.x, this->goal.y, this->goal.z);
    }

    return true;
}
