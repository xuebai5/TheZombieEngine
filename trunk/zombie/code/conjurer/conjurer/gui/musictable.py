##\musictable.py
##\brief Music Table support objects

import os.path

import servers
import format

class MusicTable:
    def __init__(self):
        self.samples = []
        self.moods = []        
        self.styles = []
        self.__server = servers.get_music_table()
        self.__build()

    def __get_number_of_samples(self):
        return self.__server.getnumberofmusicsamples()

    def __get_number_of_moods(self):
        return self.__server.getnumberofmoods()

    def __get_number_of_styles(self):
        return self.__server.getnumberofstyles()

    def __reset(self):
        self.samples = []
        self.moods = []
        self.styles = []

    def __build(self):
        self.__reset()
        self.__build_sample_list()
        self.__build_mood_list()
        self.__build_style_list()

    def __build_sample_list(self):
        number_of_samples = self.__get_number_of_samples()
        for an_index in range(number_of_samples):
            self.__add_new_record_for_sample_at_idx(an_index)

    def __build_mood_list(self):
        number_of_moods = self.__get_number_of_moods()
        for an_index in range(number_of_moods):
            self.__add_new_record_for_mood_at_idx(an_index)
            
    def __build_style_list(self):
        number_of_styles = self.__get_number_of_styles()
        for an_index in range(number_of_styles):
            self.__add_new_record_for_style_at_idx(an_index)

    def __add_sample(self, sample):
        self.samples.append(sample)

    def __add_mood(self, mood):
        self.moods.append(mood)

    def __add_style(self, style):
        self.styles.append(style)

    def __add_new_record_for_sample_at_idx(self, an_index):
        new_record = self.__new_record_for_sample_at_idx(an_index)
        self.__add_sample(new_record)

    def __add_new_record_for_mood_at_idx(self, an_index):
        new_record = self.__new_record_for_mood_at_idx(an_index)
        self.__add_mood(new_record)

    def __add_new_record_for_style_at_idx(self, an_index):
        new_record = self.__new_record_for_style_at_idx(an_index)
        self.__add_style(new_record)

    def __new_record_for_sample_at_idx(self, an_index):
        new_record = MusicTableMusicSample(an_index)
        return new_record

    def __new_record_for_mood_at_idx(self, an_index):
        new_record = MusicTableMood(an_index)
        return new_record

    def __new_record_for_style_at_idx(self, an_index):
        new_record = MusicTableStyle(an_index)
        return new_record

    def refresh(self):
        self.__build()

    def __case_insensitive_sort(self, string_list):
        tuple_list = [(x.lower(), x) for x in string_list]
        tuple_list.sort()
        return [x[1] for x in tuple_list]

    def get_sample_names(self):
        return [each.get_name() for each in self.samples]
        
    def get_sample_names_sorted(self):
        return self.__case_insensitive_sort( 
            self.get_sample_names() 
            )

    def get_mood_names(self):
        return [each.get_name() for each in self.moods]

    def get_mood_names_sorted(self):
        return self.__case_insensitive_sort( 
            self.get_mood_names() 
            )

    def get_style_names(self):
        return [each.get_name() for each in self.styles]

    def get_style_names_sorted(self):
        return self.__case_insensitive_sort( 
            self.get_style_names() 
            )


class MusicTableMusicSample:
    def __init__(self, sequence_no=None):
        self.sequence_no = sequence_no
        self.__server = servers.get_music_table()
    
    def get_name(self):
        return self.__server.getmusicsamplename(self.sequence_no)


class MusicTableMood:
    def __init__(self, sequence_no=None):
        self.sequence_no = sequence_no
        self.__server = servers.get_music_table()
    
    def get_name(self):
        return self.__server.getmoodname(self.sequence_no)


class MusicTableStyle:
    def __init__(self, sequence_no=None):
        self.sequence_no = sequence_no
        self.__server = servers.get_music_table()
    
    def get_name(self):
        return self.__server.getstylename(self.sequence_no)

