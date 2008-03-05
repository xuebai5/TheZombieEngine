## \file nebulaguisettings.py
## \brief Functionality to be able to persist application settings.

# os functionality to access to the environment variables
import os
# functionality to be able to dump in a file python info
import pickle
# some widgets related objects
import wx


# Label ids for settings
# String are used instead of numbers so a human can read the settings file more easily
ID_UndoMemory = 'undo memory'
ID_Font_ScriptEditor = 'script editor font'
ID_ScriptList = 'script list'
ID_Grid = 'grid'
ID_AngleSnapping = 'angle snapping'
ID_SizeSnapping = 'size snapping'
ID_TemporaryWorkingPath = 'temporary working path'
ID_Grimoire = 'grimore'
ID_DotPath = 'dot path'
ID_Thumbnail_TerrainMaterial = 'terrain material thumbnail'
ID_BrowserPath_File = 'file browser path'
ID_BrowserPath_Heightmap = 'heightmap browser path'
ID_BrowserPath_LocalMaterial = 'local material browser path'
ID_BrowserPath_SharedMaterial = 'shared material browser path'
ID_PresetsPath = 'presets path'
ID_SavePreset = 'save preset on exit'
ID_SettingsVersion = 'settings version'
ID_DefaultEntityExportPath = 'default entity export path'
ID_DefaultParticleSystemLibraryPath = 'default library path for particle systems'

# This structure holds the set of settings supported by this application,
# also stores the default value for a given setting and its current version.
# Each entry must be the two elements sequence [<version>, <settings data>]
settings = {
#example "key1": [4, (1,2,3)],
#example "key2": [2, ("Hola",['A','B'])]
ID_UndoMemory: [1, 5], # Memory in Mb
ID_Font_ScriptEditor: [1, {'point size': 10, 'face name': ""}],
ID_ScriptList: [1, []],
ID_Grid: [2, {'snap enabled': False, 'line spacing': 1.0, 'line grouping': 4,
    'minor color': (153,153,153), 'major color': (76,76,76)}],
ID_AngleSnapping: [1, {'enabled': False, 'angle': 5.0}],
ID_SizeSnapping: [1, {'enabled': False, 'size': 1.0}],
ID_TemporaryWorkingPath: [1, os.getenv("TEMP")],
ID_Grimoire: [1, {}],
ID_DotPath: [1, "C:/Program Files/ATT/Graphviz/bin/dot.exe"],
ID_Thumbnail_TerrainMaterial: [1, 50],
ID_BrowserPath_File: [1, ""],
ID_BrowserPath_Heightmap: [1, ""],
ID_BrowserPath_LocalMaterial: [1, ""],
ID_BrowserPath_SharedMaterial: [1, ""],
ID_PresetsPath: [1, os.getenv("HOMEDRIVE") + os.getenv("HOMEPATH") + "/conjurer/presets"],
ID_SavePreset: [2, True],
# Version of this settings structure (increment only if generic structure
# changes, not when an individual setting is added/modified).
ID_SettingsVersion: [1, "Added version info"],
ID_DefaultEntityExportPath: [1, os.getenv("HOMEDRIVE") + os.getenv("HOMEPATH")],
ID_DefaultParticleSystemLibraryPath: [1, "/editor/libraries/general"]
}


# GUISettingsRepository class
class GUISettingsRepository:
    """\brief A class to manage the settings persistency and restoration for the application.
    
    This class persists and retrieves the application settings. The settings are retrieved
    when the class is instantiated and persisted once it's free.
    
    This class retrieves the settings from a global var in the very same module where the
    class definition exists. Once the settings have been loaded from file, it compares them
    against the list of settings and removes those that are not longer used.
    """
    ##private:
    
    def __init__(self):
        """Constructor.
        
        It takes care to retrieve (if exists) the settings persisted values.
        """
        
        # list settings and default values
        self.__listsettings = settings
        
        #initializing an empty dictionary
        self.__dictionary = {}
        
        # loading previous settings
        self.__loadsettings()
        
        # updating settings
        self.__updatedictionary();
        
    def __del__(self):
        """Destructor.
        
        It takes care of persisting the last values of the settings.
        """
        # when removing the object persist data
        self.persist()
        
    # file where the settings are persisted
    def __getfilename(self):
        """Returns the name where the data will be persisted or retrieved.
        
        \return path + filename
        """

        return os.getenv("HOMEDRIVE" ) + os.getenv( "HOMEPATH" )  + "/settings.outgui"
    
    # loads the last settings
    def __loadsettings(self):
        """Takes care of retrieve the settings persisted values.
        
        \return true if successfull otherwise false.
        """
        
        filename = self.__getfilename()
        
        try:
            filehandle = file( filename, "rt" )
        except:
            return False
        
        # retrieving data
        self.__dictionary = pickle.load( filehandle )
        
        filehandle.close()
        
        return True
        
    # persist the settings
    def __savesettings(self):
        """Takes care of persist the application settings.
        
        \return true if successfull otherwise false.
        """

        filename = self.__getfilename()
        
        try:
            filehandle = file( filename, "wt" )
        except:
            return False
        
        # dumping data
        pickle.dump( self.__dictionary, filehandle )
        
        filehandle.close()
        
        return True

    # syncs the settings
    def __updatedictionary(self):
        """ This function is called after loading to 
            sync the retrieved settings. """
        # Check stored settings version, discarding stored settings if mismatch
        # with the current version
        if not self.__dictionary.has_key( ID_SettingsVersion ):
            self.__dictionary = {}
        elif self.__dictionary[ID_SettingsVersion] != self.__listsettings[ID_SettingsVersion]:
            self.__dictionary = {}
        
        #Adding new settings / replacing old ones
        for settings in self.__listsettings:
            if self.__dictionary.has_key( settings ) == False:
                # Adding a new settings key
                self.__dictionary[ settings ] = self.__listsettings[settings]
            elif self.__dictionary[settings][0] != self.__listsettings[settings][0]:
                # Replacing an old settings
                self.__dictionary[ settings ] = self.__listsettings[settings]
        
        # Removing old ones
        
        listtoberemoved = []
        
        for dkey in self.__dictionary:
            if self.__listsettings.has_key( dkey ) == False:
                listtoberemoved.append( dkey )
                
        for setting in listtoberemoved:
            del( self.__dictionary[ setting ] )
    
    ##public:
                
    # updating a setting value
    def setsettingvalue( self, settingname, value ):
        """Sets a new values for an existing settting.
        
        \param settingname setting id.
        \param value new value for the setting.
        \return true if success false otherwise.
        """
        #check key exists
        if self.__dictionary.has_key( settingname ) == False:
            return False # Failure to find the setting
        
        # setting the value in the dictionary
        self.__dictionary[ settingname ][1] = value
    
        return True #Success
    
    # retrieving a setting value
    def getsettingvalue( self, settingname ):
        """Retrieves the value(s) for an existing settting.
        
        \param settingname setting id.
        \return None if the setting doesn't exist otherwise the value of the setting.
        """
        #check key exists
        if self.__dictionary.has_key( settingname ) == False:
            return None
        
        # returning the setting value
        return self.__dictionary[ settingname ][1]
    
    def get_font(self, id):
        """
        Retrieve the font for an existing setting
        
        \param id Setting id associated to the font
        \return None if the font doesn't exist, otherwise the setting's font as a wx.Font
        """
        # retrieve the font params
        font_data = self.getsettingvalue(id)
        
        # check that font exists
        if font_data is None:
            return None
        
        # create and return the font
        return wx.Font( font_data['point size'], wx.DEFAULT, wx.NORMAL,
            wx.NORMAL, face = font_data['face name'] )
    
    def set_font(self, id, font):
        """
        Store the given font setting
        
        \param id Setting id associated to the font
        \param font Font to store, as a wx.Font
        \return True if success, false otherwise
        """
        font_data = {
            'point size': font.GetPointSize(),
            'face name': font.GetFaceName()
            }
        return self.setsettingvalue(id, font_data)
    
    def persist(self):
        self.__savesettings()
    
    
# Global var holding the settings (it should be only one instance)
Repository = GUISettingsRepository()
