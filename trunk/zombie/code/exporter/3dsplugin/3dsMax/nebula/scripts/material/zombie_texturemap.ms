/**
   @file zombie_texturemap.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo <cristobal.castillo@tragnarion.com>
   @brief Declaration of zTextureMap class.
*/
global debugZombieTexture = false
plugin textureMap  zTextureMap
name:"zBitmap"
ClassID:#(0x4010142f, 0x6a2c0818)
extends:Bitmaptexture
replaceUI:true
version:4
(
/** 
   @endcode 
*/
fn GetMapName =
(
  return ("HOLA" as string)
)


parameters zTextureParam rollout:params
(
   --backward compatibility
    bitmap2 type:#bitmap
    filename2 type:#string default: ""
   -- new properties
    filename  type:#string default: ""

    on filename set val do
    (
        if not loading do
        (
            if debugZombieTexture do ( format "Set filename %: \n" this.filename)
            if ( this.name == "zTextureMap") do
            (
                assignNewName this        
            )
            
            local index = findString this.name "#"
            local sufix = "#"
            if ( index != undefined ) do sufix = substring this.name index -1
            this.name =  (this.filename) + sufix
            
            try
            (
                delegate.filename = this.filename
            ) catch 
            ( 
                if debugZombieTexture do ( format "file no exited %: asigned error.tga : %\n" this.filename (getCurrentException()) )
                delegate.filename = "E:/renaissance/trunk/resourceZombiePlugin/error.tga" 
            )
            --this.params.btn.caption = this.filename
        )
    )
)

parameters zTextureAnimator rollout:paramAnimator 
(
    animator type:#boolean default:false ui:chk_animator  /**< has animator> */
    z_first    type:#integer default:0  -- ui:spn1        /**< backward compatiblity*/
    z_last     type:#integer default:0   --  ui:spn2     /**< backward compatiblity*/
    z_freq     type:#integer default:10  ui:spn3         /**< The frequence, the channel time*/
    z_loop     type:#integer  default:1 ui:rdo1          /**< Loop Mode */
    z_seq      type:#string default:"" ui:edt1           /**< Text sequence of textures*/

    
)

rollout paramAnimator "Animator" width:330 height:500
(
    checkbox chk_animator "Animation" pos:[16,10] width:148 height:15
    
--  spinner spn1 "First" pos:[30,40] width:77 height:16 scale:1
--  spinner spn2 "Last" pos:[130,40] width:77 height:16
    spinner spn3 "Frequency" pos:[200,40] width:77 height:16
    
    radiobuttons rdo1 "Loop Mode" pos:[20,40] width:67 height:62 labels:#("loop", "oneshoot", "pingpong") default:1 columns:1
    editText edt1 "" pos:[29,120] width:277 height:22
    


)
rollout params "Texture" width:330 height:280
(
    button btn "Button" pos:[13,27] width:302 height:22
    button btn2 "View Image" pos:[151,63] width:135 height:27
   
    on params open do
    (

        btn.caption = this.filename
    )
    on btn pressed do
    (
      local str
      
      str = getBitmapOpenFileName filename: this.filename
       
       if ((classof str) as string)  == "String" do
       (
            this.filename = str 
            
            btn.caption = this.filename
       ) -- else its pressed cancel button

    )
    on btn2 pressed do
    (
        --this.delegate.viewImage()
        try
        (
          local tmp = openBitmap ( this.filename )
          close tmp
          display tmp
          tmp = 0
          gc()  
        ) catch ( print "Error when open displayed")
        
    )
)

/**
    @fn copyFrom 
    @param mat a Bitmaptexture class
    @return this
*/


fn copyFrom mat =
(
/*
  if hasproperty mat "bitmap" do
  (
      str  = ((classof mat.bitmap) as string)
      print ( "copyFrom:  " +str )
  
      if (str == "BitMap") do
      (
         this.bitmap2 = mat.bitmap
      )
  )
  */
  this.filename = mat.filename
  
  return this;
)

fn updateto4 =
(
    tmp = this.z_first as string
    tmp = tmp + ".."
    tmp = tmp + ( this.z_last as string) 
    this.z_seq = tmp    
)

on update do 
(
   --PRINT "UPDATE"
    if version == 1 then
    (
      print "version 1"
       try
       (
       this.filename = this.bitmap2.filename
       close this.bitmap2
       --delete bitmap
       --this.bitmap2 = bitmap 0 0
       )
       catch ( delegate.filename = "E:/renaissance/trunk/resourceZombiePlugin/error.tga" )
       updateto4()
    ) else if version == 2 then
    (
       try
       (
       if debugZombieTexture do ( format "% : version 2 to 3 :%\n" this this.filename )
       this.filename = this.filename2
       close this.bitmap2
       ) catch ( delegate.filename = "E:/renaissance/trunk/resourceZombiePlugin/error.tga" )
       updateto4()
       --this.bitmap2 = bitmap 1 1 --clear bitmap
    ) else if version == 3 then
    (
        updateto4()
    )
)

)

