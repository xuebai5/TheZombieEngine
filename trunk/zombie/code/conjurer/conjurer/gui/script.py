##\file script.py
##\brief Helpers used by the user scripts


"""
Custom data for a script

Variable where custom data for each script is set previous to its
execution and where the script must place its custom data on exiting
to get it held for the next execution.

Can store any object that can be persisted to disk.
"""
custom_data = None
