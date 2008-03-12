/**
   @file materialFunctions.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo <cristobal.castillo@tragnarion.com>
   @brief main functions of scripts
   @remarks
   functions
   -showConfigLogRollout
   -nlog name level message
   -ncreatelog name description level enable
    
    Structs
    - tlog, 
  
*/


struct structNebulaMaskRender 
(
    alphatwoside_true,
    deform_billboard,
    deform_swinging,
    env_ambientlighting,
    env_diffuselighting,
    env_fogdisable,
    env_illuminationcolor,
    env_specularlighting,
    hasalpha_true,
    map_bumpmap,
    map_clipmap,
    map_colormap,
    map_controlmap,
    map_illuminationmap,
    map_levelmap,
    map_lightmap,
    map_normalmap,
    map_reflectionmap 
)

global nebMaskRend
global nebGfxMaterialTypeList
global nebMatData

struct NebStructMatData
(
    cacheGfxPropertiesMaterialName,
    cacheGfxPropertiesGfxList,
    toolSetMaterialName,
    toolGetMaterialName
)


struct NebMatOp
(
    fn declareVar =
    (
        global nebMaskRend = structNebulaMaskRender()
        global nebGfxMaterialTypeList = #("invalid","custom")
        global nebMatData = NebStructMatData()
    ),
    fn declarePersistent =
    (
        persistent global nebMaskRend
    ),
    fn resetValues =
    (
        nebMaskRend.alphatwoside_true = true
        nebMaskRend.deform_billboard = true
        nebMaskRend.deform_swinging = true
        nebMaskRend.env_ambientlighting = true
        nebMaskRend.env_diffuselighting = true
        nebMaskRend.env_fogdisable = true
        nebMaskRend.env_illuminationcolor = true
        nebMaskRend.env_specularlighting = true
        nebMaskRend.hasalpha_true = true
        nebMaskRend.map_bumpmap = true
        nebMaskRend.map_clipmap = true
        nebMaskRend.map_colormap = true
        nebMaskRend.map_controlmap = true
        nebMaskRend.map_illuminationmap = true
        nebMaskRend.map_levelmap = true
        nebMaskRend.map_lightmap = true
        nebMaskRend.map_normalmap = true
        nebMaskRend.map_reflectionmap = true
        
        global nebGfxMaterialTypeList = #("invalid","custom")
        try
        (
            -- Get th graphics material in real wroking copy
            nebGfxMaterialTypeList = nebGfxMaterialTypeList + ( nGetGFXMaterialList true)
        ) catch()
            
        nebMatData.cacheGfxPropertiesMaterialName =""
        nebMatData.cacheGfxPropertiesGfxList = #()
        nebMatData.toolSetMaterialName = "invalid"
    ),
    
    fn initModule =
    (
        NebMatOp.declareVar()
        NebMatOp.resetValues()
    ),
    
    --------------------------------------------------------------------------------
    /**
        for update object or persist data , from old version max file
        
    */
    fn updateFromVersion version=
    (
        if (version < 000300) do
        (
            -- Update from 0.2.0 to 0.3.0
        )
        
        if (version < 000400) do
        (
            -- Update from 0.3.0 to 0.4.0
        )

    ),
    
        --------------------------------------------------------------------------------
    /**
        Callback file preMerge
        Create copy of all persistent variables
    */    
    fn FilePreMerge =
    (
        global NebMergeCopyOfNebMaskRend = nebMaskRend
    ),
    
    --------------------------------------------------------------------------------
    /**
        Callback file  postMerge
        @version the nebula version of objects imported
        @listObjects array of objects imported
        restore the persistent variables
        
    */    
    fn FilePostMerge version listObjects =
    (
        nebMaskRend = NebMergeCopyOfNebMaskRend  -- restore the previous values
    ),
    
    --------------------------------------------------------------------------------
    /**
    */
    fn updateBacked mat =
    (
        if classOf mat != zStdMaterial do return
        
        --Search all "shell_materials" contains this mat
        --for mat1 in refs.dependents  mat where (  (classOf mat1) == Shell_Material ) do print mat1
        for mat1 in refs.dependents  mat where ( 
            (classOf mat1) == Shell_Material and
            (mat1.originalMaterial == mat)
            ) do 
        (
            --Copy all properties except lihghtmap
            --format "SISI2 %\n" mat1
            tmpLightmap = mat1.bakedMaterial.z_Sha_Texture_lightMap
            mat1.bakedMaterial = copy mat
            mat1.bakedMaterial.z_Sha_Texture_lightMap = tmpLightmap
        )
        -- Search MultiMaterials contains this
        for mat_multi in refs.dependents  mat where ( classOf mat_multi == Multimaterial ) do 
        (
            -- First search the slot number
            slots =#()
            for index = 1 to ( getNumSubMtls mat_multi ) where ( (getSubMtl mat_multi index) == mat ) do 
            (
                append slots index
            )
            --format "mat % \n\t  % , % \n" mat_multi ( getNumSubMtls mat ) slots
            
            -- Search shellMaterials contains the multiMaterial
            for matShell in refs.dependents  mat_multi where ( 
                classOf matShell == Shell_Material  and 
                matShell.originalMaterial == mat_multi ) do 
            (
                for index in slots do
                (
                    --Copy all properties except lihghtmap
                    mat_backed = getSubMtl matShell.bakedMaterial index
                    tmpLightmap = mat_backed.z_Sha_Texture_lightMap
                    mat_backed =  copy mat
                    mat_backed.z_Sha_Texture_lightMap = tmpLightmap
                    setSubMtl matShell.bakedMaterial index mat_backed
                )  
            )
        )
    
    ),
    
    --------------------------------------------------------------------------------
    /**
    */
    fn updateBackedArray arr =
    (
        for obj in arr where ( 
              isproperty obj "material" and
              classOf obj.material == Shell_Material
            )do 
        (
            local matShell = obj.material
            local mat_original = matShell.originalMaterial
            if classOf mat_original == zStdMaterial then
            (
                tmpLightmap = matShell.bakedMaterial.z_Sha_Texture_lightMap
                matShell.bakedMaterial= copy matShell.originalMaterial 
                matShell.bakedMaterial.z_Sha_Texture_lightMap = tmpLightmap
            ) else if classOf mat_original == Multimaterial then 
            (
                for index = 1 to ( getNumSubMtls mat_original ) where (  classOf (getSubMtl mat_original index) == zStdMaterial ) do 
                (
                    --Copy all properties except lihghtmap
                    mat_backed = getSubMtl matShell.bakedMaterial index
                    tmpLightmap = mat_backed.z_Sha_Texture_lightMap
                    mat_backed =  getSubMtl matShell.originalMaterial index
                    mat_backed.z_Sha_Texture_lightMap = tmpLightmap
                    setSubMtl matShell.bakedMaterial index mat_backed
                )
            )
        )
    ),
    --------------------------------------------------------------------------------
    /**
    */
    fn getGfxMaterialScriptInText nameMaterial =
    (
        local text = ""
        try
        (
            local fileName = nGetGFXMaterialPath()
            fileName = fileName + "/" + nameMaterial + ".n2"
            local materialFile = openFile fileName mode:"rt"
            if (materialFile != undefined ) do -- file open
            (
                try
                (
                    seek materialFile #eof
                    local length  = filePos materialFile -- the length of file
                    seek materialFile 0
                    if length > 4000 do -- I've decide only 4000 characters for read
                    (
                        length = 4000
                    )
                    text = readChars  materialFile length
                    close materialFile
                )
                catch
                (  
                    close materialFile
                    format "*** % ***\n" (getCurrentException())
                ) 
            )
        ) catch
        ( 
            text = "" 
            format "*** % ***\n" (getCurrentException())
        )
        return text
    ),
    
    --------------------------------------------------------------------------------
    /**
    */
    fn getGfxMaterialProperties nameMaterial =
    (
        local list = #()
        if ( nameMaterial == nebMatData.cacheGfxPropertiesMaterialName ) do
        (
            return nebMatData.cacheGfxPropertiesGfxList
        )
        try
        (
            local fileName = nGetGFXMaterialPath()
            fileName = fileName + "/" + nameMaterial + ".n2"
            local materialFile = openFile fileName mode:"rt"
            if (materialFile != undefined ) do -- file open
            (
                try
                (
                    while  not (eof materialFile) do
                    (
                        local par = #()
                        local myline = readLine materialFile
                        local words = filterString myLine "(), []"
                        if ( words.count >= 3 ) and ( words[1]== "nthis:addparam" ) do
                        (
                            append par words[2]
                            append par words[3]
                            append list par
                        )
                    )
                    close materialFile
                    nebMatData.cacheGfxPropertiesMaterialName = nameMaterial
                    nebMatData.cacheGfxPropertiesGfxList = list
                    
                )
                catch
                (  
                    close materialFile
                    format "*** % ***\n" (getCurrentException())
                ) 
            )
        ) catch
        ( 
            list = #() 
            format "*** % ***\n" (getCurrentException())
        )
        return list
        
    ),
    
    --------------------------------------------------------------------------------
    /**
    */
    fn getMaterialsFromSelectedObjects =
    (
        local list = #()
        for i in selection do
        (
            if isproperty i "material" do
            (
                append list i.material
            )
        )
        return list
    ),

    --------------------------------------------------------------------------------
    /**
    */    
    fn setShaderType mat depth =
    (
        if ( (classof mat) == zStdMaterial ) do
        (
            mat.shaderType = nebMatData.toolSetMaterialName
        )
    ),
    
    --------------------------------------------------------------------------------
    /**
    */    
    fn getShaderType mat depth =
    (
        if ( (classof mat) == zStdMaterial ) do
        (
            append nebMatData.toolGetMaterialName mat.shaderType
        )
    )
    ,
    
    
    --------------------------------------------------------------------------------
    /**
    */      
    fn changeEstensionToDDS fileName =
    (
        local newName = fileName
        local idx = newName.count
        --find the last dot
		while ((idx>1) and (newName[idx] != ".")) do ( idx -= 1)
		
        if (idx >0 ) then 
        (
            newName = substring newName 1  ( idx-1) -- get the name
            newName = newName + ".dds"
        )
		
		return newName
    ),
    
    --------------------------------------------------------------------------------
    /**
    */  
    fn changeNormalExtensionToDDS mat depth =
    (
        try
        (
            if ( (classof mat) == zStdMaterial ) do
            (
                print mat
                --bump
                if (mat.z_Sur_Texture_bumpMap != undefined) and mat.bumpMapIsNormalMap then
                (
                   mat.z_Sur_Texture_bumpMap.filename = NebMatOp.changeEstensionToDDS mat.z_Sur_Texture_bumpMap.filename
                   print mat.z_Sur_Texture_bumpMap.filename
                )
                
                --normalmap
                if (mat.z_Sha_Texture_BumpMap3 != undefined) then
                (
                   mat.z_Sha_Texture_BumpMap3.filename = NebMatOp.changeEstensionToDDS mat.z_Sha_Texture_BumpMap3.filename
                   print mat.z_Sha_Texture_BumpMap3.filename
                )
            )
        ) catch( format "failed %" mat)
    )
)


fn Info mat depth=
(
    val = 0
    countlink = 0
    for index in (refs.dependents  mat) do
    (
        if (SuperClassOf index == GeometryClass) then
        (
            val = val +1
            link = false
            for index2 in ( refs.dependsOn  index) do 
            (
                if (index2 == mat) do link = true
            )
            if ( link == true ) do  ( countlink = countlink + 1)
        ) 
    )
    
    format "\n"
    for i = 1 to depth do (format "\t")
    format "Class::%  name::%  numRefs:: %  geom::% link::%" (classof  mat) mat.name (refs.dependents  mat).count val countlink
    if ( (countlink != val) and (depth==0)) do format " delete::"
    if ( val  == 0) do format " noapply::"
    if ( countlink != val ) do format " nolink::"
)

fn fix mat depth =
(
    try
    (
        if ( (classof mat) == zStdMaterial ) then
        (    
            mat.fix()
        ) 
    ) catch  ()    
)

fn updateAll mat depth =
(
    try
    (
        if ( (classof mat) == zStdMaterial ) then
        (
            mat.updateAll()
        ) 
    ) catch  ()
)


fn bumpTrue mat depth =
(
    if ( (classof mat) == zStdMaterial ) do
    (
        mat.z_Param_map_bumpmap = true
    )
)


fn bumpFalse mat depth =
(
    if ( (classof mat) == zStdMaterial ) do
    (
        mat.z_Param_map_bumpmap = false
    )
)

fn showTrue mat depth = 
(
    if ( (classof mat) == zStdMaterial ) do
    (
        mat.showInViewport = true
    )
)

fn showFalse mat depth = 
(
    if ( (classof mat) == zStdMaterial ) do
    (
        mat.showInViewport = false
    )
)

fn nebShadowFalse mat depth = 
(
    if ( (classof mat) == zStdMaterial ) do
    (
        mat.shadowEmitter = false
    )
)

fn showLightmap mat depth =
(
    if ( (classof mat) == zStdMaterial ) do
    (
        --print mat
        if mat.z_Sha_Texture_lightMap != undefined do
        (
            showTextureMap  mat mat.z_Sha_Texture_lightMap on
        )
    )
       
)

fn recursiveMaterial mat func depth =
(
   --format "recursive material % %\n" depth func
   if ( (classof mat) == undefinedClass ) then
   (
        format "\n undefined material  error"
   ) else
   (
       func mat depth
        try
        (       
            for index = 1 to ( getNumSubMtls mat ) do 
            (
                pr = depth +1
                recursiveMaterial (getSubMtl mat index) func pr
            )
        ) catch ( format "\n % no is a material\n" mat)
    )
)

fn iterateMaterial func showbar selectedObjects:false =
(
    if ( selectedObjects ) then
    (
        list = nebMatOp.getMaterialsFromSelectedObjects()
    )
    else
    (
        list = sceneMaterials
    )
    
    
    if ( showbar == true) then
    (
        progressStart ( func as string)
        try
        ( 
            local k = list.count
            local count = 0
            for m in list do 
            ( 
                recursiveMaterial m func 0
                count = count +1                
                progressUpdate ( 100.0  *count /k) 
            )
        ) catch ( )
        progressEnd()
    ) else
    (  
        for m in list do ( recursiveMaterial m func 0 )
    ) 
)

fn validMultimaterial mat =
(
    case ( classof mat) of
    (
        zStdMaterial:
        (   
            return true
        )
        
        Multimaterial:
        (
            for index = 1 to ( getNumSubMtls mat ) do 
                (
                if ( not (  (classof (getSubMtl mat index) ) == zStdMaterial ) ) do return false 
                )
            return true
        )
        default:
        ( 
            return false
        )
    )


)

fn valid mat =
(
    case ( classof mat) of
    (
        Shell_Material:
        (
            tmp1 = validMultimaterial mat.originalMaterial
            tmp2 = validMultimaterial mat.bakedMaterial
            return ( tmp1 and tmp2)
            
        )
        default:
        ( 
            tmp = validMultimaterial mat
            return tmp
        )
    )
)

fn selectInvalids =
(
    clearSelection()
    for mesh in geometry do 
    (
        if (  ( NebOp.IsType mesh "graphics" ) and not (valid  mesh.material) ) then 
        (
            selectMore mesh
        ) 
    )
)

fn  deselectInvalids =
(
    for mesh in geometry do 
    (
        if ( ( NebOp.IsType mesh "graphics" ) and not (valid  mesh.material) ) then 
        (
            deselect mesh       
        )   
    )
)

--------------------------------------------------------------------------------
/**
*/

fn clearMtlEditor mtlType:zStdMaterial =
(
    local j = 1
    for k in meditMaterials do
    (
        setMeditMaterial j ( mtlType ())
        j += 1
    )
)



--------------------------------------------------------------------------------
/**
*/
fn minimumPoint p1 p2 =
(
    local temp = p1
    if temp.x > p2.x do
    (
        temp.x = p2.x
    )
    if temp.y > p2.y do
    (
        temp.y = p2.y
    )
    if temp.z > p2.z do
    (
        temp.z = p2.z
    )
    return temp
)

--------------------------------------------------------------------------------
/**
*/
fn maximumPoint p1 p2 =
(
    local temp = copy p1
    if temp.x < p2.x do
    (
        temp.x = p2.x
    )
    if temp.y < p2.y do
    (
        temp.y = p2.y
    )
    if temp.z < p2.z do
    (
        temp.z = p2.z
    )
    return temp
)

--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------
