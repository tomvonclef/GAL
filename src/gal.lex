%option noyywrap
%option interactive
%{

/* 
   UTK EECS CS 461, Project GAL, Lexer for the GAL language
   y.tab.h contains the token number values produced by the parser
   Author: Edmon Begoli
*/
//Declarations

#include <string.h>
#include <iostream>
#include "gal.h"
#include "y.tab.h"

extern int line_num;

int level;

// we use complex regular expression for the scalar 
// because of the greediness of the implementation 
// of Flex regex. expression is meant to catch first 
// the "widest" float expression with period, then exponential number 
//form, then whole number
%}

%x error
%x indent

integer [0-9]
decimal (\.[0-9]+)
prefix [-+]
exponent ([eE]{prefix}?[1-9][0-9]*)
scalar {prefix}?(({integer}{decimal}{exponent})|({integer}+{decimal})|({integer}+{exponent}?))

%%

%{ //RULES

%}


^[ ] { BEGIN(indent); level = 1; }
<indent>[ ] { level++; yymore(); }
<indent>. { yyless( yyleng-1); BEGIN(INITIAL); 
            if (VERBOSE)
                cerr << "indent" << level << endl; 
            yylval.current_indent = level;
            level = 0;
            return INDENT; }

[ \t\f\r]	;		 // ignore white space 
\n  { line_num++; return NEWLINE; }  

{scalar} {
    if (VERBOSE)
        cerr << "SCALAR: " << yytext << endl;	
    yylval.scalar_str = strdup(yytext);
    return SCALAR;
}


"set" { if (VERBOSE) cerr << "set" << endl; return SET;  }
"vel" { return VEL; } 
"pos" { return POS; } 
"rotation" { return ROTATION; } 
"counter-clockwise" { return COUNTER_CLOCKWISE; }
"clockwise" { return CLOCKWISE; } 
"default" { return DEFAULT; } 
"mass" { if (VERBOSE) cerr << "mass" << endl;  return MASS; } 
"scale" { return SCALE; } 
"radius" { return RADIUS; } 
"color" { return COLOR; } 
"blackhole" { return BLACKHOLE; } 
"star" { return STAR; } 
"planet" { return PLANET; } 
"moon" { if (VERBOSE) cerr << "moon" << endl; return MOON; } 
"comet" { return COMET; } 
"asteroid" { return ASTEROID; }
"red" { return RED; } 
"yellow" { return YELLOW; }  
"blue" { return BLUE; }  
"green" { return GREEN; }  
"orange" { return ORANGE; } 
"white" { return WHITE; } 
"gray" { return GRAY; } 
"purple" { return PURPLE; }

[a-zA-Z][a-zA-Z0-9_]* {
         if (VERBOSE)
            cerr << "ID: " << yytext << endl; 
         yylval.id = strdup(yytext);
         return ID; 
}


"=" { if (VERBOSE) cerr << "EQUALS" << endl; return EQUALS; } 
"," { if (VERBOSE) cerr << "COMMA" << endl; return COMMA; } 
":" { if (VERBOSE) cerr << "COLON" << endl; return COLON; } 
"<" { if (VERBOSE) cerr << "LT" << endl; return LT; } 
">" { return GT; }   

. { BEGIN(error); yymore(); }
<error>[^1-9=,;\<\>a-zA-Z \t\n\f\r] { yymore(); }
<error>. { yyless(yyleng-1);            
           cerr <<"line " << line_num <<": error token: " << yytext << endl; 
           BEGIN(INITIAL); }

%%
