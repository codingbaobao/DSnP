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

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
size_t CirGate::globalref=0;

void
CirGate::reportGate() const
{
	stringstream s;
	s<<getTypeStr()<<"("<<varno<<")";
	if(expression!="")
		s<<"\""<<expression<<"\"";
	s<<", line "<<lineno;
	cout<<"==================================================\n"
	    <<"= "<<left<<setw(47)<<setfill(' ')<<s.str()<<"=\n"
	    <<"==================================================\n";
}

void
CirGate::reportFanin(int level)
{
   assert (level >= 0);
	setglobal();
	unsigned space=1;
	subfan(level,space,true);
}

void
CirGate::reportFanout(int level)
{
   assert (level >= 0);
	setglobal();
	unsigned space=1;
	subfan(level,space,false);
}

void CirGate::dfsCheck(){
	if(notglobal()){
		setToglobal();
		if(!fanin.empty()){
			for(size_t i=0; i < fanin.size(); ++i)
				getGatePtr(fanin[i])->dfsCheck();
		}
	}
}


void CirGate::connectClean(){
	
	for(size_t i = 0; i < fanin.size(); ++i){
		vector<size_t>&fan = getGatePtr(fanin[i]) ->fanout;
		if(fan.size() == 1)
			fan.clear();
		else{
			for(vector<size_t>::iterator it = fan.begin(); it!=fan.end();){
				if( getGatePtr(*it)->varno == varno){
					fan.erase(it);
					it==fan.end();
				}
				else
					++it;
			}
		}
	}
}

void CirGate::foutReCon(CirGate* targetGate, bool inv){
	for(size_t i = 0; i < fanout.size(); ++i){
		vector<size_t>&fan = getGatePtr(fanout[i]) ->fanin;
		if(fan.size() == 1){
			inv = inv ^ isInv(fan[0]);
			fan.clear();
			fan.push_back(size_t(targetGate)+inv);
		}
		else{
			for(vector<size_t>::iterator it = fan.begin(); it!=fan.end();){
				if( getGatePtr(*it)->varno == varno){
					inv = inv ^ isInv(*it);
					fan.erase(it);
					fan.push_back(size_t(targetGate)+inv);
					it==fan.end();
				}
				else
					++it;
			}

		}
		targetGate->fanout.push_back( size_t(getGatePtr(fanout[i])) + inv );
	}
}

void CirGate::subfan(int lv,unsigned &space,bool in)
{
	vector<size_t> &fan=in?fanin:fanout;
	cout<<getTypeStr()<<" "<<varno;
	if(lv>0)
	{
		if(isglobal())
			cout<<" (*)\n";
		else
		{
			cout<<endl;
			if(fan.size())
			{
				setToglobal();
				for(size_t i=0,s=fan.size();i<s;++i)
				{
					for(unsigned j=0;j<space;++j)
						cout<<"  ";
					if(isInv(fan[i]))
						cout<<"!";
					getGatePtr(fan[i])->subfan(lv-1,++space,in);
				}
			}
		}
	}
	else
		cout<<endl;
	--space;
}


void CirGate::dfsAnd(GateList& list){
	if(gg!=AIG_GATE)
		return;
	CirGate *AIG=getGatePtr(fanin[0]);
	if(AIG->notglobal())
	{
		AIG->setToglobal();
		AIG->dfsAnd(list);
	}
	AIG=getGatePtr(fanin[1]);
	if(AIG->notglobal())
	{
		AIG->setToglobal();
		AIG->dfsAnd(list);
	}
	list.push_back(this);
	
}

void CirGate::dfsAll(vector<unsigned>& pilist, GateList& andlist, unsigned& max){
	if(gg == PI_GATE){
		pilist.push_back(varno);
		if(varno > max) max = varno;
		return;
	}
	if(gg != AIG_GATE) return;

	CirGate *AIG=getGatePtr(fanin[0]);
	if(AIG->notglobal())
	{
		AIG->setToglobal();
		AIG->dfsAll(pilist, andlist, max);
	}
	AIG=getGatePtr(fanin[1]);
	if(AIG->notglobal())
	{
		AIG->setToglobal();
		AIG->dfsAll(pilist, andlist, max);
	}
	andlist.push_back(this);
	if(varno > max) max = varno;
	
}

void CirGate::printGate(unsigned &index) const
{
	stringstream S;
	for(size_t i=0,s=fanin.size();i<s;++i)
	{
		CirGate *fanIn=getGatePtr(fanin[i]);
		S<<" ";
		if(fanIn->notglobal())
		{
			if(fanIn->gg==UNDEF_GATE)
				S<<"*";
			else
			{
				fanIn->setToglobal();
				fanIn->printGate(index);
			}
		}
		if(isInv(fanin[i]))
			S<<"!";
		S<<(fanIn->varno);
	}
	cout<<"["<<index++<<"] "<<left<<setw(4)<<getTypeStr()<<varno;
	cout<<S.str();
	if(expression!="")
		cout<<" ("<<expression<<")";
	cout<<endl;
}