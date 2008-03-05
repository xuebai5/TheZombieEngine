##\file triggerlib.py
##\brief Dialogs to manage trigger scripts

import wx

import pynebula

import format
#import fsmlibs
import servers


# restore function
def restore():
    """Load the whole trigger library"""
    pass
#    servers.get_trigger_server().loadall()

# __safe_new function
def __safe_new(path, type='nroot'):
    """Create an object if it doesn't exist yet"""
    try:
        return pynebula.lookup(path)
    except:
        obj = pynebula.new(type, path)
        return obj

# get_root_dir function
def get_root_dir(noh = True):
    """Return the root directory of all trigger related objects"""
    if noh:
        path = "/trigger"
        __safe_new(path)
    else:
        path = servers.get_trigger_server().getstoragepath()
    return path

# get_trigger_scripts_lib function
def get_trigger_scripts_lib(noh = True):
    """Return the path where all trigger scripts are placed"""
    path = format.append_to_path( get_root_dir(noh), "triggerscripts" )
    if noh:
        __safe_new(path)
    return path

# get_trigger_scripts function
def get_trigger_scripts():
    """Return a list with all the available trigger scripts (the class names)"""
    classes = servers.get_kernel_server().getsubclasslist( 'ntriggerscript' )
    list = []
    list.extend( classes )
    return list

# get_script_operations function
def get_script_operations():
    """Return a list with all the available operation scripts (the class names)"""
    classes = servers.get_kernel_server().getsubclasslist( 'nscriptoperation' )
    list = []
    list.extend( classes )
    return list


# create_trigger_scripts_lib_window function
def create_trigger_scripts_lib_window(parent):
    return TriggerScriptsLibraryDialog(parent)
