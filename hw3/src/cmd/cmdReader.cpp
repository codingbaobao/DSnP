/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Read the command from the standard input or dofile ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <cassert>
#include <cstring>
#include "util.h"
#include "cmdParser.h"
#include <sys/ioctl.h>

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class CmdParser
//----------------------------------------------------------------------
bool
CmdParser::readCmd(istream& istr)
{
   resetBufAndPrintPrompt();

   // THIS IS EQUIVALENT TO "readCmdInt()" in HW#2
   bool newCmd = false;
   while (!newCmd) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) {
         if (_dofile != 0)
            closeDofile();
         break;
      }
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : /* YOUR HW#2 TODO*/ if(_readBufPtr != _readBuf){ moveBufPtr(_readBufPtr-1); deleteChar(); } else { mybeep(); } break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : newCmd = addHistory();
                               cout << char(NEWLINE_KEY);
                               if (!newCmd) resetBufAndPrintPrompt();
                               break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: /* TODO */ moveBufPtr(_readBufPtr+1); break;
                               break;
         case ARROW_LEFT_KEY : /* TODO */ moveBufPtr(_readBufPtr-1); break;
                               break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : // THIS IS DIFFERENT FROM HW#2
                               { char tmp = *_readBufPtr; *_readBufPtr = 0;
                               string str = _readBuf; *_readBufPtr = tmp;
                               listCmd(str);
                               break; }
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
   return newCmd;
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // THIS IS YOUR HW#2 TODO
   if(ptr > _readBufEnd || ptr < _readBuf)
   {
      mybeep();
      return false;
   }
   // getting the terminal dimensions
   // http://stackoverflow.com/questions/1022957/getting-terminal-width-in-c
   winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);
   unsigned int w = ws.ws_col;
   char prompt[] = "cmd> ";
   int curOffset = _readBufPtr - _readBuf + strlen(prompt);
   int curY = curOffset/w;
   if(curY>0)
   {
      cout << "\e[" << curY << "F";
   }
   cout << "\e[0G";
   printPrompt();
   cout << _readBuf << " " << "\e[1D\e[0J";
   int totalOffset = _readBufEnd - _readBuf + strlen(prompt) + 1;
   int totalY = totalOffset/w, totalX = totalOffset%w;
   if(totalY>0&&totalX!=0)
   {
      cout << "\e[" << totalY << "F";
   }
   else if(totalY>1&&totalX==0)
   {
      cout << "\e[" << totalY-1 << "F";
   }
   int newOffset = ptr - _readBuf + strlen(prompt);
   int newX = newOffset%w, newY = newOffset/w;
   if(newY>0)
   {
      cout << "\e[" << newY << "E";
   }
   cout << "\e[" << newX+1 << "G";
   cout.flush();
   _readBufPtr = ptr;
   return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // THIS IS YOUR HW#2 TODO
   assert(*_readBufEnd == '\0');
   assert(_history.size()<INT_MAX);
   int _histSize = (int)_history.size();
   if(*_readBuf == '\0' || _readBufPtr == _readBuf + strlen(_readBuf)) // nothing to delete
   {
      mybeep();
      return false;
   }
   for(char* pos = _readBuf;pos <= _readBufEnd;pos++)
   {
      if(pos >= _readBufPtr)
      {
         *pos = *(pos+1);
      }
   }
   *_readBufEnd = '\0';
   _readBufEnd--;
   moveBufPtr(_readBufPtr); // just update the string
   if(_tempCmdStored && _historyIdx+1 == _histSize)
   {
      _history.pop_back();
      _tempCmdStored = false;
      _historyIdx = _histSize;
   }
   return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // THIS IS YOUR HW#2 TODO
assert(_history.size()<INT_MAX);
   int _histSize = (int) _history.size();
   if(_readBufEnd == _readBuf + READ_BUF_SIZE -1 ) // no more space to insert characters
   {
      mybeep();
      return;
   }
   for(char* pos = _readBufEnd+repeat;pos >= _readBufPtr+repeat;pos--)
   {
      *pos = *(pos-repeat);
   }
   for(char* pos = _readBufPtr;pos < _readBufPtr+repeat;pos++)
   {
      *pos = ch;
   }
   _readBufEnd+=repeat;
   moveBufPtr(_readBufPtr+repeat);
   if(_tempCmdStored && _historyIdx+1 == _histSize)
   {
      _history.pop_back();
      _tempCmdStored = false;
      _historyIdx = _histSize;
   }
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // THIS IS YOUR HW#2 TODO
strcpy(_readBuf, "");
   _readBufPtr = _readBufEnd = _readBuf;
   moveBufPtr(_readBuf);
}


// Reprint the current command to a newline
// cursor should be restored to the original location
void
CmdParser::reprintCmd()
{
   // THIS IS NEW IN HW#3
   cout << endl;
   char *tmp = _readBufPtr;
   _readBufPtr = _readBufEnd;
   printPrompt(); cout << _readBuf;
   moveBufPtr(tmp);
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // THIS IS YOUR HW#2 TODO
assert(index!=_historyIdx);
   assert(_history.size()<=INT_MAX);
   int _histSize = (int) _history.size();
   if(_histSize==0)
   {
      mybeep();
      return;
   }
   if(_historyIdx == _histSize && index < _histSize && !_tempCmdStored)
   {
      // need to save a temp version
      _tempCmdStored = true;
      _history.push_back(_readBuf);
   }
   if(_historyIdx == 0 && index < 0)
   {
      // already at top
      mybeep();
      return;
   }
   if(index < 0 )
   {
      index = 0;
   }
   if(index >= _histSize && _tempCmdStored)
   {
      index = _histSize - 1;
   }
   if( _historyIdx == _histSize && index >= _histSize)
   {  
      // already at bottom
      mybeep();
      return;
   }
   if(index >= _histSize)
   {
      index = _histSize-1;
   }
   strcpy(_readBuf, _history[index].c_str());
   _historyIdx = index;
   _readBufEnd = _readBuf + strlen(_readBuf);
   moveBufPtr(_readBufEnd);
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
// 6. Reset _readBufPtr and _readBufEnd to _readBuf
// 7. Make sure *_readBufEnd = 0 ==> _readBuf becomes null string
//
bool
CmdParser::addHistory()
{
   // THIS IS SIMILAR TO addHistory in YOUR HW#2 TODO
   // HOWEVER, return true if a new command is entered
   // return false otherwise
bool newCmd = false;
moveBufPtr(_readBufEnd); // for not pressing enter at the last line
   char trimmedStr[READ_BUF_SIZE] = "";
   char *pos1 = NULL, *pos2 = NULL;
   // pos1 will become the pointer to first non blank char, 
   // and pos2 will become the pointer to last non blank char

   for(pos1 = _readBuf;pos1 <= _readBufEnd-1 && *pos1==' ';pos1++){}
   if(pos1 == _readBufEnd) // empty string
   {
      return false;
   }
   if(_tempCmdStored)
   {
      _history.pop_back();
      _tempCmdStored = false;
   }
   for(pos2 = _readBufEnd-1;pos2 >= pos1 && *pos2==' ';pos2--){}
   for(char* pos = pos1;pos <= pos2;pos++)
   {
      trimmedStr[pos-pos1] = *pos;
   }
   *(pos2+1) = '\0';
   _history.push_back(trimmedStr);
   _historyIdx = _history.size();
   *_readBuf = '\0';
   _readBufEnd = _readBufPtr = _readBuf;
newCmd = true;
   
   return newCmd;
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
