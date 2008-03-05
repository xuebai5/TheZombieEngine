##\file servers.py
##\brief Functions to get conjurer servers

import pynebula


# get_kernel_server function
def get_kernel_server():
    return pynebula.lookup("/sys/servers/kernelserver")

# get_file_server function
def get_file_server():
    return pynebula.lookup("/sys/servers/file2")

# get_command_server function
def get_command_server():
    return pynebula.lookup("/sys/servers/undo")

# get_lua_server function
def get_lua_server():
    return pynebula.lookup("/sys/servers/script")

# get_python_server function
def get_python_server():
    return pynebula.lookup("/sys/servers/persist/npythonserver")

# get_debug_server function
def get_debug_server():
    return pynebula.lookup("/sys/servers/debug")

# get_debug_server function
def get_debug_server_modules_dir():
    return pynebula.lookup("/sys/servers/debug/modules")

# get_debug_server function
def get_loader_server():
    return pynebula.lookup("/sys/servers/loader")

# get_conjurer function
def get_conjurer():
    return pynebula.lookup("/app/conjurer")

# get_viewport_ui function
def get_viewport_ui():
    return pynebula.lookup("/app/conjurer/appviewportui")

# get_layer_manager function
def get_layer_manager():
    return pynebula.lookup("/sys/servers/layermanager")

# get_loader function
def get_loader():
    return pynebula.lookup("/app/conjurer/appstates/loader")

# TODO: Add/modify saver lookup functions here
# get_saver function
def get_saver():
    return pynebula.lookup("/app/conjurer/appstates/saver")

# get entity_object_server function
def get_entity_object_server ():
    return pynebula.lookup("/sys/servers/entityobject")

# get entity_class_server function
def get_entity_class_server ():
    return pynebula.lookup("/sys/servers/entityclass")

# get_monitor_server function
def get_monitor_server():
    return pynebula.lookup("/sys/servers/monitorserver")

# get_level_manager function
def get_level_manager():
    return pynebula.lookup("/sys/servers/levelmanager")

# get_fsm_server function
def get_fsm_server():
    return pynebula.lookup("/sys/servers/fsm")

# get_kernel_server function
def get_kernel_server():
    return pynebula.lookup("/sys/servers/kernelserver")

# get_spatial_server function
def get_spatial_server():
    return pynebula.lookup("/sys/servers/spatial")

# get_trigger_server function
def get_trigger_server():
    return pynebula.lookup("/sys/servers/trigger")

# get_sound_library function
def get_sound_library():
    return pynebula.lookup("/usr/soundlibrary")

# get_script_server function
def get_script_server():
    return pynebula.lookup("/sys/servers/scriptclass")

# get_world_interface funciton
def get_world_interface():
    return pynebula.lookup("/sys/servers/worldinterface")

# get_waypoint_server function
def get_waypoint_server():
    return pynebula.lookup( '/sys/servers/waypoint' )

# get_mission_handler function
def get_mission_handler():
    return pynebula.lookup("/sys/servers/missionhandler")

# get_music_table function
def get_music_table():
    return pynebula.lookup("/usr/musictable")

# get_music_table function
def get_global_vars():
    return pynebula.lookup("/usr/globalvars")

# get_game_materials_server function
def get_game_materials_server():
    return pynebula.lookup("/sys/servers/gamematerial")

# get_material_server function
def get_material_server():
    return pynebula.lookup("/sys/servers/material")

# get_log_server function
def get_log_server():
    return pynebula.lookup("/sys/servers/log")

# get_game_message_window_proxy function
def get_game_message_window_proxy():
    return pynebula.lookup("/sys/servers/gamemessagewindowproxy")

# get_preload_manager function
def get_preload_manager():
    return pynebula.lookup("/sys/servers/preloadmanager")

# get_input_server function
def get_input_server():
    return pynebula.lookup("/sys/servers/input")

