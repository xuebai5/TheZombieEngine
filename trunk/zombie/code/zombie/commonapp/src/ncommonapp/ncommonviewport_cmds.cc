#include "precompiled/pchncommonapp.h"
//------------------------------------------------------------------------------
//  neditorviewport_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncommonapp/ncommonviewport.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    ncommonviewport

    @cppclass
    nCommonViewport

    @superclass
    nappviewport

    @classinfo
    An encapsulation of viewport for Commonr.
*/
NSCRIPT_INITCMDS_BEGIN(nCommonViewport)
    NSCRIPT_ADDCMD('SRSF', void, SetResourceFile, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GRSF', const char *, GetResourceFile, 0, (), 0, ());
    NSCRIPT_ADDCMD('GVPE', nEntityObject*, GetViewportEntity, 0, (), 0, ());
    NSCRIPT_ADDCMD('MBVE', nEntityObject*, BeginViewportEntity, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('LSOL', void, SetObeyLightLinks, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('LGMX', float, GetMouseXFactor , 0, (), 0, ());
    NSCRIPT_ADDCMD('LSMX', void, SetMouseXFactor, 1, (float), 0, ());
    NSCRIPT_ADDCMD('LGMY', float, GetMouseYFactor , 0, (), 0, ());
    NSCRIPT_ADDCMD('LSMY', void, SetMouseYFactor, 1, (float), 0, ());
    NSCRIPT_ADDCMD('LGYI', bool, GetMouseYInvert , 0, (), 0, ());
    NSCRIPT_ADDCMD('LSYI', void, SetMouseYInvert, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('SMRD', void, SetMaxRenderDistance, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GMRD', float, GetMaxRenderDistance, 0, (), 0, ());
    NSCRIPT_ADDCMD('RSOR', void, SetOcclusionRadius, 1, (float), 0, ());
    NSCRIPT_ADDCMD('RGOR', float, GetOcclusionRadius, 0, (), 0, ());
    NSCRIPT_ADDCMD('RSMO', void, SetMaxNumOccluders, 1, (int), 0, ());
    NSCRIPT_ADDCMD('RGMO', int, GetMaxNumOccluders, 0, (), 0, ());
    NSCRIPT_ADDCMD('RSAA', void, SetAreaAngleMin, 1, (float), 0, ());
    NSCRIPT_ADDCMD('RGAA', float, GetAreaAngleMin, 0, (), 0, ());
    NSCRIPT_ADDCMD('LSOM', void, SetObeyMaxDistance, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('LGOM', bool, GetObeyMaxDistance, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nCommonViewport::SaveCmds(nPersistServer* ps)
{
    if (nAppViewport::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setresourcefile ---
        cmd = ps->GetCmd(this, 'SRSF');
        cmd->In()->SetS(this->GetResourceFile());
        ps->PutCmd(cmd);

        if (this->IsOpen())//used to persist the viewport state
        {
            const char* className = this->refViewport->GetClass()->GetName();

            //--- beginviewportentity ---
            cmd = ps->GetCmd(this, 'MBVE');
            cmd->In()->SetS(className);

            if (ps->BeginObjectWithCmd(this, cmd))
            {
                if (!refViewport->SaveCmds(ps))
                {
                    return false;
                }

                ps->EndObject(false);
            }
        }

        //--- setmousexfactor ---
        ps->Put( this, 'LSOL', this->GetObeyLightLinks() );

        //--- setmousexfactor ---
        ps->Put( this, 'LSMX', this->GetMouseXFactor() );

        //--- setmouseyfactor ---
        ps->Put( this, 'LSMY', this->GetMouseYFactor() );

        //--- setmousezfactor ---
        ps->Put( this, 'LSYI', this->GetMouseYInvert() );

        //--- setmaxrenderdistance ---
        ps->Put(this, 'SMRD', this->GetMaxRenderDistance());

        //--- setocclusionradius ---
        if ( !ps->Put(this, 'RSOR', this->GetOcclusionRadius()) )
        {
            return false;
        }
        
        //--- setmaxmunoccluders ---
        if ( !ps->Put(this, 'RSMO', this->GetMaxNumOccluders()) )
        {
            return false;
        }

        //--- setareaanglemin ---
        if ( !ps->Put(this, 'RSAA', this->GetAreaAngleMin()) )
        {
            return false;
        }

        return true;
    }
    return false;
}
