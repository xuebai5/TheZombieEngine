#-------------------------------------------------------------------------
# File: conjurerframework.py
#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2006)
#-------------------------------------------------------------------------
# Description: framework classes for Conjurer editor
#-------------------------------------------------------------------------
# Log:
#    - 12-06-2006: Dominic Ashby:: Created File
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
import wx

MESSAGE_TITLE = "Conjurer"

def show_error_message(error_string):
    wx.MessageBox(
        error_string, 
        MESSAGE_TITLE, 
        wx.OK|wx.ICON_ERROR
        )
        
def show_information_message(information_string):
    wx.MessageBox(
        information_string, 
        MESSAGE_TITLE, 
        wx.OK|wx.ICON_INFORMATION
        )

def show_warning_message(warning_string):
    wx.MessageBox(
        warning_string, 
        MESSAGE_TITLE, 
        wx.OK|wx.ICON_WARNING
        )

def confirm_yes_no(parent, confirmation_string):
    return confirm_yes_no_at_position(
        parent,
        confirmation_string, 
        None
        )

def confirm_yes_no_at_position(parent, confirmation_string, position):
    dlg = wx.MessageDialog(
                parent,
                confirmation_string, 
                MESSAGE_TITLE, 
                wx.YES_NO|wx.ICON_QUESTION
                )
    if position != None:
        dlg.SetPosition(position)
    result = dlg.ShowModal()
    dlg.Destroy()
    return result

def warn_yes_no(parent, warning_string):
    dlg = wx.MessageDialog(
                parent,
                warning_string, 
                MESSAGE_TITLE, 
                wx.YES_NO | wx.ICON_EXCLAMATION
                )
    result = dlg.ShowModal()
    dlg.Destroy()
    return result