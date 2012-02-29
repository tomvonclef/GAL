/** Implementations for GAL Abstract Syntax Tree and other 
 *  language related algorithms
 *
 *
 * authors: Edmon Begoli
 *          Tom von Clef
 */
#include "gal.h"
#include <string>
#include <iostream>

using namespace en;
using namespace std;

string vector::toString(){

     string value;
     value += "(";
     value += getI();
     value += ",";
     value += getJ();
     value += ",";
     value += getK();
     value += ")";
     return value;
}

string scalarOrVector::toString(){
     
   if ( isVector() ){
      return vector_value.toString();
   }else{
      return scalar_value.toString();
   }

}


/** returns the type of the astronomical object
 */
string getObjectType( AST_OBJECT_TYPE object ){

     switch ( object ){
	case AO_BLACKHOLE:
	  return "blackhole";
       case AO_STAR:
          return "star";
       case AO_PLANET:
	    return "planet";
       case AO_MOON:
            return "moon";
       case AO_COMET:
            return "comet";
       case AO_ASTEROID: 
            return "asteroid";
       default:
	    return "planet";
     }

}//end getType

/** Returns match for
 *  ROTATION_DIR { R_CLOCKWISE, R_COUNTERCLOCKWISE };
 */
string getDirection( ROTATION_DIR direction ){

      if ( direction == R_CLOCKWISE ){
	 return "\"clockwise\"";
      }else{
	 return "\"counter-clockwise\"";
      }

}//end method


/** returns the type of the astronomical object
 */
string stmt::getType(){

     return getObjectType( object );

}//end getType

string getColorType( COLOR_CODE color ){

      switch ( color ){
      case  CC_RED:
	  return "color.red";
      case CC_YELLOW:
          return "color.yellow";
      case CC_BLUE:
	    return "color.blue";
      case CC_GREEN:
            return "color.green";
      case CC_ORANGE:
            return "color.orange";
      case CC_WHITE:
            return "color.white";
      case CC_GRAY:
            return "\"gray\"";
      case CC_PURPLE:
            return "\"purple\"";
      default:
	    return "None";
	     
     }//end case

}//end getColorType


/** returns the color of the astronomical object
 */
string stmt::getColor(){
   
   return getColorType( color );

}//end getType


string stmt::getPosition(){

       return position.toString();
}


string stmt::getVelocity(){

       return velocity.toString();
}

string stmt::getMass(){

       return mass.toString();
}

string stmt::getRadius(){

       return radius.toString();
}

/** Produces default that may look like either
gal_funcs.default_color[ObjectType.star] = color.red
gal_funcs.default_mass[ObjectType.planet] = 6e24
gal_funcs.default_radius[ObjectType.moon] = 1.737e6
 */
string defaultStmt::emit(){

  string output = "gal_funcs.default_"; 

  if ( is_for_mass ){
    output += "mass[ObjectType.";
    output += getObjectType( type );
    output += "] = ";
    output += mass.toString();

  }else{

    output += "direction[ObjectType.";
    output += getObjectType( type );
    output += "] = ";
    output += getDirection( rotation );

  } 
  return output;

}//end emit

/** Emits something like gal_funcs.default_scale[ObjectType.moon] = 5
 */
string option::emit(){

    string output = "gal_funcs.default_"; 
    output += "scale[ObjectType.";
    output += getObjectType( type );
    output += "] = ";
    output += scale.toString();

    return output;


}//end emit

/** Produces an object representation that may look like
 
   Sun=sphere()
   Sun.obj_type = ObjectType.star
   Sun.mass=2e30
   Sun.radius=6.955e8
   Sun.color=color.yellow
   Sun.initial_pos=None
   Sun.initial_velocity=None
   Sun.name = "Sun"
   Sun.parent=None
   objs.append(Sun)
*/
string stmt::emit(){

   string prefix = name;
   prefix += "." ;

   string output = name;
   output += "=sphere()\n";

   //type
   output += prefix ;
   output += "obj_type = ObjectType.";
   output += getType(); 
   output += "\n";

   //mass
   output += prefix ;
   output += "mass=";
   output += getMass(); 
   output += "\n";

   //radius
   output += prefix;
   output += "initial_radius=";
   output += getRadius();
   output += "\n";
   
   //color
   output += prefix ;
   output += "initial_color=";
   output += getColor(); 
   output += "\n";
   
   //initial_pos 
   output += prefix ;
   output += "initial_pos=";
   output += getPosition(); 
   output += "\n";

   //initial_vel 
   output += prefix ;
   output += "initial_velocity=";
   string vel = getVelocity();
   if(vel[0]=='(') {
	   output += "vector";
   }
   output += vel; 
   output += "\n";
 
   //name
   output += prefix ;
   output += "name=\"";
   output += name;
   output += "\"\n";
   //parent
   output += prefix ;
   output += "parent=";
   output += parent; 
   output += "\n";

   output += "objs.append(";
   output += name;
   output += ")\n";
   output += "\n";


   if ( children != NULL ){
      // append the emit results from children recursively
      for (list<stmt *>::iterator ci = children->begin();  ci != children->end(); ++ci){
              output.append( (*ci)->emit() );
	      output.append( "\n" );
      }//end for
   }
   
   return output;

}

string settingsList::emit(){

   string output = "";

   //   append the emit results from list of defaults
  for (list<defaultStmt *>::iterator ci = defaults->begin(); 
	        ci != defaults->end(); ++ci){
	output.append( (*ci)->emit() );
	output.append( "\n" );
   }//end for

   // append the emit results from list of options
   for (list<option *>::iterator ci = optionsList->begin(); 
	         ci != optionsList->end(); ++ci){
	output.append( (*ci)->emit() );
	output.append( "\n" );
   }//end for

   return output;

}//end emit

/** Emits the entire simulations
 */
string prog::emit(){  
	
   string output = "#!/usr/bin/env python\n" ;
   output.append( "# Created by gal, the Graphical Astronomy Language compiler.\n" );

   output.append( "from visual import *\n");
   output.append( "from itertools import combinations\n");
   output.append( "import gal_funcs\n");
   output.append( "from gal_funcs import ObjectType\n");
   output.append( "from gal_funcs import GRAV_CONSTANT\n");
   output.append( "import sys\n" );

   output.append( "##########################\n");
   output.append( "## START Global options ##\n");
   output.append( "##########################\n");
   output.append( "scene = display(title=sys.argv[0],\
       x=0, y=0, width=600, height=600,\
       center=(0,0,0), background=(0,0,0))\n");
   output.append( "trails = False\n");
   output.append( "automatic_scaling = True    "); 
   output.append( "#sets automaic zooming\n");
   output.append( "delta_time=60               ");
   output.append( "#how many seconds to step forward per iteration\n");
   output.append( "global_radius_multiplier= 2\n");
   output.append( "##########################\n");
   output.append( "##  END  Global options ##\n");
   output.append( "##########################\n\n");
   output.append( "###########################\n");
   output.append( "## START object defaults ##\n");
   output.append( "###########################\n");

   output.append( settings->emit() );

   output.append( "###########################\n");
   output.append( "##  END  object defaults ##\n");
   output.append( "###########################\n\n");
   output.append( "objs = [] # stores all the celestial objects\n\n");
   output.append( "###################\n");
   output.append( "## START objects ##\n");
   output.append( "###################\n");
   
   // append the emit results from list of statements recursively
   for (list<stmt *>::iterator ci = stmtList.begin();  ci != stmtList.end(); ++ci){
     (*ci)->associateParent(); 
     output.append( (*ci)->emit() );
     output.append( "\n" );
   }//end for

   output.append( "###################\n" );
   output.append( "##  END  objects ##\n" );
   output.append( "###################\n\n" );
   output.append( "rootNode = gal_funcs.Node('__rootNode')\n" );
   output.append( "gal_funcs.makeObjTree(objs, rootNode)\n" );
   output.append( "gal_funcs.printTree(rootNode)\n" );
   output.append( "gal_funcs.setTreeDefaults(rootNode)\n" );
   output.append( "\n" );
   output.append( "for obj in objs:\n" );
   output.append( "    obj.radius *= gal_funcs.default_scale[obj.obj_type]\n" );
   output.append( "    obj.radius *= global_radius_multiplier\n" );
   output.append( "    obj.trail = curve(color=obj.color)\n" );
   output.append( "\n" );
   output.append( "if automatic_scaling:\n" );
   output.append( "    scene.autoscale=1\n" );
   output.append( "else:\n" );
   output.append( "    scene.autoscale=0\n" );
   output.append( "\n" );
   output.append( "time=0            #time is in seconds\n" );
   output.append( "ticks=0           #how many cycles through the loop\n" );
   output.append( "delay_time = 3000 #number of updates per second\n" );
   output.append( "centerObj = None\n" );
   output.append( "newCenter = False\n" );
   output.append( "printTime = False\n" );
   output.append( "while True:\n" );
   output.append( "    #this for loop goes through every combination of 2 objects\n" );
   output.append( "    for i, j in combinations(range(len(objs)),2):\n" );
   output.append( "        o1 = objs[i]\n" );
   output.append( "        o2 = objs[j]\n" );
   output.append( "        dist = (o1.pos-o2.pos).mag  #dist is in meters, not km\n" );
   output.append( "        if dist<10000:\n" );
   output.append( "            dist=10000  #prevents divide by zero and massive slingshot effect\n" );
   output.append( "        Fdir = (o1.pos-o2.pos)/dist #unit vector of direction\n" );
   output.append( "        Fnet = (-GRAV_CONSTANT*(o1.mass*o2.mass)/(dist**2))*Fdir  #vector of net force\n" );
   output.append( "        o1.momentum += Fnet*delta_time\n" );
   output.append( "        o2.momentum += -Fnet*delta_time\n" );
   output.append( "        if(trails):\n" );
   output.append( "            o1.trail.append(pos=o1.pos)\n" );
   output.append( "            o2.trail.append(pos=o2.pos)\n" );
   output.append( "    for o in objs: #update every object's position. instantly updates 3d window\n" );
   output.append( "        o.pos += (o.momentum/o.mass)*delta_time\n" );
   output.append( "    \n" );
   output.append( "    if scene.mouse.clicked:\n" );
   output.append( "        m = scene.mouse.getclick()\n" );
   output.append( "        if m.pick is not None:\n" );
   output.append( "            newCenter = True\n" );
   output.append( "            centerObj = m.pick\n" );
   output.append( "    if centerObj is not None:\n" );
   output.append( "        scene.center = centerObj.pos\n" );
   output.append( "\n" );
   output.append( "    if scene.kb.keys: # event waiting to be processed?\n" );
   output.append( "        s = scene.kb.getkey()\n" );
   output.append( "        if s == 't':\n" );
   output.append( "            printTime = not printTime\n" );
   output.append( "        if s == 'p':\n" );
   output.append( "            trails = not trails\n" );
   output.append( "        if s == 'left':\n" );
   output.append( "            delta_time /= 2.0\n" );
   output.append( "        if s == 'right':\n" );
   output.append( "            delta_time *= 2.0\n" );
   output.append( "        if s == ' ': #space bar\n" );
   output.append( "            scene.center = (0,0,0)\n" );
   output.append( "            centerObj = None\n" );
   output.append( "        if s == 'up' or s == 'down':\n" );
   output.append( "           if centerObj:\n" );
   output.append( "               for i in range(len(objs)):\n" );
   output.append( "                   if objs[i] == centerObj:\n" );
   output.append( "                       break\n" );
   output.append( "               if s == 'up':\n" );
   output.append( "                   if i == 0:\n" );
   output.append( "                       i = len(objs)-1\n" );
   output.append( "                   else:\n" );
   output.append( "                       i -= 1\n" );
   output.append( "               if s == 'down':\n" );
   output.append( "                   if i == len(objs)-1:\n" );
   output.append( "                      i = 0\n" );
   output.append( "                   else:\n" );
   output.append( "                      i += 1\n" );
   output.append( "               if i == len(objs):\n" );
   output.append( "                   centerObj = None\n" );
   output.append( "               else:\n" );
   output.append( "                   centerObj = objs[i]\n" );
   output.append( "                   newCenter = True\n" );
   output.append( "           else:\n" );
   output.append( "               centerObj = objs[0]\n" );
   output.append( "               newCenter = True\n" );
   output.append( "    if newCenter and centerObj:\n" );
   output.append( "        newCenter = False\n" );
   output.append( "        print centerObj.name + ':  velocity:', (centerObj.momentum/centerObj.mass), " );
   output.append( "' pos:', centerObj.pos\n" );
   output.append( "    if printTime and ticks % 75 == 0:\n" );
   output.append( "       t1 = int(time)\n" );
   output.append( "       seconds = t1 % 60\n" );
   output.append( "       t1 /= 60\n" );
   output.append( "       minutes = t1 % 60\n" );
   output.append( "       t1 /= 60\n" );
   output.append( "       hours = t1 % 24\n" );
   output.append( "       t1 /= 24\n" );
   output.append( "       days = t1 % 365\n" );
   output.append( "       t1 /= 365\n" );
   output.append( "       years = t1\n" );
   output.append( "       print years, 'years,', days, 'days,', hours, 'hours,',  minutes, 'minutes,', seconds, 'seconds'\n" );
   output.append( "    ticks += 1 \n" );
   output.append( "    time += delta_time\n" );
   output.append( "    rate(delay_time)\n" );

   return output;
}

