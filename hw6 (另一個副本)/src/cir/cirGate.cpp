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

void CirGate::reachable(IdList &printgate)
{
	if(gg!=AIG_GATE)
		return;
	CirGate *AIG=getGatePtr(fanin[0]);
	if(AIG->notglobal())
	{
		AIG->setToglobal();
		AIG->reachable(printgate);
	}
	AIG=getGatePtr(fanin[1]);
	if(AIG->notglobal())
	{
		AIG->setToglobal();
		AIG->reachable(printgate);
	}
	printgate.push_back(varno);
}

void CirGate::printGate(unsigned &index) const
{
	//cout<<"printGate\n";
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

