/** Writes out python functions needed by python target file
 *
 *
 *
 * authors: Tom von Clef
 */
#include <iostream>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include "write_python_funcs.h"

using namespace std;

void write_gal_funcs(string pathName) {

   if (!freopen((pathName + "gal_funcs.py").c_str(),"w",stdout)) {
      cerr << "Error writing gal_funcs.py\n";
      exit(1);
   }
   cout << gal_funcs_string();
   return;
}

string gal_funcs_string() {

   string output = "#!/usr/bin/env python\n" ;
   output.append( "# Created by gal, the Graphical Astronomy Language compiler.\n" );

   output.append( "#!/usr/bin/env python\n" );
   output.append( "# Author: Tom von Clef\n" );
   output.append( "\n" );
   output.append( "from visual import vector\n" );
   output.append( "from visual import color\n" );
   output.append( "import math\n" );
   output.append( "\n" );
   output.append( "# Gravitional constant using meters, seconds, and kilometers \n" );
   output.append( "# see: http://en.wikipedia.org/wiki/Gravitational_constant \n" );
   output.append( "GRAV_CONSTANT = 6.67428e-11\n" );
   output.append( "\n" );
   output.append( "class ObjectType:\n" );
   output.append( "    blackhole = 1\n" );
   output.append( "    star = 2\n" );
   output.append( "    planet = 3\n" );
   output.append( "    moon = 4\n" );
   output.append( "    comet = 5\n" );
   output.append( "    asteroid = 6\n" );
   output.append( "\n" );
   output.append( "default_mass = {   ObjectType.blackhole: 1e35,\n" );
   output.append( "                   ObjectType.star: 1e31,\n" );
   output.append( "                   ObjectType.planet: 1e26,\n" );
   output.append( "                   ObjectType.moon: 1e23,\n" );
   output.append( "                   ObjectType.comet: 1e13,\n" );
   output.append( "                   ObjectType.asteroid: 1e17 }\n" );
   output.append( "\n" );
   output.append( "default_radius = { ObjectType.blackhole: 1e12,\n" );
   output.append( "                   ObjectType.star: 1e10,\n" );
   output.append( "                   ObjectType.planet: 1e8,\n" );
   output.append( "                   ObjectType.moon: 1e7,\n" );
   output.append( "                   ObjectType.comet: 5e5,\n" );
   output.append( "                   ObjectType.asteroid: 5e6 }\n" );
   output.append( "\n" );
   output.append( "default_color = {  ObjectType.blackhole: (0.2, 0.2, 0.2),  #gray\n" );
   output.append( "                   ObjectType.star: color.yellow,\n" );
   output.append( "                   ObjectType.planet: color.blue,\n" );
   output.append( "                   ObjectType.moon: color.white,\n" );
   output.append( "                   ObjectType.comet: (0.63, 0.36, 0.94),\n" );
   output.append( "                   ObjectType.asteroid: color.green }\n" );
   output.append( "\n" );
   output.append( "default_direction={ObjectType.blackhole: 'clockwise',\n" );
   output.append( "                   ObjectType.star: 'clockwise',\n" );
   output.append( "                   ObjectType.planet: 'clockwise',\n" );
   output.append( "                   ObjectType.moon: 'clockwise',\n" );
   output.append( "                   ObjectType.comet: 'clockwise',\n" );
   output.append( "                   ObjectType.asteroid:  'clockwise'}\n" );
   output.append( "\n" );
   output.append( "default_scale = {  ObjectType.blackhole: 1,\n" );
   output.append( "                   ObjectType.star: 1,\n" );
   output.append( "                   ObjectType.planet: 1,\n" );
   output.append( "                   ObjectType.moon: 1,\n" );
   output.append( "                   ObjectType.comet: 1,\n" );
   output.append( "                   ObjectType.asteroid: 1 }\n" );
   output.append( "\n" );
   output.append( "str_to_color = {   'red': color.red,\n" );
   output.append( "                   'yellow': color.yellow,\n" );
   output.append( "                   'blue': color.blue,\n" );
   output.append( "                   'green': color.green,\n" );
   output.append( "                   'orange': color.orange,\n" );
   output.append( "                   'white': color.white, \n" );
   output.append( "                   'gray': (0.2, 0.2, 0.2),\n" );
   output.append( "                   'purple': (0.63, 0.36, 0.94) }\n" );
   output.append( "\n" );
   output.append( "\n" );
   output.append( "class Node:\n" );
   output.append( "    def __init__(self, name):\n" );
   output.append( "        self.name = name\n" );
   output.append( "        self.children = []\n" );
   output.append( "        self.parent = None\n" );
   output.append( "    \n" );
   output.append( "def makeObjTree(objs, rootNode):\n" );
   output.append( "    for obj in objs:\n" );
   output.append( "        obj.children = []\n" );
   output.append( "    for obj in objs:\n" );
   output.append( "        if obj.parent:\n" );
   output.append( "            obj.parent.children.append(obj)\n" );
   output.append( "        else:\n" );
   output.append( "            rootNode.children.append(obj)\n" );
   output.append( "\n" );
   output.append( "def printTree(node, depth=0):\n" );
   output.append( "    print ' '*(depth*4) + node.name\n" );
   output.append( "    for child in node.children:\n" );
   output.append( "        printTree(child, depth+1)\n" );
   output.append( "\n" );
   output.append( "def calcCircleVel(mass_center, mass_satellite, distance):\n" );
   output.append( "    '''returns scalar representing velocity that produces circular orbit'''\n" );
   output.append( "    return (GRAV_CONSTANT*(mass_center+mass_satellite)/distance)**.5\n" );
   output.append( "\n" );
   output.append( "def calcCircleMom(mass_center, mass_satellite, distance):\n" );
   output.append( "    '''returns scalar'''\n" );
   output.append( "    vel = calcCircleVel(mass_center, mass_satellite, distance)\n" );
   output.append( "    return vel * mass_satellite\n" );
   output.append( "\n" );
   output.append( "def calcTangentVel(initial_vel, position, obj_type):\n" );
   output.append( "    '''Momentum is scalar, position is vector\n" );
   output.append( "    returns vector initial_vel'''\n" );
   output.append( "    direction = default_direction[obj_type]\n" );
   output.append( "    if direction == 'clockwise':\n" );
   output.append( "       direction_modifier = 1\n" );
   output.append( "    else:\n" );
   output.append( "       direction_modifier = -1\n" );
   output.append( "    x = float(position[0])\n" );
   output.append( "    y = float(position[1])\n" );
   output.append( "    if x == 0:\n" );
   output.append( "        angle = math.pi/2\n" );
   output.append( "    else:\n" );
   output.append( "        angle = math.atan(y/x) \n" );
   output.append( "    right_angle = direction_modifier*math.pi/2\n" );
   output.append( "    return vector(math.cos(angle-right_angle)*initial_vel, \n" );
   output.append( "                  math.sin(angle-right_angle)*initial_vel, 0)\n" );
   output.append( "\n" );
   output.append( "\n" );
   output.append( "def calcDistance(x1, y1, z1, x2, y2, z2):\n" );
   output.append( "    return math.sqrt( (x1-x2)**2 + (y1-y2)**2 + (z1-z2)**2 )\n" );
   output.append( "\n" );
   output.append( "def isScalar(val):\n" );
   output.append( "    return (type(val) is int) or (type(val) is float) or (type(val) is long)\n" );
   output.append( "    \n" );
   output.append( "def hasParent(obj):\n" );
   output.append( "    '''Returns true if node is not rootNode and its parent is not rootNode'''\n" );
   output.append( "    return not(obj.parent is None or obj.parent.name == 'rootNode')\n" );
   output.append( "\n" );
   output.append( "def setObjPos(obj):\n" );
   output.append( "    # Set obj.pos to a vector version of obj.initial_pos\n" );
   output.append( "    if isScalar(obj.initial_pos):\n" );
   output.append( "        obj.pos=vector(0, obj.initial_pos, 0)\n" );
   output.append( "    elif obj.initial_pos is None:\n" );
   output.append( "        obj.pos=vector(0,0,0)\n" );
   output.append( "    else:  #obj.initial is a vector\n" );
   output.append( "        obj.pos=obj.initial_pos\n" );
   output.append( "\n" );
   output.append( "    # Move child to frame of reference of parent\n" );
   output.append( "    if hasParent(obj):\n" );
   output.append( "        obj.pos += obj.parent.pos\n" );
   output.append( "    \n" );
   output.append( "    # Make sure position is now a vector\n" );
   output.append( "    if not(type(obj.pos) is vector):\n" );
   output.append( "        raise Exception('Error: ' + obj.name + '.pos is not a vector!')\n" );
   output.append( "\n" );
   output.append( "def setObjVelocity(obj):\n" );
   output.append( "    # Make sure obj.initial_velocity is a vector\n" );
   output.append( "    if obj.initial_velocity is None:\n" );
   output.append( "        if hasParent(obj):\n" );
   output.append( "            scalar_initial_velocity = calcCircleVel( obj.parent.mass, obj.mass, \n" );
   output.append( "                                                     (obj.pos-obj.parent.pos).mag )\n" );
   output.append( "        else:\n" );
   output.append( "            scalar_initial_velocity = 0\n" );
   output.append( "        obj.initial_velocity = scalar_initial_velocity\n" );
   output.append( "    if isScalar(obj.initial_velocity):\n" );
   output.append( "        if hasParent(obj):\n" );
   output.append( "            obj.initial_velocity = calcTangentVel(obj.initial_velocity, \n" );
   output.append( "                                         obj.pos-obj.parent.pos, obj.obj_type)\n" );
   output.append( "        else:\n" );
   output.append( "            obj.initial_velocity = vector(obj.initial_velocity, 0, 0)\n" );
   output.append( "    \n" );
   output.append( "    # Make sure initial_velocity is now a vector\n" );
   output.append( "    if not(type(obj.initial_velocity) is vector):\n" );
   output.append( "        raise Exception('Error: ' + obj.name + '.initial_velocity is not a vector!')\n" );
   output.append( "\n" );
   output.append( "def setObjColor(obj):\n" );
   output.append( "    if obj.initial_color is None:\n" );
   output.append( "        obj.initial_color = default_color[obj.obj_type]\n" );
   output.append( "    \n" );
   output.append( "    if type(obj.initial_color) is str:\n" );
   output.append( "        obj.color = str_to_color[obj.initial_color]\n" );
   output.append( "    \n" );
   output.append( "    if type(obj.initial_color) is type(color.red):\n" );
   output.append( "        obj.color = obj.initial_color\n" );
   output.append( " \n" );
   output.append( "    #color.red could be any color.something\n" );
   output.append( "    if type(obj.color) is not type(color.red):\n" );
   output.append( "        raise Exception('Error: ' + obj.name + '.color is not a color!')\n" );
   output.append( " \n" );
   output.append( "def setObjRadius(obj):\n" );
   output.append( "    if obj.initial_radius is None:\n" );
   output.append( "        obj.radius = default_radius[obj.obj_type]\n" );
   output.append( "    elif isScalar(obj.initial_radius):\n" );
   output.append( "        obj.radius = obj.initial_radius\n" );
   output.append( "    else:\n" );
   output.append( "        raise Exception('Error: ' + obj.name + '.initial_radius is not a scalar!')\n" );
   output.append( "\n" );
   output.append( "def setObjMass(obj):\n" );
   output.append( "    if obj.mass is None:\n" );
   output.append( "       obj.mass = default_mass[obj.obj_type]\n" );
   output.append( "    \n" );
   output.append( "    if not isScalar(obj.mass):\n" );
   output.append( "        raise Exception('Error: ' + obj.name + '.mass is not a scalar!')\n" );
   output.append( " \n" );
   output.append( "def setObjDefaults(obj):\n" );
   output.append( "    if obj.name == '__rootNode':\n" );
   output.append( "        return\n" );
   output.append( "\n" );
   output.append( "    setObjColor(obj)\n" );
   output.append( "    setObjRadius(obj)\n" );
   output.append( "    setObjMass(obj) \n" );
   output.append( "    setObjPos(obj)\n" );
   output.append( "    setObjVelocity(obj)\n" );
   output.append( "\n" );
   output.append( "    # Adjust child momentum to frame of reference of parent\n" );
   output.append( "    if hasParent(obj):\n" );
   output.append( "        obj.initial_velocity += obj.parent.initial_velocity\n" );
   output.append( "    \n" );
   output.append( "    obj.momentum = obj.initial_velocity * obj.mass\n" );
   output.append( "\n" );
   output.append( "def setTreeDefaults(node):\n" );
   output.append( "    '''Recursive function that initially accepts the root node.'''\n" );
   output.append( "    setObjDefaults(node)\n" );
   output.append( "    for child in node.children:\n" );
   output.append( "        setTreeDefaults(child)\n" );
   output.append( "   \n" );
   output.append( "# Code for debugging\n" );
   output.append( "if __name__ == '__main__':\n" );
   output.append( "    import sys\n" );
   output.append( "    print sys.argv[1]\n" );

   return output;
}
