%{
/* UTK EECS CS 461 Spring 2011, GAL Project Language Parser
*  Key algorithm is the one for representation of nesting
*   based on the indentation. It represents nesting as stack
*   of lists that is at the end of translation shifted into
*   in a parent-children tree.
*   
*    Approach is similar to Python's implementation with 
*     indents/dedents, it is equally expressive but it is  
*     custom and it does not depend on dedent lexeme
*
*    See methods and in line comments for details.
* 
* Authors: Edmon Begoli, Ross Young
*/

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <list>
#include <stack>
#include <algorithm>
#include "gal.h"
#include "write_python_funcs.h"

#include <unistd.h>
#define YYDEBUG 1

    using namespace std;
    using namespace en;

    //symbol table 
    map<string, float> idTable;
    extern int yydebug;
    int VERBOSE;

    //for intermediate representation 
    // of parent-child relationship 
    stack< list<stmt *> > stmt_stack;

    prog* root;
    extern FILE * yyin;
    // to keep track of lines for error reporting 
    int line_num = 1;
    int indent_level = 0;
    int max_seen_level = 0;

    list<int> levels;


    //have here the map of lists of astObjects
    // list is mapped on the level of indentations
    // so when the new level is encountered we
    // add it to the most recent parent in the list 

    // function prototypes, we need the yylex return 
    // prototype so C++ won't complain
    int yylex();
    void yyerror(char * s);

    /** for indent/dedent */
    void place_in_stack( int stmt_level,  stmt *node );  
    void shift_to_parent( int parent_level );  

    bool has_level( int level );
    %}

    %start prog

    %union {
        char *scalar_str;
        int num;
        char *id;
        int current_indent;
        scalarOrVector *sv;
        scalar *sc;
        vector *vec;
        COLOR_CODE col;
        AST_OBJECT_TYPE ao;
        option *opt;
        list<option *> *ol; 
        stmt *st;
        defaultStmt *def;
        list<defaultStmt *> *dl;
        list<stmt *> *sl;
        settingsList *setl;
        prog *prg;

    }

%error-verbose

%token <scalar_str> SCALAR
%token <id> ID
%token <current_indent> INDENT
%token NEWLINE EQUALS COMMA COLON WHITESPACE LT GT   
%token VEL POS ROTATION CLOCKWISE COUNTER_CLOCKWISE
%token SET DEFAULT MASS SCALE RADIUS COLOR 
%token BLACKHOLE STAR PLANET MOON COMET ASTEROID
%token RED YELLOW  BLUE  GREEN  ORANGE  WHITE  GRAY PURPLE
%type <col> colorList;
%type <col> colorProp;
%type <num> indent;
%type<sv> scalarOrVector;
%type<vec> vector;
%type<sc> radiusProp;
%type<sc> massProp;
%type<sv> velProp;
%type<sv> posProp;
%type<opt> option;
%type<st> stmt;
%type<ao> astObject;
%type<ol> optionsList;
%type<def> default;
%type<dl> defaultsList;
%type<sl> stmtList;
%type<setl> settingsList;
%type<prg> prog;


%%

prog : settingsList stmtList 
     {
       shift_to_parent( 0 );
       $$ = new prog( $1, stmt_stack.top() );
       root = $$;   

     }
     ;
settingsList : optionsList defaultsList{
                   $$ = new settingsList( $1, $2 );
               }
		;

defaultsList : defaultsList default newlinelist{
                   $$ = $1;
                   $$->push_back( $2 );	 
         }
		|{
         $$ = new list<defaultStmt *>();
};

newlinelist : newlinelist NEWLINE
		|		NEWLINE

default : defaultPrefix astObject MASS EQUALS SCALAR {
	   string val( $5 );
           scalar scl( val );
	   $$ = new defaultStmt( $2, scl );
          }
        | defaultPrefix astObject VEL EQUALS scalarOrVector{
	   $$ = new defaultStmt( $2, *$5 );        
        }
        | defaultPrefix PLANET ROTATION EQUALS CLOCKWISE{
           $$ = new defaultStmt( R_CLOCKWISE ); 
        }
        | defaultPrefix PLANET ROTATION EQUALS COUNTER_CLOCKWISE{
           $$ = new defaultStmt( R_COUNTERCLOCKWISE ); 
        }

defaultPrefix : DEFAULT
	      ;

astObject : BLACKHOLE { $$ = AO_BLACKHOLE; }
	  | STAR { $$ = AO_STAR; } 
          | PLANET { $$ = AO_PLANET; }
          | MOON { $$ = AO_MOON; }
          | COMET { $$ = AO_COMET; }
          | ASTEROID { $$ = AO_ASTEROID; }
	  ;

optionsList : optionsList option newlinelist {
              $$ = $1;
              $$->push_back( $2 ); 
	  }
          | { $$ = new list<option *>; }
            ;

option : optionPrefix astObject SCALE EQUALS SCALAR {
         string str( $5 );
         scalar scal(str );
         $$ = new option( $2, scal );
       }
       ;

optionPrefix : SET 
	     ;

stmtList : stmtList indent stmt newlinelist {

           $3->setLevel( $2 );
           place_in_stack( $2, $3 );             
           indent_level = $2;

         }
         | indent stmt newlinelist {
             $2->setLevel( $1 );
             place_in_stack( $1, $2 );             
             indent_level = $1;

            
         }
         ;

indent   : INDENT { 
              if (VERBOSE)
              cerr << " indent received in parser " << $1 << endl;
              $$ = $1;
              
              bool has_seen_level = has_level( $1 );
              
              if ( $1 > 0 && ! has_level( 0 ) ){
                  yyerror(  "Non-zero initial indent is not allowed for statements."  );
                  YYERROR;
                  YYABORT;
              }
              else if ( $1 < indent_level && !has_seen_level ){
                 yyerror( "Unexpected indent. Statement does not match any previously seen indent level." );
                 YYERROR;
                 YYABORT;
              }//end if
              else if ( !has_seen_level ) {
                levels.push_back( $1 );
              }   
              if ( $1 > max_seen_level ){
                max_seen_level = $1;
              }//end if

           }
         | {  if ( levels.empty() ){
                levels.push_back( 0 ); 
              } 
             $$ = 0; 
           }
        ;

stmt : astObject ID COLON posProp velProp massProp radiusProp colorProp {
        string name( $2 );
        $$ = new stmt( $1, name, *$4, *$5, *$6, *$7, $8 );   
       }
     ;

posProp : POS EQUALS scalarOrVector{
	            $$ = $3;
                }
	;

velProp : VEL EQUALS scalarOrVector{
	            $$ = $3;
                }
		|   { $$ = new null_scalarOrVector(); }
        ;

massProp : MASS EQUALS SCALAR {
		    string val( $3 );
	            $$ = new scalar( val ); 
                }
		|   { $$ = new null_scalar(); }
         ;
radiusProp : RADIUS EQUALS SCALAR { 
		    string val( $3 );
	            $$ = new scalar( val ); 
                }
		|   { $$ = new null_scalar(); }
           ;

colorProp : COLOR EQUALS colorList { $$ = $3; }
		| { $$ = CC_DEFAULT; }
          ;

scalarOrVector : SCALAR {
                    string val( $1 );
                    scalar sc( val );
                    $$ = new scalarOrVector( sc ); 
               }
	       | vector{
                    $$ = new scalarOrVector( *$1 ); 

	       }

	       ;

colorList : RED { $$ = CC_RED; } 
	  | YELLOW { $$ = CC_YELLOW; } 
          | BLUE { $$ = CC_BLUE; } 
          | GREEN { $$ = CC_GREEN; } 
          | ORANGE { $$ = CC_ORANGE; } 
          | WHITE { $$ = CC_WHITE; }  
          | GRAY { $$ = CC_GRAY; }  
          | PURPLE { $$ = CC_PURPLE; } 
	  ;

vector : LT SCALAR COMMA SCALAR COMMA SCALAR GT{
           
            scalar sc_1( $2 );
            scalar sc_2( $4 );
            scalar sc_3( $6 );
            $$ = new vector( sc_1, sc_2, sc_3 );
           }
       ;

%%
void printHelp() {
    cerr << endl;
    cerr << "Usage: gal [options] source" << endl;
    cerr << endl;
    cerr << "Options:" << endl;
    cerr << "\t -o file \t Output to file" << endl;
    cerr << "\t -d directory \t Output to directory with default output name (directory must exist)" << endl;
    cerr << "\t -v \t\t Print out descriptive messages during compilation" << endl;
    cerr << "\t -h \t\t Print this message" << endl;
    cerr << endl;
    exit(0);
}

int main( int argc, char **argv )
{

    //yydebug = 1;
    //cout << ">>> ";
    VERBOSE = 0; 
    int c;
    string outputName(""), pathName("");
    while ((c = getopt( argc, argv, "o:d:vh")) != -1 ) {
        switch (c) {
            case 'o':
                outputName = strdup( optarg );
                break;
            case 'd':
                pathName = string( optarg ) + string("/");
                break;
            case 'v':
                VERBOSE = 1;
                //yydebug = 1;
                break;
            case 'h':
                printHelp();
                break;
            default:
                cerr << "Unknown Option" << endl;
                printHelp();
        }
    }

    if (!argv[optind]) {
        printHelp();
    }

    if (outputName == "") {
        outputName = string(argv[optind]) + string(".py");
        outputName = outputName.substr(outputName.rfind("/")+1, outputName.length());
    }
    if( (yyin = fopen( argv[optind], "r" )) == NULL ){
        cerr << "Error opening the input file \n"; 
        exit(1);
     }//end if

    int result_code =  yyparse();  

    if ( result_code == 0 ){

       if (!freopen((pathName + outputName).c_str(),"w",stdout)) {
           cerr << "Invalid filename given\n";
           exit(1);
       }//end if
       cout << root->emit();
	   write_gal_funcs(pathName);
      }else{
       cerr << "Parsing Failed\n"; 
       exit( result_code );
    }//end else 
   
}

/** statments and their nesting are represented as a 
  stack of lists. stack hierarchy represents nesting
  and list represents peer nodes at the same level
  incoming statement may be at the current level of nesting,
  above current nesting level  
  it can be at or below (same if 0).
  if above the current indent_level 
  global: indent_level
          stmt_stack
*/ 
void place_in_stack( int stmt_level,  stmt *node ){

    //if new level, push to stack new list of nodes
    // to prepare for insertion
    if ( stmt_level > indent_level ){
        list<stmt *> lst;
        stmt_stack.push( lst );           
    }//end if

    // if level is decremented shift the deeper nested
    // nodes to their parent until at the same level 
    // most recent node
    else if ( stmt_level < indent_level ){
        shift_to_parent( stmt_level );
    }//end if

    //add node to the list that is at the top of the stack, previous routines 
    //should ensure that the level is right
    if ( stmt_stack.empty() ){
        list<stmt *> lst;      
        stmt_stack.push( lst );
    }//end if
    stmt_stack.top().push_back( node );

    if (VERBOSE) {
        list<stmt*>::iterator it;
        for ( it = stmt_stack.top().begin(); 
                it != stmt_stack.top().end(); it++ ) {
            cerr << "In List " << (*it)->getLevel() << ": " 
                << (*it)->getName() << endl;
        }
    }
}//end method

/** Shifts list of nodes to the parent at the
  specified level
 */
void shift_to_parent( int parent_level ){
   
    int current_level =  stmt_stack.top().back()->getLevel(); 
    while ( parent_level < current_level ){
        list<stmt *> temp = stmt_stack.top();
        stmt_stack.pop();
        stmt_stack.top().back()->addChildren( temp );
        current_level =  stmt_stack.top().back()->getLevel(); 
    }//end while
}//end method

/** finds if the passed level 
  is in the list of seen levels 
 */
bool has_level( int level ){
  
    if ( level > max_seen_level ){
        return false;
    }//end if

    list<int>::iterator it;
    for ( it = levels.begin() ; it != levels.end(); it++ ){
        if ( *it == level ){
            return true;
        }
    }//end for

    return false;

}//end method



void yyerror(char * s)
{
    fprintf(stderr, "line %d: %s\n", line_num, s);
}



