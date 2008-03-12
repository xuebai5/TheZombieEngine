/**
   @file templatemodule.ms
   @ingroup n3dsMaxScripts
   @reamark 
   This file is a template of a nebula maxscript , 
   
*/

--------------------------------------------------------------------------------
/* 
    1 Step - Define Globals
*/
global nebNameData1 -- Data1 is a example of name
global nebNameData2


--------------------------------------------------------------------------------
/**
    2 Step Define struct data the name is NebStruct..
*/

.
struct NebStructData1
(
    val
)

struct NebStructData2
(
    val
)

--------------------------------------------------------------------------------
/**
    3 Step
    Define Nebula Module Operations is a struct
    
    example nebVisOp => nebula visibility operations
    
    The name is nebNameOp  
    This struct doesnt have a instance
    
*/
struct NebNameOp
(
    fn DeclareVar =
    (
        global nebNameData1 = NebStructData1()
        global nebNameData2 = NebStructData2()
    ),
    
    --------------------------------------------------------------------------------
    /** 
        declare persistent variables of this file
	*/
    fn declarePersistent =
    (
        persistent global nebNameData1
    ),
    
    fn ResetValues =
    (
        nebTemplateData2.val = 1
        nebTemplateData1.init()
    ),
    
    --------------------------------------------------------------------------------
	/**
	    Init this module Template
	    this call in startup/nebula.ms
	*/
    fn InitModule =
    (
        NebMatOp.declareVar()
        NebMatOp.resetValues()
    ),
    
    --------------------------------------------------------------------------------
	/**
	    for update object or persist data , from old version max file
	    
	*/
    fn UpdateFromVersion version =
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
      not is necessary , Callback file post open 
    */
    
    FilePostOpen =
    (
        

    ),
    
    --------------------------------------------------------------------------------
    /**
        Callback file preMerge
        Create copy of all persistent variables
    */    
    fn FilePreMerge =
    (
        -- nebNameData1 is persistent
        global NebMergeCopyOfnebNameData1  = copy nebNameData1
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
        local mergeNebNameData1 = copy nebNameData1 -- Values of nebNameData1 in the scene
        nebNameData1 = NebMergeCopyOfnebNameData1 -- restore the original values
        
        for obj in listObjects do
        (
           if ( obj ) do
           (
                if (version < 000300) do
                (
                    -- Update OBJ from 0.2.0 to 0.3.0
                )
                
                if (version < 000400) do
                (
                    -- Update OBJ from 0.3.0 to 0.4.0
                )
           )
        )
        
    ),
    
) 



--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------