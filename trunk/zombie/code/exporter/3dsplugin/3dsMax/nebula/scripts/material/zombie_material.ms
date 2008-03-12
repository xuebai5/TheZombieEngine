/**
   @file zombie_material.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo <cristobal.castillo@tragnarion.com>
   @brief Materials
   @remark 
   - nebMaskRend The mask of render properites of zombie material.
   
   - NebMatOp , Nebula material Operations
   -- declareVar()
   -- declarePersistent()
   -- resetValues()
   -- initModule() 
   
   - Declaration of zStdMaterial class.
   
*/

global debugZombie = false
global gcCount = 1
global nebMaskRend
global nebGfxMaterialTypeList

plugin material zStdMaterial
name:"Std Zombie Material" 

classID:#(0x126313e1, 0x5648549f)
extends:Standard
replaceUI:true
version:14
(
fn gc2  =
(
    if not loading do gc()
)


fn specPowLevel k level gloss =
(
    local x = k as float
    x = 90*(x-50.0) / 50.0
    x = cos x
    x = x ^ gloss
    x= level*x
    local x1 = ((x*100) as integer) + 1 
    if (x1 > 100) do (x1=100)
    return x1
)

fn drawSpecular  bmp level gloss= (
    if ((classof bmp)  == bitmap )  do
    (
    
        local x1 = specPowLevel 0 level gloss
        for k=0 to 99 do
        (    
            local pixels = getpixels bmp [0,k] 100
            local x2 = specPowLevel (k+1) level gloss
            
            if (x1<x2 ) then 
            (
                x3 = x1
                x4 = x2
            ) else
            (
                x3 = x2
                x4 = x1
            )
      
            
            for i=1 to x3-1 do
            (
                pixels[i].r =200.0
                pixels[i].g =200.0
                pixels[i].b =200.0
            )
            for i =x3 to x4 do
            (
                pixels[i].r = 255.0
                pixels[i].g = 255.0
                pixels[i].b = 255.0
            )
            
            for i=x4+1 to 100 do
            (
                pixels[i].r =100.0
                pixels[i].g =100.0
                pixels[i].b =100.0
            )
        
            
            setpixels bmp [0,k] pixels
            x1 =x2
        ) -- END for k=0 to 99 do 
    ) --if ((classof bmp)  == bitmap )
  
)


fn setTexture newmap stdmap channelmap mapEnable =
(
    local map = *newmap
    if ((classof map as string) == "Bitmaptexture") then
    (
        map.coordinates.mappingType = 0
        map.coordinates.mapping = 0
        map.coordinates.mapChannel = channelmap
        delegate.maps[stdmap] = map
        delegate.mapAmounts[stdmap] = 100.0
        --delegate.mapEnables[stdmap] = true
        *mapEnable = true
    ) else
    (
        --delegate.mapEnables[stdmap] = false
        *mapEnable = false
        --delegate.maps[stdmap] = undefined
        if ((classof map as string) != "UndefinedClass") do
        (
                *newmap = undefined
        )
    )
)



fn validate_param_map  param  map=
(
    local txt = (classof (*map)) as string
   if (txt != "zTextureMap") do
   (
      *param = false
   )
               
)

fn validate_z_Texture map channelmap = 
(
    local txt = (classof (*map)) as string
   
    case txt  of
    ( 
    "zTextureMap": 
         (
            local newmap = (*map)
            if ( channelmap != newmap.delegate.coordinates.mapChannel ) do
            (
                local newmap =  copy (*map)
                newmap.delegate.coordinates.mappingType = 0
                newmap.delegate.coordinates.mapping = 0
                newmap.delegate.coordinates.mapChannel = channelmap
                *map = newmap
            )
         )
    "Bitmaptexture":
         (
            local tmp = zTextureMap()
            tmp.copyFrom (*map)
            *map = tmp
            local newmap = *map
            newmap.delegate.coordinates.mappingType = 0
            newmap.delegate.coordinates.mapping = 0
            newmap.delegate.coordinates.mapChannel = channelmap
         )
         
    default: 
        *map = undefined
    )
                 
)

fn update_specular force = 
(  
    if ( (force == true) or (not loading)) do
    (
        if (  (this.z_Param_env_specularlighting and nebMaskRend.env_specularlighting  )  == false ) then
            (
                delegate.specularLevel = 0.0
            ) else
           (
                delegate.specularLevel = 100.0*this.z_Sur_Float_matLevel;
                local v = log( this.z_Sur_Float_matShininess )
                v = v / log(2)
                delegate.glossiness = 10*v
           )
    )
)

fn update_specular_map force =
(
    if ( (force == true) or (not loading)) do
    (
       update_specular force
       if (  (this.z_Param_env_specularlighting and nebMaskRend.env_specularlighting  )  == true ) then
       (
            local map_spc  = RGB_Multiply ()
            map_spc.map1Enabled = false
            map_spc.color1 = this.z_Sur_Vector_matSpecular

            if ( (this.z_Param_map_levelmap and nebMaskRend.map_levelmap  )   == false ) then
            (
                map_spc.map2Enabled = false
                map_spc.color2  = color 255 255 255 255
            ) else
            (
                map_spc.map2 = this.z_Sur_Texture_levelMap
                map_spc.map2Enabled = true      
            )
               delegate.maps[3] = map_spc
               delegate.mapAmounts[3] = 100.0
               delegate.mapenables[3] = true
            gc2()
       )
    )
)

fn update_clip_map force =
(
    if ( (force == true) or (not loading)) do
    (
        if ( this.z_Param_map_clipmap and nebMaskRend.map_clipmap  and
             this.z_Param_map_colormap and nebMaskRend.map_colormap
            ) then
        (
            local map = this.z_Sur_Texture_diffMap
            map.delegate.monoOutput = 1 -- the aplha
            delegate.maps[7] = map
            delegate.mapEnables[7] = true
            delegate.mapAmounts[7] = 100.0
        ) else
        (    
             if ( ( (this.z_Param_map_colormap and nebMaskRend.map_colormap  )  == true )  and 
                      ( (this.z_Param_hasalpha_true and nebMaskRend.hasalpha_true  )  == true ) )then
             (    
                --map = copy this.z_Sur_Texture_diffMap
                local map = this.z_Sur_Texture_diffMap
                map.delegate.monoOutput = 1 -- the aplha
                delegate.maps[7] = map
                delegate.mapEnables[7] = true
                delegate.mapAmounts[7] = 100.0
                --map.bitmap2 = this.z_Sur_Texture_diffMap.bitmap2
                        
              ) else
              (
                   delegate.mapEnables[7] = false
                   delegate.maps[7] = undefined
              )        
         )
         
         gc2()
    )
)

fn update_bumpMap force =
(
    if ( (force == true) or (not loading)) do
    (
        if ( (this.z_Param_map_bumpmap and nebMaskRend.map_bumpmap  )  == true ) then
        (
            if ( not this.bumpMapIsNormalMap) then
            (
                delegate.maps[9] = this.z_Sur_Texture_bumpMap
                delegate.mapEnables[9] = true
                delegate.mapAmounts[9] = 100
             ) else
             (
                local tmp = Normal_Bump ()
                tmp.normal_map = this.z_Sur_Texture_bumpMap
                tmp.flipred = true -- change the x orientation 
                delegate.maps[9] = tmp
                delegate.mapEnables[9] = true
                delegate.mapAmounts[9] = 100
             )
        ) else if (this.z_Param_map_normalmap and nebMaskRend.map_normalmap ) then
        (
            local tmp = Normal_Bump ()
            tmp.normal_map = this.z_Sha_Texture_BumpMap3
            tmp.flipred = true -- change the x orientation 
            delegate.maps[9] = tmp
            delegate.mapEnables[9] = true
            delegate.mapAmounts[9] = 100
            
        ) else 
        (
            delegate.mapEnables[9] = false
            delegate.maps[9] = undefined
        )
    )

)

fn update_illumination force =
(
    if ( (force == true) or (not loading)) do
    ( 
        local texture = RGB_Add()
        -- (this.z_Param_map_clipmap and nebMaskRend.map_clipmap  ) 
        delegate.maps[6] = texture
        delegate.mapAmounts[6] = 100.0
        delegate.mapEnables[6] = false
        print"delegate.mapEnables[6]"
        print delegate.mapEnables[6]
        
        --print ((classof texture) as string)
        
        
        if (  (this.z_Param_map_illuminationmap and nebMaskRend.map_illuminationmap  )  == true ) then
        (   
            texture.map1 = this.z_Sur_Texture_illuminationMap
            texture.map1Enabled = true
            delegate.mapEnables[6] = true
            texture.alphaFrom = 0
        ) else
        (
            texture.color1 = color 0 0 0 255
            texture.map1Enabled = false
        )
        
        if (  (this.z_Param_map_lightmap and nebMaskRend.map_lightmap  )  == true ) then
        (   
            texture.map2 = this.z_Sha_Texture_lightMap
            texture.map2Enabled = true
            delegate.mapEnables[6] = true
            texture.alphaFrom = 1
        ) else
        ( 
            texture.color2 = color 0 0 0 255
            texture.map2Enabled = false
        )
        
        texture.alphaFrom = 0   
        gc2()
    )
)





fn update_diffuse_ambient force =
(

    if ( (force == true) or (not loading)) do
    (
        --if debugZombie do ( format "\n%: update diffuse or ambient yes" this.name )
        
        local map_amb  = RGB_Multiply ()
        local map_diff = RGB_Multiply ()       
        delegate.maps[1] = map_amb
        delegate.mapAmounts[1] = 100.0
        delegate.mapenables[1] = true
        
        delegate.maps[2] = map_diff
        delegate.mapAmounts[2] = 100.0
        delegate.mapenables[2] = true
        
        map_amb.map1Enabled = false
        
        if (  (this.z_Param_env_ambientlighting and nebMaskRend.env_ambientlighting  )   == false ) then
        (
            map_amb.color1 = color 0 0 0 255
        ) else
        (
            map_amb.color1 = this.z_Sur_Vector_matAmbient
        )
        
        if (  (this.z_Param_env_diffuselighting and nebMaskRend.env_diffuselighting  )   == false ) then
        (
            map_diff.color1 = color 0 0 0 255
        ) else
        (
            map_diff.color1 = this.z_Sur_Vector_matDiffuse
        )
        
        
        if ( (this.z_Param_map_colormap and nebMaskRend.map_colormap  )  == false) then
        (
            map_diff.color2 = color 255 255 255 255
            map_amb.color2 = color 255 255 255 255
            map_amb.map2Enabled = false
            map_diff.map2Enabled = false    
        ) else
        (
           
            if ( (this.z_Param_map_controlmap and nebMaskRend.map_controlmap  )  == false) then
            (
                  map_amb.map2  =  this.z_Sur_Texture_diffMap
                  map_diff.map2 =  this.z_Sur_Texture_diffMap
                  
            ) else
            (
                local mixed = Mix()
                mixed.map2 = this.z_Sur_Texture_diffMap
                mixed.map1 = this.z_Sur_Texture_diffMap2
                mixed.mask = this.z_Sur_Texture_controlMap
                mixed.map1Enabled = true
                mixed.map2Enabled = true
                mixed.maskEnabled = true
                
                map_amb.map2  =  mixed
                map_diff.map2 =  mixed                   
            )
                
                map_diff.map2Enabled = true
                map_amb.map2Enabled = true           
        )
        gc2()
    )
)


fn put_all_options value = 
(
    --hasalpha_true
    this.params_prop.chk_alpha.enabled = value
    this.params_prop.rdo2.enabled = value
    --alphatwoside_true
    this.params_prop.chk_alpha_two.enabled = value
    --env_vcolormatambient
    this.params_prop.chk_vcolormatambient.enabled = value
    --env_vcolorblend
    this.params_prop.chk_vcolorblend.enabled = value


    --params_deform
    --deform_billboard
    this.params_deform.chk_billboard.enabled = value
    --deform_swinging
    this.params_deform.chk_swing.enabled = value
    --pro_notculling
    this.params_deform.chk_notculling.enabled = value
    --pro_depthbias
    this.params_deform.chk_prodepthbias.enabled = value
    
    --params_map
    --map_colormap
    this.params_map.chk_colormap.enabled = value
    this.params_map.btn_diffMap.enabled = value
    --map_clipmap
    this.params_map.chk_clipmap.enabled = value
    this.params_map.btn_clipMap.enabled = false -- obsolete map
    --map_bumpmap
    this.params_map.chk_bumpmap.enabled = value
    this.params_map.btn_bumpMap.enabled = value
    --map_lightmap
    this.params_map.chk_lightmap.enabled = value
    this.params_map.btn_lightMap.enabled = value
    --map_levelmap
    this.params_map.chk_levelmap.enabled = value
    this.params_map.btn_levelMap.enabled = value
    --map_controlmap
    this.params_map.chk_controlmap.enabled = value
    this.params_map.btn_diffMap2.enabled = value
    this.params_map.btn_controlMap.enabled = value
    --map_illuminationmap
    this.params_map.chk_illuminationmap.enabled = value
    this.params_map.btn_illuminationMap.enabled = value
    --map_normalmap
    this.params_map.chk_normalmap.enabled = value
    this.params_map.btn_normalMap.enabled = value
    --map_environmentmap2d
    this.params_map.chk_environmentmap2d.enabled = value
    this.params_map.btn_AmbientMap0.enabled = value
    this.params_map.btn_envMaskMap.enabled = value
    this.params_map.spn_envMaskFactor.enabled = value
    --map_environmentmap3d
    this.params_map.chk_environmentmap3d.enabled = value
    this.params_map.btn_CubeMap0.enabled = value
    this.params_map.btn_envMaskMap.enabled = value
    this.params_map.spn_envMaskFactor.enabled = value
    --map_parallax
    this.params_map.chk_parallaxmap.enabled = value
    this.params_map.btn_parallaxMap.enabled = value
    this.params_map.spn_parallaxFactor.enabled = value
    this.params_map.spn_parallaxFactorRatioUV.enabled = value
    -- map_levelmapasenvmaskmap
    

    --params_env
    --env_ambientlighting
    this.params_env.chk_ambientlighting.enabled = value
    this.params_env.cp_matAmbient.enabled = value
    --env_diffuselighting
    this.params_env.chk_diffuselighting.enabled = value
    this.params_env.cp_matDiffuse.enabled = value
    --env_fogdisable
    this.params_env.chk_fogdisable.enabled = value
    --env_specularlighting
    this.params_env.chk_specularlighting.enabled = value
    this.params_env.cp_matSpecular.enabled = value
    this.params_env.spn_shininess.enabled = value
    this.params_env.spn_level.enabled = value
    --env_illuminationcolor
    this.params_env.chk_autoIllum.enabled = value
    this.params_env.cp_autoIllum.enabled = value
    
    --params_ocean
    --deform_wave
    this.params_ocean.chk_wave.enabled = value
    this.params_ocean.spn_amplitude.enabled = value
    this.params_ocean.spn_frequency.enabled = value
    this.params_ocean.cp_velocity.enabled = value
    this.params_ocean.cp_texgens.enabled = value
    this.params_ocean.chk_fresnel.enabled = value
    this.params_ocean.spn_fresnelbias.enabled = value
    this.params_ocean.spn_fresnelpower.enabled = value
    
)


--------------------------------------------------------------------------------
/**
    Enable option , depend of material
*/
fn EnableOptions libName = 
(
    
    if  (this.shaderType == "invalid") or  (this.shaderType == "custom") then
    (
        put_all_options true
        return undefined
    )
    else
    (
        put_all_options false
    ) 
    
    local properties = nebMatOp.getGfxMaterialProperties libName
    
    for param in properties do
    (
        if (param[1] == "hasalpha") and (param[2] =="true") do
        (
            this.params_prop.chk_alpha.enabled = true
            this.params_prop.rdo2.enabled = true
        )
            
        if (param[1] == "alphatwoside") and (param[2] =="true") do
        (
            this.params_prop.chk_alpha_two.enabled = true
        )
        
        if (param[1] == "env") and (param[2] =="vcolormatambient") do
        (
            this.params_prop.chk_vcolormatambient.enabled = true
        )
        
        if (param[1] == "env") and (param[2] =="vcolorblend") do
        (
            this.params_prop.chk_vcolorblend.enabled = true
        )
        

        --params_deform
        if (param[1] == "deform") and (param[2] =="billboard") do
        (
            this.params_deform.chk_billboard.enabled = true
        )

        if (param[1] == "deform") and (param[2] =="swinging") do
        (
            this.params_deform.chk_swing.enabled = true
        )
        
        if (param[1] == "pro") and (param[2] =="notculling") do
        (
            this.params_deform.chk_notculling.enabled = true
        )

        if (param[1] == "pro") and (param[2] =="depthbias") do
        (
            this.params_deform.chk_prodepthbias.enabled = true
        )
            

        if (param[1] == "map") and (param[2] =="colormap") do
        (
            this.params_map.chk_colormap.enabled = true
            this.params_map.btn_diffMap.enabled = true
        )


        if (param[1] == "map") and (param[2] =="clipmap") do
        (
            this.params_map.chk_clipmap.enabled = true
            this.params_map.btn_clipMap.enabled = false -- theis button always disabled
        )
            
            
        if (param[1] == "map") and (param[2] =="bumpmap") do --map_bumpmap
        (
            this.params_map.chk_bumpmap.enabled = true
            this.params_map.btn_bumpMap.enabled = true
        )
        
        if (param[1] == "map") and (param[2] =="lightmap") do --map_lightmap
        (
            this.params_map.chk_lightmap.enabled = true
            this.params_map.btn_lightMap.enabled = true
        )
        
        if (param[1] == "map") and (param[2] =="levelmap") do --map_levelmap
        (
            this.params_map.chk_levelmap.enabled = true
            this.params_map.btn_levelMap.enabled = true
        )
        
        if (param[1] == "map") and (param[2] =="controlmap") do --map_controlmap
        (
            this.params_map.chk_controlmap.enabled = true
            this.params_map.btn_diffMap2.enabled = true
            this.params_map.btn_controlMap.enabled = true
        )
        if (param[1] == "map") and (param[2] =="illuminationmap") do --map_illuminationmap
        (
            this.params_map.chk_illuminationmap.enabled = true
            this.params_map.btn_illuminationMap.enabled = true
        )
        if (param[1] == "map") and (param[2] =="normalmap") do --map_normalmap
        (
            this.params_map.chk_normalmap.enabled = true
            this.params_map.btn_normalMap.enabled = true
        )
        
        if (param[1] == "map") and (param[2] =="environmentmap2d") do --map_environmentmap2d
        (
            this.params_map.chk_environmentmap2d.enabled = true
            this.params_map.btn_AmbientMap0.enabled = true
            this.params_map.btn_envMaskMap.enabled = true
            this.params_map.spn_envMaskFactor.enabled = true
        )
        
        if (param[1] == "map") and (param[2] =="environmentmap3d") do --map_environmentmap3d 
        (
            this.params_map.chk_environmentmap3d.enabled = true
            this.params_map.btn_CubeMap0.enabled = true
            this.params_map.btn_envMaskMap.enabled = true
            this.params_map.spn_envMaskFactor.enabled = true
        )
        
        if (param[1] == "map") and (param[2] =="parallax") do --map_parallax
        (
            this.params_map.chk_parallaxmap.enabled = true
            this.params_map.btn_parallaxMap.enabled = true
            this.params_map.spn_parallaxFactor.enabled = true
            this.params_map.spn_parallaxFactorRatioUV.enabled = true
        )
        
        
        if (param[1] == "map") and (param[2] =="levelmapasenvmaskmap") do 
        (
            -- Empty 
        )
            
        
        --params_env
        if (param[1] == "env") and (param[2] =="ambientlighting") do --env_ambientlighting
        (
            this.params_env.chk_ambientlighting.enabled = true
            this.params_env.cp_matAmbient.enabled = true
        )
        
        if (param[1] == "env") and (param[2] =="diffuselighting") do --env_diffuselighting
        (
            this.params_env.chk_diffuselighting.enabled = true
            this.params_env.cp_matDiffuse.enabled = true
        )
        
        if (param[1] == "env") and (param[2] =="fogdisable") do --env_fogdisable
        (
            this.params_env.chk_fogdisable.enabled = true
        )
        
        if (param[1] == "env") and (param[2] =="specularlighting") do --env_specularlighting
        (
            this.params_env.chk_specularlighting.enabled = true
            this.params_env.cp_matSpecular.enabled = true
            this.params_env.spn_shininess.enabled = true
            this.params_env.spn_level.enabled = true
        )
        
        if (param[1] == "env") and (param[2] =="illuminationcolor") do --env_illuminationcolor
        (
            this.params_env.chk_autoIllum.enabled = true
            this.params_env.cp_autoIllum.enabled = true
        )
        
        
            
       --params_ocean
       if (param[1] == "deform") and (param[2] =="wave") do --deform_wave
       (
            this.params_ocean.chk_wave.enabled = true
            this.params_ocean.spn_amplitude.enabled = true
            this.params_ocean.spn_frequency.enabled = true
            this.params_ocean.cp_velocity.enabled = true
            this.params_ocean.cp_texgens.enabled = true
       )
            
       --pro_fresnel
       if (param[1] == "pro") and (param[2] =="fresnel") do --pro_fresnel
       (
            this.params_ocean.chk_fresnel.enabled = true
            this.params_ocean.spn_fresnelbias.enabled = true
            this.params_ocean.spn_fresnelpower.enabled = true
       )
    )
    
)

parameters nMaterial_prop  rollout:params_prop
(
    z_Pass_alpha   type:#boolean  default:false  -- backward compatibility
    z_Param_hasalpha_true   type:#boolean  default:false  ui:chk_alpha
    z_Param_alphatwoside_true   type:#boolean  default:false  ui:chk_alpha_two
    z_Sur_int_funcDestBlend type:#integer default:6 ui:rdo2
    
    z_Param_env_vcolormatambient  type:#boolean  default:false  ui:chk_vcolormatambient
    z_Param_env_vcolorblend       type:#boolean  default:false  ui:chk_vcolorblend
    
    on z_Param_hasalpha_true set val do
    (
        update_clip_map false
    )
    
    on z_Param_env_vcolormatambient set val do
    (
        if not loading do
        (
            if (val == true) do
            (
                z_Param_env_vcolorblend = false
            )
        )
    )
    
    on z_Param_env_vcolorblend  set val do
    (
        if not loading do 
        (
            if ( val == true ) do
            (
                z_Param_env_vcolormatambient = false
            )
        )
    )
    
)

rollout params_prop "Properties" width:330 height:140
(
    group "Pass"
    (
        checkbox chk_alpha "Alpha" pos:[15,20] width:100 height:20
        checkbox chk_alpha_two "Alpha two Side" pos:[120,20] width:100 height:20
    )
     
    group "Alpha Pass"
    (
        radioButtons rdo2 "blend dest func" pos:[15,70] width:100 height:50 labels:#("ZERO", "ONE", "SRCCOLOR", "INVSRCCOLOR", "SRCALPHA", "INVSRCALPHA") columns:3
    )       
    
    group "Use vertex color as"
    (
        checkbox chk_vcolormatambient "material color" pos:[15,150] width:100 height:20
        checkbox chk_vcolorblend "control blend" pos:[15,170] width:100 height:20
    )
    
    on params_prop open do
    (
        EnableOptions this.shaderType 
    )
)

parameters nMaterial_deform rollout:params_deform
(
    z_Param_deform_billboard        type:#boolean ui:chk_billboard     default:false
    z_Param_deform_swinging         type:#boolean ui:chk_swing  default:false
    z_Param_pro_notculling type:#boolean default:false ui:chk_notculling
    z_Param_pro_depthbias type:#boolean default:false ui:chk_prodepthbias
    shadowEmitter type:#boolean     ui:chk_shadowEmitter default:false
    
    on z_Param_pro_notculling set val do
    (
        if not loading do ( delegate.twoSided == z_Param_pro_notculling ) 
    )
)


parameters nMaterial_map rollout:params_map
(
    --DONT'CHANGE THE ORDER OF PARAMETER, NEW PARAMETERS APEEND TO ENd
    
    z_Param_map_colormap         type:#boolean     ui:chk_colormap
    z_Sur_Texture_diffMap        type:#texturemap  ui:btn_diffMap
    
    z_Param_map_clipmap          type:#boolean     ui:chk_clipmap
    z_Sur_Texture_clipMap        type:#texturemap  ui:btn_clipMap
        
    z_Param_map_bumpmap          type:#boolean     ui:chk_bumpmap
    z_Sur_Texture_bumpMap        type:#texturemap  ui:btn_bumpMap
        
    z_Param_map_lightmap         type:#boolean     ui:chk_lightmap
    z_Sha_Texture_lightMap       type:#texturemap  ui:btn_lightMap
    
    z_Param_map_levelmap         type:#boolean     ui:chk_levelmap
    z_Sur_Texture_levelMap       type:#texturemap  ui:btn_levelMap

    
    z_Param_map_controlmap       type:#boolean     ui:chk_controlmap
    z_Sur_Texture_diffMap2       type:#texturemap  ui:btn_diffMap2
    z_Sur_Texture_controlMap     type:#texturemap  ui:btn_controlMap
    
    
    z_Param_map_illuminationmap   type:#boolean     ui:chk_illuminationmap
    z_Sur_Texture_illuminationMap type:#texturemap  ui:btn_illuminationMap

    z_Sur_Float_mipMapLodBias type:#float ui:spn_mipmapbias default:0.0
    bumpMapFactor  type:#integer   default:100 -- obsolete, put for backwards compability, never delete this component, not use ui
    
    
    z_Param_map_normalmap          type:#boolean     ui:chk_normalmap
    z_Sha_Texture_BumpMap3         type:#texturemap  ui:btn_normalMap
    
    z_Param_map_environmentmap2d   type:#boolean     ui:chk_environmentmap2d
    z_Sha_Texture_AmbientMap0      type:#texturemap  ui:btn_AmbientMap0
    
    z_Param_map_environmentmap3d   type:#boolean     ui:chk_environmentmap3d
    z_Sha_Texture_CubeMap0      type:#texturemap  ui:btn_CubeMap0
    
    z_Sur_Texture_envMaskMap      type:#texturemap  ui:btn_envMaskMap
    
    z_Param_map_parallax          type:#boolean     ui:chk_parallaxmap
    z_Sur_Texture_parallaxMap     type:#texturemap  ui:btn_parallaxMap
    z_Sur_Float_parallaxFactor    type:#float       ui:spn_parallaxFactor default:0.02
    
    bumpMapIsNormalMap            type:#boolean     ui:chk_bumpMapIsNormalMap default:false
    
    z_Sur_Float_envMaskFactor          type:#float       ui:spn_envMaskFactor default:1.0
    z_Param_map_levelmapasenvmaskmap   type:#boolean     ui:chk_levelmapasenvmaskmap
    
    z_Sur_Float_parallaxFactorRatioUV  type:#float       ui:spn_parallaxFactorRatioUV default:1.0
    

    on z_Param_map_controlmap set val do
    (
        if ((val == true) and (not loading)) do
        (
            validate_param_map &z_Param_map_controlmap  &z_Sur_Texture_controlMap
            validate_param_map &z_Param_map_controlmap  &z_Sur_Texture_diffMap2
            validate_param_map &z_Param_map_controlmap  &z_Sur_Texture_diffMap
        )
        update_diffuse_ambient false
    )
    
    
    on z_Sur_Texture_controlMap set val do
    (
        if not loading do
        (
            validate_z_Texture &z_Sur_Texture_controlMap 3
            z_Param_map_controlmap = true
        )
    )
    
    on z_Sur_Texture_diffMap2 set val do
    (
        if not loading do
        (
            validate_z_Texture &z_Sur_Texture_diffMap2 1
            z_Param_map_controlmap = true
        )
    )
    
    
    
    on z_Param_map_colormap set val do
    (
        if ((val == true) and (not loading) ) do
        (
            validate_param_map &z_Param_map_colormap  &z_Sur_Texture_diffMap
        )
        update_diffuse_ambient false
        update_clip_map false
    )
    
    on z_Sur_Texture_diffMap set val do
    (
         if not loading do
         (
            validate_z_Texture &z_Sur_Texture_diffMap 1
            z_Param_map_colormap = true -- if texture is not valid automatly it's change to false
            z_Param_map_controlmap = true
            z_Param_map_clipmap = z_Param_map_clipmap -- force check 
         )
    )
    
    on z_Sur_Texture_diffMap changed val do
    (
        if not loading do
        (
            z_Param_map_colormap = true
        )
    )
    
    on z_Param_map_clipmap set val do
    (
       if (val == true and (not loading)) do
       (
           z_Param_map_clipmap = z_Param_map_clipmap and  z_Param_map_colormap
       )
       update_clip_map false
    )

    on z_Param_map_bumpmap set val do
    (
        if not loading do
        (
            if (val == true ) do
            (
                validate_param_map &z_Param_map_bumpmap  &z_Sur_Texture_bumpMap
            )
            if z_Param_map_bumpmap do z_Param_map_normalmap = false
       )
       update_bumpMap false;
    )
    
    on z_Param_map_normalmap set val do
    (
       if not loading  do
       (
           if (val == true) do
           (
                validate_param_map &z_Param_map_normalmap  &z_Sha_Texture_BumpMap3
           )
       
           if z_Param_map_normalmap do 
           (
                z_Param_map_bumpmap = false
                z_Param_map_parallax = false
           )
           update_bumpMap false;   
       )
    )
    
    on z_Param_map_parallax set val do
    (
       if not loading do
       (
           if (val == true) do
           (
                 validate_param_map &z_Param_map_parallax  &z_Sur_Texture_parallaxMap
           )
           if z_Param_map_parallax do z_Param_map_normalmap = false 
           update_bumpMap false;   
       )
    )
    
    on z_Param_map_levelmapasenvmaskmap set val do
    (
        if not loading do
        (
            validate_param_map &z_Param_map_levelmapasenvmaskmap &z_Sur_Texture_levelMap
        )
    )
    
    on z_Param_map_environmentmap2d set val do
    (
       if not loading do
       (
       if (val == true) do
       (
            validate_param_map &z_Param_map_environmentmap2d  &z_Sha_Texture_AmbientMap0
            if not z_Param_map_levelmapasenvmaskmap do
            (
                validate_param_map &z_Param_map_environmentmap2d  &z_Sur_Texture_envMaskMap
            )
       )
       if z_Param_map_environmentmap2d do z_Param_map_environmentmap3d = false
       )
    )
    
    on z_Param_map_environmentmap3d set val do
    (
       if not loading do
       (
       if (val == true) do
       (
            validate_param_map &z_Param_map_environmentmap3d  &z_Sha_Texture_CubeMap0
            if not z_Param_map_levelmapasenvmaskmap do
            (
               validate_param_map &z_Param_map_environmentmap3d  &z_Sur_Texture_envMaskMap
            )
       )
       if z_Param_map_environmentmap3d do z_Param_map_environmentmap2d = false
       )
    )
       
    
    on z_Sur_Texture_bumpMap set val do
    (
        if not loading do
        (
            validate_z_Texture &z_Sur_Texture_bumpMap 1
            z_Param_map_bumpmap = true
        )
    )
    
    on z_Sha_Texture_BumpMap3 set val do
    (
        if not loading do
        (
            validate_z_Texture &z_Sha_Texture_BumpMap3 4
            z_Param_map_normalmap = true
        )
    )
    
    on z_Sur_Texture_parallaxMap set val do
    (
        if not loading do
        (
            validate_z_Texture &z_Sur_Texture_parallaxMap 1
            z_Param_map_parallax = true
        )
    )
    
    on  z_Sha_Texture_AmbientMap0 set val do
    (
        if not loading do
        (
            validate_z_Texture &z_Sha_Texture_AmbientMap0 2
            z_Param_map_environmentmap2d = true
        )
    )
    
    on  z_Sha_Texture_CubeMap0 set val do
    (
        if not loading do
        (
            validate_z_Texture &z_Sha_Texture_CubeMap0 2
            z_Param_map_environmentmap3d = true
        )
    )
    
    on  z_Sur_Texture_envMaskMap set val do
    (
        if not loading do
        (
            validate_z_Texture &z_Sur_Texture_envMaskMap 1
            if not z_Param_map_environmentmap2d do z_Param_map_environmentmap3d = true
            if not z_Param_map_environmentmap3d do z_Param_map_environmentmap2d = true
            z_Param_map_environmentmask0 = true
        )
    )
   

    on z_Param_map_lightmap set val do
    (
       if not loading do
       (
           if (val == true) do
           (
                validate_param_map &z_Param_map_lightmap  &z_Sha_Texture_lightMap
           )
           update_illumination false
       )
    )
    
    
    on z_Sha_Texture_lightMap set val do
    (
        if not loading do 
        (
           validate_z_Texture &z_Sha_Texture_lightMap 2
           z_Param_map_lightmap = true
        )
    )

    on z_Param_map_levelmap set val do
    (
       if not loading do
       (
           if (val == true) do
           (
               validate_param_map &z_Param_map_levelmap  &z_Sur_Texture_levelMap
           )
           update_specular_map false
       )
    )
    
    
    on z_Sur_Texture_levelMap set val do
    (
        if not loading do
        (
            validate_z_Texture &z_Sur_Texture_levelMap 1
            z_Param_map_levelmap = true
            z_Param_map_levelmapasenvmaskmap = z_Param_map_levelmapasenvmaskmap -- trick for update this
        )
    )

    on z_Param_map_illuminationmap set val do
    (
       if not loading do
       (
          if (val == true) do
          (
               validate_param_map &z_Param_map_illuminationmap  &z_Sur_Texture_illuminationMap
          )
          update_illumination false
       )
    )
    
    
    on z_Sur_Texture_illuminationMap set val do
    (
        if not loading do
        (
            validate_z_Texture &z_Sur_Texture_illuminationMap 1
            z_Param_map_illuminationmap = true
        )
    )
        
)


parameters nMaterial_Env rollout:params_env
(
    z_Param_env_ambientlighting  type:#boolean   ui:chk_ambientlighting   default:true
    z_Param_env_diffuselighting  type:#boolean   ui:chk_diffuselighting   default:true
    z_Param_env_fogdisable       type:#boolean   ui:chk_fogdisable        default:false
    z_Param_env_specularlighting type:#boolean   ui:chk_specularlighting  defalut:false
    z_Param_env_illuminationcolor type:#boolean ui:chk_autoIllum default:false
        
        
    z_Sur_Vector_matAmbient       type:#frgba ui:cp_matAmbient  default:(color 255 255 255 255)
    z_Sur_Vector_matDiffuse       type:#frgba ui:cp_matDiffuse  default:(color 255 255 255 255)
    z_Sur_Vector_matSpecular      type:#frgba ui:cp_matSpecular default:(color 255 255 255 255)
    z_Sur_Vector_illuminationColor type:#frgba ui:cp_autoIllum  default:(color 64 64  64 255)

    z_Sur_Float_matShininess         type:#float ui:spn_shininess 
    z_Sur_Float_matLevel             type:#float ui:spn_level
        
    on z_Param_env_ambientlighting set val do
    (
        update_diffuse_ambient false
    )
    
    on z_Param_env_diffuselighting set val do
    (
        update_diffuse_ambient false
    )
    
    on z_Sur_Vector_matAmbient set val do
    (
        update_diffuse_ambient false
    )
    
    on z_Sur_Vector_matDiffuse set val do
    (
        update_diffuse_ambient false
    )
    
    on  z_Param_env_specularlighting set val do 
    (
        update_specular false
    ) 
    
    on  z_Sur_Vector_matSpecular  set val do
    (
        update_specular_map false
    )
    
    on z_Sur_Float_matShininess set val do
    (
        update_specular false
    )
    
    on z_Sur_Float_matLevel set val do
    (
        update_specular false
    )
        

        
)



rollout params_env "Zombie Enviroment" width:330 height:240
(
        
    checkbox chk_autoIllum  "Auto Illumination" pos:[11,10] width:100 height:14 
    colorpicker cp_autoIllum  "" pos:[126,10] width:107 height:14
        
    checkbox chk_ambientlighting "Ambient lighting" pos:[11,30] width:100 height:14
    colorPicker cp_matAmbient "" pos:[126,30] width:107 height:14
    label lbl1 "X  Diffuse Map" pos:[240,30] width:78 height:15
                
    checkbox chk_diffuselighting "Diffuse lighting" pos:[11,50] width:88 height:14
    colorPicker cp_matDiffuse "" pos:[126,50] width:107 height:14 color:(color 0 0 155)
    label lbl2 "X  Diffuse Map" pos:[240,50] width:78 height:15


    checkbox chk_specularlighting "Specular lighting" pos:[11,70] width:100 height:14
    colorPicker cp_matSpecular "" pos:[126,70] width:60 height:14 color:(color 0 0 155)

    
    bitmap bmpSpec2 "Bitmap" pos:[200,70] width:100 height:100 bitmap:(bitmap 100 100 color:white)
            --bitmap bmpSpec "Bitmap" pos:[159,218] width:100 height:100 bitmap:(bitmap 100 100 color:white)

    spinner spn_shininess "Shininess" pos:[115,100] width:70 height:16 range:[1,255,0] type:#float scale:0.1
    spinner spn_level "Level      " pos:[115,130] width:70 height:16  range:[0,2,0] type:#float scale:0.01
    
    checkbox chk1 "Checkbox" pos:[11,74] width:0 height:0
    checkbox chk_fogdisable "Fog disable" pos:[11,150] width:81 height:14
        
    on params_env open do
    (
        local bmp2 = bmpSpec2.bitmap
        drawSpecular bmp2 spn_level.value spn_shininess.value
        bmpSpec2.bitmap = bmp2
    --                      print ("\nLEIDO")
        gc2 () --call garbage collector
        EnableOptions this.shaderType
    )
    
    on spn_level changed val do
    (
        local bmp2 = bmpSpec2.bitmap
        drawSpecular bmp2 spn_level.value spn_shininess.value
        bmpSpec2.bitmap = bmp2          
        gc2 ()
    )
    
    on spn_shininess changed val do
    (
        local bmp2 = bmpSpec2.bitmap
        drawSpecular bmp2 spn_level.value spn_shininess.value
        bmpSpec2.bitmap = bmp2
        gc2 ()
    )
)

rollout params2 "Zombie Parameters2" width:329 height:102
(
) 


parameters nMaterial_ocean rollout:params_ocean
(
    z_Param_deform_wave type:#boolean ui:chk_wave default:false
    z_Param_pro_fresnel  type:#boolean ui:chk_fresnel     default:false
    
    z_Sur_Float_Amplitude  type:#float  ui:spn_amplitude default:0.5
    z_Sur_Float_Frequency  type:#float  ui:spn_frequency default:0.1 
    z_Sur_Vector_Velocity  type:#point4 ui:cp_velocity   default:(color (255*0.01) (255*0.01) 0 0 )
    z_Sur_Vector_TexGenS   type:#point4 ui:cp_texgens   default:(color 0 0 0 0)
 
    z_Sur_Float_FresnelBias  type:#float ui:spn_fresnelbias default:0.0
    z_Sur_Float_FresnelPower type:#float ui:spn_fresnelpower default:0.0
 
)

parameters nMaterial_type rollout:params_type
(
    shaderType type:#string  default:"invalid"    
    on shaderType set val do
    (
               
        if (MatEditor.isOpen()) and ( medit.GetCurMtl() == this ) do
	    (
	        -- update the rollout
            this.params_type.ddl_type.selection =  findItem nebGfxMaterialTypeList shaderType
            this.params_type.edt_script.text = nebMatOp.getGfxMaterialScriptInText this.shaderType
            EnableOptions this.shaderType
	    )
    )

)

rollout params_ocean "Ocean Deform" width:260 height:81
(

    group "Wave"
    (
        checkbox chk_wave "Wave"  width:80 height:19
        spinner spn_amplitude "Amplitude"  pos:[10,50] width:90 height:16  type:#float  range:[0.01,10.0,0] scale:0.01 align:#left
        spinner spn_frequency "Frequency"  pos:[10,80] width:90 height:16  type:#float range:[0.01,10.0,0] scale:0.01 align:#left
        colorpicker cp_velocity "Velocity" pos:[170,50] width:107 height:14  --range 0.001 0.1
        colorpicker cp_texgens "TexGenS"  pos:[170,80] width:107 height:14  --range 0.1 100
    )
    
    group "Fresnel"
    (
        checkbox chk_fresnel "Fresnel" pos:[10,120]width:80 height:19
        spinner spn_fresnelbias "Fresnel bias"  pos:[10,150] width:90 height:16 range:[-10,10,0] type:#float scale:0.01 align:#left
        spinner spn_fresnelpower "Fresnel Power" pos:[170,150] width:90 height:16 range:[-10,10,0] type:#float scale:0.01 align:#left
    )
    on params_ocean open do
    (
        EnableOptions this.shaderType 
    ) 
)

rollout paramsTools "Tools" width:329 height:102
(
    button btn "Fix Material" pos:[82,10] width:160 height:20
    button btn2 "Update all Backed" pos:[82,40] width:160 height:20
    on btn pressed do
    (
       this.fix()
    )
    on btn2 pressed do
    (
       NebMatOp.updateBacked this
    )
) 
 
rollout params_map "Zombie Maps" width:329 height:280
(
    checkbox chk_colormap "ColorMap . . . . . . ." pos:[15,20] width:96 height:20 enabled:true checked:false
    mapButton btn_diffMap  pos:[120,20] width:175 height:20
    label lbl_colorMap    "UV1" pos:[300,20] width:96 height:20

    

    checkbox chk_levelmap "Spec Map  . . . . . ." pos:[15,40] width:96 height:20 enabled:true checked:false
    mapButton btn_levelMap "LevelMap" pos:[120,40] width:175 height:20
    label lbl_levelMap    "UV1" pos:[300,40] width:96 height:20
    

    checkbox chk_lightmap "LightMap . . . . . . ." pos:[15,60] width:96 height:20 enabled:true checked:false
    mapButton btn_lightMap "LightMap" pos:[120,60] width:175 height:20
    label lbl_lightMap  "UV2" pos:[300,60] width:96 height:20   

    checkbox chk_clipmap "ClipMap  . . . . . . ." pos:[15,80] width:96 height:20 enabled:true checked:false
    mapButton btn_clipMap "ClipMap" pos:[120,80] width:175 height:20
    label lbl_clipMap    "UV1" pos:[300,80] width:96 height:20

    checkbox chk_illuminationmap "illuminationMap  . . ." pos:[15,100] width:96 height:20 enabled:true checked:false
    mapButton btn_illuminationMap "illuminationMap" pos:[120,100] width:175 height:20
    label lbl_illuminationMap    "UV1" pos:[300,100] width:96 height:20
    
    group "Bump"
    (
        checkbox chk_bumpmap "BumpMap  . . . . . . ." pos:[15,140] width:68 height:20 enabled:true checked:false
        mapButton btn_bumpMap "BumpMap" pos:[120,140] width:175 height:20
        label lbl_bumMap    "UV1" pos:[300,140] width:96 height:20

        --checkbox chk_normalmap0  "NormalMap 1. . . . . ." pos:[15,160] width:96 height:20 enabled:true checked:false
        --mapButton btn_normalMap0 "NormalMap 1" pos:[120,160] width:175 height:20
        --label lbl_noramlMap0     "UV1" pos:[300,160] width:96 height:20

    
        checkbox chk_normalmap  "NormalMap 4. . . . . ." pos:[15,160] width:96 height:20 enabled:true checked:false
        mapButton btn_normalMap "NormalMap 4" pos:[120,160] width:175 height:20
        label lbl_noramlMap     "UV4" pos:[300,160] width:96 height:20
        
        checkbox chk_parallaxmap  "ParallaxMap. . . . . ." pos:[15,180] width:96 height:20 enabled:true checked:false
        mapButton btn_parallaxMap "Parallax" pos:[120,180] width:175 height:20
        label lbl_parallaxMap     "UV1" pos:[300,180] width:96 height:20
        
        checkbox chk_bumpMapIsNormalMap  "Bumpmap is a normalmap" pos:[15,200] width:150 height:20 enabled:true checked:false
        spinner spn_parallaxFactor        "Parallax factor U  " pos:[100,220] width:90 height:16 scale:0.001 range:[0,200,0.02] type:#float
        spinner spn_parallaxFactorRatioUV "Parallax ratio U/V" pos:[100,240] width:90 height:16 scale:0.001 range:[0,16,0.02] type:#float
        
        
        
        
    )
    
    group "Enviroment"
    (
        checkbox chk_environmentmap2d  "Enviroment 2D. . . ." pos:[15,280] width:96 height:20 enabled:true checked:false
        mapButton btn_AmbientMap0 "enviroment" pos:[120,280] width:175 height:20
        label lbl_AmbientMap0     "---" pos:[300,280] width:96 height:20
        
        checkbox chk_environmentmap3d  "Enviroment 3D. . . ." pos:[15,300] width:96 height:20 enabled:true checked:false
        mapButton btn_CubeMap0 "enviroment" pos:[120,300] width:175 height:20
        label lbl_CubeMap0     "---" pos:[300,300] width:96 height:20
        
        
        spinner spn_envMaskFactor "Env mask factor " pos:[70,340] width:90 height:16 scale:0.01 range:[0,1,0.01] type:#float
        
        label chk_environmentmask0  "Env Mask . . . . . ." pos:[35,360] width:96 height:20
        mapButton btn_envMaskMap "enviroment" pos:[120,360] width:175 height:20
        label lbl_envMaskMap     "UV1" pos:[300,360] width:96 height:20
    
        
        checkbox chk_levelmapasenvmaskmap  "Spec map as env mask map" pos:[35,380] width:170 height:20 enabled:true 
        
    )
     
    group "Blend Diffuse"
    (
        checkbox chk_controlMap "Blend" pos:[97,420] width:96 height:20 enabled:true checked:false
        
        label lbl2 "ColorMap2 . . . . . ." pos:[15,440] width:96 height:20
        mapButton btn_diffMap2 "MapButton" pos:[120,440] width:175 height:20
        label lbl_diffMap2    "UV1" pos:[297,440] width:96 height:20
        
        label lbl1 "BLEND . . . . . . ." pos:[15,460] width:96 height:20
        mapButton btn_controlmap "Blend " pos:[120,460] width:175 height:20
        label lbl_controlmap    "UV3" pos:[297,460] width:96 height:20
    )
    
    spinner spn_mipmapbias "MipMap bias" pos:[60,500] width:90 height:16 range:[-10,10,0] type:#float scale:0.01
    --      groupBox grp1 "GroupBox" pos:[8,197] width:307 height:112
    
    on params_map open do
    (
        EnableOptions this.shaderType 
    )
)


rollout params_deform "Zombie Deform" width:260 height:81
(
    checkbox chk_swing "Swing" pos:[16,10] width:80 height:19
    checkbox chk_billBoard "BillBoard" pos:[100,10] width:80 height:19
    checkbox chk_notculling "Not culling( two Sided )" pos:[15,30] width:150 height:20
    checkbox chk_prodepthbias "use depth bias" pos:[15,50] width:150 height:20
    checkbox chk_shadowEmitter "shadow emitter"  pos:[15,70] wifth:150 height:20
    on Params_deform open do
    (
        EnableOptions this.shaderType 
    )
)

rollout params_type "Shader" width:162 height:600
(
    dropdownList ddl_type "Shader type" pos:[16,14] width:200 height:40 items:nebGfxMaterialTypeList
    edittext edt_script "" pos:[15,72] width:300 height:126 enabled:true readOnly:true lableOnTop:false

    on params_type open do
    (        
        --Copy from shaderType to dropdownlist
        ddl_type.selection =  findItem nebGfxMaterialTypeList this.shaderType
           
        --show the script
        edt_script.text = nebMatOp.getGfxMaterialScriptInText this.shaderType 
        EnableOptions this.shaderType      
    )
    
    on ddl_type selected val do
    (
        this.shaderType = ddl_type.selected
        -- show the script
        edt_script.text = nebMatOp.getGfxMaterialScriptInText this.shaderType
        EnableOptions this.shaderType     
    )
)

on create do 
( 
    -- setup initial material 
    print ("\nPRINCIPAL CREADO")
    delegate.Ambient_Diffuse_Lock = false
    delegate.Ambient_Diffuse_Texture_Lock = false
    /*
    
            zDiffuseMap1  type:#texturemap ui:btnMap1
            diffMapEnable type:#boolean ui:chkMap1
            zSpecPow       type:#float  ui:spnSpecPow   
            zSpecLevel     type:#float  ui:spnSpecLevel 
            zSpecEnable    type:#boolean ui:chkSpecular
    */
)

on update do 
(
    if debugZombie do (format "\n%: update" this.name)
    if version == 1 then
    (
        if debugZombie do (format "\n\t version 1 to 2\n" )
        this.z_Param_hasalpha_true  = this.z_Pass_alpha
        this.z_Pass_alpha = false 
    )
    
    if version == 4 then
    (
        z_Param_map_environmentmap3d = false
        z_Param_pro_notculling       = false
        z_Param_map_parallax         = false
    )
    
    if version < 9 then -- Set the posible shadowEmitter value
    (
        this.shadowEmitter = not this.z_Param_hasalpha_true
    )
    
    if version < 12 then -- Set the posible shadowEmitter value
    (
        --empty
    )
    
    if version < 14 then 
    (
        -- obsolete texture
        z_Sur_Texture_clipMap = undefined
        print "OK v14"
    )
)

on postLoad do
(
    if debugZombie do (  format "\n%: postLoad " this.name )
    /* comment call gc in posload scene
    gcCount = mod (gcCount +1)  10
    if ( gcCount == 0 ) do 
    (
        if debugZombie do (format "\n%  Call Garbage collector and freeSceneBitmaps" )
        freeSceneBitmaps() 
        gc()
    )
    */
    --trick solution of max problem, any time if assign a map the max crashes , delegate.maps[n] = RGB_Add()
    delegate.maps = delegate.maps
)

on Load do
(
    format "\n%: Load" this.name 
)

fn updateAll =
(
   update_specular_map true
   update_clip_map true
   update_bumpMap true 
   update_illumination true
   update_diffuse_ambient true
)
fn fix =
(
   for index = 1 to 11 do
   (
        delegate.maps[index] = undefined
        delegate.mapenables[index] = false
   )
   updateAll()
)


)
--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------a                                          e
