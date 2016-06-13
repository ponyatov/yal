#include "hpp.hpp"
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cout<<YYERR; cerr<<YYERR; exit(-1); }
int main() { glob_init(); return yyparse(); }

Sym::Sym(string T,string V) { tag=T; val=V; }
Sym::Sym(string V):Sym("sym",V) {}
void Sym::push(Sym*o) { nest.push_back(o); }
Sym* Sym::pop() { nest.pop_back(); return this; }

string Sym::head() { return "<"+tag+":"+val+">"; }
string Sym::pad(int n) { string S; for (int i=0;i<n;i++) S+='\t'; return S; }
string Sym::dump(int depth) { string S = "\n"+pad(depth)+head();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		S += (*it)->dump(depth+1);
	return S; }

Sym* Sym::eval() {
	Sym*G = glob[val]; if (G) return G;					// lookup
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)	// nested eval
		(*it) = (*it)->eval();
	return this; }

Sym* Sym::eq(Sym*o) { glob[val]=new Var(val,o); return glob[val]; }
Sym* Sym::at(Sym*o) { return new Error(head()+"@"+o->head()); }
Sym* Sym::map(Sym*fn) { return new Error(fn->head()+"|"+head()); }

Sym* Sym::add(Sym*o) { return new Error(head()+"+"+o->head()); }
Sym* Sym::div(Sym*o) { return new Error(head()+"/"+o->head()); }
Sym* Sym::str() { return new Str(val); }

Sym* Sym::mk(Sym*o) { return o->mk(); }
Sym* Sym::mk() { return new Str(val); }
Sym* Sym::spx() { return str(); }
Sym* Sym::spx(Sym*o) { return o->spx(); }

Error::Error(string V):Sym("error",V) { yyerror(V); }

Var::Var(string V,Sym*o):Sym("var",V) { push(o); }
Sym* Var::eq(Sym*o) { pop(); push(o); return this; }
Sym* Var::spx() { return nest[0]->spx(); }

Str::Str(string V):Sym("str",V){}
Sym* Str::add(Sym*o) { return new Str(val+o->str()->val); }
string Str::head() { string S = "'";
	for (int i=0;i<val.length();i++)
		switch (val[i]) {
			case '\n': S+="\\n"; break;
			case '\t': S+="\\t"; break;
			default: S+=val[i];
		}
	return S+"'"; }

Vector::Vector():Sym("vector","[]"){}
string Vector::head() { return val; }
Sym* Vector::div(Sym*o) { Sym*V = new Vector();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) {
		V->push(*it); V->push(o); }
	V->pop();
	return V; }
Sym* Vector::str() { string S;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		S += (*it)->str()->val;
	return new Str(S); }
Sym* Vector::spx() { return div(glob["sp"])->str(); }
Sym* Vector::map(Sym*fn) { Sym* V = new Vector();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		V->push(fn->at(*it));
	return V; }

Op::Op(string V):Sym("op",V){}
Sym* Op::eval() {
	if (val=="~") return nest[0]; else Sym::eval();
	if (val=="=") return nest[0]->eq(nest[1]);
	if (val=="@") return nest[0]->at(nest[1]);
	if (val=="+") return nest[0]->add(nest[1]);
	if (val=="/") return nest[0]->div(nest[1]);
	if (val=="|") return nest[1]->map(nest[0]);
	return this; }

Fn::Fn(string V, FN F):Sym("fn",V) { fn=F; }
Sym* Fn::at(Sym*o) { return fn(o); }

Dep::Dep(Sym*A,Sym*B,Sym*C):Sym("dep",A->str()->val+"<-"+B->str()->val) {
	push(A); push(B); push(C); }
Sym* Dep::mk() { string S;
	S += nest[0]->spx()->val + " : " + nest[1]->spx()->val;
	S += "\n\t" + nest[2]->spx()->val;
	return new Str(S); }

map<string,Sym*> glob;
void glob_init() {
	//---------------------------------------- metainfo
	glob["MODULE"]	= new Str(MODULE);
	glob["ABOUT"]	= new Str(ABOUT);
	glob["AUTHOR"]	= new Str(AUTHOR);
	glob["LICENSE"]	= new Str(LICENSE);
	glob["GITHUB"]	= new Str(GITHUB);
	glob["LOGO"]	= new Str(LOGO);
	//---------------------------------------- constants
	glob["sp"]		= new Str(" ");
	glob["nl"]		= new Str("\n");
	glob["tab"]		= new Str("\t");
	//---------------------------------------- functions
	glob["mk"]		= new Fn("mk",Sym::mk);
	glob["spx"]		= new Fn("spx",Sym::spx);
}
