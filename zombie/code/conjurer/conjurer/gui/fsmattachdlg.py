##\file fsmattachdlg.py
##\brief Dialog to set the parent FSMs used by each agent type

import wx

import pynebula

import app
import format
import fsm
import servers
import togwin


# FSMAttacherDialog class
class FSMAttacherDialog(togwin.ChildToggableDialog):
    """Dialog to set the parent FSMs used by each agent type"""
    
    # List columns indices
    ID_AgentType = 0
    ID_FSM = 1
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "FSM attacher", parent
            )
        
        self.list = wx.ListCtrl(self, -1,
            style=wx.LC_REPORT|wx.LC_SINGLE_SEL|wx.LC_SORT_ASCENDING)
        self.label_fsm = wx.StaticText(self, -1, "Finite state machine:")
        self.choice_fsm = wx.Choice(self, -1)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # List columns
        self.list.InsertColumn(self.ID_AgentType, "Agent type")
        self.list.InsertColumn(self.ID_FSM, "Finite state machine")
        
        # Fill list
        self.list.DeleteAllItems()
        types = self.__get_agent_types()
        for type in types:
            entity_class = servers.get_entity_class_server().getentityclass( str(type) )
            sm = entity_class.getparentfsm()
            if sm == None:
                sm = "<none>"
            else:
                sm = sm.getname()
            self.list.Append( [type, sm] )
        
        # Update controls that depends on selected agent type
        self.__update_attach_ctrls()
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_fsm = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.list, 1, wx.EXPAND, 0)
        sizer_fsm.Add(self.label_fsm, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_fsm.Add(self.choice_fsm, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_fsm, 0, wx.TOP|wx.EXPAND, 5)
        sizer_border.Add(sizer_layout, 1, wx.ALL|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.on_select_agent, self.list)
        self.Bind(wx.EVT_CHOICE, self.on_select_fsm, self.choice_fsm)
    
    def __get_agent_types(self, clazz=None):
        types = []
        if clazz is None:
            clazz = app.get_main_entityobject()
        clazz = clazz.gethead()
        while clazz is not None:
            if clazz.hascomponent('ncFSMClass') and clazz.gethead() is None:
                types.append( clazz.getname() )
            types.extend( self.__get_agent_types(clazz) )
            clazz = clazz.getsucc()
        return types
    
    def __get_selected_list_index(self):
        return self.list.GetNextItem(-1, wx.LIST_NEXT_ALL, wx.LIST_STATE_SELECTED)
    
    def __get_selected_list_item(self, column):
        index = self.__get_selected_list_index()
        if index == -1:
            return ""
        else:
            return self.list.GetItem(index, column).GetText()
    
    def __update_attach_ctrls(self):
        # Update FSM choicer
        type = self.__get_selected_list_item( self.ID_AgentType )
        self.choice_fsm.Enable( type != "" )
        if type != "":
            # Fill choice with all available FSMs
            self.choice_fsm.Clear()
            self.choice_fsm.Append("<none>")
            fsm_paths = fsm.get_fsms()
            for fsm_path in fsm_paths:
                fsm_name = format.get_name( fsm_path )
                self.choice_fsm.Append( fsm_name )
            # Select current FSM
            entity_class = servers.get_entity_class_server().getentityclass( str(type) )
            current_fsm = entity_class.getparentfsm()
            if current_fsm == None:
                self.choice_fsm.SetStringSelection("<none>")
            else:
                self.choice_fsm.SetStringSelection( current_fsm.getname() )
    
    def on_select_agent(self, event):
        self.__update_attach_ctrls()
    
    def on_select_fsm(self, event):
        type = self.__get_selected_list_item( self.ID_AgentType )
        if type != "":
            sm_name = self.choice_fsm.GetStringSelection()
            if sm_name == "<none>":
                sm = None
            else:
                sm_path = format.append_to_path( fsm.get_fsms_lib(), sm_name )
                sm = pynebula.lookup( sm_path )
            entity_class = servers.get_entity_class_server().getentityclass( str(type) )
            entity_class.setparentfsm( sm )
            servers.get_entity_class_server().setentityclassdirty( entity_class, True )
            index = self.__get_selected_list_index()
            self.list.SetStringItem( index, self.ID_FSM, sm_name )
    
    def persist(self):
        data = {
            'column widths': (
                self.list.GetColumnWidth(0),
                self.list.GetColumnWidth(1)
                )
            }

        return [
            create_window, 
            (), # no parameters for create function
            data
            ]

    def restore(self, data_list):
        data = data_list[0]
        
        # column widths
        self.list.SetColumnWidth(0, data['column widths'][0])
        self.list.SetColumnWidth(1, data['column widths'][1])


# create_window function
def create_window(parent):
    return FSMAttacherDialog(parent)
