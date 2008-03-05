##\file menubar.py
##\brief Menu bar of the main window

import wx

import aicmds
import debugcmds
import editcmds
import filecmds
import helpcmds
import levelcmds
import scriptscmds
import toolscmds
import trncmds
import viewcmds


# MenuBar class
class MenuBar(wx.MenuBar):
    """Menu bar of the main window"""
    
    def __init__(self, frame):
        wx.MenuBar.__init__(self)
        self.menus = [
            # Add new menus here
            { 'name': "&File", 'menu': filecmds.FileMenu(frame) },
            { 'name': "&Edit", 'menu': editcmds.EditMenu(frame) },
            { 'name': "&View", 'menu': viewcmds.ViewMenu(frame) },
            { 'name': "&Level", 'menu': levelcmds.LevelMenu(frame) },
            { 'name': "&Terrain", 'menu': trncmds.TerrainMenu(frame) },
            { 'name': "A&I/Game", 'menu': aicmds.AIMenu(frame) },
            { 'name': "T&ools", 'menu': toolscmds.ToolsMenu(frame) },
            { 'name': "&Scripts", 'menu': scriptscmds.ScriptsMenu(frame) },
            { 'name': "&Debug", 'menu': debugcmds.DebugMenu(frame) },
            { 'name': "&Help", 'menu': helpcmds.HelpMenu(frame) }
        ]
    
    def create(self):
        """Create the menu, but don't attach it to the frame"""
        for menu in self.menus:
            menu['menu'].create()
            self.Append(menu['menu'], menu['name'])
    
    def refresh(self):
        """Rebuild all those dynamic menus"""
        for menu in self.menus:
            menu['menu'].refresh()
    
    def get_menu_function(self, cmd_id):
        """Find the function associated to a menu item, or None if no function
        is found"""
        for menu in self.menus:
            func = menu['menu'].get_function(cmd_id)
            if func is not None:
                return func
        return None
    
    def get_menu(self, name):
        for menu in self.menus:
            if menu['name'] == name:
                return menu['menu']
        return None
