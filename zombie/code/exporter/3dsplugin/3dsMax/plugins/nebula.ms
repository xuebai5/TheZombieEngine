/**
   @file nebula.ms
   @ingroup n3dsMaxScripts
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   @brief Include of Nebula 3ds Max Scripts 
   @remarks 
   This script only include all modules , only for declarations of function , struct
   and simple global variables
   
   the initialize of modules is in startup/nebula.ms

   - scriptNebulaVersion: the current version of this tool
   - nebulaVersion is a persistent global variables , dont change the name of this variable.
   
*/
global nebulaProgramersVersion  = false  -- Set this to false for release version
global scriptNebulaVersion      = 000804 -- version 00.08.02  The version of this tools
global unversionedNebulaVersion = 000200 -- version 00.02.00  The default value for unversioned files
global nebulaVersion            = scriptNebulaVersion -- Pesist variable

-- Use FileIn is better for debug errors
-- The maxscript open the corresponent file.

fileIn "nebula/scripts/log.ms"
--Config the log system
global logNebulaSystem = NebCreateLog "NebulaSystem" "core of nebula script" 2 false
nebLogServer.enable = nebulaProgramersVersion

global nebCollOp
global NebMatOp

fileIn "nebula/scripts/math.ms"
fileIn "nebula/scripts/mainfunctions.ms"

fileIn "nebula/scripts/system/nebulaop.ms"

fileIn "nebula/scripts/material/zombie_texturemap.ms"
fileIn "nebula/scripts/material/zombie_material.ms"
fileIn "nebula/scripts/material/materialFunctions.ms"
fileIn "nebula/scripts/material/RenderRollout.ms"
fileIn "nebula/scripts/material/materialUtil.ms"


fileIn "nebula/scripts/collision/collisionFunctions.ms"
fileIn "nebula/scripts/collision/collisionAttributes.ms"
fileIn "nebula/scripts/collision/collisionCallbacks.ms"
fileIn "nebula/scripts/collision/collisionUI.ms"
fileIn "nebula/scripts/collision/collisionObjects.ms"

fileIn "nebula/scripts/visibility/visportalobject.ms"
fileIn "nebula/scripts/visibility/visoccluderobject.ms"
fileIn "nebula/scripts/visibility/visibility.ms"
fileIn "nebula/scripts/visibility/visibilityUtil.ms"

fileIn "nebula/scripts/lod/lod.ms"
fileIn "nebula/scripts/lod/lodutil.ms"

fileIn "nebula/scripts/plugs/plugs.ms"
fileIn "nebula/scripts/plugs/plugsUtil.ms"

fileIn "nebula/scripts/export/export.ms"
fileIn "nebula/scripts/export/grimoirerollout.ms"
fileIn "nebula/scripts/export/exportrollout.ms"


fileIn "nebula/scripts/mainCallbacks.ms"
fileIn "nebula/scripts/tools/nebtreenormals.ms"


--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------
