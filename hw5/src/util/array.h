/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>
//#include <string.h>
using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) { }
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return *_node; }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { ++_node; return (*this); }
      iterator operator ++ (int) { 
      		iterator temp = *this;
      		++_node;
      		return temp; 
      }
      iterator& operator -- () { --_node; return (*this); }
      iterator operator -- (int) { 
      		iterator temp = *this;
      		--_node;
      		return temp; 
      }

      iterator operator + (int i) const { 
      		return iterator(_node + i); 
      }
      iterator& operator += (int i) { _node += i; return (*this); }

      iterator& operator = (const iterator& i) { _node = i._node; return (*this); }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { 
   		if(_capacity==0) return 0;
   		return iterator (_data);
   }
   iterator end() const {
   		if(_capacity==0) return 0;
   		return iterator(_data+_size);
   }
   bool empty() const { return !_size; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) {
   		if(!_capacity){ 
   			_capacity = 1;
   			_data = new T[_capacity];	
   		}
   		else if(_size == _capacity){
   			_capacity *= 2;
   			T* _newArea = new T[_capacity];
   			for(unsigned int i = 0; i < _size; i++)   *(_newArea+i) = *(_data + i);
   			//something wrong with size;
   			//memcpy( _newArea, _data, sizeof(*_data)); 
   			delete [] _data;
   			_data = _newArea;
   		}
   		_data[_size] = x;
   		_size++;
   }
   void pop_front() {
   		if(_size == 0) return;
   		_size--;
   		_data[0] = _data[_size];
   }
   void pop_back() {
   		if(_size == 0) return;
   		_size--;
   }
   bool erase(iterator pos) { 
   		if(_size == 0) return false;
   		_size--;
   		*pos = _data[_size];
   		return true; 
   }
   bool erase(const T& x) { 
   		unsigned int i = 0;
   		for(; i < _size; i++){
   			if(_data[i]==x){
   				_size--;
   				_data[i] = _data[_size];
   				return true;
   			}
   		}

   		return false; 
   }

   void clear() { _size = 0;}

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
