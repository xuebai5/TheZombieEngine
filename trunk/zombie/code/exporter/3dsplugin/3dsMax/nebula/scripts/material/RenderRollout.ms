/**
   @file renderRollout.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo
   @brief rollout use for mask render.

   (C) 2006 Conjurer Services, S.A.
*/
rollout zombieRenderRollout "Zombie Material simulate in 3ds MAX" width:280 height:556
(
    global nebMaskRend
    checkbox chk_alphatwoside_true "alpha two side" checked:nebMaskRend.alphatwoside_true pos:[20,465] width:123 height:20
    checkbox chk_deform_billboard "billboard" checked:nebMaskRend.deform_billboard pos:[20,391] width:123 height:20
    checkbox chk_deform_swinging "swinging" checked:nebMaskRend.deform_swinging pos:[20,416] width:123 height:20
    checkbox chk_env_ambientlighting "Ambient Light" checked:nebMaskRend.env_ambientlighting pos:[25,269] width:123 height:20
    checkbox chk_env_diffuselighting "Diffuse Light" checked:nebMaskRend.env_diffuselighting pos:[25,294] width:123 height:20
    checkbox chk_env_fogdisable "Fog disable" pos:[20,371] checked:nebMaskRend.env_fogdisable width:123 height:20
    checkbox chk_env_illuminationcolor "Illumination color" checked:nebMaskRend.env_illuminationcolor pos:[25,322] width:123 height:20
    checkbox chk_env_specularlighting "Specular Light" checked:nebMaskRend.env_specularlighting pos:[25,246] width:123 height:20
    checkbox chk_hasalpha_true "has alpha" checked:nebMaskRend.hasalpha_true pos:[20,440] width:123 height:20
    checkbox chk_map_bumpmap "Bumpmap" checked:nebMaskRend.map_bumpmap pos:[23,34] width:123 height:20
    checkbox chk_map_clipmap "Clipmap" checked:nebMaskRend.map_clipmap pos:[23,59] width:123 height:20
    checkbox chk_map_colormap "ColorMap" checked:nebMaskRend.map_colormap pos:[23,88] width:123 height:20
    checkbox chk_map_controlmap "Blend , Controlmap" checked:nebMaskRend.map_controlmap pos:[23,110] width:123 height:20
    checkbox chk_map_illuminationmap "Illuminationmap" checked:nebMaskRend.map_illuminationmap pos:[23,136] width:123 height:20
    checkbox chk_map_levelmap "Specular level map" checked:nebMaskRend.map_levelmap pos:[23,162] width:123 height:20
    checkbox chk_map_lightmap "Lightmap" checked:nebMaskRend.map_lightmap pos:[23,185] width:123 height:20
    button btn_ok "Ok" pos:[16,513] width:96 height:33
    button btn_cancel "Cancel" pos:[137,515] width:96 height:33
    groupBox grp2 "Maps" pos:[15,18] width:226 height:201
    groupBox grp3 "lights" pos:[17,228] width:223 height:115
    groupBox grp4 "FX" pos:[9,355] width:232 height:142
    on zombieRenderRollout open do
    (
        
        )
    on btn_ok pressed do
    (
        nebMaskRend.alphatwoside_true = chk_alphatwoside_true.state 
        nebMaskRend.deform_billboard = chk_deform_billboard.state 
        nebMaskRend.deform_swinging = chk_deform_swinging.state 
        nebMaskRend.env_ambientlighting = chk_env_ambientlighting.state 
        nebMaskRend.env_diffuselighting = chk_env_diffuselighting.state 
        nebMaskRend.env_fogdisable = chk_env_fogdisable.state 
        nebMaskRend.env_illuminationcolor = chk_env_illuminationcolor.state 
        nebMaskRend.env_specularlighting = chk_env_specularlighting.state 
        nebMaskRend.hasalpha_true = chk_hasalpha_true.state 
        nebMaskRend.map_bumpmap = chk_map_bumpmap.state 
        nebMaskRend.map_clipmap = chk_map_clipmap.state 
        nebMaskRend.map_colormap = chk_map_colormap.state 
        nebMaskRend.map_controlmap = chk_map_controlmap.state 
        nebMaskRend.map_illuminationmap = chk_map_illuminationmap.state 
        nebMaskRend.map_levelmap = chk_map_levelmap.state 
        nebMaskRend.map_lightmap = chk_map_lightmap.state
        iterateMaterial updateAll true
        destroyDialog zombieRenderRollout
        
    )
    on btn_cancel pressed do
    (
     destroyDialog zombieRenderRollout
        )
    on zombieRenderRollout close do (
       format "CERRADO\n"
    )
)

--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------