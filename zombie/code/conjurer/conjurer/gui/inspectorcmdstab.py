#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------
# Description: A command executor for nobject derived objects
#-------------------------------------------------------------------------
# Log:
#    - 21-02-2005: Javier Ferrer:: Created File
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# TODO:
#    - Special controls for some types (spinners, textures selection...)
#    - Variable selection for some know types
#-------------------------------------------------------------------------
#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
import wx
import nebulagui

# Property grid
import wx.propgrid as pg

# Conjurer
import conjurerconfig as cfg

# Help system
import nebulahelpevent as nh

# Shared variables
import readshared

# selectors
import xmlinspector
import savecmdsinspector

# Global variable that comunicate the result of commands with other modules
ret = 0.0

class ClassCommandsTab(wx.Panel):
    def __init__(self, parent, object=None, entity = False):
        wx.Panel.__init__(self, parent, -1)
        self.object = object
        self.entity = entity
        nebulagui.nebula_object = self.object
        self.parent = parent
        self.executed = False
        self.inspector = "savecmds"
        self.xml = readshared.get_xml_dom()

        ## Create controls

        # Text label

        text_class = wx.StaticText(self, -1, "Class")

        # Classes combobox. We have to get the list of classes from object
        # If some class is not in the xml file use the savecmds inspector
        class_list = list()
        if object != None:
            for class_name in object.getclasses():
                class_list.append(class_name)
        class_list.append("All")

        self.classes = wx.ComboBox(
                                self, 
                                -1, 
                                "Choose class",
                                 size=wx.DefaultSize,
                                 choices=class_list,
                                 style=wx.CB_DROPDOWN
                                 )

        # Text label
        text_command = wx.StaticText(self, -1, "Command")

        # Commands combobox
        self.cmds = wx.ComboBox(
                            self, 
                            -1, 
                            "Choose command",
                            size=wx.DefaultSize,
                            style=wx.CB_DROPDOWN|wx.CB_SORT
                            )

        # Property grid.
        self.cmds_pg = pg.wxPropertyGrid(
                                self, 
                                -1, 
                                wx.DefaultPosition, 
                                wx.DefaultSize, 
                                pg.wxPG_BOLD_MODIFIED
                                |pg.wxPG_DEFAULT_STYLE
                                |pg.wxPG_SPLITTER_AUTO_CENTER
                                |pg.wxPG_FLOAT_PRECISION
                                )

        # Get button
        self.btn_get = wx.Button(self, -1, "Get")
        self.btn_get.Disable()

        # Execute button
        self.btn_exec = wx.Button(self, -1, "Execute")
        self.btn_exec.Disable()

        # Doc text
        self.doc_box = wx.StaticBox(self, -1, "Documentation")
        self.doc_text = wx.StaticText(
                                self, 
                                -1, 
                                "Documentation for classes and commands"
                                )

        # Do layout
        border_width = cfg.BORDER_WIDTH
        sizer = wx.BoxSizer(wx.VERTICAL)
        grid_sizer = wx.FlexGridSizer(2, 2, border_width, border_width * 2)
        grid_sizer.AddGrowableCol(1)
        grid_sizer.Add(
            text_class, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE
            )
        grid_sizer.Add(self.classes, 0, wx.ADJUST_MINSIZE|wx.EXPAND)
        grid_sizer.Add(
            text_command, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE
            )
        grid_sizer.Add(self.cmds, 0, wx.ADJUST_MINSIZE | wx.EXPAND)
        sizer.Add(grid_sizer, 0, wx.ALL|wx.EXPAND, border_width)
        
        sizer_property_grid = wx.BoxSizer(wx.HORIZONTAL)
        sizer_property_grid.Add(self.cmds_pg, 1, wx.EXPAND)
        sizer.Add(sizer_property_grid, 1, wx.EXPAND|wx.ALL, border_width)

        bsizer = wx.StaticBoxSizer(self.doc_box, wx.VERTICAL)
        bsizer.Add(self.doc_text, 0, wx.ALL, border_width)
        sizer.Add(bsizer, 0, wx.LEFT|wx.RIGHT|wx.EXPAND, border_width)

        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons.Add(self.btn_get, 0)
        sizer_buttons.Add(self.btn_exec, 0, wx.LEFT, border_width)
        sizer.Add(sizer_buttons, 0, wx.ALL|wx.ALIGN_RIGHT, border_width)

        self.SetSizer(sizer)

        ## Set Tooltips

        self.btn_get.SetToolTip(
            wx.ToolTip(
                "Click to update the field values with the current " \
                "command state (Only valid for set commands)"
                )
            )
        self.btn_exec.SetToolTip(
            wx.ToolTip("Click to execute the command")
            )

        ## Bind events

        self.cmds_pg.Bind(wx.EVT_KEY_DOWN, self.on_key_down)
        self.classes.Bind(wx.EVT_COMBOBOX, self.on_class_select)
        self.classes.Bind(wx.EVT_CHAR, self.on_combo_char)
        self.classes.Bind(wx.EVT_KEY_DOWN, self.on_key_down)
        self.cmds.Bind(wx.EVT_COMBOBOX, self.on_cmds_select)
        self.cmds.Bind(wx.EVT_CHAR, self.on_combo_char)
        self.Bind(wx.EVT_BUTTON, self.on_get_button, self.btn_get)
        self.Bind(wx.EVT_BUTTON, self.on_execute_button, self.btn_exec)
        self.Bind(pg.EVT_PG_CHANGED, self.on_pg, self.cmds_pg)
        self.Bind(wx.EVT_SIZE, self.OnSize)

    def refresh(self):
        pass

    def on_key_down(self, evt):
        if evt.KeyCode() == nh.KEY_HELP:
            value = self.classes.GetValue()
            if value != "Choose classes":
                value = value + "|nebula"
                self.parent.GetEventHandler().ProcessEvent(
                    nh.HelpEvent(
                        self.GetId(),
                        value,
                        self
                        )
                    )
        else:
            evt.Skip()

    # This method must be called from the restore function
    def update (self, obj):
        self.classes.Clear()

        # Classes combobox. We have to get the list of classes from object

        for class_name in obj.getclasses():
            self.classes.Append(class_name)

        self.classes.Append("All")
        self.classes.SetValue("Choose classes")

        nebulagui.nebula_object = obj
        self.object = obj
    
    def on_combo_char(self, evt):
        evt.StopPropagation()

    def on_get_button(self, evt):
        if self.inspector == 'xml':
            xmlinspector.get_button(self)
        else:
            savecmdsinspector.get_button(self)

    def on_execute_button(self, evt):
        # Send a enter key event to property for update values

        event =  wx.KeyEvent(wx.wxEVT_CHAR)
        event.m_keyCode = wx.WXK_RETURN
        self.cmds_pg.GetEventHandler().ProcessEvent(event)

        if self.inspector == "xml":
            xmlinspector.execute(self)
        else:
            savecmdsinspector.execute(self)

    def on_pg(self, evt):
        evt.Skip()

    # The user has selected a class from the combo box
    # Get all commands for the class and add them to the combobox
    def on_class_select(self, evt):
        self.inspector = "savecmds"

        # Clear GUI
        self.cmds.Clear()
        self.cmds_pg.Clear()

        # Get class information
        class_name = evt.GetString()
        savecmdsinspector.class_select(self, class_name)

    # When select a command, fill the propertygrid with the correct properties
    def on_cmds_select(self, evt):
        if self.inspector == "xml":
            xmlinspector.cmds_select( self, evt.GetString() )
        else:
            savecmdsinspector.cmds_select(self)

    def OnSize(self, evt):
        self.Layout()

    def check_class_info(self, class_name):
        class_info = self.xml.get_class_info("class", class_name)
        if class_info != None:
            return class_info
        else:
            class_info = self.xml.get_class_info("entityclass", class_name)
            if class_info != None:
                return class_info
            else:
                class_info = self.xml.get_class_info(
                                    "componentclass", 
                                    class_name
                                    )
                if class_info != None:
                    return class_info

        return None
