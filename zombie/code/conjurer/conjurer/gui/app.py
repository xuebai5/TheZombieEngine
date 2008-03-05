##\file app.py
##\brief Functions to get conjurer application/scene/level objects

import wx

import pynebula

import servers


# get_current_state function
def get_current_state():
    return servers.get_conjurer().getcurrentstate()
    
# return true if the application is currently in gameplay mode
def is_in_gameplay_mode():
    return get_current_state() == 'game'

# get_state function
def get_state(state_name):
    return pynebula.lookup("/app/conjurer/appstates/" + state_name)

# get_object_state function
def get_object_state():
    return pynebula.lookup("/app/conjurer/appstates/object")

# get_selection_mode_function
def get_selection_mode():
    return get_object_state().getselectionmode()

# return true if object mode active
def is_object_mode_active():
    return get_selection_mode() == 0

# return true if terrain cell mode active
def is_subentity_mode_active():
    return get_selection_mode() == 1

# return true if terrain cell mode active
def is_terrain_cell_mode_active():
    return get_selection_mode() == 2

# get_viewports_dir function
def get_viewports_dir():
    return pynebula.lookup("/app/conjurer/appviewportui")

# get_viewport function
def get_viewport(viewport_name):
    return pynebula.lookup("/app/conjurer/appviewportui/" + viewport_name)

# get_stdlight_path function
def get_stdlight_path():
    return "/usr/levels/default/gfxobjects/stdlight"

# get_stdlight function
def get_stdlight():
    return pynebula.lookup(get_stdlight_path())

# get_grid function
def get_grid():
    return pynebula.lookup("/editor/grid")

# get_select_object_tool function
def get_select_object_tool():
    return pynebula.lookup("/editor/object/selectionTool")

# get_translate_tool function
def get_translate_tool():
    return pynebula.lookup("/editor/object/translationTool")

# get_rotate_tool function
def get_rotate_tool():
    return pynebula.lookup("/editor/object/rotationTool")

# get_scale_tool function
def get_scale_tool():
    return pynebula.lookup("/editor/object/scaleTool")

# get_measure_tool function
def get_measure_tool():
    return pynebula.lookup("/editor/object/measureTool")
    
# get_navbuilder function
def get_navbuilder():
    return pynebula.lookup("/app/conjurer/appstates/navbuilder")

# get_placetool function
def get_place_tool():
    return pynebula.lookup("/editor/object/placeTool")

# get_multiplace_tool function
def get_multiplace_tool():
    return pynebula.lookup("/editor/object/multiplePlacerTool")

# get_libraries function
def get_libraries():
    return pynebula.lookup("/editor/libraries")

# get_levels_path function
def get_levels_path():
    return servers.get_file_server().manglepath('wc:levels/')

# get_parent_entity_object function
def get_main_entityobject ():
    return pynebula.lookup("/sys/nobject/nentityobject")

# get_level function
def get_level():
    return servers.get_level_manager().getcurrentlevelobject()

# get_outdoor function
def get_outdoor():
    obj = get_outdoor_obj()
    if obj == None:
        return None
    else:
        return obj.getentityclass()

# get_outdoor_obj function
def get_outdoor_obj():
    id = get_level().findentity('outdoor')
    if id == 0:
        return None
    else:
        return servers.get_entity_object_server().getentityobject(id)

# get_ai_tester() function
def get_ai_tester():
    return pynebula.lookup("/sys/servers/aitester")

# get_top_window function
# the parameter is just kept for backwards compatibility
def get_top_window(window=None):
    return wx.GetApp().GetTopWindow()
