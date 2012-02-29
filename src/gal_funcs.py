#!/usr/bin/env python
# Author: Tom von Clef

from visual import vector
from visual import color
import math

# Gravitional constant using meters, seconds, and kilometers 
# see: http://en.wikipedia.org/wiki/Gravitational_constant 
GRAV_CONSTANT = 6.67428e-11

class ObjectType:
    blackhole = 1
    star = 2
    planet = 3
    moon = 4
    comet = 5
    asteroid = 6

default_mass = {   ObjectType.blackhole: 1e35,
                   ObjectType.star: 1e31,
                   ObjectType.planet: 1e26,
                   ObjectType.moon: 1e23,
                   ObjectType.comet: 1e13,
                   ObjectType.asteroid: 1e17 }

default_radius = { ObjectType.blackhole: 1e12,
                   ObjectType.star: 1e10,
                   ObjectType.planet: 1e8,
                   ObjectType.moon: 1e7,
                   ObjectType.comet: 5e5,
                   ObjectType.asteroid: 5e6 }

default_color = {  ObjectType.blackhole: (0.2, 0.2, 0.2),  #gray
                   ObjectType.star: color.yellow,
                   ObjectType.planet: color.blue,
                   ObjectType.moon: color.white,
                   ObjectType.comet: (0.63, 0.36, 0.94),
                   ObjectType.asteroid: color.green }

default_direction={ObjectType.blackhole: "clockwise",
                   ObjectType.star: "clockwise",
                   ObjectType.planet: "clockwise",
                   ObjectType.moon: "clockwise",
                   ObjectType.comet: "clockwise",
                   ObjectType.asteroid:  "clockwise"}

default_scale = {  ObjectType.blackhole: 1,
                   ObjectType.star: 1,
                   ObjectType.planet: 1,
                   ObjectType.moon: 1,
                   ObjectType.comet: 1,
                   ObjectType.asteroid: 1 }

str_to_color = {   "red": color.red,
                   "yellow": color.yellow,
                   "blue": color.blue,
                   "green": color.green,
                   "orange": color.orange,
                   "white": color.white, 
                   "gray": (0.2, 0.2, 0.2),
                   "purple": (0.63, 0.36, 0.94) }


class Node:
    def __init__(self, name):
        self.name = name
        self.children = []
        self.parent = None
    
def makeObjTree(objs, rootNode):
    for obj in objs:
        obj.children = []
    for obj in objs:
        if obj.parent:
            obj.parent.children.append(obj)
        else:
            rootNode.children.append(obj)

def printTree(node, depth=0):
    print " "*(depth*4) + node.name
    for child in node.children:
        printTree(child, depth+1)

def calcCircleVel(mass_center, mass_satellite, distance):
    """returns scalar representing velocity that produces circular orbit"""
    return (GRAV_CONSTANT*(mass_center+mass_satellite)/distance)**.5

def calcCircleMom(mass_center, mass_satellite, distance):
    """returns scalar"""
    vel = calcCircleVel(mass_center, mass_satellite, distance)
    return vel * mass_satellite

def calcTangentVel(initial_vel, position, obj_type):
    """Momentum is scalar, position is vector
    returns vector initial_vel"""
    direction = default_direction[obj_type]
    if direction == "clockwise":
       direction_modifier = 1
    else:
       direction_modifier = -1
    x = float(position[0])
    y = float(position[1])
    if x == 0:
        angle = math.pi/2
    else:
        angle = math.atan(y/x) 
    right_angle = direction_modifier*math.pi/2
    return vector(math.cos(angle-right_angle)*initial_vel, 
                  math.sin(angle-right_angle)*initial_vel, 0)


def calcDistance(x1, y1, z1, x2, y2, z2):
    return math.sqrt( (x1-x2)**2 + (y1-y2)**2 + (z1-z2)**2 )

def isScalar(val):
    return (type(val) is int) or (type(val) is float) or (type(val) is long)
    
def hasParent(obj):
    """Returns true if node is not rootNode and its parent is not rootNode"""
    return not(obj.parent is None or obj.parent.name == "rootNode")

def setObjPos(obj):
    # Set obj.pos to a vector version of obj.initial_pos
    if isScalar(obj.initial_pos):
        obj.pos=vector(0, obj.initial_pos, 0)
    elif obj.initial_pos is None:
        obj.pos=vector(0,0,0)
    else:  #obj.initial is a vector
        obj.pos=obj.initial_pos

    # Move child to frame of reference of parent
    if hasParent(obj):
        obj.pos += obj.parent.pos
    
    # Make sure position is now a vector
    if not(type(obj.pos) is vector):
        raise Exception("Error: " + obj.name + ".pos is not a vector!")

def setObjVelocity(obj):
    # Make sure obj.initial_velocity is a vector
    if obj.initial_velocity is None:
        if hasParent(obj):
            scalar_initial_velocity = calcCircleVel( obj.parent.mass, obj.mass, 
                                                     (obj.pos-obj.parent.pos).mag )
        else:
            scalar_initial_velocity = 0
        obj.initial_velocity = scalar_initial_velocity
    if isScalar(obj.initial_velocity):
        if hasParent(obj):
            obj.initial_velocity = calcTangentVel(obj.initial_velocity, 
                                         obj.pos-obj.parent.pos, obj.obj_type)
        else:
            obj.initial_velocity = vector(obj.initial_velocity, 0, 0)
    
    # Make sure initial_velocity is now a vector
    if not(type(obj.initial_velocity) is vector):
        raise Exception("Error: " + obj.name + ".initial_velocity is not a vector!")

def setObjColor(obj):
    if obj.initial_color is None:
        obj.initial_color = default_color[obj.obj_type]
    
    if type(obj.initial_color) is str:
        obj.color = str_to_color[obj.initial_color]
    
    if type(obj.initial_color) is type(color.red):
        obj.color = obj.initial_color
 
    #color.red could be any color.something
    if type(obj.color) is not type(color.red):
        raise Exception("Error: " + obj.name + ".color is not a color!")
 
def setObjRadius(obj):
    if obj.initial_radius is None:
        obj.radius = default_radius[obj.obj_type]
    elif isScalar(obj.initial_radius):
        obj.radius = obj.initial_radius
    else:
        raise Exception("Error: " + obj.name + ".initial_radius is not a scalar!")

def setObjMass(obj):
    if obj.mass is None:
       obj.mass = default_mass[obj.obj_type]
    
    if not isScalar(obj.mass):
        raise Exception("Error: " + obj.name + ".mass is not a scalar!")
 
def setObjDefaults(obj):
    if obj.name == "__rootNode":
        return

    setObjColor(obj)
    setObjRadius(obj)
    setObjMass(obj) 
    setObjPos(obj)
    setObjVelocity(obj)

    # Adjust child momentum to frame of reference of parent
    if hasParent(obj):
        obj.initial_velocity += obj.parent.initial_velocity
    
    obj.momentum = obj.initial_velocity * obj.mass

def setTreeDefaults(node):
    """Recursive function that initially accepts the root node."""
    setObjDefaults(node)
    for child in node.children:
        setTreeDefaults(child)
   
# Could be for debugging
if __name__ == '__main__':
    import sys
    print sys.argv[1]
