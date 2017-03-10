/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator( int numB = 0, int bP = 0, vector<Data>* l = 0)
               :_numBuckets(numB), bucketPos(bP), index(0), _list(l) {}

      ~iterator() {}

      const Data& operator * () const { return _list[bucketPos][index]; }

      Data& operator * (){ return _list[bucketPos][index]; }

      iterator& operator ++ (){
         if(_list[bucketPos][index] == _list[bucketPos].back()){
            do{ ++bucketPos; }
            while(_list[bucketPos].empty() && bucketPos < _numBuckets);
            index = 0;
         }
         else
            index++;
         return *this;
      }

      iterator operator ++ (int){
         iterator temp = (*this);
         ++(*this);
         return temp;
      }

      iterator& operator -- (){
         --index;
         if(index < 0 ){
            --bucketPos;
            for(; bucketPos >= 0; --bucketPos){
               if(!_list[bucketPos].empty())
                  index = _list[bucketPos].size() - 1;
               break;
            }
            if(bucketPos < 0){  //equal to end
               bucketPos = _numBuckets;
               index = 0;
            }
         }

         return this;
      }

      iterator operator -- (int){
         iterator temp = (*this);
         --(*this);
         return temp;
      }

      iterator& operator = (const iterator& i){ return i; }

      bool operator == (const iterator& i) const{ return (bucketPos == i.bucketPos && index == i.index);}

      bool operator != (const iterator& i) const{ return (bucketPos != i.bucketPos && index != i.index); }

   private:
      int _numBuckets, bucketPos, index; 
      vector<Data>* _list;
   };


   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      for(int i = 0; (unsigned)i < _numBuckets; i++){
         if(! _buckets[i].empty())
            return iterator(_numBuckets, i, _buckets );
      }
      return end(); //equal to end
   }
   // Pass the end
   //define end : bucketPos = _numBuckets
   //             index = 0 
   iterator end() const {
      return iterator(_numBuckets, _numBuckets, _buckets); 
   }
   // return true if no valid data
   bool empty() const {
      for(int i = 0; i < _numBuckets; i++){
         if(!_buckets[i].empty())
            return false;
      }
      return true;
   }
   // number of valid data
   size_t size() const {
      int s = 0;
      for(int i = 0; i < _numBuckets; i++)
            s += _buckets[i].size();
      return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      int n = bucketNum(d);
      for(int m = 0; (unsigned)m < _buckets[n].size(); m++){
         if(_buckets[n][m] == d)
            return true;
      }
      return false;
   }

   // query  if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      int n = bucketNum(d);
      for( int m = 0; (unsigned)m < _buckets[n].size(); m++){
         if(_buckets[n][m] == d){
            d = _buckets[n][m];
            return true;
         }
      }
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      int n = bucketNum(d);
      for(int m = 0; (unsigned)m < _buckets[n].size(); m++){
         if(_buckets[n][m] == d){
            _buckets[n][m] = d;
            return true;
         }
      }
      _buckets[n].push_back(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      int n = bucketNum(d);
      for(int m = 0; (unsigned)m < _buckets[n].size(); m++){
         if(_buckets[n][m] == d)
            return false;
      }
      _buckets[n].push_back(d);
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
      int n = bucketNum(d);
      for(int m = 0; (unsigned)m < _buckets[n].size(); m++){
         if(_buckets[n][m] == d){
            _buckets[n].erase(_buckets[n].begin() + m);
            return true;
         }
      }
      return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
