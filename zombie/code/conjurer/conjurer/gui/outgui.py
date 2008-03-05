#-------------------------------------------------------------------------
# File: outgui.py
#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------
# Description: main file for the renaissance project outer GUI.
#-------------------------------------------------------------------------
# Log:
#    - 01-02-2005: David Reyes:: Created File
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Global path
#-------------------------------------------------------------------------
sys.path.insert( 0, lookup( '/sys/servers/file2' ).manglepath( 'outgui:' ) )

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
from nebulagui import createapp
import wx
import servers

#-------------------------------------------------------------------------
# Main Code
#-------------------------------------------------------------------------

# Debug class to store global references
##class VarRepository:
##    def __init__(self):
##        pass
##    def SetApplication( self, app ):
##        self.app = app
##    def GetApplication( self ):
##        return self.app
##    
##    
### Global Instance
##varRepository = VarRepository()

# Invoke this function when openning the outer GUI
def StartOutterGUI(renderwindow):
    # Creates the GUI application
    createapp(renderwindow)
##    varRepository.SetApplication(createapp(renderwindow))
    # Changes the message dispatching trigger
    setTrigger( Trigger )
##    setTrigger(varRepository.GetApplication().Trigger)

# Invoke this function when clossing the outer GUI
def EndOutterGUI(force_to_close = False):
    if wx.GetApp() is not None:
        wx.GetApp().force_to_close = force_to_close
        wx.GetApp().quit_pending = True
##    varRepository.GetApplication().force_to_close = force_to_close
##    varRepository.GetApplication().quit_pending = True
    # TODO: remember to reset the scripter engine
#    MainApplication.Close()

def Restore():
    wx.GetApp().restore_pending = True
##    varRepository.GetApplication().restore_pending = True

def Trigger():
    """Takes care of the application messaging system."""
    app = wx.GetApp()
    if app is None:
        return

    # exit without saving
    if app.force_to_close:
        app.GetTopWindow().quit_without_saving = True
        app.GetTopWindow().Close()
        app.keep_going = False

    # ask to save changes before exiting
    if app.GetTopWindow():
        if app.GetTopWindow().quit_requested:
            app.GetTopWindow().quit_requested = False
            app.quit_pending = True
        if app.quit_pending:
            app.quit_pending = False
            if app.GetTopWindow().Close():
                # This makes Python release its references to Nebula objects
                app.keep_going = False

    # restore
    if app.restore_pending:
        load_objects()
        app.frame.restore_settings()
        app.frame.perform_restore()
        app.frame.refresh_all()
        app.restore_pending = False

    # Don't ask
    try:
        if app.frame.IsActive() == True:
            wx.EventLoop.SetActive( app.evtloop )
        else:
            wx.EventLoop.SetActive( None )
    except:
        wx.EventLoop.SetActive( None )

    # at last, this is what this trigger method was built for
    while app.Pending():
        app.Dispatch()
    app.ProcessIdle()
    if app.keep_going == False:
        app = None
        wx.EventLoop.SetActive( None )
        setTrigger( DoNothing )
        wx.Exit()
        pynebula.exit()

def DoNothing():
    pass

def load_objects():  
    # Create libaries for grimoire
    if not pynebula.exists("/editor/libraries"):
        string_list = pynebula.new("nroot", "/editor/libraries")
        kernel = servers.get_kernel_server()
        kernel.loadas(
            "wc:libs/grimoire/general.n2", 
            "/editor/libraries/general"
            )

