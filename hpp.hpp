#ifndef _H_HPP
#define _H_HPP

#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
using namespace std;
#include "meta.hpp"

struct Sym {
	vector<Sym*> nest; void push(Sym*);
	string dump(int=0); virtual string head(); string pad(int);
	virtual Sym* eval();
};
extern map<string,Sym*> glob;
extern void glob_init();

extern int yylex();
extern int yylineno;
extern char* yytext;
#define TOC(C,X) { yylval.o = new C(yytext); return X; }
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_HPP
