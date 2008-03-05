##\file fsm.py
##\brief FSM helper functions

import pynebula

import format
import servers


# restore function
def restore():
    """Load the whole FSM library"""
    fs = servers.get_fsm_server()
    fs.loadall()
    
    # Create event conditions for all the new events handled by a FSM
    ts = servers.get_trigger_server()
    for i in range( ts.geteventtypesnumber() ):
        if ts.isafsminevent( i ):
            # This creates the event if it doesn't exist yet
            id_name = ts.geteventpersistentid( i )
#            return
            fs.geteventcondition( str(id_name) )
#            fs.geteventcondition( str("event" + str(i)) )
            # Create the event if it doesn't exist yet
##            path = format.append_to_path( get_event_conditions_lib(), "event" + str(i) )
##            try:
##                pynebula.lookup( str(path) )
##            except:
##                cond = pynebula.new( 'neventcondition', str(path) )
##                cond.setevent( i )


# get_free_name function
def get_free_name(dir_path, prefix):
    """
    Get a free name for an object to be placed in the given path and begining with the given prefix.
    
    Names are got by appending an index to the prefix.
    """
    i = 0
    while True:
        obj_path = get_path( dir_path, prefix, str(i) )
        try:
            pynebula.lookup( obj_path )
            i = i + 1
        except:
            return obj_path

# get_path function
def get_path(dir_path, prefix, index):
    """Build the same name as the get_free_name function, but providing the desired index"""
    return format.append_to_path( dir_path, prefix + index )

# get_index function
def get_index(obj_path, prefix):
    """Return the index associated to the given object path to make it unique"""
    return format.get_name(obj_path)[len(prefix):]

# find_path function
def find_path(dir_path, target_gui_name, gui_name_generator):
    """Find and return the object's path that matches the given gui name"""
    obj = pynebula.lookup( dir_path ).gethead()
    while obj != None:
        if target_gui_name == gui_name_generator( obj.getfullname() ):
            return obj.getfullname()
        obj = obj.getsucc()
    return None


# __safe_new function
def __safe_new(path, type='nroot'):
    """Create an object if it doesn't exist yet"""
    try:
        return pynebula.lookup(path)
    except:
        obj = pynebula.new(type, path)
#        obj.setsaveupsidedown(True)
        return obj

# get_root_dir function
def get_root_dir(noh = True):
    """Return the root directory of all fsm related objects"""
    if noh:
        path = "/fsm"
        __safe_new(path)
    else:
        path = servers.get_fsm_server().getstoragepath()
    return path

# get_fsms_lib function
def get_fsms_lib(noh = True):
    """Return the path where all fsms are placed"""
    path = format.append_to_path( get_root_dir(noh), "fsms" )
    if noh:
        __safe_new(path)
    return path

# get_event_conditions_lib function
def get_event_conditions_lib(noh = True):
    """Return the path where all event conditions are placed"""
    path = format.append_to_path( get_root_dir(noh), "eventconditions" )
    if noh:
        __safe_new(path)
    return path

# get_script_conditions_lib function
def get_script_conditions_lib(noh = True):
    """Return the path where all script conditions are placed"""
    path = format.append_to_path( get_root_dir(noh), "scriptconditions" )
    if noh:
        __safe_new(path)
    return path

# get_fsm_selectors_lib function
def get_fsm_selectors_lib(noh = True):
    """Return the path where all fsm selector scripts are placed"""
    path = format.append_to_path( get_root_dir(noh), "fsmselectors" )
    if noh:
        __safe_new(path)
    return path

# get_action_scripts_lib function
def get_action_scripts_lib(noh = True):
    """Return the path where all action scripts are placed"""
    path = format.append_to_path( get_root_dir(noh), "actionscripts" )
    if noh:
        __safe_new(path)
    return path

# get_temp_dir function
def get_temp_dir():
    """Return the working directory used only by the editor"""
    path = format.append_to_path( get_root_dir(), "temp")
    __safe_new(path)
    return path

# get_fsm_of_state function
def get_fsm_of_state(state_path):
    """Return the fsm path which the given state belongs to"""
    return state_path.rsplit('/',1)[0]

# get_fsm_of_transition function
def get_fsm_of_transition(transition_path):
    """Return the fsm path which the given transition belongs to"""
    return get_fsm_of_state( get_state_of_transition(transition_path) )

# get_state_of_transition function
def get_state_of_transition(transition_path):
    """Return the state path which the given transition belongs to"""
    return transition_path.rsplit('/',2)[0]

# get_state_of_transition function
def get_state_of_behaction(behaction_path):
    """Return the state path which the given behavioural action belongs to"""
    return behaction_path.rsplit('/',1)[0]

# get_transitions_of_state function
def get_transitions_dir_of_state(state_path, create_path=True):
    """Return the transitions directory path of the given state"""
    path = format.append_to_path(state_path, 'transitions')
    if create_path:
        __safe_new(path)
    return path

# get_filteredcondition_of_transition function
def get_filteredcondition_of_transition(transition_path):
    """Return the path of the transition's condition with a filter"""
    return format.append_to_path( transition_path, 'condition' )

# get_behaction_of_state function
def get_behaction_of_state(state_path):
    """Return the behavioural action path of the given state"""
    return format.append_to_path( state_path, 'behaction' )


# get_fsms function
def get_fsms():
    """Return a sequence with all the FSM paths"""
    paths = []
    fsm = pynebula.lookup( get_fsms_lib() ).gethead()
    while fsm != None:
        paths.append( fsm.getfullname() )
        fsm = fsm.getsucc()
    return paths

# get_states function
def get_states(fsm_path):
    """Return a sequence with all the states paths of the given fsm"""
    paths = []
    state = pynebula.lookup( fsm_path ).gethead()
    while state != None:
        paths.append( state.getfullname() )
        state = state.getsucc()
    return paths

# get_transitions function
def get_transitions(state_path):
    """Return a sequence with all the transition paths of the given state"""
    paths = []
    transitions_path = get_transitions_dir_of_state( state_path )
    transition = pynebula.lookup( transitions_path ).gethead()
    while transition != None:
        paths.append( transition.getfullname() )
        transition = transition.getsucc()
    return paths

# get_event_conditions function
def get_event_conditions():
    """Return a sequence with all the event conditions paths"""
    paths = []
    conds_path = get_event_conditions_lib()
    cond = pynebula.lookup( conds_path ).gethead()
    while cond != None:
        paths.append( cond.getfullname() )
        cond = cond.getsucc()
    return paths

# get_script_conditions function
def get_script_conditions():
    """Return a sequence with all the script conditions paths"""
    # Generate a singleton for each condition class
    classes = servers.get_kernel_server().getsubclasslist('nscriptcondition')
    for class_name in classes:
        servers.get_fsm_server().getscriptcondition( str(class_name) )
    
    paths = []
    conds_path = get_script_conditions_lib()
    cond = pynebula.lookup( conds_path ).gethead()
    while cond != None:
        paths.append( cond.getfullname() )
        cond = cond.getsucc()
    return paths

# get_fsm_selectors function
def get_fsm_selectors():
    """Return a list with all the available FSM selectors (the class names)"""
    classes = servers.get_kernel_server().getsubclasslist( 'nfsmselector' )
    list = []
    list.extend( classes )
    return list

# get_action_scripts function
def get_action_scripts():
    """Return a list with all the available action scripts (the class names)"""
    classes = servers.get_kernel_server().getsubclasslist( 'gpactionscript' )
    list = []
    list.extend( classes )
    return list

# get_action_classes function
def get_action_classes():
    """Return a list with all the available action classes (the class names)"""
    open_list = ['ngpbasicaction']
    close_list = []
    while len(open_list) > 0:
        clazz = open_list.pop()
        close_list.append( clazz )
        subclasses = servers.get_kernel_server().getsubclasslist( clazz )
        open_list.extend( subclasses )
    close_list.remove( 'ngpbasicaction' )
    close_list.remove( 'gpactionscript' )
    return close_list


# __copy function function
def __copy(source_dir, target_dir):
    """Copy all objects from a directory to another directory (not recursive)"""
    obj = pynebula.lookup( source_dir ).gethead()
    while obj != None:
        path = format.append_to_path( target_dir, obj.getname() )
        obj.clone( path )
        obj = obj.getsucc()

# copy_to_temp function function
def copy_to_temp(source_dir):
    """Copy all objects from a directory to the temp directory (not recursive)"""
    __copy( source_dir, get_temp_dir() )

# copy_from_temp function function
def copy_from_temp(target_dir):
    """Copy all objects from the temp directory to another directory (not recursive)"""
    __copy( get_temp_dir(), target_dir )

# clear_temp function
def clear_temp():
    """Delete all objects in the temp directory (not recursive)"""
    temp_dir = pynebula.lookup( get_temp_dir() )
    while temp_dir.gethead() != None:
        pynebula.delete( temp_dir.gethead().getfullname() )
