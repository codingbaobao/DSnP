/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/


#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <map>
using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
//static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}
 // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate*
   CirMgr::getGate(unsigned gid) const
   {
    if(_Gate[gid]->getTypeStr() == "UNDEF") return NULL;
    return _Gate[gid];
   }


/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
  ifstream file(fileName.c_str(), ios::in);
  if(!file.is_open()) {
      cerr << "Error: cannot open file \"" << fileName << "\"!!" << endl;
      return false;
  }

  string header;
  file >> header;
  if(header == "aag") file >> m >> i >> l >> o >> a;
  else{
    errMsg = header;
    parseError(ILLEGAL_IDENTIFIER);
    return false;
  }
  lineNo++;
  //add PI
  for(unsigned n = 0; n < i; ++n){
    unsigned e ;
    file >> e;
    CirGate* g = new PIGate(e/2, ++lineNo);
    _Gate[e/2] = g;

   // g  =  _Gate[e/2];
   // cout << g -> getID() <<" " << g -> getTypeStr() <<endl;
  }
  //add PO
  for(unsigned n = 1; n <= o; ++n){
    unsigned e ;
    file >> e;
    CirGate* g = new POGate(m+n, ++lineNo);
    _Gate[m+n] = g;
    g -> setFin(e);
    //g  =  _Gate[m+n];
    //cout << g -> getID() <<" " << g -> getTypeStr() <<endl;
    if(_Gate.count(e/2)){
      _Gate[e/2] -> setFout((m+n)*2 + e%2);
    }
    else{ //temporary setup connected obj.
      if(e/2 == 0){  // CONST --> PO
        g = new CONSTGate();
        _Gate[0] = g;
        g -> setFout((m+n)*2 + e%2);
      }
      else{
        g  = new UNDEFGate(e/2);
        _Gate[e/2] = g;
        g -> setFout((m+n)*2 + e%2);
      }
    }

  }
  //add AIG
  for(unsigned n = 0; n < a; ++n){
    unsigned aig, in[2];
    file >> aig >> in[0] >>in[1];
    //check UNDEF
    //if exist delete it and get fanout info
    vector<unsigned> outInfo;
    bool check = _Gate.count(aig/2);
    if(check){
      CirGate* temp = _Gate[aig/2];
      for(unsigned k = 0; k <( temp -> getFoutAmount() ); ++k)
        outInfo.push_back(temp ->getFoutNo(k));
      delete temp; 
    }
    //create AIG
    CirGate* g = new AIGGate(aig/2, ++lineNo);
    _Gate[aig/2] = g;
    g -> setFin(in[0]);
    g -> setFin(in[1]);
    if(check){
      for(unsigned k = 0; k < outInfo.size() ; ++k)
        g -> setFout(outInfo[k]);
    }
    //check input gate
    for(unsigned k = 0; k < 2; ++k){
      if(_Gate.count(in[k]/2)){
        _Gate[in[k]/2] -> setFout(aig + in[k]%2);
      }
      else{ //temporary setup connected obj.
        if(in[k]/2 == 0){  // CONST --> PO
          g = new CONSTGate();
          _Gate[0] = g;
          g -> setFout(aig + in[k]%2);
        }
        else{
          g  = new UNDEFGate(in[k]/2);
          _Gate[in[k]/2] = g;
          g -> setFout(aig + in[k]%2);
        } 
      }
    }
  }
  //add symbol & comment
  while(file >> header){
    if(header == "c"){
      getline(file, _comment); //let iterator go to the nextline first
      getline(file, _comment);
     // cout << "comment  "<<_comment <<endl;
      break;
    }
    if(header.substr(0, 1) == "i"){
      int num;
      myStr2Int(header.substr(1, 1), num);
      unordered_map<unsigned, CirGate*>::iterator it = _Gate.begin();
      for(; it!= _Gate.end();++it){
        CirGate* g = it -> second;
        if(g -> getLineNo() == (unsigned)(num+2)){  //because PI start from line 2
          file >> header;
          g -> setSymbol(header);
        }
      }
    }
    else if(header.substr(0, 1) == "o"){
      int num;
      myStr2Int(header.substr(1, 1), num);
      unordered_map<unsigned, CirGate*>::iterator it = _Gate.begin();
      for(; it!= _Gate.end();++it){
        CirGate* g = it -> second;
        if(g -> getLineNo() == (num+2+i)){  //because PO start from line 2 + PI
          file >> header;
          g -> setSymbol(header);
        }
      }
    }
  }

  return true;

}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
  cout << "Circuit Statistics" << endl
       << "==================" << endl
       << "  PI"  << setw(12) << right << i << endl
       << "  PO"  << setw(12) << right << o << endl
       << "  AIG" << setw(11) << right << a << endl
       << "------------------" << endl
       << "  Total" << setw(9) << right << i+o+a << endl;
}

void
CirMgr::printNetlist() const
{
  cout << endl;
  unsigned num = 0;
  unordered_map<unsigned, CirGate*>::iterator it = _Gate.begin();
  for(unsigned count=0; it != _Gate.end(); ++it){
    if(it -> second ->getTypeStr() == "PO")
      it -> second -> dfs(num);
    if(count == o) break;
  }
  //cout <<"done"<<endl;
  _Gate[0]-> markClean();
}

void
CirMgr::printPIs() const
{
  cout << "PIs of the circuit:";
  map<unsigned, unsigned> arrange;
  unordered_map<unsigned, CirGate*>::iterator it = _Gate.begin();
  for(unsigned count=0; it != _Gate.end(); ++it){
    if(it -> second ->getTypeStr() == "PI"){
      arrange[it -> second ->getLineNo()] = it -> first;
      count++;
    }
    if(count == i) break;
  }
  map<unsigned, unsigned>::iterator ait = arrange.begin();
  for(; ait != arrange.end(); ++ait)
    cout << " " << ait -> second;
}

void
CirMgr::printPOs() const
{
  cout << "POs of the circuit:";
  map<unsigned, unsigned> arrange;
  unordered_map<unsigned, CirGate*>::iterator it = _Gate.begin();
  for(unsigned count=0; it != _Gate.end(); ++it){
    if(it -> second ->getTypeStr() == "PO"){
      arrange[it -> second ->getLineNo()] = it -> first;
      count++;
    }
    if(count == o) break;
  }

  map<unsigned, unsigned>::iterator ait = arrange.begin();
  for(; ait != arrange.end(); ++ait)
    cout << " " << ait -> second;
}

void
CirMgr::printFloatGates() const  
{
 vector<unsigned> floatFin, unUse;

  unordered_map<unsigned, CirGate*>::iterator it = _Gate.begin();
  for(; it!=_Gate.end() ;it++){
    CirGate* g =it -> second;

    if(g->getTypeStr() == "PO"){
      CirGate* next_g =_Gate[ (g ->getFinNo(0))/2];
      if(next_g -> getTypeStr() == "UNDEF")
        floatFin.push_back(g -> getID());
    } 

    else if(g -> getTypeStr() == "PI"){
      if(g -> getFoutAmount() == 0)
        unUse.push_back(g -> getID());
      
    }
    else if(g -> getTypeStr() == "AIG"){
      if(g -> getFoutAmount() == 0) 
        unUse.push_back(g -> getID());
      CirGate* next_g1 = _Gate[ (g ->getFinNo(0))/2];
      CirGate* next_g2 = _Gate[ (g ->getFinNo(1))/2];

      if(next_g1 -> getTypeStr() == "UNDEF" || next_g2 -> getTypeStr() == "UNDEF")
        floatFin.push_back(g -> getID());
    }
  }

  if(!floatFin.empty()){
    sort(floatFin.begin(), floatFin.end());
    cout << "Gates with floating fanin(s):";
    for(unsigned n = 0; n < floatFin.size(); ++n)
      cout << " " << floatFin[n];
    cout << endl;
  }
  if(!unUse.empty()){
    sort(unUse.begin(), unUse.end());
    cout << "Gates defined but not used  :";
    for(unsigned n = 0; n < unUse.size(); ++n)
      cout << " " << unUse[n];
    cout << endl;
  }

}

void
CirMgr::writeAag(ostream& outfile) const
{
  stringstream ss, aa; //ss for symbol, aa for aig
  outfile << "aag" << " " << m << " " << i << " " << l << " " << o << " " << a << endl;


  //PI&PO&AIG
  map<unsigned, CirGate*> arrange;
  unordered_map<unsigned, CirGate*>::iterator it = _Gate.begin();
  for(unsigned ci=0, co=0; it!=_Gate.end(); it++){
    CirGate* g = it -> second;
    if(g -> getTypeStr() == "PI") {
      ++ci;
      arrange[g ->getLineNo()] = g;
    }
    if(g -> getTypeStr() == "PO") {
      ++co;
      arrange[g ->getLineNo()] = g;
      g = _Gate[g ->getFinNo(0)/2];
      g -> dfs(aa);
    }
    if(ci == i && co == o) break;
  }
  it -> second -> markClean();
 
  map<unsigned, CirGate*>::iterator ait = arrange.begin();

  //print PI & get PI symbol
  for(unsigned count = 0; count < i; ++ait, ++count){
    CirGate* g = ait -> second;
    string symbol = g ->getSymbol();
    if(!symbol.empty()) ss << "i" << count << " " << symbol << endl;
    outfile << (g -> getID())*2 <<endl;
  }
  //print PO & get PO symbol
  for(unsigned count = 0; count < o; ++ait, ++count){
    CirGate* g = ait -> second;
    string symbol = g ->getSymbol();
    if(!symbol.empty()) ss << "o" << count << " " << symbol << endl;
    outfile << g -> getFinNo(0) <<endl;
  }

  //print AIG
  string s = aa.str();
  outfile << s;
  //print symbol
  s = ss.str();
  outfile << s;
  if(!_comment.empty())
    outfile << "c" << endl
            << _comment;
  else
    outfile << "c" << endl
            << "Output by Max" << endl;
}
