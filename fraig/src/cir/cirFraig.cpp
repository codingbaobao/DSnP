/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
HashKey
CirMgr::keyGenerate(CirGate* thisGate){
	size_t a = 2* CirGate::getGatePtr(thisGate->fanin[0])->varno + CirGate::isInv(thisGate->fanin[0]);
	size_t b = 2* CirGate::getGatePtr(thisGate->fanin[1])->varno + CirGate::isInv(thisGate->fanin[1]);
	return HashKey(a, b);
}
/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
	HashMap<HashKey, CirGate*> _hashMap(getHashSize(max));
	vector<CirGate*> _DList;
	getDList(_DList);
	for(unsigned i = 0; i < _DList.size(); ++i){

		HashKey _key = keyGenerate(_DList[i]);
		//cout <<_DList[i]->varno<<" " <<_key()<<endl;
		if(!_hashMap.insert(_key, _DList[i])){

			CirGate* replaceG;
			_hashMap.query(_key, replaceG);
			cout << "Strashing: " << replaceG->varno << " merging " << _DList[i]->varno << "..." << endl;
			_DList[i]-> connectClean();
			_DList[i]->foutReCon(replaceG, false);
			cirAnd.erase(_DList[i]->varno);
			delete _DList[i];
		}

	}
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/