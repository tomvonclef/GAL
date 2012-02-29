#!/usr/bin/env python
# Created by gal, the Graphical Astronomy Language compiler.
from visual import *
from itertools import combinations
import gal_funcs
from gal_funcs import ObjectType
from gal_funcs import GRAV_CONSTANT
import sys
##########################
## START Global options ##
##########################
scene = display(title=sys.argv[0],       x=0, y=0, width=600, height=600,       center=(0,0,0), background=(0,0,0))
trails = False
automatic_scaling = True    #sets automaic zooming
delta_time=60               #how many seconds to step forward per iteration
global_radius_multiplier= 2
##########################
##  END  Global options ##
##########################

###########################
## START object defaults ##
###########################
gal_funcs.default_mass[ObjectType.moon] = 7.3477e22
gal_funcs.default_mass[ObjectType.planet] = 5.9736e24
gal_funcs.default_mass[ObjectType.star] = 1.9891e30
gal_funcs.default_mass[ObjectType.asteroid] = 0.43e23
gal_funcs.default_direction[ObjectType.planet] = "clockwise"
gal_funcs.default_scale[ObjectType.planet] = 1e6
gal_funcs.default_scale[ObjectType.moon] = 1e6
gal_funcs.default_scale[ObjectType.star] = 1e8
gal_funcs.default_scale[ObjectType.asteroid] = 0.1
###########################
##  END  object defaults ##
###########################

objs = [] # stores all the celestial objects

###################
## START objects ##
###################
Sol=sphere()
Sol.obj_type = ObjectType.star
Sol.mass=1.9891e30
Sol.initial_radius=6.9566
Sol.initial_color=color.yellow
Sol.initial_pos=0
Sol.initial_velocity=0
Sol.name="Sol"
Sol.parent=None
objs.append(Sol)

Earth=sphere()
Earth.obj_type = ObjectType.planet
Earth.mass=None
Earth.initial_radius=6.371
Earth.initial_color=color.green
Earth.initial_pos=1.4709829e11
Earth.initial_velocity=2.978e4
Earth.name="Earth"
Earth.parent=Sol
objs.append(Earth)

Luna=sphere()
Luna.obj_type = ObjectType.moon
Luna.mass=None
Luna.initial_radius=1.7371
Luna.initial_color="gray"
Luna.initial_pos=4.0541e8
Luna.initial_velocity=1.022e3
Luna.name="Luna"
Luna.parent=Earth
objs.append(Luna)



Juno=sphere()
Juno.obj_type = ObjectType.asteroid
Juno.mass=2.67e19
Juno.initial_radius=3.2e-1
Juno.initial_color="purple"
Juno.initial_pos=5.0205e11
Juno.initial_velocity=1.793e4
Juno.name="Juno"
Juno.parent=Sol
objs.append(Juno)


Jupiter=sphere()
Jupiter.obj_type = ObjectType.planet
Jupiter.mass=1.8986e27
Jupiter.initial_radius=6.9911e1
Jupiter.initial_color=color.orange
Jupiter.initial_pos=8.165208e11
Jupiter.initial_velocity=1.307e4
Jupiter.name="Jupiter"
Jupiter.parent=Sol
objs.append(Jupiter)

IO=sphere()
IO.obj_type = ObjectType.moon
IO.mass=8.9319e22
IO.initial_radius=1.8213
IO.initial_color=color.blue
IO.initial_pos=4.2e8
IO.initial_velocity=1.7334e4
IO.name="IO"
IO.parent=Jupiter
objs.append(IO)


Europa=sphere()
Europa.obj_type = ObjectType.moon
Europa.mass=4.8e22
Europa.initial_radius=1.569
Europa.initial_color=color.white
Europa.initial_pos=6.64862e8
Europa.initial_velocity=1.3740e4
Europa.name="Europa"
Europa.parent=Jupiter
objs.append(Europa)



Mercury=sphere()
Mercury.obj_type = ObjectType.planet
Mercury.mass=3.3022
Mercury.initial_radius=2.4397e-3
Mercury.initial_color=None
Mercury.initial_pos=6.98169e10
Mercury.initial_velocity=4.787e4
Mercury.name="Mercury"
Mercury.parent=Sol
objs.append(Mercury)


Venus=sphere()
Venus.obj_type = ObjectType.planet
Venus.mass=4.8685e24
Venus.initial_radius=6.0518
Venus.initial_color=None
Venus.initial_pos=1.08942109e11
Venus.initial_velocity=3.502e4
Venus.name="Venus"
Venus.parent=Sol
objs.append(Venus)


Mars=sphere()
Mars.obj_type = ObjectType.planet
Mars.mass=6.4185e26
Mars.initial_radius=3.3962
Mars.initial_color=None
Mars.initial_pos=2.492093e11
Mars.initial_velocity=2.4077e4
Mars.name="Mars"
Mars.parent=Sol
objs.append(Mars)


Saturn=sphere()
Saturn.obj_type = ObjectType.planet
Saturn.mass=5.6846e26
Saturn.initial_radius=6.0268e1
Saturn.initial_color=None
Saturn.initial_pos=1.513325783e12
Saturn.initial_velocity=9.69e3
Saturn.name="Saturn"
Saturn.parent=Sol
objs.append(Saturn)


Uranus=sphere()
Uranus.obj_type = ObjectType.planet
Uranus.mass=8.6810e25
Uranus.initial_radius=2.5559e-2
Uranus.initial_color=None
Uranus.initial_pos=3.004419704e12
Uranus.initial_velocity=6.81e3
Uranus.name="Uranus"
Uranus.parent=Sol
objs.append(Uranus)


Neptune=sphere()
Neptune.obj_type = ObjectType.planet
Neptune.mass=1.0243e26
Neptune.initial_radius=2.4764e-2
Neptune.initial_color=None
Neptune.initial_pos=4.553946490e12
Neptune.initial_velocity=5.43e3
Neptune.name="Neptune"
Neptune.parent=Sol
objs.append(Neptune)



###################
##  END  objects ##
###################

rootNode = gal_funcs.Node('__rootNode')
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
        if s == 't':
            printTime = not printTime
        if s == 'p':
            trails = not trails
        if s == 'left':
            delta_time /= 2.0
        if s == 'right':
            delta_time *= 2.0
        if s == ' ': #space bar
            scene.center = (0,0,0)
            centerObj = None
        if s == 'up' or s == 'down':
           if centerObj:
               for i in range(len(objs)):
                   if objs[i] == centerObj:
                       break
               if s == 'up':
                   if i == 0:
                       i = len(objs)-1
                   else:
                       i -= 1
               if s == 'down':
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
        print centerObj.name + ':  velocity:', (centerObj.momentum/centerObj.mass), ' pos:', centerObj.pos
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
       print years, 'years,', days, 'days,', hours, 'hours,',  minutes, 'minutes,', seconds, 'seconds'
    ticks += 1 
    time += delta_time
    rate(delay_time)
