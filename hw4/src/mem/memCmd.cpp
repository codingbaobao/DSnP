/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
  vector<string> options;
  if (!CmdExec::lexOptions(option, options)){
    return CMD_EXEC_ERROR;
  }
  int optionSize  = options.size();
  if(!optionSize){
    return CmdExec::errorOption(CMD_OPT_MISSING,"");
  }

  int numObjects = 0;

  if(myStr2Int( options[0], numObjects) && numObjects > 0 ){
    if(optionSize == 1)
      mtest.newObjs((size_t)numObjects);
    else if(optionSize == 2){
      if(!myStrNCmp("-Array", options[1], 2))
        return CmdExec::errorOption(CMD_OPT_MISSING,options[1]);
      else
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
    }
    else if(optionSize >= 3){
      if(!myStrNCmp("-Array", options[1], 2)){
        int arraySize = 0;
        if(myStr2Int( options[2], arraySize) && arraySize > 0 )
          if(optionSize > 3)
            return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
          else
            mtest.newArrs((size_t)numObjects, (size_t)arraySize);
        else 
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
      }
      else 
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
    }
  }
  else if(!myStrNCmp("-Array", options[0], 2)){
    if(optionSize == 1)
      return CmdExec::errorOption(CMD_OPT_MISSING,options[0]);
    else if(optionSize == 2){
      int arraySize = 0;
      if(myStr2Int( options[1], arraySize) && arraySize > 0 )
        return CmdExec::errorOption(CMD_OPT_MISSING,"");
      else 
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
    }
    else if(optionSize >= 3){
      int arraySize = 0;
      if(myStr2Int( options[1], arraySize) && arraySize > 0 ){
        if(myStr2Int( options[2], numObjects) && numObjects > 0 ){
          if(optionSize == 3)
            mtest.newArrs((size_t)numObjects, (size_t)arraySize);
          else
            return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
        }
        else
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
      }
      else
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
    }



  }

  else
    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);


  return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
  vector<string> options;
  if (!CmdExec::lexOptions(option, options))  
    return CMD_EXEC_ERROR;
  if(options.empty()) 
    return CmdExec::errorOption(CMD_OPT_MISSING, "");
  int optionSize = options.size();

  if(!myStrNCmp("-Index", options[0], 2)){

    int objId;

    if(optionSize == 1) 
      return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
    if( !myStr2Int( options[1], objId) || objId < 0) 
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
    if(mtest.getObjListSize() <= (size_t)objId){
      cerr<<"Size of object list ("<<mtest.getObjListSize()<<") is <= "<<objId<<"!!"<<endl;
      return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
    }

    if(optionSize == 2) mtest.deleteObj((size_t)objId);
    else if(!myStrNCmp("-Array", options[2], 2)){
      if(optionSize == 3)
        mtest.deleteArr((size_t)objId);
      else
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
    }
    else 
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
  }

  else if(!myStrNCmp("-Random", options[0], 2)){
    int numRandId;
    if(optionSize ==1) 
      return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
    if( !myStr2Int( options[1], numRandId) || numRandId <= 0) 
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);

    if(optionSize ==2){
      if(!mtest.getObjListSize()){
        cerr<<"Size of object list is 0!!"<<endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
      }
      for(int i = 0; i < numRandId;i++)
        mtest.deleteObj( rnGen(mtest.getObjListSize()) );
    }
    else if(!myStrNCmp("-Array", options[2], 2)){
      if(!mtest.getArrListSize()){
        cerr<<"Size of array list is 0!!"<<endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
      }
      for(int i = 0; i < numRandId;i++)
        mtest.deleteArr( rnGen(mtest.getArrListSize()) );
    }
    else 
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
  }

  else if(!myStrNCmp("-Array", options[0], 2)){
    if(optionSize == 1)
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

    else if(!myStrNCmp("-Random", options[1], 2)){
      int numRandId;
      if(optionSize == 2)
        return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
      else if(myStr2Int( options[2], numRandId) && numRandId > 0){
        if(optionSize == 3){
          for(int i = 0; i < numRandId;i++)
            mtest.deleteArr( rnGen(mtest.getArrListSize()) );
        }
        else
          return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
      }
      else
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
        

    }
    else if(!myStrNCmp("-Index", options[1], 2)){
      int objId;
      if(optionSize == 2)
        return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
      else if(myStr2Int( options[2], objId) && objId > 0){
        if(optionSize == 3){
          for(int i = 0; i < objId;i++)
            mtest.deleteArr(objId);
        }
        else
          return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
      }
      else
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);

    }
    else 
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
  }

  else 
    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);

  return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


