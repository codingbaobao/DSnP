/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
friend class CirMgr;

public:
   CirGate(unsigned l,unsigned v,GateType ggg)
	:lineno(l),varno(v),ref(0),gg(ggg),expression("") {}
   virtual ~CirGate() {}

	unsigned getLineNo() const { return lineno; }
	unsigned getVarNo() const { return varno; }
	string getTypeStr() const
	{
		switch(gg)
		{
			case UNDEF_GATE:
				return "UNDEF";
			case PI_GATE:
				return "PI";
			case PO_GATE:
				return "PO";
			case AIG_GATE:
				return "AIG";
			case CONST_GATE:
				return "CONST";
			default:
				return "haha";
		}
	}

   // Printing functions
   void reportGate() const;
   void reportFanin(int level);
   void reportFanout(int level);

private:
	vector<size_t> fanin;
	vector<size_t> fanout;
	unsigned lineno;
	unsigned varno;
	size_t ref;
	static size_t globalref;
	GateType gg;
	string expression;

	static CirGate* getGatePtr(size_t fan) { return (CirGate*)(fan & ~size_t(0x1)); }
	static bool isInv(size_t fan) { return (fan & 0x1); }

	bool isglobal() { return (ref==globalref); }
	bool notglobal() { return (ref!=globalref); }
	void setToglobal() { ref=globalref; }
	static void setglobal() { ++globalref; }

	void printGate(unsigned&) const;
	void reachable(IdList&);
	void subfan(int,unsigned&,bool);
};
#endif // CIR_GATE_H
