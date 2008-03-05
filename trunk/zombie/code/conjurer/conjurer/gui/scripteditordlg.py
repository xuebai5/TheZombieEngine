##\file scripteditordlg.py
##\brief Script editor dialog

import codecs

import wx

import childdialoggui
import format
import nebulaguisettings as cfg
import scripttextctrl
import servers

import conjurerframework as cjr


# ScriptEditorDialog class
class ScriptEditorDialog(childdialoggui.InstanceTrackingChildDialogGUI):
    """Script editor with syntax highlight"""

    def init(self, parent, filename):
        childdialoggui.InstanceTrackingChildDialogGUI.init(
            self, "Unknown", parent
            )

        self.filename = filename
        self.stc = scripttextctrl.ScriptTextCtrl(self)
        self.button_run = wx.Button(self, -1, "&Run")

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.SetSize((500, 400))
        self.stc.set_font( cfg.Repository.get_font(cfg.ID_Font_ScriptEditor) )
        if self.filename != "":
            self.load( format.mangle_path(self.filename) )

    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.stc, 1, wx.EXPAND, 0)
        sizer.Add(self.button_run, 0, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        self.Layout()

    def __bind_events(self):
        self.Bind(wx.EVT_CLOSE, self.on_close)
        self.Bind(wx.stc.EVT_STC_MODIFIED, self.__on_script_modified, self.stc)
        self.Bind(wx.EVT_BUTTON, self.on_run, self.button_run)

    def __on_script_modified(self, event):
        self.__update_title()

    def __update_title(self):
        # Set the dialog title to match the file name, plus a modified mark
        # if file has unsaved changes
        title = self.filename
        if title == "":
            title = "Unknown"
        if self.stc.GetModify():
            title = title + "*"
        self.SetTitle(title)

    def __set_highlighting(self):
        if self.filename.endswith('.lua'):
            self.stc.set_lua_highlighting()
        elif self.filename.endswith('.py'):
            self.stc.set_python_highlighting()

    def get_filename(self):
        return self.filename

    def load(self, filename):
        self.filename = filename

        # Set the correct syntax colouring
        self.__set_highlighting()

        # Load the file
        f = codecs.open(self.filename,'r','latin-1','replace')
        self.stc.set_text( f.read() )
        f.close()

        # Set the dialog title to match the file name
        self.__update_title()

    def persist(self):
        return [
            create_window,
            ( str(self.filename), ) #   trailing comma makes it a tuple
            ]

    def refresh(self):
        self.stc.set_font( cfg.Repository.get_font(cfg.ID_Font_ScriptEditor) )

    def on_run(self, event):
        # ask for save before run if script has been modified
        if self.stc.GetModify():
            answer = cjr.confirm_yes_no(
                            self,
                            "Save changes to '%s' ?" % self.filename
                            )
            if answer == wx.ID_YES:
                self.save(True)
            elif answer == wx.ID_CANCEL:
                return

        if self.filename == "":
            cjr.show_information_message(
                "I'm quite stupid, so please you Supreme Intelligence\n" \
                "save the file so I can look at its extension and know\n" \
                "which file type this script is."
                )
        if self.filename.endswith('.lua'):
            servers.get_lua_server().runscript(
                str( format.mangle_path(self.filename) )
                )
        elif self.filename.endswith('.py'):
            servers.get_python_server().runscript(
                str( format.mangle_path(self.filename) )
                )

    def save(self, overwrite=True):
        """
        Save the dialog's script
        
        If it's a new script or the user doesn't want to overwrite the file
        a new file name is asked, otherwise the old file name is overwrited.
        
        \param overwrite False to ask for a new file name, true to overwrite it
        \return True if script has been saved, false otherwise
        """

        # It's a new script?
        if self.filename == "" or overwrite == False:
            # Ask for a file name
            file_dlg = wx.FileDialog(
                self.GetParent(), 
                message="Choose a script file",
                wildcard = "Lua script files (*.lua)|*.lua|" \
                           "Python script files (*.py)|*.py",
                style = wx.SAVE|wx.OVERWRITE_PROMPT|wx.CHANGE_DIR
                )

            # Set new file name or cancel save
            if file_dlg.ShowModal() == wx.ID_OK:
                self.filename = file_dlg.GetPath()
                self.__set_highlighting()
                file_dlg.Destroy()
            else:
                file_dlg.Destroy()
                return False

        # Save the script
        f = codecs.open(format.mangle_path(self.filename),'w','latin-1','replace')
        f.write( self.stc.GetText() )
        f.close()
        self.stc.empty_undo_buffer()
        self.__update_title()
        return True

    def on_close(self, event):
        # If there are some changes ask to save the script
        if self.stc.GetModify():
            answer = cjr.confirm_yes_no(
                            self,
                            "Save changes to '%s' ?" % self.filename
                            )
            if answer == wx.ID_YES:
                if not self.save(True):
                    return
            elif answer == wx.ID_CANCEL:
                return

        self.Destroy()

    def is_dirty(self):
        return self.stc.GetModify()

    def get_saver(self):
        return self

    def get_brief(self):
        if self.filename == "":
            name = "<Unknown>"
        else:
            name = format.get_name( format.mangle_path(self.filename) )
        return "(script) " + name

    def handle_undo(self):
        self.stc.Undo()
        return True

    def handle_redo(self):
        self.stc.Redo()
        return True

    def handle_cut(self):
        self.stc.Cut()
        return True

    def handle_copy(self):
        self.stc.Copy()
        return True

    def handle_paste(self):
        self.stc.Paste()
        return True


# create_window function
def create_window(parent, filename=""):
    return ScriptEditorDialog(parent, filename)
