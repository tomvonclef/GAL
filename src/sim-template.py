#!/usr/bin/env python
# Author: Tom von Clef

from visual import *
from itertools import combinations
import gal_funcs
from gal_funcs import ObjectType
from gal_funcs import GRAV_CONSTANT

##########################
## START Global options ##
##########################
trails = False
automatic_scaling = True     #sets automaic zooming
delta_time=60                #how many seconds to step forward per iteration
global_radius_multiplier= 2
##########################
##  END  Global options ##
##########################


###########################
## START object defaults ##
###########################
gal_funcs.default_color[ObjectType.planet] = color.red
gal_funcs.default_mass[ObjectType.planet] = 6e24
gal_funcs.default_radius[ObjectType.moon] = 1.737e6
gal_funcs.default_direction[ObjectType.moon] = "counter-clockwise"
gal_funcs.default_scale[ObjectType.planet] = 4
###########################
##  END  object defaults ##
###########################

objs = [] # stores all the celestial objects

###################
## START objects ##
###################
Sun=sphere()
Sun.obj_type = ObjectType.star
Sun.mass=2e30
Sun.initial_radius=None #6.955e8
Sun.initial_color=None
Sun.initial_pos=None
Sun.initial_velocity=None #50000
Sun.name="Sun"
Sun.parent=None
objs.append(Sun)

Earth=sphere()
Earth.obj_type = ObjectType.planet
Earth.mass=None #6e24
Earth.initial_radius=6371000
Earth.initial_color=None
Earth.initial_pos=1.5e11
Earth.initial_velocity=None #vector(29821, 0, 0)
Earth.name="Earth"
Earth.parent=Sun
objs.append(Earth)

Moon=sphere()
Moon.obj_type = ObjectType.moon
Moon.mass=7e22
Moon.initial_radius=None #1.737e6
Moon.initial_color=color.white
Moon.initial_pos=vector(0,5e8,0)
Moon.initial_velocity=None #900
Moon.name="Moon"
Moon.parent=Earth
objs.append(Moon)
###################
##  END  objects ##
###################

rootNode = gal_funcs.Node("__rootNode")
gal_funcs.makeObjTree(objs, rootNode)
gal_funcs.printTree(rootNode)
gal_funcs.setTreeDefaults(rootNode)

for obj in objs:
    obj.radius *= gal_funcs.default_scale[obj.obj_type]
    obj.radius *= global_radius_multiplier
    obj.trail = curve(color=obj.color)

if automatic_scaling:
    scene.autoscale=1
else:
    scene.autoscale=0

time=0            #time is in seconds
ticks=0           #how many cycles through the loop
delay_time = 3000 #number of updates per second
centerObj = None
newCenter = False
printTime = False
while True:
    #this for loop goes through every combination of 2 objects
    for i, j in combinations(range(len(objs)),2):
        o1 = objs[i]
        o2 = objs[j]
        dist = (o1.pos-o2.pos).mag  #dist is in meters, not km
        if dist<10000:
            dist=10000  #prevents divide by zero and massive slingshot effect
        Fdir = (o1.pos-o2.pos)/dist #unit vector of direction
        Fnet = (-GRAV_CONSTANT*(o1.mass*o2.mass)/(dist**2))*Fdir  #vector of net force
        o1.momentum += Fnet*delta_time
        o2.momentum += -Fnet*delta_time
        if(trails):
            o1.trail.append(pos=o1.pos)
            o2.trail.append(pos=o2.pos)
    for o in objs: #update every object's position. instantly updates 3d window
        o.pos += (o.momentum/o.mass)*delta_time
    
    if scene.mouse.clicked:
        m = scene.mouse.getclick()
        if m.pick is not None:
            newCenter = True
            centerObj = m.pick
    if centerObj is not None:
        scene.center = centerObj.pos

    if scene.kb.keys: # event waiting to be processed?
        s = scene.kb.getkey()
        #print "key'" + s + "'"
        if s == "t":
            printTime = not printTime
        if s == "left":
            delta_time /= 2.0
        if s == "right":
            delta_time *= 2.0
        if s == " ": #space bar
            scene.center = (0,0,0)
            centerObj = None
        if s == "y":
            trails = not trails
        if s == "up" or s == "down":
           if centerObj:
               for i in range(len(objs)):
                   if objs[i] == centerObj:
                       break
               if s == "up":
                   if i == 0:
                       i = len(objs)-1
                   else:
                       i -= 1
               if s == "down":
                   if i == len(objs)-1:
                      i = 0
                   else:
                      i += 1
               if i == len(objs):
                   centerObj = None
               else:
                   centerObj = objs[i]
                   newCenter = True
           else:
               centerObj = objs[0]
               newCenter = True
    if newCenter and centerObj:
        newCenter = False
        print centerObj.name + ":  velocity:", (centerObj.momentum/centerObj.mass), \
                                " pos:", centerObj.pos
            
    if printTime and ticks % 75 == 0:
       t1 = int(time)
       seconds = t1 % 60
       t1 /= 60
       minutes = t1 % 60
       t1 /= 60
       hours = t1 % 24
       t1 /= 24
       days = t1 % 365
       t1 /= 365
       years = t1
       print years, "years,", days, "days,", hours, "hours,",  minutes, "minutes,", seconds, "seconds"

    ticks += 1 
    time += delta_time
    rate(delay_time)

