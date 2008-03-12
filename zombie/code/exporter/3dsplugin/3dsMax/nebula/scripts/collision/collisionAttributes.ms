/**
   @file collisionAttributes.ms
   @ingroup n3dsMaxScripts
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   @brief Collision Attributes definition.
*/
--------------------------------------------------------------------------------
/**
*/
CollCompositeParamsData = attributes CollCompositeParams
(
	parameters main
	(
		geometries type:#nodeTab tabSizeVariable:true
	)
)

--------------------------------------------------------------------------------
/**
*/
CollParamsData = attributes CollParams
attribID:#(0x3b922c83, 0x4d80ac61)
(
	parameters main 
	( 
		material type:#string
		space    type:#string
		wall     type:#boolean
		ground   type:#boolean
		ceiling  type:#boolean
		walkable type:#boolean
	)
)

--------------------------------------------------------------------------------
/**
*/
fn redefineCollParamsData =
(
	local collisionMaterials = getCollisionMaterials()
	--print  collisionMaterials
	local materialString = "#("
	
	if (( Array == classof collisionMaterials)  and (collisionMaterials.count > 0)) do
	(
	    --print collisionMaterials[1]
	    local materialString = materialString + "\"" + collisionMaterials[1] 
        for i = 2 to collisionMaterials.count do ( 
            --format "% %\n" i collisionMaterials[i]
            materialString =   materialString +"\", \"" +  collisionMaterials[i]
       )
       materialString = materialString +"\""
	)
	materialString = materialString + ")"
	
	print materialString
	local spaceNames = NebCollOp.getPhysicSpaceNames()
	local spaceString = spaceNames as String
	
	local CollParamsString = "attributes CollParams \n" +
	"( \n" +
	"	parameters main rollout:params \n" +
	"	( \n" +
	"		material type:#string default:\"default\" ui:material_dd \n" +
	"		space    type:#string default:\"<none>\" \n" +
	"		wall     type:#boolean default:false ui:wall_cb\n" +
	"		ground   type:#boolean default:false ui:ground_cb\n" +
	"		ceiling  type:#boolean default:false ui:ceiling_cb\n" +
	"		walkable type:#boolean default:false ui:walkable_cb\n" +
	"	) \n" +
	" "	+
	"	rollout params \"Collision Parameters\" \n" +
	"	( \n" +
	"		dropdownlist material_dd \"Material\" items: " + materialString + " \n" +
	"		dropdownlist spaces_dd \"Space\" items: " + spaceString + " \n" +
	"		checkbox wall_cb \"Wall\" \n" +
	"		checkbox ground_cb \"Ground\" \n" +
	"		checkbox ceiling_cb \"Ceiling\" \n" +
	"		checkbox walkable_cb \"Walkable\" \n" +
	" " +
	"       on params open do \n" +
	"       ( \n" +
	"			local index = 1 \n" +
	"			for name in material_dd.items do \n" +
	"           ( " +
	"				if ( name == material ) do \n" +
	"				( \n" +
	"					material_dd.selection = index \n" +
	"				) \n" +
	"				index = index + 1 \n" +
	"			) \n" +
	" " +
	"			index = 1 \n" +
	"			for name in spaces_dd.items do \n" +
	"           ( " +
	"				if ( name == space ) do \n" +
	"				( \n" +
	"					spaces_dd.selection = index \n" +
	"				) \n" +
	"				index = index + 1 \n" +
	"			) \n" +
	"		) \n"+	
	" " +
	"       on material_dd selected i do \n" +
	"       ( \n" +
	"			material = material_dd.selected \n" +
	"       ) \n" +
	" " +
	"       on spaces_dd selected i do \n" +
	"       ( \n" +
	"			space = spaces_dd.selected \n" +
	"       ) \n" +
	" " +
	"       on wall_cb changed state do \n" +
	"       ( \n" +
	"			if state then \n" +
	"			( \n" +
	"				ground = false \n" +
	"				ceiling = false \n" +
	"				setCollisionTexture $ \"wall\" \n" +
	"			) \n" +
	"           else \n" +
	"           ( \n" +
	"          		setCollisionTexture $ \"normal\" \n" +
	"           ) \n" +
	"       ) \n" +
	" " +
	"       on ground_cb changed state do \n" +
	"       ( \n" +
	"			if state then \n" +
	"			( \n" +
	"				wall = false \n" +
	"				ceiling = false \n" +
	"				setCollisionTexture $ \"ground\" \n" +
	"			) \n" +
	"           else \n" +
	"           ( \n" +
	"          		setCollisionTexture $ \"normal\" \n" +
	"			) \n" +
	"       ) \n" +
	" " +
	"       on ceiling_cb changed state do \n" +
	"       ( \n" +
	"			if state then \n" +
	"			( \n" +
	"				wall = false \n" +
	"				ground = false \n" +
	"				setCollisionTexture $ \"ceiling\" \n" +
	"			) \n" +
	"           else \n" +
	"           ( \n" +
	"          		setCollisionTexture $ \"normal\" \n" +
	"			) \n" +
	"       ) \n" +
	"	) \n" +
	") "
	
	try
	(
		custAttributes.redefine CollParamsData CollParamsString
	)catch()
)

--------------------------------------------------------------------------------
/**
*/
fn addCollAttributes =
(
	local objects = #()
	for object in $* do
	(
		if isPhysicCollision object do
		(
			append objects object
		)
	)
	
	custAttributes.add objects CollParamsData
)

--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------
