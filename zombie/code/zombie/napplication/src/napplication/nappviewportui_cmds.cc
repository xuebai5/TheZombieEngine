#include "precompiled/pchnapplication.h"
//------------------------------------------------------------------------------
//  nappviewportui_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "napplication/nappviewportui.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nappviewportui

    @cppclass
    nAppViewportUI

    @superclass
    nroot

    @classinfo
    A layout manager for application viewports.
*/
NSCRIPT_INITCMDS_BEGIN(nAppViewportUI)
    NSCRIPT_ADDCMD('SCRT', void, SetClientRect, 4, (int, int, int, int), 0, ());
    NSCRIPT_ADDCMD('GCRT', void, GetClientRect, 0, (), 4, (int&, int&, int&, int&));
    NSCRIPT_ADDCMD('SMVP', void, SetMaximizedViewport, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('SSVP', void, SetSingleViewport, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GSVP', bool, GetSingleViewport, 0, (), 0, ());
    NSCRIPT_ADDCMD('SVIS', void, SetVisible, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GVIS', bool, GetVisible, 0, (), 0, ());
    NSCRIPT_ADDCMD('SBDS', void, SetBorderWidth, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GBDS', float, GetBorderWidth, 0, (), 0, ());
    NSCRIPT_ADDCMD('OPDB', void, OpenDragbar, 3, (const char *, const char *, float), 0, ());
    NSCRIPT_ADDCMD('CLDB', void, CloseDragbar, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('SDBK', void, SetDragbarKnob, 2, (const char *, float), 0, ());
    NSCRIPT_ADDCMD('GDBK', float, GetDragbarKnob, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('ADVP', void, AddViewport, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('DKVP', void, DockViewport, 3, (const char *, const char *, const char *), 0, ());
    NSCRIPT_ADDCMD('UDVP', void, UndockViewport, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('CLVP', void, CloseViewport, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GNVP', int, GetNumViewports, 0, (), 0, ());
    NSCRIPT_ADDCMD('ECLR', void, ClearAll, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nAppViewportUI::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd;

        // --- clearall ---
        // called to allow setting a state more than just once on this object
        cmd = ps->GetCmd(this, 'ECLR');
        ps->PutCmd(cmd);

        // --- setsingleviewport ---
        cmd = ps->GetCmd(this, 'SSVP');
        cmd->In()->SetB(this->GetSingleViewport());
        ps->PutCmd(cmd);

        // --- addviewport ---
        for (int i = 0; i < this->numViewports; i++)
        {
            cmd = ps->GetCmd(this, 'ADVP');
            cmd->In()->SetS(this->viewports[i].GetName().Get());
            ps->PutCmd(cmd);
        }

        for (int i = 0; i < this->dragBars.Size(); i++)
        {
            DragBar& dragBar = this->dragBars[i];

            // --- opendragbar ---
            cmd = ps->GetCmd(this, 'OPDB');
            cmd->In()->SetS(dragBar.name.Get());
            cmd->In()->SetS(DragBarTypeToString(dragBar.type));
            cmd->In()->SetF(dragBar.knob);
            ps->PutCmd(cmd);

            // --- dockviewport ---
            for (int vpIndex = 0; vpIndex < dragBar.numDockedViewports; vpIndex++)
            {
                cmd = ps->GetCmd(this, 'DKVP');
                cmd->In()->SetS(dragBar.docks[vpIndex].viewportName.Get());
                cmd->In()->SetS(dragBar.name.Get());
                cmd->In()->SetS(DockSideToString(dragBar.docks[vpIndex].side));
                ps->PutCmd(cmd);
            }
        }

        return true;
    }

    return false;
}
