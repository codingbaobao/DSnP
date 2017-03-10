/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include "cirGate.h"
#include "myHashMap.h"

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
typedef map<unsigned,CirGate*> CirMap;
typedef pair<unsigned,CirGate*> CirPair;

public:
   CirMgr():max(0) { constGate = new CirGate(0,0,CONST_GATE); }
  //const0: line=0, var=0
   ~CirMgr()
  {
    for(CirMap::iterator i=cirAnd.begin(),e=cirAnd.end();i!=e;delete (i++)->second);
    for(GateList::iterator i=pin.begin(),e=pin.end();i!=e;delete *(i++));
    for(GateList::iterator i=pout.begin(),e=pout.end();i!=e;delete *(i++));
    for(CirMap::iterator i=unDefined.begin(),e=unDefined.end();i!=e;delete (i++)->second);
  }
  
   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const
  {
    if(gid == 0) return constGate;
    CirMap::const_iterator i = cirAnd.find(gid);
    if( i!=cirAnd.end() ) return i -> second;
    i = cirPO.find(gid);
    if( i!=cirPO.end() ) return i -> second;
    i = cirPI.find(gid);
    if( i!=cirPI.end() ) return i -> second;
    return 0;
  }
   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

private:
  ofstream           *_simLog;
  GateList pin; //primary input
  GateList pout; //primary output
  CirMap cirAnd; //and gate gate
  CirMap cirPO;
  CirMap cirPI;
  CirGate* constGate;
  CirMap unDefined; //all gates that appear at least once but remain undefined
  unsigned max;

  //consider the condition that there exists any undefined gate used by more than one gate
  CirGate* getunD(unsigned gid) //create an undefined gate if it does not exists
  {
    CirMap::iterator i=unDefined.find(gid);
    if(i!=unDefined.end())
      return i->second;
    return (unDefined.insert(CirPair(gid,new CirGate(0,gid,UNDEF_GATE)))).first->second;
    //undefined: line=0, has var
  }
  //not include PO PI UNDEF  CONST
  void getDList(GateList& _dfsList)const{
    CirGate::setglobal();
    for(size_t i=0,s=pout.size();i<s;++i)
    {
      CirGate* AIG=CirGate::getGatePtr(pout[i]->fanin[0]);
      if(AIG->notglobal())
      {
        AIG->setToglobal();
        AIG->dfsAnd(_dfsList);
      }
    }
  }

  HashKey keyGenerate(CirGate* thisGate);
};

#endif // CIR_MGR_H
