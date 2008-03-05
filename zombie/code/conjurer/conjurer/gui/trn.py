##\file trn.py
##\brief Functions to get conjurer terrain objects

import pynebula


# get_terrain_module function
def get_terrain_module():
    return pynebula.lookup(
        '/app/conjurer/appstates/terrain'
        )


# get_terrain_tool_path function
def get_terrain_tool_path(tool_name):
    return '/editor/terrain/%s' % tool_name


# get_terrain_tool function
def get_terrain_tool(tool_name):
    return pynebula.lookup(
        get_terrain_tool_path(tool_name) 
        )


# get_heightmap function
def get_heightmap():
    return pynebula.lookup(
        '/usr/terrain/geomipmap/heightmap'
        )


# get_layer_manager function
def get_layer_manager():
    try:
        return pynebula.lookup(
            '/usr/terrain/geomipmap/layermanager'
            )
    except:
        return None
