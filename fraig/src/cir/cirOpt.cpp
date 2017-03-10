/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep(){
	CirGate::setglobal();
	for(CirMap::iterator i=cirPO.begin(); i != cirPO.end();++i)
		i->second->dfsCheck();
	for(CirMap::iterator i=++cirAnd.begin(); i != cirAnd.end();++i){
		CirGate* thisGate = i -> second;
		if(thisGate->notglobal()){
			thisGate-> connectClean();
			cout <<"Sweeping: "<<thisGate->getTypeStr() <<"("<<thisGate->varno<<")  removed...\n";
			delete thisGate;
			cirAnd.erase(i);
		}
	}
	for(CirMap::iterator i=unDefined.begin(); i != unDefined.end();++i){
		CirGate* thisGate = i -> second;
		if(thisGate->notglobal()){
			thisGate-> connectClean();
			cout <<"Sweeping: "<<thisGate->getTypeStr() <<"("<<thisGate->varno<<")  removed...\n";
			delete thisGate;
			unDefined.erase(i);
		}
	}


}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
// Optimize only performs on the gates in the DFS list
// Situation:
// 1 & gate      -> turn to gate
// 0 & gate      -> turn to 0
// 1 & 0         -> turn to 0
// 1 & 1         -> turn to 1
// 0 & 0         -> turn to 0
// gate1 & gate1 -> turn to gate1
// gate1 & !gate1-> turn to 0
void
CirMgr::optimize()
{
	//mark the gates in dfs list
	GateList _dfsList;
	getDList(_dfsList);

	for(size_t i = 0; i <_dfsList.size(); ++i){

		CirGate* fan_0 =  CirGate::getGatePtr( _dfsList[i]-> fanin[0] );
		CirGate* fan_1 =  CirGate::getGatePtr( _dfsList[i]-> fanin[1] );
 		//both CONST/PI/UNDEF/AND
		if(fan_0 == fan_1){
			unsigned char inv = CirGate::isInv(_dfsList[i]-> fanin[0]) + 
								CirGate::isInv(_dfsList[i]-> fanin[1]);

			

			if(inv == 1) //one inverted
			{
				cout <<"Simplifying: 0 merging "<< _dfsList[i]->varno << "...\n";
				_dfsList[i] -> connectClean();
				_dfsList[i] -> foutReCon(constGate, 0);
				if(fan_0->getTypeStr() == "UNDEF" && fan_0->fanout.empty() )
				{
					cout << "UNDEF(" << fan_0->varno <<") is removed...\n";
					unDefined.erase(fan_0->varno);
					delete fan_0;
				}
			}
			else //no inverted or both inverted
				cout <<"Simplifying: "<< fan_0->varno <<" merging ";
				if(inv)	cout<<"!";
				cout << _dfsList[i]->varno << "...\n";

				_dfsList[i] -> connectClean();
				_dfsList[i] -> foutReCon( fan_0, bool(inv));

			cirAnd.erase(_dfsList[i]->varno);
			delete _dfsList[i];
		}
		//one const one gate
		else if((fan_0 -> getTypeStr() == "CONST" && fan_1-> getTypeStr() != "CONST") ||
			    (fan_0 -> getTypeStr() != "CONST" && fan_1-> getTypeStr() == "CONST") )
		{
			bool determine =(fan_0 -> getTypeStr() == "CONST")? 0:1;
			bool inv =CirGate::isInv(_dfsList[i]-> fanin[determine]);
			//CONST=1
			if(inv){
				inv = CirGate::isInv(_dfsList[i]-> fanin[!determine]);
				fan_0 = determine? fan_0: fan_1;

				cout <<"Simplifying: "<< fan_0->varno <<" merging ";
				if(inv)	cout<<"!";
				cout << _dfsList[i]->varno << "...\n";

				_dfsList[i] -> connectClean();
				_dfsList[i] -> foutReCon(fan_0, inv);
			}
			//CONST=0
			else{
				cout <<"Simplifying: 0 merging "<< _dfsList[i]->varno << "...\n";
				_dfsList[i] -> connectClean();
				_dfsList[i] -> foutReCon(constGate, 0);
				fan_0 = determine?fan_0:fan_1; 
				if(fan_0->getTypeStr() == "UNDEF" && fan_0->fanout.empty() ){
					cout << "UNDEF(" << fan_0->varno <<") is removed...\n";
					unDefined.erase(fan_0->varno);
					delete fan_0;
				}
			}

			cirAnd.erase(_dfsList[i]->varno);
			delete _dfsList[i];
		}
		
	}


}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
