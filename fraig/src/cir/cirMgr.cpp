/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <algorithm>

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError { 
   NO_ERROR,
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

enum readStatus { HEADER,INPUT,OUTPUT,AND,SYMBOL,END };
bool readline(readStatus&,CirParseError&,vector<string>&,fstream&);

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
         cerr << "[ERROR] Line " << lineNo << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
  lineNo=0;
  fstream AIGER(fileName.c_str(),ios::in);
  if(!AIGER)
    {cerr<<"Cannot open design "<<fileName<<"!!\n"; return false; }
  vector<string> token;
  CirParseError error=NO_ERROR;
  //header
  readStatus read=HEADER;
  readline(read,error,token,AIGER);
  int data[5];
  for(size_t i=0;i<5;++i)
    myStr2Int(token[i],data[i]);
  max=data[0];
  //input
  read=INPUT;
  int n;
  for(int i=0;i<data[1];++i)
  {
    readline(read,error,token,AIGER);
    myStr2Int(token[0],n);
    pin.push_back(new CirGate(lineNo,n/2,PI_GATE));
    cirPI.insert(CirPair(n/2,pin[i]));
  }
  //output
  read=OUTPUT;
  for(int i=0;i<data[3];++i)
  {
    readline(read,error,token,AIGER);
    myStr2Int(token[0],n);
    pout.push_back(new CirGate(lineNo,max+i+1,PO_GATE));
    cirPO.insert(CirPair(max+i+1,pout[i]));
    (pout[i]->fanin).push_back(size_t(n));
  }
  //and
  read=AND;
  for(int i=0;i<data[4];++i)
  {
    readline(read,error,token,AIGER);
    myStr2Int(token[0],n);
    //cout <<"add AND "<<n/2<<endl;
    CirGate* andgate=new CirGate(lineNo,n/2,AIG_GATE);
    cirAnd.insert(CirPair(n/2,andgate));
    myStr2Int(token[1],n);
    (andgate->fanin).push_back(size_t(n));
    //cout <<"in1 = " <<andgate->fanin[0]<<endl;
    myStr2Int(token[2],n);
    (andgate->fanin).push_back(size_t(n));
    //cout <<"in2 = " <<andgate->fanin[1]<<endl;
  }
  //symbol to the end
  read=SYMBOL;
  while(true)
  {
    readline(read,error,token,AIGER);
    if(read==END)
      break;
    myStr2Int(token[1],n);
    (token[0]=="i"?pin:pout)[n]->expression=token[2];
  }

  token.clear(); //reduce memory use

  //connect the circuit and perhaps need to create some undefined gate
  for(unsigned i = 0; i < pout.size();++i){
    CirGate* thisGate = pout[i];
    unsigned litID=thisGate->fanin[0];
    CirGate* fanIn=getGate(litID/2);
    if(fanIn==0)
      fanIn=getunD(litID/2);
    (thisGate->fanin)[0]=size_t(fanIn)+litID%2;
    (fanIn->fanout).push_back(size_t(thisGate)+litID%2);
  }
  //cout <<"AND MAP's size = " <<cirAnd.size()<<endl;
  for(CirMap::iterator i=cirAnd.begin(),e=cirAnd.end(); i!=e ;++i )
  {
    CirGate* thisGate=i->second;
    //cout <<"check AND "<< thisGate ->getVarNo() <<endl;
    unsigned litID=thisGate->fanin[0];
    //cout<<"in1 = "<<litID<<endl;
    CirGate* fanIn=getGate(litID/2);
    if(fanIn==0){
      //cout<<"undef\n";
      fanIn=getunD(litID/2);
    }
    thisGate->fanin[0]=size_t(fanIn)+litID%2;
    (fanIn->fanout).push_back(size_t(thisGate)+litID%2);

    litID=thisGate->fanin[1];
    //cout<<"in2 = "<<litID<<endl;
    fanIn=getGate(litID/2);
    if(fanIn==0){
      //cout<<"undef\n";  
      fanIn=getunD(litID/2);
    }
    thisGate->fanin[1]=size_t(fanIn)+litID%2;
    (fanIn->fanout).push_back(size_t(thisGate)+litID%2);
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
  size_t si=pin.size(),
         so=pout.size(),
         ss=cirAnd.size();
  cout<<"\nCircuit Statistics"
      <<"\n=================="
      <<"\n  PI"<<right<<setw(12)<<si
      <<"\n  PO"<<right<<setw(12)<<so
      <<"\n  AIG"<<right<<setw(11)<<ss
      <<"\n------------------"
      <<"\n  Total"<<right<<setw(9)<<ss+si+so<<endl;
}

void
CirMgr::printNetlist() const
{
  CirGate::setglobal();
  unsigned idx=0;
  for(size_t i=0,s = pout.size();i < s;++i){
    pout[i]->printGate(idx);
  };
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
  for(size_t i=0,s=pin.size();i<s;cout<<' '<<pin[i++]->varno);
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
  for(size_t i=0,s=pout.size();i<s;cout<<' '<<pout[i++]->varno);
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
  stringstream s_fl,s_und;
  //AND
  for(CirMap::const_iterator i=cirAnd.begin(),e=cirAnd.end();++i!=e;)
  {
    CirGate *thisGate=i->second;
    if(CirGate::getGatePtr((thisGate->fanin)[0])->gg==UNDEF_GATE ||
       CirGate::getGatePtr((thisGate->fanin)[1])->gg==UNDEF_GATE )
      s_fl<<" "<<i->first;
    if( (thisGate->fanout).empty() )
      s_und<<" "<<i->first;
  }
  //PO
  for(CirMap::const_iterator i=cirPO.begin(),e=cirPO.end();++i!=e;){
    CirGate *thisGate=i->second;
    if( CirGate::getGatePtr(thisGate->fanin[0])-> gg == UNDEF_GATE )
      s_fl<<" "<<i->first;
  }
  //PI
  for(CirMap::const_iterator i=cirPI.begin(),e=cirPI.end();++i!=e;){
    CirGate *thisGate=i->second;
    if( (thisGate->fanout).empty() )
      s_und<<" "<<i->first;
  }


  if(s_fl.str()!="")
    cout<<"Gates with floating fanin(s):"<<s_fl.str()<<endl;
  if(s_und.str()!="")
    cout<<"Gates defined but not used  :"<<s_und.str()<<endl;
}

void
CirMgr::writeAag(ostream& outfile) const
{
  size_t si=pin.size(),so=pout.size(), sa = 0;
  GateList dfslist;
  //AND
  getDList(dfslist);
  sa = dfslist.size();
  
  //header
  outfile<<"aag "<<max<<" "<<si<<" 0 "<< so <<" "<< sa <<endl; 
  //PI
  for(size_t i=0;i<si;outfile<<(pin[i++]->varno)*2<<endl); 
  //PO
  for(size_t i=0;i<so;++i) 
    outfile << (CirGate::getGatePtr(pout[i]->fanin[0])->varno)*2 + CirGate::isInv(pout[i]->fanin[0])<<endl;

  //AND
  for(size_t i = 0; i < sa; ++i){
    outfile << dfslist[i]-> varno*2 << " "
            << (CirGate::getGatePtr(dfslist[i]-> fanin[0])-> varno)*2 + CirGate::isInv(dfslist[i]-> fanin[0])
            << " "
            << (CirGate::getGatePtr(dfslist[i]-> fanin[1])-> varno)*2 + CirGate::isInv(dfslist[i]-> fanin[1])
            <<endl;
  
  }

  for(size_t i=0;i<si;++i) //PI symbol
    if(pin[i]->expression!="")
      outfile<<"i"<<i<<" "<<pin[i]->expression<<endl;
  for(size_t i=0;i<so;++i) //PO symbol
    if(pout[i]->expression!="")
      outfile<<"o"<<i<<" "<<pout[i]->expression<<endl;
  outfile<<"c\nha! ha! ha! ha! ha!\nthis homework is so hard QAQ"; //comment


  //ss_and.seekg(0, ios::end);
  //cout<< "size of ss_and: " << ss_and.tellg()/1024.0/1024.0 <<" Mb" <<endl;
  //cout<< "size of gatelist: " << sa/1024.0/1024.0 <<" Mb"<<endl;
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
  unsigned maxNum = 0;
  vector<unsigned> piList;
  GateList andList;
  CirGate::setglobal();
  g->dfsAll(piList, andList, maxNum);
  //header
  outfile << "aag " << maxNum << " " << piList.size() <<" 0 1 "<< andList.size() <<endl;
  //PI
  for(size_t i = 0; i < piList.size(); ++i)
    outfile << piList[i]*2 << endl;
  //PO
  cout << g->varno *2 << endl;
  //AND
  for(size_t i = 0; i < andList.size(); ++i){ 
    outfile << andList[i]-> varno*2 << " "
            << (CirGate::getGatePtr(andList[i]-> fanin[0])-> varno)*2 + CirGate::isInv(andList[i]-> fanin[0])
            << " "
            << (CirGate::getGatePtr(andList[i]-> fanin[1])-> varno)*2 + CirGate::isInv(andList[i]-> fanin[1])
            <<endl;
  }
  //SYMBOL
  outfile << "o0 " << g->varno << endl;
  outfile << "c\nWrite gate (" << g->varno << ") by .....bye........\n";
}

void
CirMgr::printFECPairs() const
{
}



bool readline(readStatus &S,CirParseError &E,vector<string> &token,fstream &f)
{
  ++lineNo;
  token.clear();
  string line;
  getline(f,line);
  if(line.empty())
    if(S==SYMBOL)
      { S=END; return true;}
  switch(S)
  {
    case HEADER: //header
      for(size_t k=0,i,j=3;k<5;++k)
      {
        j=line.find_first_of(' ',i=j+1);
        token.push_back(line.substr(i,j-i));
      }
      return true;

    case AND: //and
      for(size_t k=0,i,j=-1;k<3;++k)
      {
        j=line.find_first_of(' ',i=j+1);
        token.push_back(line.substr(i,j-i));
      }
      return true;

    case SYMBOL: //symbol
      if(line=="c")
        { S=END; return true; }
      token.push_back(line.substr(0,1));
      token.push_back(line.substr(1,line.find_first_of(' ')-1));
      token.push_back(line.substr(line.find_first_of(' ')+1));
      return true;

    default: //input, output
      token.push_back(line);
      return true;
  }
}