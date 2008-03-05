##\file trnlayersdlg.py
##\brief Terrain texture layers manager dialog

import wx
import wx.lib.buttons
import wx.lib.colourselect

import os

import app
import format
import imagepreview
import nebulaguisettings as guisettings
import servers
import togwin
import trnlayerprefsdlg

import conjurerconfig as cfg
import conjurerframework as cjr

# Layers limit
MaxLayers = 255


# Event used to notify selection of a layer control
EVT_SELECT_TYPE = wx.NewEventType()
EVT_SELECT = wx.PyEventBinder(EVT_SELECT_TYPE, 1)

# LayerSelectedEvent class
class LayerSelectedEvent(wx.PyCommandEvent):
    def __init__(self, id, object):
        wx.PyCommandEvent.__init__(self, EVT_SELECT_TYPE, id)
        self.SetEventObject(object)


# LayerCtrl class
class LayerCtrl(wx.PyControl):
    """Control used to handle a single terrain texture layer"""
    
    def __init__(self, parent, id, layer_id):
        wx.PyControl.__init__(self, parent, id, style=wx.NO_BORDER)
        self.layer_id = layer_id
        self.is_selected = False
        
        # controls
        self.tog_lock = wx.lib.buttons.GenBitmapToggleButton(
                                self, -1, None, size=(22, 22)
                                )
        self.tog_show = wx.lib.buttons.GenBitmapToggleButton(
                                self, -1, None, size=(22, 22)
                                )
        self.tog_texture = wx.lib.buttons.GenBitmapToggleButton(
                                    self, -1, None, size=(22, 22)
                                    )
        self.bitmap_texture = wx.StaticBitmap(self, -1, wx.NullBitmap)
        self.btn_zoom = wx.lib.buttons.GenBitmapButton(
                                self, -1, None, size=(22, 22)
                                )
        self.label_name = wx.StaticText(self, -1, "unnamed")
        self.color_sel = wx.lib.colourselect.ColourSelect(self, -1)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # images
        fileserver = servers.get_file_server()
        self.tog_lock.SetBitmapLabel(
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/lock.bmp"), 
                wx.BITMAP_TYPE_ANY
                ) 
            )
        self.tog_lock.SetUseFocusIndicator(False)
        self.tog_show.SetBitmapLabel(
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/show.bmp"), 
                wx.BITMAP_TYPE_ANY
                ) 
            )
        self.tog_show.SetUseFocusIndicator(False)
        self.tog_texture.SetBitmapLabel(
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/texture.bmp"), 
                wx.BITMAP_TYPE_ANY
                )
            )
        self.tog_texture.SetUseFocusIndicator(False)
        self.btn_zoom.SetBitmapLabel(
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/zoom.bmp"), 
                wx.BITMAP_TYPE_ANY
                ) 
            )
        self.btn_zoom.SetUseFocusIndicator(False)
        
        # layer state
        material = self.__get_material()
        self.tog_lock.SetToggle(False)
        self.tog_show.SetToggle(True)
        self.tog_texture.SetToggle(True)
        self.__update_texture_thumbnail(material)
        self.label_name.SetLabel( material.getlabel() )
        self.color_sel.SetColour(
            format.unit_rgb_2_byte_rgb( material.getmaskcolor() )
            )
    
    def __do_layout(self):
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(
            self.tog_lock, 
            0, 
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            self.tog_show, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            0
            )
        sizer.Add(
            self.tog_texture, 
            0, 
            wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            self.bitmap_texture, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer.Add(
            self.btn_zoom, 
            0, 
            wx.ALIGN_BOTTOM|wx.FIXED_MINSIZE
            )
        sizer.Add(
            self.label_name, 
            1, 
            wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            self.color_sel, 
            0, 
            wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer)
    
    def __bind_events(self):
        self.Bind(wx.EVT_SIZE, self.on_size)
        self.Bind(wx.EVT_LEFT_DOWN, self.on_select_layer)
        self.bitmap_texture.Bind(wx.EVT_LEFT_DOWN, self.on_select_layer)
        self.label_name.Bind(wx.EVT_LEFT_DOWN, self.on_select_layer)
        self.Bind(wx.EVT_LEFT_DCLICK, self.on_edit_layer)
        self.bitmap_texture.Bind(wx.EVT_LEFT_DCLICK, self.on_edit_layer)
        self.label_name.Bind(wx.EVT_LEFT_DCLICK, self.on_edit_layer)
        self.Bind(wx.EVT_BUTTON, self.on_toggle_lock, self.tog_lock)
        self.Bind(wx.EVT_BUTTON, self.on_toggle_show, self.tog_show)
        self.Bind(wx.EVT_BUTTON, self.on_toggle_texture, self.tog_texture)
        self.Bind(wx.EVT_BUTTON, self.on_zoom_texture, self.btn_zoom)
        self.Bind(
            wx.lib.colourselect.EVT_COLOURSELECT, 
            self.on_select_color, 
            self.color_sel
            )

    def __get_material(self):
        return app.get_outdoor().getlayerbyhandle(self.layer_id)

    def __update_texture_thumbnail(self, material):
        size = guisettings.Repository.getsettingvalue(
                    guisettings.ID_Thumbnail_TerrainMaterial 
                    )
        tex_path = servers.get_file_server().manglepath(
            material.gettexturethumbnail(size) )
        self.bitmap_texture.SetBitmap( wx.Bitmap(tex_path) )
        os.remove( tex_path )

    def __update_colors(self):
        if self.is_selected:
            bgcolor = wx.SystemSettings.GetColour(wx.SYS_COLOUR_INACTIVECAPTION)
        else:
            bgcolor = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE)
            
        if self.tog_lock.GetValue() == False:
            fgcolor = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNTEXT)
        else:
            fgcolor = wx.SystemSettings.GetColour(wx.SYS_COLOUR_GRAYTEXT)

        self.SetBackgroundColour(bgcolor)
        self.label_name.SetForegroundColour(fgcolor)
        self.label_name.SetBackgroundColour(bgcolor)
        self.Refresh()

    def on_size(self, event):
        self.Layout()
        event.Skip()

    def on_select_layer(self, event):
        self.select()
        self.GetEventHandler().ProcessEvent(
            LayerSelectedEvent( self.GetId(), self )
            )
        event.Skip()

    def on_edit_layer(self, event):
        if self.tog_lock.GetValue() == False:
            dlg = trnlayerprefsdlg.LayerSettingsDialog(self.GetParent(), self)
            if dlg.ShowModal() == wx.ID_OK:
                dlg.apply_layer_settings(self)
                dlg.Refresh()
            dlg.Destroy()
        else:
            wx.Bell()
        event.Skip()

    def on_toggle_lock(self, event):
        if event.GetIsDown():
            if self.is_selected:
                app.get_outdoor().selectlayer(-1)
        else:
            if self.is_selected:
                app.get_outdoor().selectlayer(self.layer_id)
        self.__update_colors()

    def on_toggle_show(self, event):
        # TODO when there's script support
        pass
    
    def on_toggle_texture(self, event):
        # TODO when there's script support
        pass

    def on_zoom_texture(self, event):
        material = self.__get_material()
        tex_path = servers.get_file_server().manglepath(
            material.gettexturethumbnail(0) )
        dlg = imagepreview.PreviewDialog( app.get_top_window(self), tex_path,
            self.get_texture_filename() )
        dlg.Show()
        os.remove( tex_path )

    def on_select_color(self, event):
        self.set_color_mask( self.color_sel.GetColour().Get() )

    def select(self):
        self.is_selected = True
        if self.tog_lock.GetValue() == False:
            app.get_outdoor().selectlayer(self.layer_id)
        else:
            app.get_outdoor().selectlayer(-1)
        self.__update_colors()

    def deselect(self):
        self.is_selected = False
        self.__update_colors()

    def get_layer_id(self):
        return self.layer_id

    def get_layer_name(self):
        return self.label_name.GetLabel()

    def set_layer_name(self, name):
        material = self.__get_material()
        material.setlabel(name)
        self.label_name.SetLabel( material.getlabel() )

    def is_locked(self):
        return self.tog_lock.GetValue()

    def set_lock(self, value):
        self.tog_lock.SetValue(value)
        self.__update_colors()

    def get_texture_filename(self):
        return self.__get_material().gettexturefilename()

    def set_texture_filename(self, path):
        material = self.__get_material()
        material.settexturefilename( str(path) )
        material.loadresources()
        self.__update_texture_thumbnail(material)

    def get_uv_scale(self):
        return self.__get_material().getuvscale()

    def set_uv_scale(self, uv):
        self.__get_material().setuvscale(uv[0], uv[1])

    def get_projection(self):
        return self.__get_material().getprojection()

    def set_projection(self, projection):
        self.__get_material().setprojection(projection)

    def get_color_mask(self):
        return self.color_sel.GetColour()

    def set_color_mask(self, color):
        material = self.__get_material()
        color = format.byte_rgb_2_unit_rgba(color)
        material.setmaskcolor( color[0], color[1], color[2], color[3] )
        self.color_sel.SetColour(
            format.unit_rgb_2_byte_rgb(
                material.getmaskcolor()
                ) 
            )

    def get_game_material_name(self):
        material = self.__get_material()
        return material.getgamematerialname()

    def set_game_material(self, name):
        material = self.__get_material()
        material.setgamematerialbyname( str(name) )


# LayerManagerDialog class
class LayerManagerDialog(togwin.ChildToggableDialog):
    """Dialog to manage terrain texture layers"""
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(self, "Terrain materials", parent)
        self.layer_selected = None
        
        # all controls but layers
        self.scroll_layers = wx.ScrolledWindow(
                                    self, 
                                    -1, 
                                    style=wx.NO_BORDER|wx.TAB_TRAVERSAL
                                    )
        self.button_new = wx.Button(self, -1, "&New material")
        self.button_delete = wx.Button(self, -1, "Delete &material")
        self.button_close = wx.Button(self, wx.ID_CANCEL, "&Close")
        self.lines = []
        
        # layer controls
        self.layers = []
        outdoor = app.get_outdoor()
        if outdoor is not None:
            layer_count = outdoor.getlayercount()
            for i in range(layer_count):
                layer_id = outdoor.getlayerhandle(i)
                self.__add_layer_ctrl(layer_id)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.SetSize((400, 300))
        self.scroll_layers.SetScrollRate(10, 10)
        
        if app.get_outdoor() is None:
            return
        
        # default layer states not stored in nebula terrain object
        layers = []
        for layer in self.layers:
            layers.append({
                'lock': False
                })
        
        data = {
            'selected layer': app.get_outdoor().getselectedlayerhandle(),
            'layers': layers
            }
        self.restore([data])
    
    def __do_layout(self):
        main_sizer = wx.BoxSizer(wx.VERTICAL)
        main_sizer.Add(
            self.scroll_layers, 1, wx.EXPAND|wx.ALL, cfg.BORDER_WIDTH
            )
        horizontal_line = wx.StaticLine(
                                self, 
                                -1
                                )
        main_sizer.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)        
        sizer_buttons.Add(
            self.button_new, 0, wx.FIXED_MINSIZE
            )
        sizer_buttons.Add(
            self.button_delete, 0, wx.FIXED_MINSIZE|wx.LEFT, cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_close, 0, wx.FIXED_MINSIZE|wx.LEFT, cfg.BORDER_WIDTH
            )
        main_sizer.Add(
            sizer_buttons, 0, wx.ALIGN_RIGHT|wx.ALL, cfg.BORDER_WIDTH
            )
        self.SetSizer(main_sizer)
        self.__refresh_layers()

    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_new_layer, self.button_new)
        self.Bind(wx.EVT_BUTTON, self.on_delete_layer, self.button_delete)
        self.Bind(wx.EVT_BUTTON, self.on_click_close_button, self.button_close)

    def on_new_layer(self, event):
        if app.get_outdoor() is None:
            cjr.show_information_message(
                "You need to create a terrain first"
                )
            return

        if app.get_outdoor().getlayercount() >= MaxLayers:
            cjr.show_information_message(
                "You cannot have more than %s layers" % str(MaxLayers)
                )
            return

        dlg = trnlayerprefsdlg.NewLayerDialog(self)
        if dlg.ShowModal() == wx.ID_OK:
            try:
                try:
                    wx.BeginBusyCursor()
                    # create new layer
                    layer_id = app.get_outdoor().createlayer()
                    app.get_outdoor().getlayerbyhandle(layer_id).loadresources()
                    # create new layer control
                    self.Freeze()
                    layer_ctrl = self.__add_layer_ctrl(layer_id)
                    dlg.apply_layer_settings(layer_ctrl)
                    self.__refresh_layers()
                    self.Thaw()
                finally:
                    wx.EndBusyCursor()
            except:
                # make sure any errors are not hidden
                raise

        dlg.Destroy()

    def on_delete_layer(self, event):
        if self.layer_selected is not None:
            msg = "Are you sure that you want to delete the '%s' layer?"\
                        % self.layer_selected.get_layer_name()
            result = cjr.confirm_yes_no(self, msg)
            if result == wx.ID_YES:
                # delete selected layer
                app.get_outdoor().removelayer(
                    self.layer_selected.get_layer_id()
                    )
                # delete layer control
                self.Freeze()
                self.__delete_layer_ctrl(self.layer_selected)
                self.__refresh_layers()
                self.Thaw()

    def on_click_close_button(self, event):
        self.Close()

    def on_select_layer(self, event):
        if self.layer_selected != event.GetEventObject():
            if self.layer_selected is not None:
                self.layer_selected.deselect()
            self.layer_selected = event.GetEventObject()

    def __add_layer_ctrl(self, layer_id):
        layer_ctrl = LayerCtrl(self.scroll_layers, -1, layer_id)
        self.layers.append(layer_ctrl)
        self.Bind(EVT_SELECT, self.on_select_layer, layer_ctrl)
        return layer_ctrl

    def __delete_layer_ctrl(self, layer_ctrl):
        self.Unbind(EVT_SELECT, layer_ctrl)
        self.layers.remove(layer_ctrl)
        if self.layer_selected == layer_ctrl:
            self.layer_selected = None
        layer_ctrl.Destroy()

    def __get_layers_sorted_by_name(self):
        return sorted(
            self.layers,
            lambda x, y: cmp( 
                x.get_layer_name().lower(), 
                y.get_layer_name().lower() 
                )
            )

    def __refresh_layers(self):
        # clean up layers subwindow
        for line in self.lines:
            line.Destroy()
        self.lines = []
        self.scroll_layers.SetSizer(None)

        # rebuild layers subwindow
        sizer_layers = wx.BoxSizer(wx.VERTICAL)
        for layer in self.__get_layers_sorted_by_name():
            sizer_layers.Add(
                layer,
                0, 
                wx.EXPAND
                )
            line = wx.StaticLine(self.scroll_layers, -1)
            self.lines.append(line)
            sizer_layers.Add(
                line, 
                0,
                wx.EXPAND
                )

        # refresh layers subwindow
        self.scroll_layers.SetSizerAndFit(sizer_layers)
        sizer_layers.SetVirtualSizeHints(self.scroll_layers)
        self.Layout()

        # enable/disable delete button
        self.button_delete.Enable( len(self.layers) > 1 )

    def persist(self):
        # layer states not stored in nebula terrain object
        layers = []
        for layer in self.layers:
            layers.append({
                'lock': layer.is_locked()
                })

        # selected layer
        if app.get_outdoor() is None:
            layer_id = -1
        else:
            layer_id = app.get_outdoor().getselectedlayerhandle()

        data = {
            'selected layer': layer_id,
            'layers': layers
            }
        return [
            create_window,
            (), # no parameters for create function
            data
            ]

    def restore(self, data_list):
        data = data_list[0]

        # layer states not stored in nebula terrain object
        for i in range(min( len(self.layers), len(data['layers']) )):
            layer_data = data['layers'][i]
            self.layers[i].set_lock( layer_data['lock'] )

        # selected layer
        if app.get_outdoor() is not None:
            app.get_outdoor().selectlayer( data['selected layer'] )
            if data['selected layer'] != -1:
                for layer in self.layers:
                    if layer.get_layer_id() == data['selected layer']:
                        if self.layer_selected is not None:
                            self.layer_selected.deselect()
                        self.layer_selected = layer
                        layer.select()
                        break

        # enable/disable delete button
        self.button_delete.Enable( len(self.layers) > 1 )

    def is_restorable(self, data_list):
        data = data_list[0]
        return True


# create_window function
def create_window(parent):
    try:
        try:
            wx.BeginBusyCursor()
            win = LayerManagerDialog(
                        parent
                        )
        finally:
            wx.EndBusyCursor()
    except:
        # make sure any errors are not hidden
        raise
    return win

