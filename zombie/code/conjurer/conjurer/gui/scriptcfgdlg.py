##\file scriptcfgdlg.py
##\brief Script settings dialog used by the script manager

import wx

import imgdlg
import servers


# ScriptSettingsDialog class
class ScriptSettingsDialog(wx.Dialog):
    """Dialog used to modify the settings for a custom script"""
    
    def __init__(self, parent, script):
        wx.Dialog.__init__(self, parent,
            style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER)
        self.script = script
        
        self.label_sizer_staticbox = wx.StaticBox(self, -1, "Label")
        self.help_sizer_staticbox = wx.StaticBox(self, -1, "Help (only for image buttons)")
        self.button_sizer_staticbox = wx.StaticBox(self, -1, "Button")
        self.script_sizer_staticbox = wx.StaticBox(self, -1, "Script")
        self.label_desc = wx.StaticText(self, -1, "Description:")
        self.text_desc = wx.TextCtrl(self, -1, "")
        self.label_file = wx.StaticText(self, -1, "File:")
        self.text_file = wx.TextCtrl(self, -1, "")
        self.button_file = wx.Button(self, -1, "Chose a file...")
        self.text_info = wx.TextCtrl(self, -1, "A Python script can access to the 'script.custom_data' variable to store any desired custom data. This data will persist between executions, but only if the script is ran from the script manager or from its tool button/menu item (not from other sources, like running it from the script editor).", style=wx.TE_MULTILINE|wx.TE_READONLY)
        self.label_shortcut = wx.StaticText(self, -1, "Shortcut:")
        self.text_shortcut = wx.TextCtrl(self, -1, "")
        self.checkbox_button = wx.CheckBox(self, -1, "Show in toolbar")
        self.radio_image = wx.RadioButton(self, -1, "Image button:", style=wx.RB_GROUP)
        self.bitmap_path = script['button image']
        self.bitmap_button = wx.BitmapButton(self, -1, wx.Bitmap(
            servers.get_file_server().manglepath(str(self.bitmap_path)),
            wx.BITMAP_TYPE_ANY))
        self.button_preset_image = wx.Button(self, -1, "Preset image...")
        self.button_custom_image = wx.Button(self, -1, "Custom image...")
        self.radio_text = wx.RadioButton(self, -1, "Text button:")
        self.text_label = wx.TextCtrl(self, -1, "")
        self.label_tooltip = wx.StaticText(self, -1, "Tool tip text:")
        self.text_tooltip = wx.TextCtrl(self, -1, "")
        self.label_statusbar = wx.StaticText(self, -1, "Status bar text:")
        self.text_statusbar = wx.TextCtrl(self, -1, "")
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, -1, "&Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.SetTitle("Script settings")
        self.text_desc.SetValue( self.script['description'] )
        self.text_file.SetValue( self.script['path'] )
        self.text_shortcut.SetValue( self.script['shortcut'] )
        self.checkbox_button.SetValue( self.script['show button'] )
        self.radio_image.SetValue( self.script['button type'] == 'image' )
        self.radio_text.SetValue( self.script['button type'] == 'text' )
        self.bitmap_button.SetSize(self.bitmap_button.GetBestSize())
        self.text_label.SetValue( self.script['button text'] )
        self.text_tooltip.SetValue( self.script['tooltip text'] )
        self.text_statusbar.SetValue( self.script['statusbar text'] )
    
    def __do_layout(self):
        layout_sizer = wx.BoxSizer(wx.VERTICAL)
        buttons_sizer = wx.BoxSizer(wx.HORIZONTAL)
        button_sizer = wx.StaticBoxSizer(self.button_sizer_staticbox, wx.VERTICAL)
        help_sizer = wx.StaticBoxSizer(self.help_sizer_staticbox, wx.VERTICAL)
        statusbar_sizer = wx.BoxSizer(wx.HORIZONTAL)
        brief_sizer = wx.BoxSizer(wx.HORIZONTAL)
        label_sizer = wx.StaticBoxSizer(self.label_sizer_staticbox, wx.VERTICAL)
        text_sizer = wx.BoxSizer(wx.HORIZONTAL)
        image_sizer = wx.BoxSizer(wx.HORIZONTAL)
        script_sizer = wx.StaticBoxSizer(self.script_sizer_staticbox, wx.VERTICAL)
        shortcut_sizer = wx.BoxSizer(wx.HORIZONTAL)
        file_sizer = wx.BoxSizer(wx.HORIZONTAL)
        desc_sizer = wx.BoxSizer(wx.HORIZONTAL)
        desc_sizer.Add(self.label_desc, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        desc_sizer.Add(self.text_desc, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        script_sizer.Add(desc_sizer, 0, wx.ALL|wx.EXPAND, 5)
        file_sizer.Add(self.label_file, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        file_sizer.Add(self.text_file, 1, wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        file_sizer.Add(self.button_file, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        script_sizer.Add(file_sizer, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 5)
        shortcut_sizer.Add(self.label_shortcut, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        shortcut_sizer.Add(self.text_shortcut, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        script_sizer.Add(shortcut_sizer, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 5)
        script_sizer.Add(self.text_info, 1, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 5)
        layout_sizer.Add(script_sizer, 1, wx.ALL|wx.EXPAND, 10)
        button_sizer.Add(self.checkbox_button, 0, wx.ALL|wx.FIXED_MINSIZE, 5)
        image_sizer.Add(self.radio_image, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        image_sizer.Add(self.bitmap_button, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        image_sizer.Add(self.button_preset_image, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        image_sizer.Add(self.button_custom_image, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        label_sizer.Add(image_sizer, 0, wx.ALL|wx.EXPAND, 5)
        text_sizer.Add(self.radio_text, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        text_sizer.Add(self.text_label, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        label_sizer.Add(text_sizer, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 5)
        button_sizer.Add(label_sizer, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 5)
        brief_sizer.Add(self.label_tooltip, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        brief_sizer.Add(self.text_tooltip, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        help_sizer.Add(brief_sizer, 0, wx.ALL|wx.EXPAND, 5)
        statusbar_sizer.Add(self.label_statusbar, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        statusbar_sizer.Add(self.text_statusbar, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        help_sizer.Add(statusbar_sizer, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 5)
        button_sizer.Add(help_sizer, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 5)
        layout_sizer.Add(button_sizer, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        buttons_sizer.Add(self.button_ok, 1, wx.FIXED_MINSIZE, 0)
        buttons_sizer.Add(self.button_cancel, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        layout_sizer.Add(buttons_sizer, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(layout_sizer)
        layout_sizer.Fit(self)
        layout_sizer.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_file, self.button_file)
        self.Bind(wx.EVT_BUTTON, self.on_preset_image, self.button_preset_image)
        self.Bind(wx.EVT_BUTTON, self.on_custom_image, self.button_custom_image)
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)
        self.Bind(wx.EVT_RADIOBUTTON, self.on_button_type, self.radio_image)
        self.Bind(wx.EVT_RADIOBUTTON, self.on_button_type, self.radio_text)
    
    def on_button_type(self, event):
        # Force other radio buttons to false or otherwise they get in an
        # unstable state
        if event.GetEventObject() is self.radio_image:
            self.radio_text.SetValue(False)
        else:
            self.radio_image.SetValue(False)
    
    def on_file(self, event):
        # Ask for a script file
        dlg = wx.FileDialog(
            self, message="Choose a script file",
            wildcard = "Lua script files (*.lua)|*.lua|" \
                       "Python script files (*.py)|*.py|" \
                       "All script files|*.lua;*.py",
            style=wx.OPEN | wx.CHANGE_DIR
            )
        if ( dlg.ShowModal() == wx.ID_OK ):
            self.text_file.SetValue( dlg.GetPath() )
        dlg.Destroy()
    
    def on_preset_image(self, event):
        # Ask to select an image among the preset ones
        dlg = imgdlg.ImagesDialog(self, "Choose an image",
            "outgui:images/tools", image_size=wx.Size(16,16),
            scale_images=False)
        if dlg.ShowModal() == wx.ID_OK:
            if dlg.get_path() != "":
                self.bitmap_path = dlg.get_path()
                self.bitmap_button.SetBitmapLabel( wx.Bitmap(
                    servers.get_file_server().manglepath(
                    str(self.bitmap_path))) )
                self.bitmap_button.Refresh()
        dlg.Destroy()
    
    def on_custom_image(self, event):
        # Ask for an image file
        dlg = imgdlg.ImageFileDialog(self, style=wx.OPEN|wx.CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            self.bitmap_path = dlg.GetPath()
            self.bitmap_button.SetBitmapLabel( wx.Bitmap(
                servers.get_file_server().manglepath(str(self.bitmap_path))) )
        dlg.Destroy()
    
    def on_ok(self, event):
        # Update repository
        self.script['description'] = self.text_desc.GetValue()
        self.script['path'] = self.text_file.GetValue()
        self.script['shortcut'] = self.text_shortcut.GetValue()
        if self.radio_text.GetValue():
            self.script['button type'] = 'text'
        else:
            self.script['button type'] = 'image'
        self.script['show button'] = self.checkbox_button.GetValue()
        self.script['button image'] = self.bitmap_path
        self.script['button text'] = self.text_label.GetValue()
        self.script['tooltip text'] = self.text_tooltip.GetValue()
        self.script['statusbar text'] = self.text_statusbar.GetValue()
        self.EndModal(wx.ID_OK)
    
    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)
    
    def get_script(self):
        return self.script
