from xml.dom import minidom

import sys
import time

class ParseXML:
    def __init__ (self, doc):
        self.doc = minidom.parse(doc)
        self.first_child = self.doc.firstChild       

    def get_text(self, nodelist):
        rc = ""
        for node in nodelist:
            if node.nodeType == node.TEXT_NODE:
                rc = rc + node.data
        return rc
    		
    # return a structure with information about the class
    def get_class_info (self, type, class_name):
        class_info = {}

        for child in self.first_child.childNodes:
            if child.nodeType != 3 and child.nodeName == type:
                if class_name.lower() == child.attributes.getNamedItem("cppclass").nodeValue.lower():
                    cl = child
                    class_info["group"] = child.attributes.getNamedItem("group").nodeValue
                    #class_info["doc"] = self.get_text(cl.getElementsByTagName("doc")[0].childNodes).strip()
                    class_info["doc"] = ""
                    class_info["superclass"] = child.attributes.getNamedItem("superclass").nodeValue
                    class_info["cppclass"] = class_name
                    class_info["properties"] = cl.getElementsByTagName("property")
                    command_tag = cl.getElementsByTagName("commands")[0]
                    class_info["commands"] = command_tag.getElementsByTagName("cmd")
                    class_info["components"] = cl.getElementsByTagName("component")
                    return class_info        
        
        #print "ERROR: Class %s not found in XML file" %(class_name)
        return None
               
    def get_prop_name (self, prop):
        return prop.getAttribute("name")

    def get_prop_doc (self, prop):
        return self.get_text(prop.getElementsByTagName("doc")[0].childNodes).strip()

    # Return a list with information about the property attributes
    def get_prop_attrs (self, prop):
        attr_list = []
        for attr in prop.getElementsByTagName("attrs"):                        
            att = {}
            att["type"] = attr.getAttribute("type")
            att["value"] = attr.getAttribute("value")
            attr_list.append(att)

        return attr_list

    # Return a list that describes the setters commands
    def get_prop_setters (self, prop):        
        setter_list = []
        settter_list_tag = prop.getElementsByTagName("setterlist")[0]
        for s in settter_list_tag.getElementsByTagName("setter"):
            setter = {}
            setter["name"] = s.getAttribute("name")
            setter["subtype"] = s.getAttribute("subtype")
            setter["doc"] = self.get_text(s.getElementsByTagName("doc")[0].childNodes).strip()

            setter_list.append(setter)

        return setter_list

    # Return a list that describes the getters commands
    def get_prop_getters (self, prop):        
        getter_list = []
        gettter_list_tag = prop.getElementsByTagName("getterlist")[0]
        for s in gettter_list_tag.getElementsByTagName("getter"):
            getter = {}
            getter["name"] = s.getAttribute("name")
            getter["subtype"] = s.getAttribute("subtype")
            getter["doc"] = self.get_text(s.getElementsByTagName("doc")[0].childNodes).strip()

            getter_list.append(getter)

        return getter_list

    # Return a list with all the DOM params tag inside the property
    # TODO: This function must return the same format that get_cmd_output
    # because the interface is quite confuse now...
    def get_prop_params (self, prop):
        param_list = []
        format_tag = prop.getElementsByTagName("format")[0]
        for p in format_tag.getElementsByTagName("param"):
            param = {}
            param['name'] = p.getAttribute("name")
            param['type'] = p.getAttribute("type")
            param['subtype'] = p.getAttribute("subtype")
            param['doc'] = self.get_text(p.getElementsByTagName("doc")[0].childNodes).strip()            

            attr_list = []
            for attr in p.getElementsByTagName("attr"):
                att = {}
                att["type"] = attr.getAttribute("type")
                att["value"] = attr.getAttribute("value")
                attr_list.append(att)

            param["attrs"] = attr_list
            param_list.append(param)

        return param_list

    def get_attr_type (self, attr):
        return attr.getAttribute("type")

    def get_attr_value (self, attr):
        return attr.getAttribute("value")

    # Return a structure with information about a param
    def get_param_name(self, param):
        return param.getAttribute("name")

    def get_param_doc(self, param):
        return self.get_text(param.getElementsByTagName("doc")[0].childNodes).strip()
    
    def get_param_type (self, param):
        return param.getAttribute("type")        

    def get_param_attrs (self, param):
        attr_list = []

        for a in param.getElementsByTagName("attr"):
            attr_list.append(a)

        return attr_list

    def get_cmd_name (self, cmd):
        return cmd.getAttribute("name")

    def get_cmd_doc (self, cmd):
        return self.get_text(cmd.getElementsByTagName("doc")[0].childNodes).strip()

    def get_cmd_output (self, cmd):
        output_list = []
        if cmd.getElementsByTagName("output"):
            out = cmd.getElementsByTagName("output")[0]

            for param in out.getElementsByTagName("param"):
                output_list.append(param)

        return output_list

    def get_cmd_input (self, cmd):
        input_list = []
        if cmd.getElementsByTagName("input"):
            input = cmd.getElementsByTagName("input")[0]

            for param in input.getElementsByTagName("param"):
                input_list.append(param)

        return input_list

    def get_cmd_by_name (self, cmd_list, name):        
        for cmd in cmd_list:
            if self.get_cmd_name(cmd) == name:
                return cmd

        return ""

    def get_param_subtype (self, param):
        for attr in self.get_param_attrs(param):
            attr_type = self.get_attr_type(attr)               
   
            if "subtype" == attr_type:
                return self.get_attr_value(attr)

        return ""

    def get_comp_name (self, component):
        return component.getAttribute("name")

