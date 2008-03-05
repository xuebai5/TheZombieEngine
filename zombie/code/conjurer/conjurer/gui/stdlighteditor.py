##\file stdlighteditor.py
##\brief Specific stdlight editor panel

import wx

import fogeditor
import lighteditor


# create_all_editors function
def create_all_editors(stdlight, parent):
    light_editor = lighteditor.create_all_editors(stdlight, parent)
    fog_editor = fogeditor.create_all_editors(stdlight, parent)
    return fog_editor + light_editor
