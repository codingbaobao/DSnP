/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include <sstram>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
	for(unsigned int i = 0;i < r.size()-1;i++){
		if(r[i]==INT_MAX) os<<". ";
		else	os<<r[i]<<" ";
	}
	if(r[i]==INT_MAX) os<<".";
	else	os<<r[i];
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
	for(unsigned int i=0;i<t.nRows();i++){
    if(i!=0) os<<endl;
		for(unsigned int j=0;j<t.nCols();j++){
			if(t[i][j]==NULL) os<<setw(6<<right<<".";
			else  os<<setw(6)<<right<<t[i][j];
		}
	}
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
  
  while(true){
    getline(fs,str,'\r');
    if(fs.eof()) break;
    string str;
    bool endRow = false;
    DBRow tempRow;
    while(!endRow){
      int commaPos = 0;
      commaPos = str.find(",");
      if(commaPos == string::npos){
        endRow = true;
      }
      else if(currentRead == commaPos){ //empty value
          tempRow.addData(INT_MAX);
          str = str.substr(commaPos+1);
      }
      else{
        int temp;
        stringstream ss;
        ss << str.substr(0,commaPos)
        ss >> temp;
        tempRow.addData(temp);
        str.substr(commaPos+1);
      }
    }
    t.addRow(tempRow);
  }
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
	_data.earse(c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
  _table.clear();

}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.

  for(int  i = 0, n = d.size(); i<n; i++) _table[i].addData(d[i]);
   //add empty for remaining col;
  for(int i =d.size(), n = _table.size(); i < n; i++) _table[i].addData(INT_MAX);
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
  _table.earse(c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
  vector<float> max;
  for(size_t i = 1, n = _table.size(); i < n; i++){
      if(_table[i][c]!=INT_MAX){
        if(max.empty()) max.push_back(_table[i][c]);
        else{
          if(_table[i][c] > max[0]) max[0] = _table[i][c];
        }
      }
  }
  return max[0];
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
  float  min = _table[0][c];
  for(size_t i = 1, n = _table.size(); i < n; i++){
      if((float)_table[i][c] < max) max = _table[i][c];
  }
  return min;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
  float sum=0;
  for(size_t i = 0, n = _table.size(); i < n; i++){
    if(_table[i][c]==INT_MAX) continue;
    sum+=_table[i][c];
  }
  return sum;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
  vector<int>counter;
  for (int i = 0, n = _table.size(); i < n; i++) {
    bool same = false;
    if(_table[i][c]==INT_MAX) continue;
    else{
      if(std::find(counter.begin(), vector.end(), _table[i][c]) == counter.end() )//not find
        counter.push_back(_table[i][c]);
    }
    return counter.size();
  }

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
  float sum=0;
  for(size_t i = 0, n = _table.size(); i < n; i++){
    if(_table[i][c]==INT_MAX) continue;
    sum+=_table[i][c];
  }
  return sum/(float)_table.size();
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
  for(int i = 0, n = _table.size()-1; i < n;i++){
    if(_table[i][c] == INT_MAX) ccout<<". ";
    else cout<<_table[i][c]<<" ";
  }
  n = _table.size();
  if(_table[n][c] == INT_MAX) ccout<<".";
  else cout<<_table[n][c];
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

