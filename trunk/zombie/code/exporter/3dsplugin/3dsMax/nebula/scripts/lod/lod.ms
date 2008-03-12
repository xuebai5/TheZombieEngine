/**
   @file lod.ms
   @ingroup n3dsMaxScripts
   
*/

--------------------------------------------------------------------------------
/* 
    1 Step - Define Globals
*/
global nebLodData -- Data1 is a example of name


--------------------------------------------------------------------------------
/**
    2 Step Define struct data the name is NebStruct..
*/


struct NebStructLodData
(
    num,
    list,
    
    fn check =
    (
        if num < 0  do num = 0
        if num > 6 do num = 6
        
        for i = 2  to num do
        (
            if ( list[i]  < list[i-1])  do list[i] = list[i-1]
        )
    )
)


--------------------------------------------------------------------------------
/**
    3 Step
    Define Nebula Module Operations is a struct
    
    example nebVisOp => nebula visibility operations
    
    The name is nebNameOp  
    This struct doesnt have a instance
    
*/
struct NebLodOp
(
    fn DeclareVar =
    (
        persistent global nebLodData = NebStructLodData()
        print "DECLARADA LOD"
    ),
    
    --------------------------------------------------------------------------------
    /** 
        declare persistent variables of this file
	*/
    fn declarePersistent =
    (
        persistent global nebLodData
    ),
    
    fn ResetValues =
    (
        nebLodData.num = 0
        nebLodData.list = #()
             
        nebLodData.list = #( 0.0 , 0.0 , 0.0 , 0.0 ,0.0,0.0,0.0)
        --nebLodData.list = 5
    ),
    
    --------------------------------------------------------------------------------
	/**
	    Init this module Template
	    this call in startup/nebula.ms
	*/
    fn InitModule =
    (
        NebLodOp.declareVar()
        NebLodOp.resetValues()
    ),
    
    --------------------------------------------------------------------------------
	/**
	    for update object or persist data , from old version max file
	    
	*/
    fn UpdateFromVersion version =
    (
         if (version < 000600) do
        (
            nebLodData.num  = 0
        )
    ),
    
    --------------------------------------------------------------------------------
    /**
        Callback file preMerge
        Create copy of all persistent variables
    */    
    fn FilePreMerge =
    (
        global NebMergeCopyOfNebLodData = nebLodData
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
        nebLodData = NebMergeCopyOfNebLodData -- restore original value

    ),
    
     --------------------------------------------------------------------------------
    /**
      not is necessary , Callback file post open 
    */
    
    fn FilePostOpen =
    (
        

    ),
    
    fn FilePostNew =
    (
        NebLodOp.ResetValues()
    )
    

) 



--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------