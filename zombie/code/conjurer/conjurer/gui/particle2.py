##\file particle2.py
##\brief Function to create a new particle system class

import pynebula
import servers
import app

import conjurerframework as cjr

#---------------------------------------------------------------------
def set_default_options(particle_node, texture_name):
    particle_node.settexture('diffMap', texture_name)
    particle_node.setint('funcSrcBlend', 5)
    particle_node.setint('funcDestBlend', 6)
    particle_node.setdependency(
        'wc:export/assets/aDefaultParticle2/materials/Surface.n2', 
        '/sys/servers/dependency/nsurfacedependencyserver', 
        'setsurface'
        )
    particle_node.setmesh(
        'wc:export/assets/aDefaultParticle2/meshes/particle.n3d2'
        )
    particle_node.setgroupindex(0)
    particle_node.setmeshusage(
        'ReadOnly|PointSprite|NeedsVertexShader|'
        )
    particle_node.setemissionfrequency(
        50.000000, 50.000000, 50.000000, 50.000000, 0.200000, 0.800000, 10.000000, 5.000000, 0
        )
    particle_node.setparticlelifetime(
        7.000000, 7.000000, 7.000000, 7.000000, 0.200000, 0.800000, 10.000000, 0.500000, 0
        )
    particle_node.setparticlespreadmin(
        0.000000, 0.000000, 0.000000, 0.000000, 0.200000, 0.800000, 0.000000, 0.000000, 0
        )
    particle_node.setparticlespreadmax(
        20.000000, 20.000000, 20.000000, 20.000000, 0.200000, 0.800000, 0.000000, 0.000000, 0
        ) 
    particle_node.setparticlestartvelocity(
        1.250000, 1.250000, 1.250000, 1.250000, 0.200000, 0.800000, 0.000000, 0.000000, 0
        )
    particle_node.setparticlerotationvelocity(
        10.000000, 10.000000, 10.000000, 10.000000, 0.200000, 0.800000, 0.000000, 0.000000, 0
        )
    particle_node.setparticlesize(
        0.000000, 0.700000, 0.960000, 1.000000, 0.200000, 0.800000, 1.000000, 0.100000, 0
        )
    particle_node.setparticlealpha(
        1.000000, 1.000000, 1.000000, 1.000000, 0.200000, 0.800000, 0.000000, 0.000000, 0
        )
    particle_node.settimemanipulator(
        1.000000, 1.000000, 1.000000, 1.000000, 0.200000, 0.800000, 0.000000, 0.000000, 0
        )
    particle_node.setparticlevelocityfactor(
        1.000000, 1.000000, 1.000000, 1.000000, 0.200000, 0.800000, 1.000000, 0.000000, 0
        ) 
    particle_node.setparticleairresistance(
        0.000000, 0.000000, 0.000000, 0.000000, 0.200000, 0.800000, 0.000000, 0.000000, 0
        )
    particle_node.setparticlergb(
        1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 0.200000, 0.800000
        )
    particle_node.setemissionduration(10.000000)
    particle_node.setloop(True)
    particle_node.setactivitydistance(50.000000)
    particle_node.setrenderoldestfirst(False)
    particle_node.setstartrotationmin(0.000000)
    particle_node.setstartrotationmax(0.000000)
    particle_node.setgravity(-9.810000)
    particle_node.setparticlestretch(0.000000)
    particle_node.settiletexture(0)
    particle_node.setstretchtostart(False) 
    particle_node.setparticlevelocityrandomize(0.000000)
    particle_node.setparticlerotationrandomize(0.000000)
    particle_node.setparticlesizerandomize(0.000000)
    particle_node.setprecalctime(0.000000)
    particle_node.setrandomrotdir(True)
    particle_node.setstretchdetail(0)
    particle_node.setviewanglefade(False)
    particle_node.setstartdelay(0.000000) 


def add_emitter(
    particle_system, 
    emitter_name, 
    texture_name = 'home:export/textures/materials/rauch.dds'
    ):
    particle_system_name = particle_system.getname()
    root_node = particle_system.getrootnode()
    # check there isn't already one with the same name
    for each_child in root_node.getchildren():
        if each_child.getname() == emitter_name:
            cjr.show_error_message(
                "There is already an emitter "\
                "called '%s'." % emitter_name
                )
            return False
    particle_NOH =  root_node.getfullname() +  '/' + emitter_name
    # force particle instances to release resources before 
    # adding new emiiter
    particle_system.refresh()
    particle_node = pynebula.new(
                            'nparticleshapenode2', 
                            particle_NOH 
                            )

    if particle_node is None:
        return False
    else:
        set_default_options(
            particle_node, 
            texture_name 
            )
        particle_node.binddirtydependence(root_node)
        particle_node.setobjectdirty(True)
        return True


def remove_emitter(particle_system, emitter):
    # force particle instances to release resources before 
    # adding new emiiter
    emitter_name = emitter.getname()
    particle_system.refresh()
    pynebula.delete(emitter)
    # check if the emitter has been deleted ok by 
    # looking for it (delete doesn't return success 
    # or failure)
    root_node = particle_system.getrootnode()
    match = False
    for each_child in root_node.getchildren():
        if each_child.getname() == emitter_name:
            match = True
            break
    if match:
        # not deleted
        return False
    else:
        particle_system.setobjectdirty(True)
        return True


def create_class(
    name, 
    parent_name, 
    library_path
    ):

    name = name.capitalize() # valid name for a class
    entity_server = servers.get_entity_class_server()

    if not entity_server.checkclassname( name ):
        cjr.show_error_message(
            "'%s' is not a valid name for a class" % (name)
            )
        return False

    if entity_server.getentityclass( name ) is not None:
        cjr.show_error_message(
            "A class called '%s' already exists" % (name)
            )
        return False

    #Get the class for the given parent class name, checking that it is valid
    parent_class = entity_server.getentityclass(parent_name)
    if parent_class is None:
        cjr.show_error_message(
            "The new class cannot derive from '%s' "\
                "because it does not exist" % (parent_name)
            )
        return False

    new_class = entity_server.newentityclass( parent_class, name )
    new_class.setclasskeyint( "ParticleSystem", 1 )
    new_class.setasseteditable( True )
    new_class.setbbox( 0.0, 0.5, 0.0, 0.5, 0.5, 0.5 )
    
    asset_path = 'wc:export/assets/%s' % name
    file_server = servers.get_file_server()
    if ( not file_server.makepath( asset_path ) ):
        return False
    new_class.setresourcefile( asset_path )
   
    scene_path = asset_path + '/scene'
    if ( not file_server.makepath( scene_path ) ):
        return False 
    materials_path = asset_path + '/materials' 
    if ( not file_server.makepath( materials_path ) ):
        return False
    
    # create the root node
    root_node_path = scene_path + '/' + name
    root_node = pynebula.new( 'ntransformnode', root_node_path )
    result = root_node.saveas( root_node_path + '.n2' ) 
    if not result:
        return False

    library = pynebula.lookup( library_path )
    library.appendstring( name )

    # Mark the library as dirty
    app.get_libraries().setobjectdirty( True )
    
    return True

