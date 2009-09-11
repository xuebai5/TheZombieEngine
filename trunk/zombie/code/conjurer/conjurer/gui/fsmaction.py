##\file fsmaction.py
##\brief Control to edit an AI agent action

import wx

import pynebula

import events
import floatctrl
import fsm
import fsmevents
import propgrid
import scriptingmodelsingleobject

import conjurerframework as cjr


# get_action_gui_name function
def get_action_gui_name( action_path ):
    action = pynebula.lookup( action_path )
    return action.getactionclass()
##    type = action.getactiontype()
##    names = {
##        0: "Move to point",
##        8: "Search target",
##        9: "Forget target",
##        10: "Shoot",
##        11: "Melee",
##        12: "Ironsight"
##        }
##    if names.has_key( type ):
##        return names[type]
##    elif type == 999:
##        action.rewindargs()
##        return action.getstringarg()
##    else:
##        return action.getname()


# create_default_action function
def create_default_action( path ):
    action = pynebula.new( 'nactiondesc', path )
    action_classes = fsm.get_action_classes()
    if len(action_classes) == 0:
        return None
    action.setactionclass( str(action_classes[0]) )
##    action.setintarg(1)
##    action.setactiontype(8)
##    action.appendintarg(1)
    return action


# ActionPanel class
class ActionPanel(wx.Panel):
    """Base panel for all actions"""
    
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)
        self.action_path = None
        # Remember to set self.action_id in inherited classes
    
    def get_action(self):
        return pynebula.lookup( self.action_path )
    
    def get_action_id(self):
        return self.action_id
    
    # Bind to this method the events that are thrown when some action's param changes
    def on_change_param(self, event):
        self.update_action( self.get_action() )
    
    # Overwrite if the action has arguments (to update controls)
    def set_default_values(self):
        """Set the default values for the given action, returning false if unable"""
        return True
    
    # Overwrite if the action has arguments (to set action arguments)
    def update_action(self, action):
        """Update the arguments of the given action"""
        self.action_path = action.getfullname()
    
    # Overwrite if the action has arguments (to update controls)
    def set_action(self, action_path):
        self.action_path = action_path
    
    def quiet_refresh(self):
        pass


# MovePanel class
class MovePanel(ActionPanel):
    """Panel with the controls specifics for a move action"""
    
    def __init__(self, parent):
        ActionPanel.__init__(self, parent)
        self.action_id = 0
        
        self.label_x = wx.StaticText( self, -1, "X:" )
        self.float_x = floatctrl.FloatCtrl( self, -1, value=0.0 )
        self.label_y = wx.StaticText( self, -1, "Y:" )
        self.float_y = floatctrl.FloatCtrl( self, -1, value=0.0 )
        self.label_z = wx.StaticText( self, -1, "Z:" )
        self.float_z = floatctrl.FloatCtrl( self, -1, value=0.0 )
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer_coord = wx.BoxSizer(wx.VERTICAL)
        sizer_x = wx.BoxSizer(wx.HORIZONTAL)
        sizer_y = wx.BoxSizer(wx.HORIZONTAL)
        sizer_z = wx.BoxSizer(wx.HORIZONTAL)
        sizer_x.Add(self.label_x, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_x.Add(self.float_x, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_coord.Add(sizer_x, 0, wx.EXPAND, 0)
        sizer_y.Add(self.label_y, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_y.Add(self.float_y, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_coord.Add(sizer_y, 0, wx.TOP|wx.EXPAND, 5)
        sizer_z.Add(self.label_z, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_z.Add(self.float_z, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_coord.Add(sizer_z, 0, wx.TOP|wx.EXPAND, 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_coord)
        sizer_coord.Fit(self)
        sizer_coord.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(floatctrl.EVT_FLOAT, self.on_change_param, self.float_x)
        self.Bind(floatctrl.EVT_FLOAT, self.on_change_param, self.float_y)
        self.Bind(floatctrl.EVT_FLOAT, self.on_change_param, self.float_z)
    
    def set_default_values(self):
        self.float_x.set_value( 0.0, False )
        self.float_y.set_value( 0.0, False )
        self.float_z.set_value( 0.0, False )
        return True
    
    def update_action(self, action):
        ActionPanel.update_action(self, action)
        action.clearargs()
        action.appendvector3arg(
            self.float_x.get_value(),
            self.float_y.get_value(),
            self.float_z.get_value()
            )
    
    def set_action(self, action_path):
        ActionPanel.set_action(self, action_path)
        if action_path != None:
            action = self.get_action()
            action.rewindargs()
            x, y, z = action.getvector3arg()
            self.float_x.set_value( x, False )
            self.float_y.set_value( y, False )
            self.float_z.set_value( z, False )


# SearchTargetPanel class
class SearchTargetPanel(ActionPanel):
    """Panel with the controls specifics for a search target action"""
    
    Closest = 1<<0
    Farthest = 1<<1
    InRange = 1<<2
    MostDangerous = 1<<3
    LessDangerous = 1<<4
    Enemy = 1<<5
    Ally = 1<<6
    
    def __init__(self, parent):
        ActionPanel.__init__(self, parent)
        self.action_id = 8
        
        self.checkbox_closest = wx.CheckBox(self, -1, "Closest")
        self.checkbox_farthest = wx.CheckBox(self, -1, "Farthest")
        self.checkbox_in_range = wx.CheckBox(self, -1, "In range")
        self.checkbox_most_dangerous = wx.CheckBox(self, -1, "Most dangerous")
        self.checkbox_less_dangerous = wx.CheckBox(self, -1, "Less dangerous")
        self.checkbox_enemy = wx.CheckBox(self, -1, "Enemy")
        self.checkbox_ally = wx.CheckBox(self, -1, "Ally")
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer_criteria = wx.BoxSizer(wx.VERTICAL)
        sizer_criteria.Add(self.checkbox_closest, 0, wx.FIXED_MINSIZE, 0)
        sizer_criteria.Add(self.checkbox_farthest, 0, wx.TOP|wx.FIXED_MINSIZE, 5)
        sizer_criteria.Add(self.checkbox_in_range, 0, wx.TOP|wx.FIXED_MINSIZE, 5)
        sizer_criteria.Add(self.checkbox_most_dangerous, 0, wx.TOP|wx.FIXED_MINSIZE, 5)
        sizer_criteria.Add(self.checkbox_less_dangerous, 0, wx.TOP|wx.FIXED_MINSIZE, 5)
        sizer_criteria.Add(self.checkbox_enemy, 0, wx.TOP|wx.FIXED_MINSIZE, 5)
        sizer_criteria.Add(self.checkbox_ally, 0, wx.TOP|wx.FIXED_MINSIZE, 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_criteria)
        sizer_criteria.Fit(self)
        sizer_criteria.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_CHECKBOX, self.on_change_param, self.checkbox_closest)
        self.Bind(wx.EVT_CHECKBOX, self.on_change_param, self.checkbox_farthest)
        self.Bind(wx.EVT_CHECKBOX, self.on_change_param, self.checkbox_in_range)
        self.Bind(wx.EVT_CHECKBOX, self.on_change_param, self.checkbox_most_dangerous)
        self.Bind(wx.EVT_CHECKBOX, self.on_change_param, self.checkbox_less_dangerous)
        self.Bind(wx.EVT_CHECKBOX, self.on_change_param, self.checkbox_enemy)
        self.Bind(wx.EVT_CHECKBOX, self.on_change_param, self.checkbox_ally)
    
    def __flags_to_checks(self, flags):
        self.checkbox_closest.SetValue( flags & self.Closest )
        self.checkbox_farthest.SetValue( flags & self.Farthest )
        self.checkbox_in_range.SetValue( flags & self.InRange )
        self.checkbox_most_dangerous.SetValue( flags & self.MostDangerous )
        self.checkbox_less_dangerous.SetValue( flags & self.LessDangerous )
        self.checkbox_enemy.SetValue( flags & self.Enemy )
        self.checkbox_ally.SetValue( flags & self.Ally )
    
    def __checks_to_flags(self):
        return self.checkbox_closest.GetValue() * self.Closest + \
            self.checkbox_farthest.GetValue() * self.Farthest + \
            self.checkbox_in_range.GetValue() * self.InRange + \
            self.checkbox_most_dangerous.GetValue() * self.MostDangerous + \
            self.checkbox_less_dangerous.GetValue() * self.LessDangerous + \
            self.checkbox_enemy.GetValue() * self.Enemy + \
            self.checkbox_ally.GetValue() * self.Ally
    
    def set_default_values(self):
        self.__flags_to_checks(0)
        return True
    
    def update_action(self, action):
        ActionPanel.update_action(self, action)
        action.clearargs()
        action.appendintarg( self.__checks_to_flags() )
    
    def set_action(self, action_path):
        ActionPanel.set_action(self, action_path)
        if action_path != None:
            action = self.get_action()
            action.rewindargs()
            flags = action.getintarg()
            self.__flags_to_checks( flags )


# ForgetTargetPanel class
class ForgetTargetPanel(ActionPanel):
    """Panel with the controls specifics for a forget target action"""
    
    def __init__(self, parent):
        ActionPanel.__init__(self, parent)
        self.action_id = 9


# ShootPanel class
class ShootPanel(ActionPanel):
    """Panel with the controls specifics for a shoot action"""
    
    def __init__(self, parent):
        ActionPanel.__init__(self, parent)
        self.action_id = 10


# MeleePanel class
class MeleePanel(ActionPanel):
    """Panel with the controls specifics for a melee action"""
    
    def __init__(self, parent):
        ActionPanel.__init__(self, parent)
        self.action_id = 11


# IronsightPanel class
class IronsightPanel(ActionPanel):
    """Panel with the controls specifics for an ironsight action"""
    
    def __init__(self, parent):
        ActionPanel.__init__(self, parent)
        self.action_id = 12
        
        self.checkbox_ironsight = wx.CheckBox(self, -1, "To ironsight style")
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer_criteria = wx.BoxSizer(wx.VERTICAL)
        sizer_criteria.Add(self.checkbox_ironsight, 0, wx.FIXED_MINSIZE, 0)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_criteria)
        sizer_criteria.Fit(self)
        sizer_criteria.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_CHECKBOX, self.on_change_param, self.checkbox_ironsight)
    
    def set_default_values(self):
        self.checkbox_ironsight.SetValue( False )
        return True
    
    def update_action(self, action):
        ActionPanel.update_action(self, action)
        action.clearargs()
        action.appendboolarg( self.checkbox_ironsight.GetValue() )
    
    def set_action(self, action_path):
        ActionPanel.set_action(self, action_path)
        if action_path != None:
            action = self.get_action()
            action.rewindargs()
            self.checkbox_ironsight.SetValue( action.getboolarg() )


# ScriptPanel class
class ScriptPanel(ActionPanel):
    """Panel with the controls specifics for a script action"""
    
    def __init__(self, parent):
        ActionPanel.__init__(self, parent)
        self.action_id = 999
        
        self.label_script = wx.StaticText(self, -1, "Script:")
        self.choice_script = wx.Choice(self, -1, choices=[])
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_script = wx.BoxSizer(wx.HORIZONTAL)
        sizer_script.Add(self.label_script, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_script.Add(self.choice_script, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_script, 0, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_CHOICE, self.on_change_param, self.choice_script)
    
    def set_default_values(self):
        # get available action scripts, vetoing the change if there isn't any
        script_list = fsm.get_action_scripts()
        if len( script_list ) == 0:
            cjr.show_error_message(
                "There isn't any action script available.\n" \
                "Couldn't set a default action script."
                )
            return False
        
        # fill action scripts list and select the first one as the default one
        self.choice_script.Clear()
        script_list.sort()
        self.choice_script.AppendItems( script_list )
        self.choice_script.SetSelection(0)
        return True
    
    def update_action(self, action):
        ActionPanel.update_action(self, action)
        action.clearargs()
        class_name = str( self.choice_script.GetStringSelection() )
        action.appendstringarg( class_name )
        fsmevents.signal_fsm_change( self )
    
    def set_action(self, action_path):
        ActionPanel.set_action(self, action_path)
        if action_path != None:
            # fill action scripts list
            self.choice_script.Clear()
            script_list = fsm.get_action_scripts()
            script_list.sort()
            self.choice_script.AppendItems( script_list )
            
            # select current action script
            action = self.get_action()
            action.rewindargs()
            script_name = action.getstringarg()
            self.choice_script.SetStringSelection( script_name )
    
    def quiet_refresh(self):
        self.set_action( self.action_path )


# ActionChoicebook class
class ActionChoicebook(wx.Panel):
    """Choicebook with specific action type controls"""
    
    def __init__(self, *args, **kwds):
        wx.Panel.__init__(self, *args, **kwds)
        self.action_path = None
        self.skip_change_type_event = False
        
##        self.panel_move = MovePanel(self)
##        self.panel_search_target = SearchTargetPanel(self)
##        self.panel_forget_target = ForgetTargetPanel(self)
##        self.panel_script = ScriptPanel(self)
##        self.panel_shoot = ShootPanel(self)
##        self.panel_melee = MeleePanel(self)
##        self.panel_ironsight = IronsightPanel(self)
##        self.AddPage( self.panel_move, "Move to point" )
##        self.AddPage( self.panel_script, "Script" )
##        self.AddPage( self.panel_search_target, "Search target" )
##        self.AddPage( self.panel_forget_target, "Forget target" )
##        self.AddPage( self.panel_shoot, "Shoot" )
##        self.AddPage( self.panel_melee, "Melee" )
##        self.AddPage( self.panel_ironsight, "Ironsight" )
        
        self.choice = wx.Choice(self, -1)
        self.pg = propgrid.PropertyGrid(self)
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.choice, 0, wx.EXPAND, 0)
        sizer.Add(self.pg, 1, wx.TOP|wx.EXPAND, 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.SetMinSize((-1, self.GetMinSize().y*2))
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_CHOICE, self.on_change_type, self.choice)
        self.Bind(events.EVT_CHANGING, self.on_change_action, self.pg)
        self.Bind(events.EVT_CHANGED, self.on_change_action, self.pg)
    
    def __get_action(self):
        try:
            return pynebula.lookup( str(self.action_path) )
        except:
            return None
    
    def __get_inputs_formats(self):
        # create a temporary object so the model can know about its init command
        action = self.__get_action()
        class_name = action.getactionclass()
        model_obj = pynebula.new( str(class_name) )
        model = scriptingmodelsingleobject.new_scripting_model_for_object( model_obj )
        
        # get the init command inputs' format
        try:
            inputs = model.get_command('init')['input']
        except:
            inputs = []
        
        # destroy the temporary model object
        pynebula.delete( model_obj )
        model_obj = None
        
        # return the inputs' format, discarding the first one, which is
        # reserved for internal usage (the entity owning the action)
        return inputs[1:]
    
    def __update_choice(self):
        # fill choice control
        self.choice.Clear()
        action_classes = fsm.get_action_classes()
        action_classes.sort()
        self.choice.AppendItems( action_classes )
        # select current action
        class_name = self.__get_action().getactionclass()
        self.choice.SetStringSelection( class_name )
    
    def __update_propgrid(self):
        action = self.__get_action()
        if action is not None:
            # fill the property grid with the input arguments
            inputs = self.__get_inputs_formats()
            i = 0
            self.pg.begin()
            for format in inputs:
                name = format['name']
                pid = self.pg.append_property( None, name, format )
                type = format['type']
                if type == propgrid.Type_Bool:
                    value = bool( action.getboolarg(i) )
                elif type == propgrid.Type_Float:
                    value = action.getfloatarg(i)
                elif type == propgrid.Type_Int:
                    value = action.getintarg(i)
                elif type == propgrid.Type_Object:
                    value = action.getentityarg(i)
                elif type == propgrid.Type_String:
                    value = action.getstringarg(i)
                elif type == propgrid.Type_Vector3:
                    value = action.getvector3arg(i)
                else:
                    raise TypeError, "Unsupported type '" + type + "' for property '" + name + "'"
                self.pg.set_property_value( pid, value )
                i = i + len(type)
            self.pg.end()
    
    def on_changing_type(self, event):
        if self.action_path != None and not self.skip_change_type_event:
            # forbid action type change if the default values for the new action
            # type cannot be set
            action_panel = self.GetPage( event.GetSelection() )
            if not action_panel.set_default_values():
                event.Veto()
    
    def on_change_type(self, event):
        # replace old action with a new one for the new action type
        if self.action_path != None and not self.skip_change_type_event:
            state_path = fsm.get_state_of_behaction( self.action_path )
            state = pynebula.lookup( state_path )
            action = pynebula.lookup( str(self.action_path) )
##            action_panel = self.GetPage( event.GetSelection() )
##            action.setactiontype( action_panel.action_id )
##            action_panel.update_action( action )
            action.setactionclass( str( self.choice.GetStringSelection() ) )
            self.__update_propgrid()
            fsmevents.signal_fsm_change( self )
    
    def on_change_action(self, event):
        action = self.__get_action()
        i = 0
        pid = self.pg.get_first_child_of_category(None)
        while pid is not None:
            type = self.pg.get_property_type(pid)
            if type == propgrid.Type_Bool:
                action.setboolarg( i, self.pg.get_property_value(pid) )
            elif type == propgrid.Type_Float:
                action.setfloatarg( i, self.pg.get_property_value(pid) )
            elif type == propgrid.Type_Int:
                action.setintarg( i, self.pg.get_property_value(pid) )
            elif type == propgrid.Type_Object:
                action.setentityarg( i, self.pg.get_property_value(pid) )
            elif type == propgrid.Type_String:
                action.setstringarg( i, str(self.pg.get_property_value(pid)) )
            elif type == propgrid.Type_Vector3:
                v = self.pg.get_property_value(pid)
                action.setvector3arg( i, v[0], v[1], v[2] )
            else:
                name = self.pg.get_property_name(pid)
                raise TypeError, "Unsupported type '" + type + "' for property '" + name + "'"
            i = i + len(type)
            pid = self.pg.get_next_sibling(pid)
        fsmevents.signal_fsm_change( self )
    
    def set_action(self, action_path):
        self.action_path = action_path
        if self.action_path != None:
            action = pynebula.lookup( self.action_path )
            self.skip_change_type_event = True
            self.__update_choice()
            self.__update_propgrid()
            self.skip_change_type_event = False
##            for i in range( self.GetPageCount() ):
##                action_type = self.GetPage(i).get_action_id()
##                if action_type == action.getactiontype():
##                    self.skip_change_type_event = True
##                    self.SetSelection(i)
##                    self.skip_change_type_event = False
##                    self.GetPage(i).set_action( action_path )
##                    break
        self.Enable( self.action_path != None )
    
    def quiet_refresh(self):
        if self.action_path != None:
            self.__update_choice()
##        if self.action_path != None:
##            self.GetPage( self.GetSelection() ).quiet_refresh()


# ActionCtrl class
class ActionCtrl(wx.PyControl):
    """Control to edit an AI agent action"""
    
    def __init__(self, parent, id, title):
        wx.PyControl.__init__(self, parent, id, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)
        self.action_path = None
        
        self.choicebook_type = ActionChoicebook(self, -1)
        self.staticbox_action = wx.StaticBox(self, -1, title)
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer_action = wx.StaticBoxSizer(self.staticbox_action, wx.VERTICAL)
        sizer_action.Add(self.choicebook_type, 0, wx.ALL|wx.EXPAND, 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_action)
        sizer_action.Fit(self)
        sizer_action.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(events.EVT_CHANGED, self.__on_change_action, self.choicebook_type)
    
    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()
    
    def __on_change_action(self, event):
        fsmevents.signal_fsm_change( self )
    
    def set_action(self, action_path):
        self.action_path = action_path
        if self.action_path != None:
            self.choicebook_type.set_action( self.action_path )
        self.Enable( self.action_path != None )
    
    def quiet_refresh(self):
        if self.action_path != None:
            self.choicebook_type.quiet_refresh()
