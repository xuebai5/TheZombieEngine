/**
   @file log.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo <cristobal.castillo@tragnarion.com>
   @brief main functions of scripts
   @remarks
   functions
   - Neblog name level message
   - NebCreateLog name description level enable
   - NebAssert
   - NebWarning
   - NebMessage
   
	Structs
	- tlog, 
  
*/

struct NebStructLog (
	name,
	description,
	level,
	enable,
	fn log warning message =
	(
		if ( (enable == true) and (warning <= level) ) then
		(
			format "%:%: " name  warning
			format "%" message
			format "\n"
		)
	)
)

fn NebCompareLog v1 v2 =
(
   case of
   (
   (v1.name < v2.name ): -1
   (v1.name > v2.name ): 1
   default: 0
   )
)

global nebLogServer
struct NebStructLogServer (
	list,
	enable,
	fn Init =
	(
		list =#()
		enable = false
	),
	fn Find name =
	( 
	 -- Bynary search
	    if ( enable == true ) do
		(
			local minV = 1
			local maxV = list.count
			local idx
			local invalid = true
			while ( (invalid) and ( minV<=maxV) ) do
			(
				idx = ( minV + maxV) /2
				log = list[idx]
				case of
				(
					( name < log.name): maxV = idx - 1
					( name > log.name): minV = idx + 1
					default: invalid = false
				)
			)
			
			if ( invalid ) then return 0
			else return log
		)
		return 0
	),

	fn  create name description level enable =
	(
		local log = find name
		if (log == 0) then 
		(
			log = NebStructLog()
			log.name = name
			log.description  = description
			append list log
			qsort list NebCompareLog	
		)
		
		log.level = level
		log.enable = enable
		return log
	),
	
	fn showRollout =
    (
         rci = rolloutCreator "logcConfigRollout" "Log config"
    	 rci.begin()
    	 local y = 20
    	 for i in list do
    	 (
    		local strEnableParam = "  height:23 checked:" + ( i.enable as string)
    		strEnableParam += " pos:[75, " +  ( y  as string ) +"]"
    		
    		local strLevelParam = " width:46 height:16 type:#integer scale:1 "
    		strLevelParam += "range:[0 , 100, " + (i.level as string) +"] "	
    		strLevelParam += " pos:[22, " +  ( (y +2) as string ) +"]"
    		
    		rci.addControl #checkbox ("chk_"+i.name) i.name paramStr:strEnableParam
    		rci.addControl #spinner ("spn_"+i.name) "" paramStr: strLevelParam
    		y+=30
    	 )
    
    	 for i in list do
    	 (
     	    local strEnable = "local log = nebLogServer.find @" + i.name +"@ \n log.enable = val"
    		local strLevel  = "local log = nebLogServer.find @" + i.name +"@ \n log.level = val"
    		rci.addHandler ("chk_"+i.name) #changed filter:on paramStr:"val" codeStr: strEnable
    		rci.addHandler ("spn_"+i.name) #changed filter:on paramStr:"val" codeStr: strLevel
    	 )
    
    	 local strEnableParam = "height:23 checked:" + ( nebLogServer.enable as string)
    	 strEnableParam += " pos:[75, " +  ( (y+30)  as string ) +"]"
    	 format "%\n"  strEnableParam
    	 rci.addControl #checkbox ("chk_vis_all") "enable log" paramStr:strEnableParam
    	 rci.addHandler ("chk_vis_all") #changed filter:on paramStr:"val" codeStr: "nebLogServer.enable = val"
    
    	 
    	 rci.end()
    	 createDialog  rci.def  width:300 style:#(#style_titlebar, #style_border, #style_sysmenu, #style_resizing)
    )
)


global nebLogServer = NebStructLogServer()
nebLogServer.init()




--------------------------------------------------------------------------------
/**
*/
fn NebLog name level message  =
(
    local log = nebLogServer.find name
	if ( log != 0) then
	(
		log.log level message
	) 
)

--------------------------------------------------------------------------------
/**
*/

fn NebCreateLog name description level enable =
(
	return (nebLogServer.create name description level enable)
)

--------------------------------------------------------------------------------
/**
    @brief if expresion is false it's false show a message 
    if programers version then raise exception else exit of max
    @param expr The expresion
    @param message The message to show ( string )
    @param logname The log's name  ( string )
*/

fn NebAssert expr message ex:"NebulaSystem" =
(
	if  ( (expr == false) or (expr == undefined )) then
	(	
		local msg = "Nebula Assert: " + ex + "\n\n"
		msg += message

      
        if ( nebulaProgramersVersion == true ) then 
		(
		    msg += "\n\n\Continue ?"
		    local result = queryBox msg
		    if ( result != true) do throw ex
		    return true
		) else
		(
			msg += "\n\nRESTART 3DS MAX"
		    messageBox msg
		    quitmax() -- Exit Max with dialog to save
		)
	)
	return false
)

--------------------------------------------------------------------------------
/**
    @brief Show a message if expr it's true
    if programers version then raise exception else continue
    @param expr The expression
    @param message The message to show ( string )
    @param exname The name of exception to raise ( string )
    @param logname The log's name  ( string )
*/
fn NebWarning expr message ex:"NebulaSystem" log:"NebulaSystem" =
(
	if  ( (expr == false) or (expr == undefined )) then
	(	
		
		NebLog log 0 message
		
		local msg = "Nebula Warning: " + exname + "\n\n"
				
		if ( nebulaProgramersVersion == true ) then 
		(
		    msg += message + "\n\n" + "Continue ?"
		    local result = queryBox msg
		    if ( result != true) do throw ex
		    
		) else
		(
		    messageBox msg
		)
		
		
		return true
	)
	return false
)

--------------------------------------------------------------------------------
/**
    @brief Show a message
    @param expr The expression
    @param message The message to show ( string )
    @param logname The log's name  ( string )
*/
fn NebMessage message log:"NebulaSystem" =
(
	local msg = "Nebula Message: " + "\n\n"
	msg += message
	local result = messageBox msg
	NebLog log 1 message
	return false
	return true
)

--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------