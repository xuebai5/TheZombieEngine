#-----------------------------------------------------------------------------
# Library that contains generic functions for get informatcion about nebula
# objects. This not need to be a class, just only an storage used function
# that have been to be reused in diferent modules multiple times.
#-----------------------------------------------------------------------------

import os
import servers
import app

def get_detailed_object_description ( object, object_id ):
    """ Get a detailed descrition of an object, used for get
        the title of an editable object, for example.
    """ 
    name = ''
    if isinstance(object_id, int):
        # entity object format: [gameplay_name - ] hex_id object_class ( entityclass_path )
        name = get_entityobject_desc( object_id )
    elif object_id.startswith('/'):
        # entity class format: CLASS class_name ( entityclass_path )
        if object.isa("nentityclass"):
            name = "CLASS %s" % object.getname()
            class_path_cleared = os.path.dirname(
                                            object.getfullname().replace(
                                                '/sys/nobject/nentityobject/'
                                                ,'')
                                            )
            if class_path_cleared == '':
                class_path_cleared = 'native'
            name += " ( %s )" % class_path_cleared
        else:
            name = "%s ( %s )" % ( object.getfullname(), object.getclass() )
    else:
        entity_id = app.get_level().findentity( object_id )
        if entity_id == 0:
            name =  "%s ( %s )" % ( str(object_id), object.getclass() )
        else:
            name = get_entityobject_desc( entity_id )
        
    return name

def get_entityobject_desc ( entity_id ):    
    name = ''
    object = servers.get_entity_object_server().getentityobject(entity_id)
    if object.hascommand("getname"):
        gameplay_name = object.getname()
        if gameplay_name != '':
            name = "%s - "  % object.getname()
    name += str(hex(object.getid())) + " - " + object.getclass()
    entity_class = object.getentityclass()
    class_path_cleared = entity_class.getfullname().replace(
                                    '/sys/nobject/nentityobject/',
                                    ''
                                    )
    name += " ( %s )" % os.path.dirname(class_path_cleared)

    return name


