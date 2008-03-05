import wx
import wx.py as py
import sys

# Regular expression operations
import re
import os

# Conjurer
import pynebula
import childdialoggui
import freedialoggui
import nebulahelpevent as nh
import app
import stringtree
import servers

class TestListCtrl(wx.ListCtrl):
    def __init__(self, parent, ID, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0):
        wx.ListCtrl.__init__(self, parent, ID, pos, size, style)

class StringLister (wx.Panel):

    def __init__(self, parent, ID, style=0):
        wx.Panel.__init__(self, parent, -1, style=wx.WANTS_CHARS)
        
        tID = wx.NewId()

        self.string_list = {}

        self.scale = (64, 64)
        self.il = wx.ImageList(64, 64)
        self.list = TestListCtrl(self, tID,
                                 style=wx.LC_ICON
                                 | wx.BORDER_SUNKEN
                                 | wx.LC_EDIT_LABELS
                                 | wx.LC_SORT_ASCENDING
                                 | wx.LC_AUTOARRANGE 
                                 )
         
        
        self.list.SetImageList(self.il, wx.IMAGE_LIST_NORMAL)
        
        # Bind events
        self.Bind(wx.EVT_SIZE, self.on_size)
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.on_item_selected, self.list)
        self.Bind(wx.EVT_LIST_ITEM_ACTIVATED, self.on_item_activated, self.list)        

        self.list.Bind(wx.EVT_LEFT_DCLICK, self.on_double_click)
        self.list.Bind(wx.EVT_COMMAND_RIGHT_CLICK, self.on_right_click)        
            
    def display_normal (self):
        self.scale = (64, 64)
        self.il = wx.ImageList(64, 64)
        self.list.SetImageList(self.il, wx.IMAGE_LIST_NORMAL)
        self.list.SetWindowStyleFlag(wx.LC_ICON
                                    | wx.BORDER_SUNKEN
                                    | wx.LC_EDIT_LABELS
                                    | wx.LC_SORT_ASCENDING
                                    | wx.LC_AUTOARRANGE 
                                    )

    def display_list (self):
        self.scale = (16, 16)
        self.il = wx.ImageList(16, 16)
        self.list.SetImageList(self.il, wx.IMAGE_LIST_SMALL)
        self.list.SetWindowStyleFlag(wx.LC_LIST
                                    | wx.BORDER_SUNKEN
                                    | wx.LC_EDIT_LABELS
                                    | wx.LC_SORT_ASCENDING
                                    | wx.LC_AUTOARRANGE 
                                    )

    def display_large (self):
        self.scale = (128, 128)
        self.il = wx.ImageList(128, 128)
        self.list.SetImageList(self.il, wx.IMAGE_LIST_NORMAL)
        self.list.SetWindowStyleFlag(wx.LC_ICON
                                    | wx.BORDER_SUNKEN
                                    | wx.LC_EDIT_LABELS
                                    | wx.LC_SORT_ASCENDING
                                    | wx.LC_AUTOARRANGE 
                                    )    
    
    def on_item_selected(self, event):
        self.current_item = event.m_itemIndex
        
        event.Skip()

    def on_item_activated(self, event):
        self.current_item = event.m_itemIndex

    def on_double_click (self, event):
        event.Skip()

    def on_right_click (self, event):
        event.Skip()

    def populate_list(self, string_list):
        self.string_list = string_list
        items = string_list.items()
        self.list.DeleteAllItems()
        for key, data in items:
            bmp = wx.Bitmap(data[0])
            
            # scale if needed

            if self.scale != (128,128):                
                image = bmp.ConvertToImage()
                bmp = wx.BitmapFromImage(image.Scale(self.scale[0], self.scale[1]))

            self.idx1 = self.il.Add(bmp)
            index = self.list.InsertImageStringItem(sys.maxint, data[1], self.idx1)
            self.list.SetItemData(index, key)

        self.current_item = 0
       
    
    def on_size(self, event):
        w,h = self.GetClientSizeTuple()
        self.list.SetDimensions(0, 0, w, h)

    def get_list (self):
        return self.list
