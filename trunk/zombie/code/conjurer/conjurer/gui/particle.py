##\file particle.py
##\brief Function to create a new particle system class

import wx
import pynebula
import servers
import app


#---------------------------------------------------------------------
def setDefualtOptions( particleNode, textureName ):
    particleNode.setmesh( 'wc:export/assets/aDefaultParticle/meshes/particle.n3d2' )
    particleNode.setdependency( 'wc:export/assets/aDefaultParticle/materials/Surface.n2', '/sys/servers/dependency/nsurfacedependencyserver', 'setsurface' )
    particleNode.settexture( 'diffMap' ,  textureName )
    particleNode.setint( 'funcSrcBlend' , 5 )
    particleNode.setint( 'funcDestBlend' , 6 )
    particleNode.setgroupindex(0)
    particleNode.setmeshusage('ReadOnly|PointSprite|NeedsVertexShader|')
    particleNode.setemissionfrequency(50.000000, 50.000000, 50.000000, 50.000000, 0.300000, 0.685714, 10.000000, 5.000000, 0)
    particleNode.setparticlelifetime(7.000000, 7.000000, 7.000000, 7.000000, 0.307143, 0.700000, 10.000000, 0.500000, 0)
    particleNode.setparticlestartvelocity(1.250000, 1.250000, 1.250000, 1.250000, 0.314286, 0.707143, 0.000000, 0.000000, 0)
    particleNode.setparticlerotationvelocity(100.000000, 100.000000, 86.000000, 0.000000, 0.321429, 0.750000, 0.000000, 0.000000, 0)
    particleNode.setparticlescale(0.000000, 0.700000, 0.960000, 1.000000, 0.335714, 0.657143, 1.000000, 0.100000, 0)
    particleNode.setparticleweight(0.100000, 0.100000, 0.100000, 0.100000, 0.321429, 0.671429, 0.000000, 0.000000, 0)
    particleNode.setparticlergb(0.433000, 0.433000, 0.433000, 0.467000, 0.467000, 0.467000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 0.192857, 0.800000)
    particleNode.setparticlealpha(0.000000, 0.320000, 0.500000, 0.000000, 0.142857, 0.750000, 0.000000, 0.000000, 0)
    particleNode.setparticlesidevelocity1(0.000000, 0.000000, 0.000000, 0.000000, 0.371429, 0.742857, 1.000000, 0.150000, 0)
    particleNode.setparticlesidevelocity2(0.000000, 0.000000, 0.000000, 0.000000, 0.307143, 0.728571, 0.750000, 0.250000, 1)
    particleNode.setparticleairresistance(0.000000, 0.520000, 1.000000, 1.000000, 0.357143, 0.828571, 0.000000, 0.000000, 0)
    particleNode.setparticlevelocityfactor(1.000000, 0.680000, 0.520000, 0.340000, 0.350000, 0.657143, 0.000000, 0.000000, 0)
    particleNode.setemissionduration(10.000000)
    particleNode.setloop(True)
    particleNode.setactivitydistance(50.000000)
    particleNode.setspreadangle(0.000000)
    particleNode.setbirthdelay(0.500000)
    particleNode.setstartrotation(0.000000)
    particleNode.setrenderoldestfirst(True)
    particleNode.setglobalscale(1.000000)
    particleNode.setparticlesfollownode(False)
    particleNode.seteffectactive(True)
    

#---------------------------------------------------------------------
def CreateAsset( name , textureName ):
    fileServer =  servers.get_file_server()
    assetPath = 'wc:export/assets/' + name
    transformNOH = '/lib/scene/' + name
    particleNOH =  transformNOH + '/pemiter_0_0'
    
    rootNode = pynebula.new( 'ntransformnode' , transformNOH )
    particleNode = pynebula.new( 'nparticleshapenode' , particleNOH )
    setDefualtOptions( particleNode , textureName )
    #save shapeNode
    if ( not fileServer.makepath( assetPath ) ):
        return None, "" 
    if ( not fileServer.makepath( assetPath + '/scene' ) ):
        return None , "" 
    if ( not fileServer.makepath( assetPath + '/materials' ) ):
        return None, "" 
        
    if ( not rootNode.saveas( assetPath +'/scene/' + name + '.n2' ) ):
        return None,""
        
    #pynebula.delete(rootNode.getfullname())
    #rootNode = 0
    return particleNode , assetPath 


#---------------------------------------------------------------------
def CreateClass( name, libraryPath, textureName = 'home:export/textures/materials/rauch.dds' ):
    name = name.capitalize() # valid name for a class
    entityServer = servers.get_entity_class_server()
    neBrushClass = entityServer.getentityclass( 'nebrush' )
    
    if not entityServer.checkclassname(name):
        wx.MessageBox("\"%s\" is invalid name for a class" %(name), "Conjurer", wx.ICON_ERROR)
        return None
        
    if None != entityServer.getentityclass( name ):
        wx.MessageBox("the class \"%s\" already exists" %(name), "Conjurer", wx.ICON_ERROR)
        return None

    particleNode, assetPath = CreateAsset( name , textureName )
    if particleNode == None:
        return None
    
    newClass = entityServer.newentityclass( neBrushClass, name )
    newClass.setasseteditable(True)
    newClass.setresourcefile( assetPath )
    newClass.setbbox( 0.0, 0.5 , 0.0, 0.500000, 0.5, 0.500000)
    
    library = pynebula.lookup( libraryPath )
    library.appendstring( name )
    # Mark the library as dirty
    app.get_libraries().setobjectdirty( True )
    
    return  particleNode
