#-------------------------------------------------------------------
#                      Module for share variables
#
# Little trick for allow share variables between modules in python.
# There are one module (writeshared) that write variable into this 
# module and other (readshared) that read the variables from this 
# module.
#
# WARNING: This modules have only to be used if is stricly necessary.
# For exmaple, originaly was created for allow all modules access 
# to a global dom object that contains information about all the 
# components, entities and classes of the editor. So, I only have
# to parse the file one time.
#
#---------------------------------------------------------------------

pass
