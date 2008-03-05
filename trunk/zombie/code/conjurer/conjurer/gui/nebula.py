## \file: nebula.py
## \brief Main translation code to be able to interact with nebula.

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------

#import for the mapping of the wxWidgets keys to nebula
import wx

# Dictionary mapping wxWidgers keys to nebula
KeyTranslationToNebula = {
    wx.WXK_BACK : 4,
    wx.WXK_TAB: 5,
    wx.WXK_RETURN: 6,
    wx.WXK_ESCAPE: 12,
    wx.WXK_SPACE: 13,
    wx.WXK_DELETE: 27,
    wx.WXK_START: 0, # check
    wx.WXK_LBUTTON: 1,
    wx.WXK_RBUTTON: 2,
    wx.WXK_CANCEL: 0, # check
    wx.WXK_MBUTTON: 3,
    wx.WXK_CLEAR: 0, # check
    wx.WXK_SHIFT: 7,
    wx.WXK_ALT: 9,
    wx.WXK_CONTROL: 8,
    wx.WXK_MENU: 9, # check
    wx.WXK_PAUSE: 10,
    wx.WXK_CAPITAL: 11,
    wx.WXK_PRIOR:14,
    wx.WXK_NEXT:15,
    wx.WXK_END:16,
    wx.WXK_HOME:17,
    wx.WXK_LEFT:18,
    wx.WXK_UP:19,
    wx.WXK_RIGHT:20,
    wx.WXK_DOWN:21,
    wx.WXK_SELECT:22,
    wx.WXK_PRINT:23,
    wx.WXK_EXECUTE:24,
    wx.WXK_SNAPSHOT:25,
    wx.WXK_INSERT:26,
    wx.WXK_HELP:28,
    wx.WXK_NUMPAD0:68,
    wx.WXK_NUMPAD1:69,
    wx.WXK_NUMPAD2:70,
    wx.WXK_NUMPAD3:71,
    wx.WXK_NUMPAD4:72,
    wx.WXK_NUMPAD5:73,
    wx.WXK_NUMPAD6:74,
    wx.WXK_NUMPAD7:75,
    wx.WXK_NUMPAD8:76,
    wx.WXK_NUMPAD9:77,
    wx.WXK_MULTIPLY:78,
    wx.WXK_ADD:79,
    wx.WXK_SEPARATOR:80,
    wx.WXK_SUBTRACT:81,
    wx.WXK_DECIMAL:82,
    wx.WXK_DIVIDE:83,
    wx.WXK_F1:84,
    wx.WXK_F2:85,
    wx.WXK_F3:86,
    wx.WXK_F4:87,
    wx.WXK_F5:88,
    wx.WXK_F6:89,
    wx.WXK_F7:90,
    wx.WXK_F8:91,
    wx.WXK_F9:92,
    wx.WXK_F10:93,
    wx.WXK_F11:94,
    wx.WXK_F12:95,
    wx.WXK_F13:96,
    wx.WXK_F14:97,
    wx.WXK_F15:98,
    wx.WXK_F16:99,
    wx.WXK_F17:100,
    wx.WXK_F18:101,
    wx.WXK_F19:102,
    wx.WXK_F20:103,
    wx.WXK_F21:104,
    wx.WXK_F22:105,
    wx.WXK_F23:106,
    wx.WXK_F24:107,
    wx.WXK_NUMLOCK:108,
    wx.WXK_SCROLL:109,
    wx.WXK_NUMPAD_HOME:17,
    wx.WXK_NUMPAD_LEFT:18,
    wx.WXK_NUMPAD_UP:19,
    wx.WXK_NUMPAD_RIGHT:20,
    wx.WXK_NUMPAD_DOWN:21,
    wx.WXK_NUMPAD_PRIOR:15,
    wx.WXK_NUMPAD_END:16,
    wx.WXK_NUMPAD_BEGIN:15,
    wx.WXK_NUMPAD_INSERT:26,
    wx.WXK_NUMPAD_DELETE:27,
    wx.WXK_NUMPAD_MULTIPLY:78,
    wx.WXK_NUMPAD_ADD:79,
    wx.WXK_NUMPAD_SUBTRACT:81,
    wx.WXK_NUMPAD_DECIMAL:82,
    wx.WXK_NUMPAD_DIVIDE:83,
    wx.WXK_NUMPAD_ENTER:6,
    ord('A'):39,
    ord('B'):40,
    ord('C'):41,
    ord('D'):42,
    ord('E'):43,
    ord('F'):44,
    ord('G'):45,
    ord('H'):46,
    ord('I'):47,
    ord('J'):48,
    ord('K'):49,
    ord('L'):50,
    ord('M'):51,
    ord('N'):51,
    ord('O'):53,
    ord('P'):54,
    ord('Q'):55,
    ord('R'):56,
    ord('S'):57,
    ord('T'):58,
    ord('U'):59,
    ord('V'):60,
    ord('W'):61,
    ord('X'):62,
    ord('Y'):63,
    ord('Z'):64,
    ord('a'):39,
    ord('b'):40,
    ord('c'):41,
    ord('d'):42,
    ord('e'):43,
    ord('f'):44,
    ord('g'):45,
    ord('h'):46,
    ord('i'):47,
    ord('j'):48,
    ord('k'):49,
    ord('l'):50,
    ord('m'):51,
    ord('n'):51,
    ord('o'):53,
    ord('p'):54,
    ord('q'):55,
    ord('r'):56,
    ord('s'):57,
    ord('t'):58,
    ord('u'):59,
    ord('v'):60,
    ord('w'):61,
    ord('x'):62,
    ord('y'):63,
    ord('z'):64,
    ord('1'):29,
    ord('2'):30,
    ord('3'):31,
    ord('4'):32,
    ord('5'):33,
    ord('6'):34,
    ord('7'):35,
    ord('8'):36,
    ord('9'):37,
    ord('0'):38,
}

# Nebula type of events
INPUT_KEY_DOWN=1
INPUT_KEY_UP=2
INPUT_KEY_CHAR=3
INPUT_MOUSE_MOVE=4
INPUT_BUTTON_DOWN=6
INPUT_BUTTON_UP=7
INPUT_BUTTON_DBLCLICK=8

# Nebula type of devices
IDEV_KEYBOARD=1<<16
IDEV_MOUSE=1<<17

# Neula mouse button ids
LEFT_BUTTON=0
RIGHT_BUTTON=1
MIDDLE_BUTTON=2


def translateKeys(keycode):
    """Function that takes care to translate wxWidgets keys codes to nebula."""

    if KeyTranslationToNebula.has_key(keycode) == True:
        return KeyTranslationToNebula[ keycode ]
    return keycode

