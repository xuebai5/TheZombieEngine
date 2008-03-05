##\file layoutdlg.py
##\brief InGUI viewport layout configuration dialog

import wx

import app
import imgbox
import servers
import togwin

import conjurerframework as cjr
import conjurerconfig as cfg

   
# unset_layout function
def unset_layout():
    vui = servers.get_viewport_ui()
    vui.undockviewport('viewport0')
    vui.undockviewport('viewport1')
    vui.undockviewport('viewport2')
    vui.undockviewport('viewport3')
    app.get_viewport('viewport0').setvisible(False)
    app.get_viewport('viewport1').setvisible(False)
    app.get_viewport('viewport2').setvisible(False)
    app.get_viewport('viewport3').setvisible(False)

# make_selected_viewport_first function
def make_selected_viewport_first(viewport_list):
    """
    Move the selected viewport to the beginning of the list, or replace the
    first element with the selected viewport if it isn't already in the list.
    """
    current_vp = servers.get_conjurer().getcurrentviewport()
    if current_vp in viewport_list:
        viewport_list[ viewport_list.index(current_vp) ] = viewport_list[0]
    viewport_list[0] = current_vp

# set_layout_1view function
def set_layout_1view():
    vps = ['viewport0']
    make_selected_viewport_first(vps)
    unset_layout()
    app.get_viewport(vps[0]).setvisible(True)

# set_layout_2cols function
def set_layout_2cols():
    vps = ['viewport0', 'viewport1']
    make_selected_viewport_first(vps)
    unset_layout()
    vui = servers.get_viewport_ui()
    vui.dockviewport(vps[0], 'v0', 'left')
    vui.dockviewport(vps[1], 'v0', 'right')
    vui.setdragbarknob('v0', 0.5)
    app.get_viewport(vps[0]).setvisible(True)
    app.get_viewport(vps[1]).setvisible(True)

# set_layout_2rows function
def set_layout_2rows():
    vps = ['viewport0', 'viewport1']
    make_selected_viewport_first(vps)
    unset_layout()
    vui = servers.get_viewport_ui()
    vui.dockviewport(vps[0], 'h0', 'up')
    vui.dockviewport(vps[1], 'h0', 'down')
    vui.setdragbarknob('h0', 0.5)
    app.get_viewport(vps[0]).setvisible(True)
    app.get_viewport(vps[1]).setvisible(True)

# set_layout_2left function
def set_layout_2left():
    vps = ['viewport0', 'viewport1', 'viewport2']
    make_selected_viewport_first(vps)
    unset_layout()
    vui = servers.get_viewport_ui()
    vui.dockviewport(vps[0], 'v0', 'right')
    vui.dockviewport(vps[1], 'v0', 'left')
    vui.dockviewport(vps[1], 'h0', 'up')
    vui.dockviewport(vps[2], 'v0', 'left')
    vui.dockviewport(vps[2], 'h0', 'down')
    vui.setdragbarknob('h0', 0.5)
    vui.setdragbarknob('v0', 0.5)
    app.get_viewport(vps[0]).setvisible(True)
    app.get_viewport(vps[1]).setvisible(True)
    app.get_viewport(vps[2]).setvisible(True)

# set_layout_2right function
def set_layout_2right():
    vps = ['viewport0', 'viewport1', 'viewport2']
    make_selected_viewport_first(vps)
    unset_layout()
    vui = servers.get_viewport_ui()
    vui.dockviewport(vps[0], 'v0', 'left')
    vui.dockviewport(vps[1], 'v0', 'right')
    vui.dockviewport(vps[1], 'h0', 'up')
    vui.dockviewport(vps[2], 'v0', 'right')
    vui.dockviewport(vps[2], 'h0', 'down')
    vui.setdragbarknob('h0', 0.5)
    vui.setdragbarknob('v0', 0.5)
    app.get_viewport(vps[0]).setvisible(True)
    app.get_viewport(vps[1]).setvisible(True)
    app.get_viewport(vps[2]).setvisible(True)

# set_layout_2up function
def set_layout_2up():
    vps = ['viewport0', 'viewport1', 'viewport2']
    make_selected_viewport_first(vps)
    unset_layout()
    vui = servers.get_viewport_ui()
    vui.dockviewport(vps[0], 'h0', 'down')
    vui.dockviewport(vps[1], 'v0', 'left')
    vui.dockviewport(vps[1], 'h0', 'up')
    vui.dockviewport(vps[2], 'v0', 'right')
    vui.dockviewport(vps[2], 'h0', 'up')
    vui.setdragbarknob('h0', 0.5)
    vui.setdragbarknob('v0', 0.5)
    app.get_viewport(vps[0]).setvisible(True)
    app.get_viewport(vps[1]).setvisible(True)
    app.get_viewport(vps[2]).setvisible(True)

# set_layout_2down function
def set_layout_2down():
    vps = ['viewport0', 'viewport1', 'viewport2']
    make_selected_viewport_first(vps)
    unset_layout()
    vui = servers.get_viewport_ui()
    vui.dockviewport(vps[0], 'h0', 'up')
    vui.dockviewport(vps[1], 'v0', 'left')
    vui.dockviewport(vps[1], 'h0', 'down')
    vui.dockviewport(vps[2], 'v0', 'right')
    vui.dockviewport(vps[2], 'h0', 'down')
    vui.setdragbarknob('h0', 0.5)
    vui.setdragbarknob('v0', 0.5)
    app.get_viewport(vps[0]).setvisible(True)
    app.get_viewport(vps[1]).setvisible(True)
    app.get_viewport(vps[2]).setvisible(True)

# set_layout_4view function
def set_layout_4view():
    vps = ['viewport0', 'viewport1', 'viewport2', 'viewport3']
#    make_selected_viewport_first(vps)
    unset_layout()
    vui = servers.get_viewport_ui()
    vui.dockviewport(vps[0], 'v0', 'left')
    vui.dockviewport(vps[0], 'h0', 'up')
    vui.dockviewport(vps[1], 'v0', 'right')
    vui.dockviewport(vps[1], 'h0', 'up')
    vui.dockviewport(vps[2], 'v0', 'left')
    vui.dockviewport(vps[2], 'h0', 'down')
    vui.dockviewport(vps[3], 'v0', 'right')
    vui.dockviewport(vps[3], 'h0', 'down')
    vui.setdragbarknob('h0', 0.5)
    vui.setdragbarknob('v0', 0.5)
    app.get_viewport(vps[0]).setvisible(True)
    app.get_viewport(vps[1]).setvisible(True)
    app.get_viewport(vps[2]).setvisible(True)
    app.get_viewport(vps[3]).setvisible(True)

# set_layout_3up function
def set_layout_3up():
    vps = ['viewport0', 'viewport1', 'viewport2', 'viewport3']
    make_selected_viewport_first(vps)
    unset_layout()
    vui = servers.get_viewport_ui()
    vui.dockviewport(vps[0], 'h0', 'down')
    vui.dockviewport(vps[1], 'h0', 'up')
    vui.dockviewport(vps[2], 'h0', 'up')
    vui.dockviewport(vps[3], 'h0', 'up')
    vui.dockviewport(vps[1], 'v0', 'left')
    vui.dockviewport(vps[2], 'v0', 'right')
    vui.dockviewport(vps[2], 'v1', 'left')
    vui.dockviewport(vps[3], 'v1', 'right')
    vui.setdragbarknob('h0', 0.33)
    vui.setdragbarknob('v0', 0.33)
    vui.setdragbarknob('v1', 0.67)
    app.get_viewport(vps[0]).setvisible(True)
    app.get_viewport(vps[1]).setvisible(True)
    app.get_viewport(vps[2]).setvisible(True)
    app.get_viewport(vps[3]).setvisible(True)

# layout_funcs variable
layout_funcs = [
    set_layout_1view,
    set_layout_2cols,
    set_layout_2rows,
    set_layout_2left,
    set_layout_2right,
    set_layout_2up,
    set_layout_2down,
    set_layout_4view,
    set_layout_3up
    ]

# set_layout function
def set_layout(index):
    if index in range(len(layout_funcs)):
        layout_funcs[index]()
        return True
    else:
        return False

# set_single_viewport function
def set_single_viewport(enabled):
    servers.get_viewport_ui().setsingleviewport(enabled)


# LayoutDialog class
class LayoutDialog(togwin.ChildToggableDialog):
    """ Dialog that allows the user to set the viewport 
        layout to some predefined layout. """
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "Viewport layout", parent
            )
        
        # controls
        self.label_layout = wx.StaticText(
                                    self,
                                    -1, 
                                    "Current viewport layout"
                                    )
        self.imgbox_layouts = imgbox.ImageBox(
                                        self,
                                        image_size=wx.Size(64, 64),
                                        scale_images=True
                                        )
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        # set layout images
        layouts = [
            "outgui:images/layouts/00.bmp",
            "outgui:images/layouts/01.bmp",
            "outgui:images/layouts/02.bmp",
            "outgui:images/layouts/03.bmp",
            "outgui:images/layouts/04.bmp",
            "outgui:images/layouts/05.bmp",
            "outgui:images/layouts/06.bmp",
            "outgui:images/layouts/07.bmp",
            "outgui:images/layouts/08.bmp"
            ]
        fileserver = servers.get_file_server()
        for layout in layouts:
            path = fileserver.manglepath(layout)
            bmp = wx.Bitmap(path, wx.BITMAP_TYPE_ANY)
            self.imgbox_layouts.append_image(bmp)

    def __do_layout(self):
        border_sizer = wx.BoxSizer(wx.VERTICAL)
        layout_sizer = wx.BoxSizer(wx.VERTICAL)
        layout_sizer.Add(
            self.label_layout,
            0, 
            wx.BOTTOM|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        layout_sizer.Add(
            self.imgbox_layouts,
            1,
            wx.EXPAND
            )
        border_sizer.Add(
            layout_sizer, 
            1,
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(border_sizer)
        border_sizer.SetSizeHints(self)
        self.SetSize((300, 280))

    def __bind_events(self):
        self.Bind(
            imgbox.EVT_IMAGE, 
            self.on_change_layout,
            self.imgbox_layouts
            )

    def on_change_layout(self, event):
        if set_layout( event.get_selection() ):
            pass
        else:
            cjr.show_information_message(
                "Sorry, the selected layout isn't supported yet."
                )

    def persist(self):
        return [
            create_window,
            () # no parameters for create function
            ]


# create_window function
def create_window(parent):
    return LayoutDialog(parent)
