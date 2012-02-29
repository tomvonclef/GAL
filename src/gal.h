#ifndef __GAL_H__
#define __GAL_H__

/** Headers for GAL Abstract Syntax Tree 
 * and other language related algorithms 
 * I kept some small but very relevant functions 
 *  for a better understanding of their purpose
 *  Some of the key methods that play central role
 *  in emitting of the code are: associateParent, addChildren
 *  toString and emit()
 *
 * authors: Edmon Begoli
 *

prog : settingsList stmtList;
settingsList : optionsList defaultsList;
defaultsList : defaultsList default NEWLINE
        |;
default : defaultPrefix astObject MASS EQUALS SCALAR 
        | defaultPrefix astObject VEL EQUALS scalarOrVector
        | defaultPrefix PLANET ROTATION EQUALS CLOCKWISE
        | defaultPrefix PLANET ROTATION EQUALS COUNTER_CLOCKWISE;
defaultPrefix : DEFAULT;
astObject : BLACKHOLE | STAR | PLANET | MOON | COMET | ASTEROID;
optionsList : optionsList option NEWLINE
        |;                  	  
option : optionPrefix astObject SCALE EQUALS SCALAR optionPrefix : SET;                                                        
stmtList : stmtList indent stmt NEWLINE                                     
        | indent stmt NEWLINE;                     
indent   : INDENT
        |;                                                                      
stmt : astObject ID COLON posProp velProp massProp radiusProp colorProp;
posProp : POS EQUALS scalarOrVector                                             
velProp : VEL EQUALS scalarOrVector
        |;                                                                
massProp : MASS EQUALS SCALAR
        |;                                                                
radiusProp : RADIUS EQUALS SCALAR
        |;                                                                
colorProp : COLOR EQUALS colorList
        |;                                                                
scalarOrVector : SCALAR | vector colorList : RED | YELLOW | BLUE | GREEN | ORANGE | WHITE | GRAY | PURPLE;
vector : LT SCALAR COMMA SCALAR COMMA SCALAR GT;                                  
*/

// NOTES: I developed a simplified AST because our grammar is generally a list
// of properties organized into either "global" properties and defaults or a
// list statemets representing astObjects of which some are parents to
// others please take a look at the implementation details (e begoli) 

#include <string>
#include <list>

namespace en{
    enum COLOR_CODE { CC_DEFAULT, CC_RED, CC_YELLOW, CC_BLUE, CC_GREEN, 
        CC_ORANGE, CC_WHITE, CC_GRAY, CC_PURPLE };
    enum AST_OBJECT_TYPE { AO_BLACKHOLE, AO_STAR, AO_PLANET, AO_MOON,
        AO_COMET, AO_ASTEROID };
    enum ROTATION_DIR { R_CLOCKWISE, R_COUNTERCLOCKWISE };
};


using namespace std;
using namespace en;

extern int VERBOSE;

class scalar{
    protected:
        string value;
    public: 
        scalar(){ value = "None"; }; 
        
        scalar( string in ) : value( in ){};

        /** copy constructor */
        scalar( const scalar &in ) : value( in.value ){};
        string getValue(){ return value; }
        
        /** Use this to check if this scalar was set. It is useful in
         * determining what vectorOrScalar holds, or if, for example, mass 
         * was not specified  */
        bool empty() { return value.empty(); } 
	string toString(){ return value; };
};

/** Represents null object (see GoF design patterns)
 */
class null_scalar : public scalar{
   public:
      null_scalar() : scalar( "None" ) {};
};

/** Changed floats to string for vector function. We are accepting Scientific 
 *  Notation and Python recognizes scientific notation.
 */

class vector{
    protected: 
        string i;
        string j;
        string k;
    public:
        vector(){}; 
        vector( scalar in_i, scalar in_j, scalar in_k ):
            i( in_i.getValue() ), 	j( in_j.getValue() ), 	k( in_k.getValue() ){}; 

        /** copy constructor */
        vector( const vector &in ):
            i( in.i ), j(  in.j ), k( in.k ){}; 
        string getI(){ return i; }
        string getJ(){ return j; }
        string getK(){ return k; }
	virtual string toString();

};//end class

/** Represents null object (see GoF design patterns)
 */
class null_vector : public vector{
   public:
      null_vector() : vector() { };
      string toString() { return "None"; }
      
};

class scalarOrVector{
    protected:
        scalar scalar_value;
        vector vector_value;
	bool is_vector;
    public:
        scalarOrVector(){}; 
        scalarOrVector( scalar &in ): scalar_value( in ), is_vector( false ){};
        scalarOrVector( vector &in ): vector_value( in ), is_vector( true ){};
        scalar scalarValue() { return scalar_value; };
        vector vectorValue() { return vector_value; };
	bool isVector(){ return is_vector; }

        /** Copy constructor */
        scalarOrVector(  const scalarOrVector &in ) : is_vector( in.is_vector), vector_value(in.vector_value),
	         scalar_value( in.scalar_value){};
	string toString();
};

/** Represents null object (see GoF design patterns)
 */
class null_scalarOrVector : public scalarOrVector{
   public:
      null_scalarOrVector() : scalarOrVector() {  };
      string toString(){ return "None"; }
};


class option{
    private:
        AST_OBJECT_TYPE type;
        scalar scale;
    public:
        option( AST_OBJECT_TYPE in, scalar sc ): type( in ), scale( sc ){};
	string emit();
};


class defaultStmt{
    private:
        AST_OBJECT_TYPE type;
        ROTATION_DIR rotation;   
        scalar mass;
        scalarOrVector velocity;
        bool is_for_rotation;
	bool is_for_mass;
    public:
	defaultStmt() : is_for_rotation(false), is_for_mass(false){};
        defaultStmt( AST_OBJECT_TYPE ast, scalar in_mass ) : 
            type( ast ), mass( in_mass ), is_for_rotation(false), is_for_mass(true){};
        defaultStmt( AST_OBJECT_TYPE ast, scalarOrVector in_vel ) : 
            type(ast), velocity( in_vel ), is_for_rotation(false), is_for_mass(false){};     
        defaultStmt( ROTATION_DIR in ) : type( AO_PLANET ), rotation( in ), is_for_rotation( true ),
                is_for_mass(false){};
        defaultStmt(  AST_OBJECT_TYPE ast, ROTATION_DIR in ) : type( ast ), rotation( in ),
           	is_for_rotation( true ), is_for_mass(false){};
	
        defaultStmt( const defaultStmt &i ) : type( i.type ), rotation( i.rotation ),
           	is_for_rotation( i.is_for_rotation ), is_for_mass( i.is_for_mass ){};


        bool isRotationSet(){ return is_for_rotation; }
        AST_OBJECT_TYPE getType() { return type; }
        ROTATION_DIR getRotation() { return rotation; }
        scalar getMass() { return mass; }
        scalarOrVector getVelocity() { return velocity; }
	string emit();

};//end class

//NOTE: this is a main "class" - it holds all the properties about the astr. object
// and it can also hold other objects if it is a parent - i.e. if there are
// other objects nested. Its own nesting level is specified with "level" property
// and its children are specified with children list
class stmt{
    private:
        int level; //Use to delienate nesting level
        string name;
        string parent;
        AST_OBJECT_TYPE object;
        scalarOrVector position;
        scalarOrVector velocity;
        scalar mass;
        scalar radius;
        COLOR_CODE color;  
        list<stmt *> *children; 


    public: 
        stmt() : parent( "None" ){};
        stmt( AST_OBJECT_TYPE obj, string &in, scalarOrVector &pos, 
                scalarOrVector &vel, scalar &m, scalar &r, COLOR_CODE &cc ): 
            name( in ), object( obj ), position( pos ), velocity( vel ),
            mass( m ), radius( r ), color( cc ), parent( "None" ) {};
        stmt( const stmt &i ): 
            name( i.name ), object( i.object ), position( i.position ), velocity( i.velocity ),
            mass( i.mass ), radius( i.radius ), color( i.color ), parent( i.parent ) {};


        void setLevel( int l ){ level = l; }
        void addChild( stmt *child ){
            if ( children == NULL ){
                children = new list<stmt *>();
            }//end if
            children->push_back( child );
        }
        void addChildren( list<stmt *> chldrn ){
            children = new list<stmt *>( chldrn );
        } 
        list<stmt *> getChildren(){ return *children; }
        string getName(){ return name; }

        int getLevel(){ return level; }
        void setParent( string par_name ){ parent = par_name; };

        void associateParent(){
	    if ( children != NULL ){
               for (list<stmt *>::iterator ci = children->begin(); 
                    ci != children->end(); ++ci){
                     (*ci)->setParent( name );
                     (*ci)->associateParent();
               }//end for
	    }
        }//end method

	string getParent(){ return parent; }
	string getMass();
	string getVelocity();
	string getPosition();
	string getType();
	string getColor();
	string getRadius();
	string emit();

        /** Sets option onto the stmt
        */
        void setOption( option &option ){
            // then do the same for children
            for (list<stmt *>::iterator ci = children->begin(); 
                    ci != children->end(); ++ci){
                (*ci)->setOption( option );
            }//end for

        }//end setDefault


        /** Sets defaults onto the stmt
        */
        void setDefault( defaultStmt &deflt ){

            // then do the same for children
            for (list<stmt *>::iterator ci = children->begin(); 
                    ci != children->end(); ++ci){
                (*ci)->setDefault( deflt );
            }//end for

        }//end setDefault

};//end stmt

class settingsList{
    private:
        list<option *> *optionsList;
        list<defaultStmt *> *defaults;
    public:
        settingsList( list<option *> *op, list<defaultStmt *> *def ):
            optionsList( op ), defaults( def ){};

        settingsList( settingsList &orig  ):
            optionsList( orig.optionsList ), defaults( orig.defaults ) {};
        string emit();

};

class prog{
    private:
        settingsList *settings;
        list<stmt *> stmtList;
    public:
        prog( settingsList *setList, list<stmt *> stmtLs):
            settings( setList ), stmtList( stmtLs ){}; 
	string emit();
};

#endif
