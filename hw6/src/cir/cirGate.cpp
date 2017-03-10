/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;


// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
	stringstream ss;
	ss << "= " << _GType << "(" << _id << ")";
	if(_symbol.size()) ss << "\"" << _symbol << "\"";
	ss << ", line " << _line;
	cout << "==================================================" << endl;
	cout.width(49);
	string str = ss.str();
	cout << left << str
		 << "=" << endl
		 << "==================================================" << endl;
	
}

void
CirGate::reportFanin(int level) 
{
   assert (level >= 0);
   dfs(false, false, level ,0);
   markClean();

}

void
CirGate::reportFanout(int level) 
{
   assert (level >= 0);
   dfs(true, false, level ,0);
   markClean();
}

void
CirGate::dfs(bool forward = true, bool neg = false, int level = 0, int curLev = 0)
{
	mark = true; //mark for reported;
	for(int i = 0; i < curLev; i++) cout << "  ";
	if(neg) cout << "!";
	cout << _GType << " " << _id;

	if( _GType == "UNDEF" || _GType == "CONST" || level == curLev){
		cout << endl;
		return;
	}
	if(_GType == "PO" && forward){
		cout << endl;
		return;
	}
	if(_GType == "PI" && !forward){
		cout << endl;
		return;
	}
	unsigned nextID = (forward ? _out[0] : _in[0])/2;
	if( _Gate[nextID]->getMark()){ //reported in previous
		cout << " (*)" << endl;
		return;
	}
	cout << endl;
	

	vector<unsigned>::iterator it;
	vector<unsigned>::iterator it_end;
	if(forward){
		it = _out.begin();
		it_end = _out.end();
	}
	else{
		it = _in.begin();
		it_end = _in.end();
	}
	for(; it!=it_end; it++){
		CirGate* target = _Gate[(*it) / 2];
		target -> dfs (forward, (*it) % 2, level, curLev+1);

	}

}

void
CirGate::markClean()
{
	unordered_map<unsigned, CirGate*>::iterator it = _Gate.begin();
	for(; it!=_Gate.end(); ++it)
		it->second->setMark(false);
}

void
CirGate::dfs(unsigned &num)
{
	mark = true; //mark
	if(_GType == "PI"){
		cout << "[" << num << "] " << _GType << "  " << _id ;
		if(_symbol.size()) cout << " (" << _symbol <<")";
		cout << endl;
		num++;
		return; 
	}
	if(_GType == "CONST"){
		cout << "[" << num << "] " << _GType << _id << endl;
		num++;
		return; 
	}
	if(_GType == "UNDEF"){
		return;
	}
	for(unsigned i = 0; i < _in.size(); i++){
		CirGate* g = _Gate[_in[i]/2];
		if( g -> getMark()) continue;
		g -> dfs(num);
	}

	cout << "[" << num << "] " << _GType << " ";
	if(_GType == "PO") cout << " ";
	cout << _id << " " ;

	if( _Gate[_in[0]/2]->_GType == "UNDEF" ) 
		cout << "*";
	if(_in[0]%2) 
		cout << "!";
	cout << _in[0]/2;
	if(_GType == "PO"){
		if(_symbol.size())
			cout << " (" << _symbol << ")";
		cout << endl;
		num++;
		return;
	}
	cout << " ";
	if( _Gate[_in[1]/2]->_GType == "UNDEF" )
		cout << "*";
	if(_in[1]%2)
		cout << "!";
	cout  << _in[1]/2 << endl;
	num++;
}
void
CirGate::dfs(stringstream& aa){
	mark = true;
	for(unsigned i = 0; i < _in.size(); i++){
		CirGate* g = _Gate[_in[i]/2];
		if(g -> getMark()) continue; //reported
		if(g->_GType == "UNDEF" || g ->_GType == "PI" || g -> _GType == "CONST") continue;
		g -> dfs(aa);
	}
	aa << _id*2 << " " << _in[0] << " " << _in[1] <<endl;

}





AIGGate::AIGGate(unsigned i, unsigned l){
	_id = i;
	_line = l;
	_GType = "AIG";
	_Gate[i] =(CirGate*)this;
}
POGate::POGate(unsigned i, unsigned l){
	_id = i;
	_line = l;
	_GType = "PO";
	_Gate[i] = (CirGate*)this;
}
PIGate::PIGate(unsigned i, unsigned l){
	_id = i;
	_line = l;
	_GType = "PI";
	_Gate[i] = (CirGate*)this;
}
UNDEFGate::UNDEFGate(unsigned i){
	_id = i;
	_line = 0 ;
	_GType = "UNDEF";
	_Gate[i] =(CirGate*)this;
}
CONSTGate::CONSTGate(){
	_id = 0;
	_line = 0;
	_GType = "CONST";
	_Gate[0] =(CirGate*) this;
	
}
