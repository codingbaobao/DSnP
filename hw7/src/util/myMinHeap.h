/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }
   //bool empty() const { return _data.empty(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) {
      _data.push_back(d);
      int t = _data.size();
      while(t > 1){
        int p = t /2;
        if(d.getLoad() >= _data[p-1].getLoad())   //because vector idx start from 0
          break;
        _data[t-1] = _data[p-1];
        t = p;
      }
      _data[t-1] = d;

   }

   void delMin() {
      delData(0);
   }

   void delData(size_t i) {
    int p = i+1, t = 2*p, n = _data.size();
    if(p == n) {
      _data.pop_back();
      return;
    }
    
    while(t < n){
      if( t < n){ //has right child
        if( _data[t].getLoad() < _data[t-1].getLoad())
          ++t;
      }
      if( _data[n-1].getLoad() < _data[t-1].getLoad())
        break;
      _data[p-1] = _data[t-1];
      p = t;
      t = 2*p;
    }
    _data[p-1] = _data[n-1];
    _data.pop_back();
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
