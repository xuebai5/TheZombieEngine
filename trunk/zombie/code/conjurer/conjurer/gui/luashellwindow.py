#-------------------------------------------------------------------------
# File: luashellwindow.py
#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------
# Description: main code for a lua shell.
#-------------------------------------------------------------------------
# Log:
#    - 01-02-2005: David Reyes:: Created File
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
import wx
import wx.stc  as  stc

# Retriving commands of the python server in nebula
from pynebula import *

#import base class for dialogs
import childdialoggui

# Setup for the text editor
faces = { 
      'times': 'Times',
      'mono' : 'Courier',
      'helv' : 'Helvetica',
      'other': 'new century schoolbook',
      'size' : 12,
      'size2': 10,
     }

#-------------------------------------------------------------------------
# Main Code
#-------------------------------------------------------------------------
class LuaShell:    
    def __init__(self, parent, sizer):        
        
        # Creating a rich text editor to display lua code
        self.ExecutionLuaWindow = self.new_lua_window(parent)

        # Not allowing to write, just watch no touch ;)
        self.ExecutionLuaWindow.SetReadOnly(True)

        # Making the editor sizeable
        sizer.Add( self.ExecutionLuaWindow, 8, wx.EXPAND )
        
        # Creating a rich text editor to edit lua code
        self.NewCodeLuaWindow = self.new_lua_window(parent)
        
        # Add number in the margin for better functionality when error reporting
        self.NewCodeLuaWindow.SetMarginType(
            1, 
            stc.STC_MARGIN_NUMBER
            )
        self.NewCodeLuaWindow.SetMarginWidth(1, 25)

        # Making the editor sizeable
        sizer.Add( self.NewCodeLuaWindow, 2, wx.EXPAND )

        # Creating the execution button
        self.run_button = wx.Button( parent, -1, 'R&un' )
        
        # Making the button sizeable
        sizer.Add( self.run_button, 0, wx.EXPAND)
        
        # Updating the size of the sizer to fit the different controls
        sizer.Fit(parent)
        
        # Retrieving a reference to the lua script server
        self.luaserver = lookup( '/sys/servers/script' )
    
        # set focus to editor so user can start typing straight away
        self.NewCodeLuaWindow.SetFocus()

    def new_lua_window(self, parent):
        # Creating a rich text window for code display and/or editing
        lua_interpreter = stc.StyledTextCtrl(
                                    parent,
                                    0,
                                    wx.DefaultPosition,
                                    wx.DefaultSize,
                                    0
                                    )
        # Setting up the language to be edited/displayed
        lua_interpreter.SetLexer(stc.STC_LEX_LUA )

        # List of lua keywords
        keywords = [
            'and', 'break', 'do', 'else', 'elseif', 'false', 'for', 'function',
            'if', 'in', 'local', 'nil', 'not', 'or', 'repeat', 'return',
            'true', 'until', 'while'
            ]

        # Setting the reserved keywords list for this language
        lua_interpreter.SetKeyWords(0, " ".join(keywords))

        # Adding a feature to be albe to zoom in and out for better reading
        lua_interpreter.CmdKeyAssign(
            ord('B'), 
            stc.STC_SCMOD_CTRL, 
            stc.STC_CMD_ZOOMIN
            )
        lua_interpreter.CmdKeyAssign(
            ord('N'), 
            stc.STC_SCMOD_CTRL, 
            stc.STC_CMD_ZOOMOUT
            )

        # Setting the color for reserved words
        lua_interpreter.StyleSetSpec(
            stc.STC_P_WORD, 
            "fore:#00007F,bold,size:%(size)d" % faces
            )

        # Giving back the new generated control
        return lua_interpreter


class ChildWindow(childdialoggui.childDialogGUI):
    def __init__(self, parent):
        childdialoggui.childDialogGUI.__init__(self, "Lua Shell", parent )
        
        # Make this window visible
        self.Show(True)

        # Creating Boxer
        self.size_boxer = wx.BoxSizer(wx.VERTICAL)
        
        # Fitting  the sizer boxer
        self.size_boxer.Fit(self)
        
        # Setting this window main sizer
        self.SetSizer(self.size_boxer)
        
        # Creating the lua shell interface
        self.shell = LuaShell( self, self.size_boxer )
        
        # Final fitting
        self.Fit()
        
        # Setting action when the run button it's pressed
        self.Bind(wx.EVT_BUTTON, self.on_click_run, self.shell.run_button)

        # Setting default window size
        self.SetSize(wx.Size(600, 400))

    def on_click_run(self, evt):

        # retrieve code
        code = self.shell.NewCodeLuaWindow.GetText()
        
        # Emptiing the log buffer
        self.shell.luaserver.txtoutput()
        
        # run code
        answer = self.shell.luaserver.run( str(code) )
        
        self.shell.ExecutionLuaWindow.SetReadOnly(False)
        
        # Checking for errors: if empty no errors reported
        if answer == '':
            # Preparing code text
            formatted_code = code + '\n'
            # show code to be run
            self.shell.ExecutionLuaWindow.AppendText( formatted_code )
            # clean
            self.shell.NewCodeLuaWindow.ClearAll()
            #output message
            outputmsg = self.shell.luaserver.txtoutput()
            if outputmsg != '':
                string_list = outputmsg.split( '\n' )   
                for eachstring in string_list:
                    omsg = eachstring + '\n'
                    if omsg == '\n':
                        continue
                    self.shell.ExecutionLuaWindow.AppendText( omsg )
        else: 
            #building error message
            error_message = '# Error:' + answer + '\n'
            # show error
            self.shell.ExecutionLuaWindow.AppendText( error_message )

        # Pointing to the last message
        self.shell.ExecutionLuaWindow.GotoLine(
            self.shell.ExecutionLuaWindow.GetLineCount()-1 
            )
        # restoring state
        self.shell.ExecutionLuaWindow.SetReadOnly(True)

    def persist(self):
        return [
            create_window,
            ()  # no parameters for create function
            ]


def create_window(frame):
    # Creating lua shell window
    win = ChildWindow( frame )
    return win