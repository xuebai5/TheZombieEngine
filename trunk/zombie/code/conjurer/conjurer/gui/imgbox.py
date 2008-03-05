##\file imgbox.py
##\brief Image box control

import wx


#------------------------------------------------------------------------------
# EVT_IMAGE event

# Event used to notify selection of an image in the image box control
EVT_IMAGE_TYPE = wx.NewEventType()
EVT_IMAGE = wx.PyEventBinder(EVT_IMAGE_TYPE, 1)

# ImageSelectedEvent class
class ImageSelectedEvent(wx.PyCommandEvent):
    """Event class for an EVT_IMAGE event"""
    
    def __init__(self, id, index, object):
        wx.PyCommandEvent.__init__(self, EVT_IMAGE_TYPE, id)
        self.index = index
        self.SetEventObject(object)
    
    def get_selection(self):
        """Retrieve the index of the selected image at the time this event was generated"""
        return self.index


#------------------------------------------------------------------------------
# ImageItem class
class ImageItem:
    """Each of the items stored by the ImageBox"""
    
    def __init__(self, name, bmp, user_data=None):
        self.name = name
        self.original_bitmap = bmp
        self.scaled_bitmap = bmp
        self.user_data = user_data

    def get_name(self):
        return self.name
    
    def scale_bitmap(self, size):
        img = wx.ImageFromBitmap( self.original_bitmap )
        img.Rescale(size.width, size.height)
        self.scaled_bitmap = img.ConvertToBitmap()
    
    def get_bitmap(self):
        return self.scaled_bitmap
    
    def get_user_data(self):
        return self.user_data
    
    def set_user_data(self, data):
        self.user_data = data


#------------------------------------------------------------------------------
# ImageBox class
class ImageBox(wx.PyScrolledWindow):
    """Control that displays a set of images and allows to select one of them"""
    
    def __init__(self, parent, image_size=wx.Size(32,32),
                 scale_images=True, selector_color=wx.RED,
                 text_size=0
                ):
        """
        Constructor
        
        \param parent Parent control
        \param image_size Size in pixels of each image
        \param scale_images When true, images are rescaled
        \param selector_color Color used to show the current image selected
        \param text_size Font size to show the image names, or 0 to hide them
        """
        wx.PyScrolledWindow.__init__(self, parent, style=wx.SUNKEN_BORDER)
        
        self.items = []
        self.selected_index = wx.NOT_FOUND
        self.image_size = image_size
        self.selector_color = selector_color
        self.scale_images = scale_images
        self.text_size = text_size
        self.text_height = 0
        
        self.columns = 0
        self.rows = 0
        self.set_image_size(image_size)
        
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(wx.EVT_LEFT_DOWN, self.__on_left_down, self)
        self.Bind(wx.EVT_PAINT, self.__on_paint)
        
        # Init buffer
        self.__draw_images()
    
    # When resizing the control
    def __on_size(self, event):
        self.__update_layout()
        event.Skip()
    
    # When painting the control
    def __on_paint(self, event):
        wx.BufferedPaintDC(self, self.buffer, style=wx.BUFFER_VIRTUAL_AREA)
        event.Skip()
    
    # When pressing the left mouse button
    def __on_left_down(self, event):
        pos = self.CalcUnscrolledPosition( event.GetPosition() )
        index = self.__get_index(pos)
        if index != self.get_selection() and index != wx.NOT_FOUND:
            # Selection changed -> Notify it and update selector
            self.set_selection(index)
            self.GetEventHandler().ProcessEvent(
                ImageSelectedEvent( self.GetId(), self.get_selection(), self )
                )
            
        event.Skip()
    
    # Make sure that the current visible area is repainted on the next cycle
    def __post_repaint(self):
        self.RefreshRect( (0,0, self.GetClientSize().width,
            self.GetClientSize().height) )
    
    # Return the image index for a point position,
    # or wx.NOT_FOUND if there isn't any image below the point
    def __get_index(self, pos):
        ix = pos.x / self.image_size.width
        iy = pos.y / (self.image_size.height + self.text_height)
        if ix >= self.columns:
            return wx.NOT_FOUND
        else:
            index = iy * self.columns + ix
            if index >= self.get_count():
                return wx.NOT_FOUND
            else:
                return index
    
    # Set up scrollbars and images in a grid layout
    def __update_layout(self, force_update=False):
        old_columns = self.columns
        old_rows = self.rows
        
        # Get columns and rows needed to display all images
        size = self.GetClientSize()
        self.columns = size.width / self.image_size.width
        if self.columns == 0:
            self.columns = 1
        self.rows = self.get_count() / self.columns
        if self.get_count() % self.columns:
            self.rows = self.rows + 1
        
        # It's needed repaint of visible area when image organization changes
        if force_update or old_columns != self.columns or old_rows != self.rows:
            self.__post_repaint()
            
            # Update scrollbars, which in turn update the virtual size
            self.SetScrollbars( self.image_size.width, self.image_size.height + self.text_height,
                self.columns, self.rows )
            self.SetVirtualSizeHints( self.image_size.width * self.columns,
                (self.image_size.height + self.text_height) * self.rows )
            
            # Draw images
            self.__draw_images()
    
    # Draw the images in a grid layout
    def __draw_images(self):
        # Init bitmap buffer where to paint
        size = self.GetVirtualSize()
        self.buffer = wx.EmptyBitmap(size.width, size.height)
        cdc = wx.ClientDC(self)
        self.PrepareDC(cdc)
        dc = wx.BufferedDC(cdc, self.buffer)
        dc.SetBackground(wx.Brush(self.GetBackgroundColour()))
        dc.Clear()
        
        # Draw images
        dc.BeginDrawing()
        i = 0
        x = 0
        y = 0
        for item in self.items:
            # Draw image
            dc.DrawBitmap(item.get_bitmap(), x, y, False)
            # Draw image name
            if self.text_size > 0:
                self.__draw_text(dc, item.get_name(), x,
                    y + self.image_size.height, self.image_size.width )
            # Mark selected image
            if i == self.get_selection():
                dc.SetPen(wx.Pen(self.selector_color))
                dc.SetBrush(wx.Brush(wx.WHITE, wx.TRANSPARENT))
                dc.DrawRectangle(x,y, self.image_size.width,
                    self.image_size.height)
            i = i + 1
            y = ( i / self.columns ) * ( self.image_size.height + self.text_height )
            x = ( i % self.columns ) * self.image_size.width
        dc.EndDrawing()
    
    def __draw_text(self, dc, text, x, y, width):
        """Draw clipped and centered text"""
        # Set clipping region
        dc.SetFont( wx.Font(self.text_size, wx.SWISS, wx.NORMAL, wx.NORMAL) )
        w, h, descent, external_leading = dc.GetFullTextExtent(text)
        self.text_height = h + descent + external_leading
#        dc.DestroyClippingRegion()
#        dc.SetClippingRegion( x, y, width, self.text_height )
        # Draw text, centered
        x2 = x + (1 + width - w) / 2
        dc.DrawText( text, x2, y )
        # Release clipping region
        dc.DestroyClippingRegion()
    
    # TODO: Test
    def __assert_index_in_range(self, index):
        if index < 0 or index >= self.get_count():
            raise IndexError, "Given index (" + str(index) + ") is out of " \
                "range [0," + str(self.get_count()) + "]"
    
    def append_image(self, bmp, name=''):
        """
        Add an image at the end
        
        \return Index of the newly added image
        """
        # Create the image item
        item = ImageItem(name,bmp)
        if self.scale_images:
            item.scale_bitmap( self.image_size )
        self.items.append(item)
        
        # Update display and scrollbars
        self.__update_layout(True)
        
        # Return item's index
        return self.get_count() - 1
    
    # TODO: Test
    def clear_all(self):
        """Delete all images"""
        self.items.clear()
        self.selected_index = wx.NOT_FOUND
        self.columns = 0
        self.rows = 0
        self.__update_layout(True)
    
    # TODO: Test
    def delete_image(self, index):
        """Delete an image"""
        self.__assert_index_in_range(index)
        self.items.remove(index)
        
        # Update selection:
        #  - If there isn't any more items, clear the selection
        #  - If it has been removed the current item or later, move the
        #    selection towards the beginning, if not already at the start
        if self.is_empty():
            self.deselect()
        sel = self.get_selection()
        if sel != wx.NOT_FOUND and sel >= index and sel > 0:
            self.set_selection(sel-1)
            
        self.__update_layout(True)
    
    # TODO: Test
    def deselect(self):
        """Clear current selection"""
        self.selected_index = wx.NOT_FOUND
        self.__draw_images()
        self.__post_repaint()
    
    # TODO: Test
    def find_image_by_name(self, name):
        """Return the index of the image associated to a name"""
        i = 0
        for item in items:
            if item.get_name() == name:
                return i
            i = i + 1
        return wx.NOT_FOUND
    
    # TODO: Test
    def get_client_data(self, index):
        """Return the user data attached to an image"""
        self.__assert_index_in_range(index)
        return self.items[index].get_user_data()
    
    def get_count(self):
        """Return the number of images in the control"""
        return len(self.items)
    
    # TODO: Test
    def get_name_selection(self):
        """
        Return the name of the current selected image
        
        An empty string is returned if no image is currently selected
        """
        if self.selected_index == wx.NOT_FOUND:
            return ""
        else:
            return self.items[self.selected_index].get_name()
    
    def get_selection(self):
        """
        Return the index of the current selected image
        
        wx.NOT_FOUND is returned if no image is currently selected
        """
        return self.selected_index
    
    # TODO: Test
    def insert_image(self, index, bmp, name=''):
        """
        Insert an image before the given index
        
        \return Index of the newly inserted image
        """
        self.__assert_index_in_range(index)
        
        # Create the image item
        item = ImageItem(name,bmp)
        if self.scale_images:
            item.scale_bitmap( self.image_size )
        self.items.insert(index, item)
            
        # Update selection:
        #  - If the item has been inserted before the current selection,
        #    move the selection towards the end
        sel = self.get_selection()
        if sel != wx.NOT_FOUND and sel >= index:
            self.set_selection(sel+1)
            
        # Update display and scrollbars
        self.__update_layout(True)
            
        # Return item's index
        return index
    
    def is_empty(self):
        """Return true if the control doesn't contain any image, false otherwise"""
        return self.get_count() == 0
    
    # TODO: Test
    def set_client_data(self, index, data):
        """Attach user data to an image"""
        self.__assert_index_in_range(index)
        self.items[index].set_data(data)
    
    def set_image_size(self, size):
        """
        Sets the size for all current and new images
        
        The resize takes place in an images' copy, keeping the originals
        untouched to avoid quality loss in future resizes.
        """
        self.image_size = size
        
        # Set client min size to be the size of a single image
        # Also resize the control to contain at least one image
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.image_size)
        if self.text_size > 0:
            if self.text_height > 0:
                sizer.Add((0, self.text_height))
            else: # height not calculated yet, guess it
                sizer.Add((0, self.text_size + 5))
        self.SetSizer(sizer)
        sizer.SetSizeHints(self)
        
        # Resize all images
        if self.scale_images:
            for item in self.items:
                item.scale_bitmap( self.image_size )
        
        # Reorganize displayed images
        self.__update_layout(True)
    
    # TODO: Test
    def set_name_selection(self, name):
        """Select the image with the given name"""
        i = 0
        for item in items:
            if item.get_name() == name:
                self.set_selection(i)
                break
    
    def set_selection(self, index):
        """Select the image for the given index"""
        self.__assert_index_in_range(index)
        self.selected_index = index
        self.__draw_images()
        self.__post_repaint()
