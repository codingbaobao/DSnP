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
#include <unordered_map>
#include "cirDef.h"

using namespace std;

class CirGate;
static unordered_map<unsigned, CirGate*> _Gate;
//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{

public:
   CirGate() {}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { return _GType; }
   unsigned getLineNo() const { return _line; }
   string getSymbol() const { return _symbol; }
   unsigned getID() const { return _id; }
   unsigned getFinAmount() const { return _in.size(); }
   unsigned getFoutAmount() const { return _out.size(); }
   unsigned getFinNo(int i = 0) const { return _in[i]; }
   unsigned getFoutNo(int i = 0) const { return _out[i]; }
   bool getMark(){ return mark; }
   void  setFin(unsigned id) { _in.push_back(id); } //in literal
   void  setFout(unsigned id) { _out.push_back(id); }//in literal
   void setSymbol(string str) { _symbol = str; }
   void setMark(bool state = true){mark = state;}


   // Printing functions
  // virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) ;
   void reportFanout(int level) ;
   void dfs(bool forward, bool neg, int level, int curLev);
   void dfs(unsigned &num); //for netlist
   void dfs(stringstream& aa);
   void markClean(); //call everytime after using dfs
   

protected:
  unsigned _id, _line;
  bool mark;
  string _GType, _symbol;
  vector<unsigned> _in, _out; //record literalID


};

class AIGGate : public CirGate
{
public:
  AIGGate(unsigned id, unsigned line);
  ~AIGGate()  {}
};

class POGate : public CirGate
{
public:
  POGate(unsigned id, unsigned line);
  ~POGate() {_Gate.erase(_id);}
};

class PIGate : public CirGate
{
public:
  PIGate(unsigned id, unsigned line);
  ~PIGate() {_Gate.erase(_id);}
};

class UNDEFGate : public CirGate
{
public:
  UNDEFGate(unsigned id);
  ~UNDEFGate() {_Gate.erase(_id);}
};

class CONSTGate : public CirGate
{
public:
  CONSTGate();
  ~CONSTGate() {_Gate.erase(_id);}
};
#endif // CIR_GATE_H
