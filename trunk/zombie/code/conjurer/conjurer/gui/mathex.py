##\file mathex.py
##\brief Some math functions not found in Python

import math


# distance3D function
def distance3D(v0, v1):
    """Return the distance between two 3D vectors"""
    dist = (
        v1[0] - v0[0],
        v1[1] - v0[1],
        v1[2] - v0[2]
        )
    return math.sqrt(
        dist[0]*dist[0] +
        dist[1]*dist[1] +
        dist[2]*dist[2]
        )
