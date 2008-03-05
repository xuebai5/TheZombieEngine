##\soundlib.py
##\brief Sound Library support objects

import os.path

import servers
import format

class SoundLibrary:
    def __init__(self):
        self.records = []
        self.tree = None
        self.filter_enabled = False
        self.filter_dictionary = dict()
        self.__server = servers.get_sound_library()
        self.root_directory = "wc:export/sounds"
        self.__build_library()

    def __get_number_of_records(self):
        return self.__server.getnumberofsounds()

    def __reset(self):
        self.records = []

    def set_filter_enabled(self, boolean_value):
        self.filter_enabled = boolean_value

    def __reset_filter_dictionary(self):
        self.filter_dictionary = dict()

    def set_filter_from_list(self, seq_no_list):
        self.__reset_filter_dictionary()
        for seq_no in seq_no_list:
            self.filter_dictionary[seq_no] = True

    def get_mangled_root_directory(self):
        return format.mangle_path(self.root_directory)

    def create_new_sound_for_id_and_resource(self, sound_id, resource_string):
        resource_path = ''.join(
                                    [self.root_directory, "/", resource_string]
                                    )
        assigned_number = self.__server.adddefaultsoundwithresource(
                                    sound_id,
                                    str(resource_path)
                                    )
        if assigned_number < 0:
            return False
        self.__server.setdirty(True)
        self.refresh()
        return True

    def stop_all_sounds(self):
        self.__server.stopallsounds()
    
    def clear_mute_and_solo_on_all(self):
        self.__server.resetallmutesolo()

    def add_new_record_for_sound_at_idx(self, an_index):
        new_record = self.__new_record_for_sound_at_idx(an_index)
        self.__add_record(new_record)

    def __build_library(self):
        self.__reset()
        number_of_records = self.__get_number_of_records()
        for an_index in range(number_of_records):
            self.add_new_record_for_sound_at_idx(an_index)
        self.tree = SoundLibraryTreeBuilder(self)

    def __add_record(self, sound_record):
        self.records.append(sound_record)

    def __new_record_for_sound_at_idx(self, an_index):
        new_record = SoundLibraryRecord(an_index)
        return new_record

    def refresh(self):
        self.__build_library()

    def get_node_for_path(self, path_string):
        try:
            return self.tree.get_node_for_directory(path_string)
        except:
            return None

    def get_child_records_for_path(self, path_string):
        node = self.get_node_for_path(path_string)
        if node == None:
            return []
        else:
            return self.get_child_records_for_node(node)

    def get_child_records_for_node(self, node):
        all_records = node.get_child_records_sorted() 
        if self.filter_enabled:
            def is_in_dict(x): return self.filter_dictionary.has_key(x.sequence_no)
            return filter (is_in_dict, all_records)
        else:
            return all_records

    def get_sound_with_id(self, some_id):
        for i in range(self.__server.getnumberofsounds() ):
            if self.__server.getsoundid(i) == some_id:
                return True
        return False
        
    def do_file_scan(self, starting_directory, recursive_flag):
        create_sound_sources_flag = False
        scheduler = self.__server.getsoundscheduler()
        scheduler.scannewsounds(
            starting_directory, 
            create_sound_sources_flag,
            recursive_flag
            )
        self.refresh()

    def refresh_all_files(self):
        self.do_file_scan(self.root_directory, True)

    def save_library(self):
        self.__server.saveas("wc:export/sounds/library.n2")
        self.__server.setdirty(False)

    def has_changes_pending(self):
        return self.__server.getdirty()

    def remove_sound(self, sound_record):
        did_remove = self.__server.removesound(sound_record.sequence_no)
        if did_remove:
            self.__server.setdirty(True)
            self.refresh()
            return True
        else:
            return False

class SoundLibraryRecord:
    def __init__(self, sequence_no=None):
        self.sequence_no = sequence_no
        self.__server = servers.get_sound_library()
    
    def get_location(self):
        file_path_string = self.get_resource()
        if file_path_string:
            return os.path.dirname(file_path_string)
        else:
            return None
            
    def get_name(self):
        file_path_string = self.get_resource()
        if file_path_string:
            return os.path.basename(file_path_string)
        else:
            return None

    def __mark_library_as_dirty(self):
        self.__server.setdirty(True)

    #sound id
    def get_sound_id(self):
        return self.__server.getsoundid(self.sequence_no)
    
    def set_sound_id(self, value):
        self.__server.setsoundid(
            self.sequence_no,
            str(value)
            )
        self.__mark_library_as_dirty()

    #sound id
    def get_sound_id_lower_case(self):
        return self.get_sound_id().lower()

    # mute
    def get_mute(self):
        return self.__server.getmuteflag(self.sequence_no)
    
    def set_mute(self, value):
        self.__server.setmuteflag(
            self.sequence_no,
            value
            )
        self.__mark_library_as_dirty()

    # solo
    def get_solo(self):
        return self.__server.getsoloflag(self.sequence_no)
    
    def set_solo(self, value):
        self.__server.setsoloflag(
            self.sequence_no,
            value
            )
        self.__mark_library_as_dirty()

    # resource
    def get_resource(self):
        return self.__server.getresource(self.sequence_no)
    
    def set_resource(self, value):
        self.__server.setresource(
            self.sequence_no, 
            str(value)
            )
        self.__mark_library_as_dirty()

    # ambient
    def get_ambient(self):
        return self.__server.get2dflag(self.sequence_no)
    
    def set_ambient(self, value):
        self.__server.set2dflag(
            self.sequence_no,
            value
            )
        self.__mark_library_as_dirty()

    # volume
    def get_volume(self):
        return self.__server.getvolume(self.sequence_no)

    def set_volume(self, value):
        self.__server.setvolume(
            self.sequence_no,
            value
            )
        self.__mark_library_as_dirty()

    # min
    def get_min_dist(self):
        return self.__server.getmindist(self.sequence_no)
    
    def set_min_dist(self, value):
        self.__server.setmindist(
            self.sequence_no,
            value
            )
        self.__mark_library_as_dirty()

    # max dist
    def get_max_dist(self):
        return self.__server.getmaxdist(self.sequence_no)
    
    def set_max_dist(self, value):
        self.__server.setmaxdist(
            self.sequence_no,
            value
            )
        self.__mark_library_as_dirty()

    # pitch var
    def get_pitch_var(self):
        return self.__server.getpitchvar(self.sequence_no)
    
    def set_pitch_var(self, value):
        self.__server.setpitchvar(
            self.sequence_no,
            value
            )
        self.__mark_library_as_dirty()

    # pitch flag
    def get_pitch_control(self):
        return self.__server.getpitchcontrol(self.sequence_no)
    
    def set_pitch_control(self, value):
        self.__server.setpitchcontrol(
            self.sequence_no,
            value
            )
        self.__mark_library_as_dirty()

    # delay
    def get_delay(self):
        return self.__server.getdelay(self.sequence_no)
    
    def set_delay(self, value):
        self.__server.setdelay(
            self.sequence_no,
            value
            )
        self.__mark_library_as_dirty()

    # volume
    def get_volume_var(self):
        return self.__server.getvolumevar(self.sequence_no)
    
    def set_volume_var(self, value):
        self.__server.setvolumevar(
            self.sequence_no,
            value
            )
        self.__mark_library_as_dirty()

    # mobile
    def get_mobile(self):
        return self.__server.getmobile(self.sequence_no)
        
    def set_mobile(self, value):
        self.__server.setmobile(
            self.sequence_no,
            value
            )
        self.__mark_library_as_dirty()

    # max instances
    def get_max_instances(self):
        return self.__server.getmaxinstances(self.sequence_no)
    
    def set_max_instances(self, value):
        self.__server.setmaxinstances(
            self.sequence_no, 
            value
            )
        self.__mark_library_as_dirty()

    # streamed
    def get_streamed(self):
        return self.__server.getstreamed(self.sequence_no)
    
    def set_streamed(self, value):
        self.__server.setstreamed(
            self.sequence_no, 
            value
            )
        self.__mark_library_as_dirty()
    
    # priority 
    def get_priority(self):
        return self.__server.getpriority(self.sequence_no)
        
    def set_priority(self, value):
        self.__server.setpriority(
            self.sequence_no, 
            value
            )
        self.__mark_library_as_dirty()

    def play(self):
        self.__server.play(self.sequence_no)


class SoundLibraryNodeTreeItem:
    def __init__(self, directory = None):
        self.child_nodes = []
        self.child_records = []
        self.directory = directory

    def reset(self):
        self.child_nodes = []
        self.child_records = []
        
    def name(self):
        return os.path.basename(self.directory)

    def get_directory_name_lower_case(self):
        return self.directory.lower()
    
    def has_child_nodes(self):
        return len(self.child_nodes)

    def add_record(self, record):
        self.child_records.append(record)

    def add_node(self, node):
        self.child_nodes.append(node)

    def get_node_for_directory(self, directory_string):
        if (self.directory).lower() == directory_string.lower():
            return self
        for each in self.child_nodes:
            match = each.get_node_for_directory(directory_string)
            if match:
                return match
        return None

    def get_or_create_child_node_for_directory(self, directory_string):
        for each in self.child_nodes:
            if (each.directory).lower() == (directory_string).lower():
                return each
        new_node = SoundLibraryNodeTreeItem(directory_string)
        self.add_node(new_node)
        return new_node

    def get_child_records_sorted(self):
        return sorted(
            self.child_records, 
            lambda x, y: cmp(
                                x.get_sound_id_lower_case(), 
                                y.get_sound_id_lower_case()
                                )
            )

    def get_child_nodes_sorted(self):
        return sorted(
            self.child_nodes, 
            lambda x, y: cmp(
                                x.get_directory_name_lower_case(), 
                                y.get_directory_name_lower_case()
                                )
            )

class SoundLibraryTreeBuilder:
    def __init__(self, sound_library):
        self.sound_library = sound_library
        self.root_tree_item = None
        self.is_for_flat_list = False
        self.root_directory = sound_library.root_directory
        self.node_dictionary = dict()
        self.create_root()
        self.__populate_all()

    def __get_sound_record_list(self):
        return self.sound_library.records

    def get_node_for_record(self, record):
        location_string = record.get_location()
        node = self.root_tree_item
        for each_element in self.__get_path_as_list(location_string):
            node = node.get_or_create_child_node_for_directory(each_element)
        return node

    def get_node_for_directory(self, path_string):
        return self.root_tree_item.get_node_for_directory(path_string)

    def __populate_all(self):
        for each_record in self.__get_sound_record_list():
            owning_node = self.get_node_for_record(each_record)
            owning_node.add_record(each_record)

    def create_root(self):
        self.root_tree_item = SoundLibraryNodeTreeItem(self.root_directory)
        
    def __is_valid_sound_file_path(self, path_string):
        absolute_root_path = self.__get_root_directory_abs()
        absolute_path_string = os.path.abspath(path_string)
        return os.path.commonprefix(
            [absolute_root_path, 
            absolute_path_string]
            ) == absolute_root_path

    def __get_root_directory_abs(self):
        return os.path.abspath(self.root_directory)

    def __get_path_as_list(self, path_string):
        #if no path supplied, or path not valid, use root
        if not self.__is_valid_sound_file_path(path_string):
            path_string = self.root_directory
        path_list = []
        parent = path_string
        while parent != self.root_directory:
            path_list.append(parent)
            parent = os.path.dirname(parent)
        return reversed(path_list)

    def refresh(self):
        self.__populate_all()