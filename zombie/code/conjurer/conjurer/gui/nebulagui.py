## \file: nebulagui.py
## \brief Main application code for the outgui application.

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------

#import for the windows system
import wx

#import to be able to persist python objects
import pickle

# os access for the envrironment variables
import os

# Bars
import menubar
import statusbar
import toolbar

# Toggable windows manager
import togwin

# Conjurer
import pynebula
import servers
import app

# Restore functions
import filecmds
import levelcmds
import prefsdlg
import fsm
import triggerlib

# Help
import nebulahelpevent as nh

#Instantiating the settings repository
import nebulaguisettings

#Special frame for rendering nebula
import renderwindow

#To have direct conections to values/constants of nebula
import nebula

import iehelpwindow

import dialoggui
import xmlschemaparser

# module for share variables
import writeshared
import shared

# Other miscellaneous behaviour
import filedlg
import conjurerframework as cjr

#-------------------------------------------------------------------------
# Command IDs
#-------------------------------------------------------------------------
nebula_object = None

# IDs for help system
ID_Contents_Help = wx.NewId()

#-------------------------------------------------------------------------
# Main Code
#-------------------------------------------------------------------------

# AppWindowFrame class
class AppWindowFrame(wx.MDIParentFrame):
    """\brief class that represents the main application window.
    
    This is a MDI parent window, so all the other dialogs and windows,
    should be a child of this one. It also takes care of persisting the
    last windows setup.
    """

    ## private:
    def __init__(self, needs_render_window ):
        """Constructor.

        Initializes the application main window and restores the previous run.
        """
        wx.MDIParentFrame.__init__(
            self, 
            None, 
            -1, 
            "Conjurer GUI",
            size = (1024, 768)
            )
        # window icon
        file_server = servers.get_file_server()
        icon_path = file_server.manglepath("outgui:images/conjurer.ico")
        self.SetIcon( wx.Icon(icon_path, wx.BITMAP_TYPE_ICO) )

        # Let the InGUI know that the OutGUI is open
        pynebula.new('nroot', '/editor/outguiisopened')

        self.quit_without_saving = False
        self.quit_requested = False

        # create a render window if required
        if needs_render_window:
            self.render_window = renderwindow.CreateWindow(self)
            parent_hwnd_env = pynebula.new('nenv','/sys/env/hwnd')
            parent_hwnd_env.seti(self.render_window.GetPanelHandle())
        else:
            self.render_window = None

        # preparing repository
        nebulaguisettings.Repository

        # toggable window manager
        self.togwinmgr = togwin.ToggableWindowMgr(self)

        # menu bar
        self.__menubar = menubar.MenuBar(self)
        self.__menubar.create()
        self.SetMenuBar(self.__menubar)

        # status bar
        self.__statusbar = statusbar.StatusBar(self)
        self.__statusbar.create()
        self.SetStatusBar(self.__statusbar)

        # tool bar
        self.__toolbar = toolbar.ToolBar(self)
        self.__toolbar.create()
        self.SetToolBar(self.__toolbar)
        self.__toolbar.Realize()
        # events
        self.Bind(wx.EVT_CLOSE, self.on_close_window)
        self.Bind(nh.EVT_CONJURER_HELP, self.on_conjurer_help)

        # TODO: Delete when found a direct way to know the mouse position
        self.__mouse_position = wx.Point(0, 0)
        self.__toolbar.Bind(wx.EVT_LEFT_UP, self.__on_left_click)

    def get_togwinmgr(self):
        return self.togwinmgr

    def get_menubar(self):
        return self.__menubar

    def get_toolbar(self):
        return self.__toolbar

    def get_statusbar(self):
        return self.__statusbar

    # These 2 functions are used only to popup a menu when clicking on the
    # toolbar's new icon.
    # TODO: Delete when found a direct way to know the mouse position
    def __on_left_click(self, evt):
        """Store the position of the mouse when left clicking"""
        self.__mouse_position = evt.GetPosition()
        evt.Skip()

    def get_mouse_position(self):
        return self.__mouse_position

    def __check_ok_to_exit_application(self):
        return self.__check_not_in_gameplay_mode() and \
            self.__check_ai_not_active()

    def __check_not_in_gameplay_mode(self):
        if app.is_in_gameplay_mode():
            cjr.show_error_message(
                "Please exit gameplay mode before closing Conjurer."
                )
            return False
        else:
            return True

    def __check_ai_not_active(self):
        if app.get_ai_tester().isgameplayupdatingenabled():
            cjr.show_error_message(
                "Please switch off AI before closing Conjurer."
                )
            return False
        else:
            return True

    def on_close_window(self, evt):
        """Takes care to close and persist the application state."""
        result_ok = self.__check_ok_to_exit_application()
        if not result_ok:
            #need to veto the close event, otherwise ingui left inconsistent
            evt.Veto()
            return
        if self.quit_without_saving:
            # Needs to have the focus to exit gracefully
            self.SetFocus()
            try:
                pynebula.delete('/editor/outguiisopened')
            except:
                pass
            evt.Skip()
            return
        dlg = filecmds.SaveAllDialog(self, exiting=True)
        result = dlg.ShowModal()
        dlg.Destroy()
        if result == wx.ID_NO or result == wx.ID_YES:
            self.__persist()
            nebulaguisettings.Repository.persist()
            # Make the InGUI know that the OutGUI is closed
            pynebula.delete('/editor/outguiisopened')
            try:
                del shared.object
                del shared.result
            except:
                pass
            evt.Skip()
        else:
            evt.Veto()

    def on_conjurer_help(self, evt):
        query = evt.GetValue()

        query_list = query.split('|', 2)
        engine = query_list[1]

        if engine == "wiki":
            iehelpwindow.CreateWindow(self, "http://thezombieengine.sourceforge.net/"\
                                            "tiki-searchresults.php?words=",
                                      "&where=wikis&search=go",
                                      query_list[0],
                                      "http://thezombieengine.sourceforge.net/")
        elif engine == "trac":
            iehelpwindow.CreateWindow(self, "http://sourceforge.net/tracker/?group_id=219911"\
                                      "nebula2rns/search?q=",
                                      "&wiki=on&changeset=on&ticket=on", 
                                      query_list[0],
                                      "http://sourceforge.net/tracker/?group_id=219911")
        elif engine == "nebula":
            iehelpwindow.CreateWindow(self, "http://thezombieengine.wiki.sourceforge.net/"\
                                      "nightlybuild/doc/nebula2/html/class",
                                      ".html",
                                      query_list[0],
                                      "http://thezombieengine.wiki.sourceforge.net/"\
                                      "nightlybuild/doc/nebula2/"\
                                      "html/index.html")

    def load_window_layout_from_file(self, file_path):
        loader = WindowLayoutLoader(
                        self,
                        file_path)
        loader.load_windows()

    def load_window_layout(self):
        dlg = filedlg.FileDialog(
                    self, 
                    filedlg.OPEN,
                    'layout file', 
                    'Load window layout', 
                    self.__window_layout_file_location(), 
                    self.__window_layout_file_suffixes(),
                    import_button=False
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that OK to close any open child windows
            ok_to_load = False
            child_views = self.get_child_dialogs()
            if len(child_views) == 0:
                ok_to_load = True
            else:
                msg = "Conjurer will close any open child windows.\n\n" \
                            "Are you sure you want to load the '%s' layout file?" \
                            % dlg.get_guiname()
                result = cjr.warn_yes_no(
                                self, 
                                msg 
                                )
                if result == wx.ID_YES:
                    for each_child in child_views:
                        each_child.Close()
                    ok_to_load = True
            if ok_to_load:
                # Load the window layout
                self.load_window_layout_from_file( dlg.get_path() )
        dlg.Destroy()

    def __persist(self):
        self.__persist_windows()

    def __persist_windows(self):
        """Persists the windows state."""
        self.save_window_layout_to_file(
            self.__default_window_layout_file_name()
            )

    def save_window_layout_to_file(self, file_path):
        saver = WindowLayoutSaver(
                        self, 
                        file_path
                        )
        return saver.save_windows()

    def save_current_window_layout(self):
        # Ask for a name for the layout file
        dlg = filedlg.FileDialog(
                    self, 
                    filedlg.SAVE,
                    'layout file', 
                    'Save window layout', 
                    self.__window_layout_file_location(),
                    self.__window_layout_file_suffixes()
                    )
        if dlg.ShowModal() == wx.ID_OK:
            saved_ok = self.save_window_layout_to_file( dlg.get_path() )
            if saved_ok:
                #let the user know if the file saved OK
                msg = "Saved window layout as '%s'." % dlg.get_guiname()
                cjr.show_information_message(msg)
        dlg.Destroy()

    def __window_layout_file_location(self):
        return os.getenv("HOMEDRIVE" ) + os.getenv( "HOMEPATH" )

    def __window_layout_file_suffixes(self):
        return ["sav2"]

    def __default_window_layout_file_name(self):
        return "".join(
            (
                self.__window_layout_file_location(),
                "\outgui.sav"
                )
            )

    def get_child_dialogs(self):
        # return all child dialogs, excluding the render window
        all_child_windows = self.GetChildren()
        def can_be_persisted(child_window): 
            return isinstance(child_window, dialoggui.guiDialog) and \
                child_window != self.render_window
        return filter (
            can_be_persisted, 
            all_child_windows
            )

    def get_toolbar_height(self):
        return self.__toolbar.GetSize().y

    def perform_restore(self):
        """Restores the windows state."""
        self.load_window_layout_from_file(
            self.__default_window_layout_file_name()
                )

    def restore_settings(self):
        """
        Synchronize the conjurer with the settings

        Must be called after having retrieved all setting values to update
        conjurer accordingly. Also it must be called after the conjurer scene
        has been created, since some settings depend on some objects (the grid
        for instance).
        """
        prefsdlg.restore()
        levelcmds.restore()
        fsm.restore()
        triggerlib.restore()

    def refresh_scene_values(self):
        """Tell all children to refresh their scene values"""
        # refresh children windows
        for child in self.GetChildren():
            if child == self.GetToolBar():
                # Skip toolbar
                continue
            elif child == self.GetStatusBar():
                # Skip status bar
                continue
            elif isinstance(child, wx.Frame):
                if child.GetTitle() == "wxPython: stdout/stderr":
                    continue
            try:
                child.refresh()
            except:
                pass

    def refresh_all(self):
        # refresh bars
        self.__menubar.refresh()
        self.__toolbar.refresh()
        self.__statusbar.refresh()
        # refresh scene values
        self.refresh_scene_values()

    def emit_app_event(self, event):
        """Propagate an event to all children"""
        for child in self.get_child_dialogs():
            child.on_app_event(event)

    def process_keys_down(self, evt):
        """Takes care of transfer to nebula the keys down input"""
        if self.GetActiveChild() != self.render_window:
            return False
        iserver = servers.get_input_server()
        keycode = evt.GetKeyCode()
        iserver.newevent(
            nebula.INPUT_KEY_DOWN, 
            nebula.IDEV_KEYBOARD, 
            nebula.translateKeys(keycode)
            )
        return True

    def process_keys_char(self, evt):
        """Takes care of transfer to nebula the keys char input"""
        if self.GetActiveChild() != self.render_window:
            return False

        iserver = servers.get_input_server()

        keycode = evt.GetKeyCode()

        iserver.newevent(
            nebula.INPUT_KEY_CHAR, 
            nebula.IDEV_KEYBOARD, 
            keycode
            )

        return True

    def process_keys_up(self, evt):
        """Takes care of transfer to nebula the keys up input"""
        if self.GetActiveChild() != self.render_window:
            return False

        iserver = servers.get_input_server()

        keycode = evt.GetKeyCode()

        iserver.newevent(
            nebula.INPUT_KEY_UP, 
            nebula.IDEV_KEYBOARD, 
            nebula.translateKeys(keycode)
            )

        return True

    def process_mouse(self, evt):
        """Takes care of transfer to nebula mouse input"""
        if self.GetActiveChild() != self.render_window:
            return False

        iserver = servers.get_input_server()

        pos = evt.GetPosition()
        mousebuttons = evt.GetButton()

        sizewindow = self.render_window.panel.GetSize()

        if mousebuttons == 0:
            iserver.neweventmotion(
                nebula.INPUT_MOUSE_MOVE, 
                nebula.IDEV_MOUSE, 
                pos.x, 
                pos.y, 
                float(sizewindow.x), 
                float(sizewindow.y) 
                )
            return True

        # Type of event
        if evt.ButtonDown() == True:
            typeevent = nebula.INPUT_BUTTON_DOWN
        elif evt.ButtonUp() == True:
            typeevent = nebula.INPUT_BUTTON_UP
        elif evt.ButtonDClick() == True:
            if evt.ButtonUp(mousebuttons):
                typeevent = nebula.INPUT_BUTTON_UP
            else:
                typeevent = nebula.INPUT_BUTTON_DOWN
        else: 
            return True

        if mousebuttons == 1: # Left button
            button = nebula.LEFT_BUTTON
        elif mousebuttons == 2: # Middle button
            button = nebula.MIDDLE_BUTTON
        elif mousebuttons == 3: # Right button
            button = nebula.RIGHT_BUTTON
        else: 
            return True

        iserver.neweventbutton(
            typeevent, 
            nebula.IDEV_MOUSE, 
            pos.x, 
            pos.y, 
            float(sizewindow.x), 
            float(sizewindow.y), 
            button
            )

        return True


class WindowLayoutLoader:
    def __init__(self, parent_window, file_name):
        self.parent_window = parent_window
        self.file_name = file_name
        self.input_file = None

    def load_windows(self):
        try:
            self.__open_input_file()
        except:
            cjr.show_error_message(
                "Could not open input file '%s'" % self.file_name
                )
            return False
        try:
            try:
                self.__read_data_from_file()
            except: 
                cjr.show_error_message(
                    "Error reading from input file '%s'" % self.file_name
                    )
        finally:
            self.__close_input_file()

    def __open_input_file(self):
        self.input_file = file(self.file_name, "rt")

    def __close_input_file(self):
        self.input_file.close()

    def __read_data_from_file(self):
        if self.__check_header():
            self.__read_body()

    def __check_header(self):
        first_line = self.input_file.readline()
        if first_line == "#ConjurerWindowLayout\n":
            return True
        else:
            cjr.show_error_message(
                "Invalid input file format: %s" % self.file_name
                )
            return False

    def __read_body(self):
        self.__read_parent_window()
        self.__read_child_windows()

    def __read_parent_window(self):
        self.__set_parent_window(self.parent_window)

    def __read_child_windows(self):
        self.keep_going = True
        while self.keep_going:
            try:
                list_persisted = pickle.load( self.input_file )
                create_function = list_persisted[0]
                create_parameters = list_persisted[1]
                child_window = create_function(
                                        self.parent_window, 
                                        *create_parameters
                                        )
                self.__set_child_window(child_window)
                if len(list_persisted ) > 2:
                    data_list = list_persisted[2:]
                    if child_window.is_restorable(data_list):
                        child_window.restore(data_list)
                    else:
                        child_window.Destroy()
                        child_window = None
                # register open toggable windows
                if child_window != None:
                    self.parent_window.togwinmgr.set_window(
                        create_function, 
                        child_window
                        )
            except:
                self.keep_going = False

    def __set_window(self, some_window, is_child_window):
        if is_child_window:
            position_fix = wx.Point(
                                0, 
                                ( self.parent_window.get_toolbar_height() ) * 2 
                                )
        else:
            position_fix = wx.Point(0, 0)
        size = pickle.load( self.input_file )
        position = pickle.load( self.input_file )
        iconized = pickle.load( self.input_file )
        maximized = pickle.load( self.input_file )
        some_window.SetPosition( position -  position_fix )
        some_window.SetSize( size )
        if maximized:
            some_window.Maximize(True)
        if iconized:
            some_window.Iconize(True)
        some_window.SetFocus()    

    def __set_child_window(self, child_window):
        self.__set_window(
            child_window, 
            True
            )

    def __set_parent_window(self, parent_window):
        self.__set_window(
            parent_window, 
            False
            )


class WindowLayoutSaver:
    def __init__(self, parent_window, file_name):
        self.file_name = file_name
        self.parent_window = parent_window
        self.output_file = None

    def __open_output_file(self):
        self.output_file = file(self.file_name, "wt")

    def __close_output_file(self):
        self.output_file.close()

    def save_windows(self):
        try:
            self.__open_output_file()
        except:
            cjr.show_error_message(
                "Could not open output file '%s'" % self.file_name
                )
            return False
        try:
            try:
                self.__write_data_to_file()
            except: 
                cjr.show_error_message(
                    "Error writing to output file '%s'" % self.file_name
                    )
                return False
        finally:
            self.__close_output_file()

        return True

    def __write_data_to_file(self):
        self.__write_header()
        self.__write_body()

    def __write_header(self):
        self.output_file.write("#ConjurerWindowLayout\n")

    def __write_body(self):
        self.__write_parent_window()
        self.__write_child_windows()

    def __write_parent_window(self):
        self.__write_window(self.parent_window, True)

    def __get_child_windows(self):
        return self.parent_window.get_child_dialogs()

    def __write_child_windows(self):
        for each_window in self.__get_child_windows():
            self.__write_child_window(each_window)

    def __write_child_window(self, some_window):
        self.__write_window(some_window, False)

    def __write_window(self, some_window, is_parent):
        if not is_parent:
            list_of_children = some_window.persist()
            if len(list_of_children) == 0:
                return False
            else:    
                pickle.dump(list_of_children, self.output_file)
        maximized = some_window.IsMaximized()
        iconized = some_window.IsIconized()
        # a minimized window has a bad position and size
        if iconized:
            some_window.Iconize(False)
        #store the size and position
        pickle.dump(some_window.GetSize(), self.output_file)
        pickle.dump(some_window.GetPosition(), self.output_file)
        pickle.dump(iconized, self.output_file)
        pickle.dump(maximized, self.output_file)

# NebulaWXApp class
class NebulaWXApp(wx.App):
    """\brief class that represents the main application."""

    def OnInit(self):
        """Constructor."""
        return True

    def init(self, needs_render_window):
        # Sets a render window if needed
        self.should_render_window = needs_render_window
        # Create the main frame
        self.frame = AppWindowFrame(self.should_render_window)
        # Make it visible
        self.frame.Show(True)
        # Make it a top window
        self.SetTopWindow(self.frame)
        app.get_top_window.top_window = self.frame
        # Messages dispatching init state
        self.keep_going = True
        self.force_to_close = False
        self.quit_pending = False
        self.restore_pending = False
        # Creating event loop
        self.evtloop = wx.EventLoop()
        # Parse the XML data base for editor
        parse_xml()
    ## public:
        self.Bind(wx.EVT_CHAR, self.on_char_events)
        self.Bind(wx.EVT_KEY_DOWN, self.on_key_down)
        self.Bind(wx.EVT_KEY_UP, self.on_key_up)
        self.Bind(wx.EVT_MOUSE_EVENTS, self.on_mouse_events)

    def on_char_events(self, evt):
        if not self.should_render_window:
            evt.Skip()
            return

        if evt.GetKeyCode() <=0x7E and evt.GetKeyCode() >= 0x20:
            self.frame.process_keys_char(evt)
        evt.Skip()

    def on_key_down(self, evt):
        """Captures when a key has been down."""
        if not self.should_render_window:
            evt.Skip()
            return

        self.frame.process_keys_down( evt )
        evt.Skip()

    def on_key_up(self, evt):
        """Captures when a key has been up."""
        if not self.should_render_window:
            evt.Skip()
            return

        self.frame.process_keys_up( evt )
        evt.Skip()

    def on_mouse_events(self, evt):
        """Captures when a mouse event."""
        if not self.should_render_window:
            evt.Skip()
            return
        if self.frame.process_mouse( evt ) == False:
            evt.Skip()

def parse_xml ():
    file_server = servers.get_file_server()
    xml_parser = xmlschemaparser.ParseXML(
                        file_server.manglepath("home:xmlschema.xml")
                        )
    writeshared.set_xml_dom(xml_parser)

def createapp(needs_render_window):
    """Wrapper function to create the application.

    \return application reference.
    """
    application = NebulaWXApp()
    application.init(needs_render_window)
    return application
