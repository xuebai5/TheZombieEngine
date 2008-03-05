##\file format.py
##\brief Some functions to uniform usual conversions and formattings

import os
import sys

import servers


# bytes2mb_float function
def bytes2mb_float(bytes, decimals=1):
    """
    Return the given bytes in mb, rounded to the closest mb
    
    The value is rounded to the closest mb with the specified precision.
    For instance, the value returned for 5 000 000 bytes is '4.8'.
    
    \param bytes size to round
    \return megabytes float value
    """
    f = float(bytes)/float(1<<20)
    return round(f, decimals)


# bytes2best_int function
def bytes2mb_int(bytes):
    """
    Return the given bytes in mb
    
    The value is rounded to the closest mb.
    For instance, the value returned for 5 000 000 bytes is '5'.
    
    \param bytes size to round
    \return megabytes integer value
    """
    return int( bytes2mb_float(bytes,0) )


# mb2bytes function
def mb2bytes(mb):
    """
    Return the given mb in bytes
    
    \param mb megabytes value, either integer or float
    \return bytes integer value
    """
    return int( mb * (1<<20) )


# bytes2best_str function
def bytes2best_str(bytes, decimals=1):
    """
    Return the most intuitive string to represent a memory size
    
    For instance, the string returned for 5 000 000 bytes is '4.8 Mb'.
    The automatic conversions include b, Kb, Mb and Gb.
    
    \param bytes size to be formatted
    \param decimals number of decimal number to display (always 0 if bytes)
    \return string representing an intuitive size for a human observer
    """
    sizes = (
        (1<<0, 'b'),
        (1<<10, 'Kb'),
        (1<<20, 'Mb'),
        (1<<30, 'Gb'),
        (sys.maxint, 'Gb')
        )
    # find the best index in sizes array for the given bytes value
    for i in range(len(sizes)-1):
        if bytes < sizes[i+1][0]:
            break
    # bytes always must be displayed without decimals
    if i == 0:
        decimals = 0
    # format the string
    f = float(bytes)/float(sizes[i][0])
    format = '%(value).' + str(decimals) + 'f %(magnitude)s'
    s = format % {'value': f, 'magnitude': sizes[i][1]}
    return s


# unit_rgb_2_byte_rgb function
def unit_rgb_2_byte_rgb(color):
    return (int(color[0]*255), int(color[1]*255), int(color[2]*255))


# byte_rgb_2_unit_rgba function
def byte_rgb_2_unit_rgba(color):
    r = (float(color[0])) / 255.0
    g = (float(color[1])) / 255.0
    b = (float(color[2])) / 255.0
    return (r, g, b, 1.0)


# vector3_to_str function
def vector3_to_str(vector, decimals=6):
    # For some stupid reason if using the repr() function Python sometimes
    # adds tones of decimals, even on a rounded number. So we need to build
    # our own repr() function for a vector.
    x = str( round(vector[0], decimals) )
    y = str( round(vector[1], decimals) )
    z = str( round(vector[2], decimals) )
    return "(" + x + ", " + y + ", " + z + ")"


# align_labels function
def align_labels(labels):
        """
        Resize labels to the longest label
        
        This is intended to be used to align a set of controls which all have
        a label preppended. If this is your intention, remember to set the
        wx.ALIGN_RIGHT style in the labels.
        
        \param sliders wx.StaticText sequence to align
        """
        # get longest label width
        max_width = -1
        for label in labels:
            width = label.GetSize().width
            max_width = max(max_width, width)
        
        # resize all labels to the longest width
        for label in labels:
            label.SetSize((max_width,-1))


# strip_extension
def strip_extension(filename):
    """Return the given file name without its file type extension"""
    return filename.rsplit('.',1)[-2]

    
# get_name function
def get_name(path):
    """Get the last name of a NOH path"""
    return path.rsplit('/',1)[1]


# get_directory function
def get_directory(path):
    """Remove the last name of a NOH/file path"""
    return mangle_path(path).rsplit('/',1)[0]


# append_to_path function
def append_to_path(path, name):
    """
    Append the name to the given path and return it.
    
    The path can either end with '/' or ':' or neither.
    """
    if path[-1] == '/' or path[-1] == ':':
        return path + name
    else:
        return str(path) + str('/') + str(name)


# get_relative_path function
def get_relative_path(dir, full_path):
    """
    Return the relative path of a file to a directory.
    
    The path can either end with '/' or ':' or neither.
    """
    if dir[-1] == '/' or dir[-1] == ':':
        return full_path[ len(dir) : ]
    else:
        return full_path[ len(dir)+1 : ]


# mangle_path function
def mangle_path(path):
    """Same as Nebula mangle path, but giving paths without the special
    directories '.' and '..'"""
    # Remove assigns
    path = servers.get_file_server().manglepath( str(path) )
    # Remove parent special directories
    path = os.path.abspath( path )
    # Convert path to Nebula format (slashes instead of backslashes)
    path = servers.get_file_server().manglepath( str(path) )
    # Convert drive letter to lowercase
    if len(path) > 1:
        if path[1] == ':':
            path = path[:1].lower() + path[1:]
    return path
