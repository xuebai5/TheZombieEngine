#-------------------------------------------------------------------------
# File: objbrowserwindow.py
#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------
# Description: A NOH browser with search and help engines.
# This window can be a free dialog or a child dialog. For this reason
# Two diferents classes for initialized it are provided.
#
#-------------------------------------------------------------------------
# Log:
#    - 09-02-2005: Javier Ferrer:: Created File
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
import wx

# Regular expression operations
import re

# Conjurer
import pynebula
import childdialoggui
import freedialoggui
import objdlg
import nohtree
import conjurerconfig as cfg
import conjurerframework as cjr

class BrowserPanel(wx.Panel):
    def __init__(self, parent, modal, path='/'):
        wx.Panel.__init__(self, parent, -1)

        # Some display depends of dialog type
        
        if modal == True:
            self.modal = True
        else:
            self.modal = False
            
        self.parent = parent
        self.matches = 0             # Search matches
        self.search_mode = "object"  # Search object name by default
        self.current_path = path
        
        self.tree = nohtree.NOHTree(
                        self, 
                        -1, 
                        pynebula.lookup(self.current_path), 
                        modal = modal
                        )

        # Button 1
        btn1 = wx.Button(self, -1, "Open")

        # Button 2
        btn2 = wx.Button(self, -1, "Set Cwd")
        
        # Text label "Find"
        txt = wx.StaticText(self, -1, "Find")

        # Find combobox
        self.find = wx.ComboBox(
                          self, 
                          -1, 
                          "Type your pattern",
                          size=wx.DefaultSize,
                          style=wx.CB_DROPDOWN|
                                    wx.PROCESS_ENTER|
                                    wx.CB_SORT 
                            )

        # Select radio buttons
        self.radio1 = wx.RadioButton( self, -1, "Search by object", 
                                                style = wx.RB_GROUP )
        self.radio2 = wx.RadioButton( self, -1, "Search by class")

        self.radio3 = wx.RadioButton( self, -1, "Search by path")

        # Search text
        self.matches_text = wx.StaticText(self, -1, "Matches:")

        # Text label "Class Name: nRoot"
        self.obj_class = wx.StaticText(self, -1, "Class: nRoot")

        # Set properties
        if self.modal:
            btn1.SetLabel("&OK")                   
        if self.modal:
            btn2.SetLabel("&Cancel")
        self.tree.SelectItem(self.tree.root)    # Root node by default

        # Set tooltips
        self.obj_class.SetToolTip(wx.ToolTip("Object class name"))
        self.find.SetToolTip(
            wx.ToolTip(
                "You can use almost any complex "\
                    " regular expresion here. Type the expression and press enter."
                )
            )
        if self.modal:
            btn1.SetToolTip(wx.ToolTip("Put the selected object in the "\
                                       "property field and close this dialog"))
            btn2.SetToolTip(wx.ToolTip("Exit without selecting an object"))
        else:
            btn1.SetToolTip(wx.ToolTip("Open an inspector on the selected "\
                             "object\nYou can also do this by double clicking"))
            btn2.SetToolTip(wx.ToolTip("Make the selected object the working "\
                             "object"))

        # do layout
        border_width = cfg.BORDER_WIDTH
        sizer = wx.BoxSizer (wx.VERTICAL)
        horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)        
        horizontal_sizer.Add(self.tree, 1, wx.EXPAND)
        sizer.Add(
            horizontal_sizer, 
            1, 
            wx.EXPAND|wx.ALL,
            border_width)
        horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)
        horizontal_sizer.Add(btn1, 0, wx.EXPAND)
        horizontal_sizer.Add(btn2, 0, wx.EXPAND|wx.LEFT, border_width)
        sizer.Add(
            horizontal_sizer, 
            0, 
            wx.ALIGN_RIGHT|wx.LEFT|wx.RIGHT|wx.BOTTOM,
            border_width
            )
        horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(wx.StaticLine(self, -1), 0, wx.EXPAND|wx.ALL, 1)
        horizontal_sizer.Add(txt, 0, wx.CENTER|wx.ALL, border_width)
        horizontal_sizer.Add(self.find, 1, wx.EXPAND|wx.ALL, border_width)
        sizer.Add(horizontal_sizer, 0, wx.EXPAND)
        grid1 = wx.BoxSizer(wx.HORIZONTAL)
        grid1.Add( self.radio1, 0, wx.EXPAND|wx.ALL, border_width)
        grid1.Add( self.radio2, 0, wx.EXPAND|wx.ALL, border_width)
        grid1.Add( self.radio3, 0, wx.EXPAND|wx.ALL, border_width)
        sizer.Add(grid1, 0, wx.EXPAND|wx.ALL, border_width)
        sizer.Add(wx.StaticLine(self, -1), 0, wx.EXPAND|wx.ALL, 1)
        horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)
        horizontal_sizer.Add(self.obj_class, 1, wx.EXPAND|wx.ALL, border_width)
        horizontal_sizer.Add(wx.StaticLine(self, -1, style=wx.LI_VERTICAL, 
                     size=wx.Size(1,13)), 0, wx.ALL, border_width)
        horizontal_sizer.Add(
            self.matches_text, 
            1, 
            wx.EXPAND|wx.ALL, 
            border_width
            )
        sizer.Add(horizontal_sizer, 0, wx.EXPAND)
        self.SetSizer(sizer)

        # Bind events
        self.Bind(wx.EVT_BUTTON, self.on_button1, btn1)
        self.Bind(wx.EVT_BUTTON, self.on_button2, btn2)
        self.Bind(wx.EVT_COMBOBOX, self.on_find_select, self.find)
        self.find.Bind(wx.EVT_KEY_UP, self.on_find_key)
        self.Bind(wx.EVT_RADIOBUTTON, self.on_radio_select, self.radio1)
        self.Bind(wx.EVT_RADIOBUTTON, self.on_radio_select, self.radio2)
        self.Bind(wx.EVT_RADIOBUTTON, self.on_radio_select, self.radio3)

    def __finder(self, obj, pattern, mode):
        """
        Search a pattern in the NOH tree. You can search by object names
        or object class names. This is a good way for list all objects 
        of a specific class.

        \param Object where the search begins
        \param Pattern to find
        \param Search mode: Object name or class name
        """
        
        if obj != None:                
            if obj.hascommand("getfullname"):
                try:
                    while (obj != None):
                        if mode == "object":
                            search_string = obj.getname()
                        elif mode == "class":
                            search_string = obj.getclass()
                        else:
                            search_string = obj.getfullname()
                        if re.compile(pattern, re.IGNORECASE).search(search_string):
                            self.find.Append(obj.getfullname())
                            self.matches = self.matches + 1

                        self.__finder(obj.gethead(), pattern, self.search_mode)
                        obj = obj.getsucc()
                                            
                except re.error:
                    cjr.show_information_message(
                        "Bad expresion, try again"
                        )

    def on_radio_select (self, evt):
        # Update the search mode
        radio = evt.GetEventObject()
        if radio == self.radio1:
            self.search_mode = "object"
        elif radio == self.radio2:
            self.search_mode = "class"
        else:
            self.search_mode = "path"

    def on_find_select(self, event):
        # Expand tree to selection
        select = self.find.GetStringSelection()
        if self.current_path != '/':
            self.tree.expand_tree(select.replace(self.current_path,''))
        else:
            self.tree.expand_tree(select)
        self.parent.SetTitle(select)

    def on_find_key(self, evt):
        # Execute the search user query
        if evt.KeyCode() == wx.WXK_RETURN:
            self.find.Clear()
            word = self.find.GetValue()
            if word != "":
                obj = pynebula.sel('/')
                self.__finder(obj.gethead(), word, self.search_mode)
                self.matches_text.SetLabel("Matches: %d" % self.matches)
                self.find.SetValue(word)
                self.matches = 0
            else:
                cjr.show_information_message(
                    "Please enter a search expression."
                    )
        else:
            evt.Skip()

    def on_button1(self, event):
        if self.modal == True:
            self.parent.Hide()
            self.parent.EndModal(wx.ID_OK)
        else:
            obj = pynebula.lookup(self.parent.GetTitle())
            win = objdlg.create_window(
                        self.parent.parent, 
                        obj.getfullname()
                        )
            win.display()

    def on_button2(self, event):
        if self.modal == True:
            self.parent.Hide()
            self.parent.EndModal(wx.ID_CANCEL)
        else:
            pynebula.SetCwd(self.parent.GetTitle())

class ObjBrowser(childdialoggui.childDialogGUI):
    # If the object is intanced from this class, will be a child dialog

    def __init__(self, parent, modal=False,path='/'):
        childdialoggui.childDialogGUI.__init__(self, "Object Browser", parent)

        self.browser = BrowserPanel(self, modal)

        # Settings
        self.parent = parent
        self.SetSize( (400, 400) )

    def persist(self):
        data = { 'path': self.browser.current_path }
        return [
            create_window,
            (), # no parameters for create function
            data
            ]

    def restore(self, data_list):
        data = data_list[0]
        path = data['path']
        self.browser.tree.expand_tree(path)
    
    def refresh(self):
        pass

class ObjBrowserDialog (freedialoggui.freeDialogGUI):
    def __init__(self, parent, modal, path='/'):
        # If the object is intanced from this class, will be a free dialog
        freedialoggui.freeDialogGUI.__init__(self, "Object Browser", parent)

        self.browser = BrowserPanel(self, modal, path)

        # Settings
        self.parent = parent       
        self.SetSize( (400, 400) )

def create_window(frame, modal=False, path='/'):
    return ObjBrowser( frame, modal, path)

def create_dialog(frame, modal=True, path='/'):
    return ObjBrowserDialog(frame, modal, path)
