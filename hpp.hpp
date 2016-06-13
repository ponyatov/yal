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
	vector<Sym*> nest; void push(Sym*); Sym* pop();
	string dump(int=0); virtual string head(); string pad(int);
	virtual Sym* eval();
	virtual Sym* str();
	virtual Sym* eq(Sym*);
	virtual Sym* at(Sym*);
	virtual Sym* map(Sym*);
	virtual Sym* add(Sym*);
	virtual Sym* div(Sym*);
	virtual Sym* mk();
	virtual Sym* mkdef(string="=");
	static Sym*mk(Sym*);
	virtual Sym* spx();		// represent object as space delimited items
	static Sym* spx(Sym*);
};
extern map<string,Sym*> glob;
extern void glob_init();

struct Error: Sym { Error(string); };

struct Var: Sym { Var(string,Sym*); Sym*eq(Sym*); Sym*spx();
	Sym*mk(); Sym*mkdef(string); };

struct Str: Sym { Str(string); string head(); Sym*add(Sym*); };

struct Vector: Sym { Vector(); string head(); Sym*div(Sym*); Sym*str();
	Sym*spx(); Sym*map(Sym*); };

struct Op: Sym { Op(string); Sym*eval(); Sym*mk(); };

typedef Sym* (*FN)(Sym*);
struct Fn: Sym { Fn(string,FN); FN fn; Sym*at(Sym*); };

struct Dep: Sym { Dep(Sym*,Sym*,Sym*); Sym*mk(); };		// dependence
struct Def: Sym { Def(Sym*); static Sym*def(Sym*);		// definition
	Sym*mk(); };
struct Ifdef: Def { Ifdef(Sym*); static Sym*ifdef(Sym*);// if definition
	Sym*mk(); };

extern int yylex();
extern int yylineno;
extern char* yytext;
#define TOC(C,X) { yylval.o = new C(yytext); return X; }
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_HPP
