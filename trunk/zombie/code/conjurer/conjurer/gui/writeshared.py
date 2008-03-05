import shared

#------------------------------------------------------
# This module write variables in the shared module.
# See shared.py for more information
#------------------------------------------------------

def set_xml_dom (dom):
    shared.xml_dom = dom

def set_object (object):
    shared.object = None
    shared.object = object

def set_result (ret):
    shared.result = ret

