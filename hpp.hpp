#ifndef _H_HPP
#define _H_HPP

#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
using namespace std;
#include "meta.hpp"

struct Sym {
	string tag,val;
	Sym(string,string); Sym(string);
	vector<Sym*> nest; void push(Sym*);
	string dump(int=0); virtual string head(); string pad(int);
	virtual Sym* eval();
	virtual Sym* str();
	virtual Sym* eq(Sym*);
	virtual Sym* add(Sym*);
	virtual Sym* div(Sym*);
};
extern map<string,Sym*> glob;
extern void glob_init();

struct Error: Sym { Error(string); };

struct Str: Sym { Str(string); string head(); Sym*add(Sym*); };

struct Vector: Sym { Vector(); string head(); Sym*div(Sym*); Sym*str(); };

struct Op: Sym { Op(string); Sym*eval(); };

typedef Sym* (*FN)(Sym*);
struct Fn: Sym { Fn(string,FN); FN fn; Sym*at(Sym*); };

extern int yylex();
extern int yylineno;
extern char* yytext;
#define TOC(C,X) { yylval.o = new C(yytext); return X; }
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_HPP
