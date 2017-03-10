/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include "util.h"
using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
	friend class BSTree<T>;
	friend class BSTree<T>::iterator;
   // TODO: design your own class!!
	BSTreeNode(const T& d, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0, BSTreeNode<T>* p = 0):
		_data(d), _lChild(l), _rChild(r), _parent(p) {}

	T 				 _data;
	BSTreeNode<T>* _lChild;
	BSTreeNode<T>* _rChild;
	BSTreeNode<T>* _parent;

	


};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
	BSTree(){}
	~BSTree(){  clear(); }

	class iterator
	{
		friend class BSTree;
	public:
   		iterator(BSTreeNode<T>* n= NULL, BSTreeNode<T>* r=NULL): _node(n), _getRoot(r){}
    	iterator(const iterator& i): _node(i._node), _getRoot(NULL) {}
    	~iterator() {} // Should NOT delete _node

    	const T& operator * () const{ return _node -> _data; }

    	T& operator * () { return _node -> _data; }

    	iterator& operator ++ (){
    		//if it's the largest, return null
    		BSTreeNode<T>* _nextNode = succ(_node);
    		if(_nextNode ==NULL){
    			_nextNode = _node -> _parent;
    			while(_nextNode != NULL){
    				if(_nextNode -> _data < _node -> _data){
    					_nextNode = _nextNode -> _parent;
    				}
    				else
    					break;
    			}
    		}

    		_node = _nextNode;
 			return(*this);
    	}

    	iterator operator ++ (int){
    		iterator temp = (*this);
    		++(*this);
    		return temp;
    	}

    	iterator& operator -- (){
    		if(_node==0){
    			_node = findMAX(_getRoot);
    			return (*this);
    		}
    		BSTreeNode<T>* _prevNode = pred(_node);
    		if(_prevNode == NULL){
    			_prevNode = _node -> _parent;
    			while(_prevNode !=NULL){
    				if(_prevNode->_data >= _node->_data)
    					_prevNode = _prevNode -> _parent;
    				else
    					break;
    			}
    		}
    		_node = _prevNode;
    		return(*this);
    	}

    	iterator operator -- (int){
    		iterator temp = (*this);
    		--(*this);
    		return temp;
    	}

    	iterator& operator = (const iterator& i){_node = i._node; return (*this);}

     	bool operator != (const iterator& i) const { return _node != i._node; }

     	bool operator == (const iterator& i) const { return _node == i._node; }

    private:
    	BSTreeNode<T>* _node;
    	BSTreeNode<T>* _getRoot;
    	BSTreeNode<T>* findMIN(BSTreeNode<T>* t) const{
			if(t == NULL ||t -> _lChild == NULL)
				return t;
			else
				return findMIN(t ->_lChild);	
		}

		BSTreeNode<T>* findMAX(BSTreeNode<T>* t) const{
			if(t == NULL ||t -> _rChild == NULL)
				return t;
			else
				return findMAX(t ->_rChild);	
		}

		//next larger
		BSTreeNode<T>* succ(BSTreeNode<T>* t) const {
			if(t -> _rChild == NULL) 
				return NULL;
			else 
				return findMIN(t -> _rChild);
		}

		//prev smaller
		BSTreeNode<T>* pred(BSTreeNode<T>* t) const {
			if(t -> _lChild == NULL) 
				return NULL;
			else 
				return findMAX(t -> _lChild);
		}


   	};

   	iterator begin() const {
   		if(_root == NULL) return iterator();
   		iterator i;
   		i._node = i.findMIN(_root);
   		i._getRoot = _root;
   		return i;
   	}
   	iterator end() const {
   		if(_root == NULL) return iterator();
   		return iterator(0,_root);
   	}
   bool empty() const {return (_root==NULL);}
   size_t size() const {return _size;} 
   void insert(const T& x) {
   		BSTreeNode<T>* newNode = new BSTreeNode<T>(x);
   		if(empty()){
   			_root = newNode;
   			_size++;
   			return;
   		}

   		BSTreeNode<T>* parentNode = _root;
   		
   		while(true){
   			if(parentNode->_data < x){
   				if(parentNode ->_rChild == NULL){
   					parentNode ->_rChild = newNode;
   					newNode ->_parent = parentNode;
   					_size++;
   					return;
   				}
   				parentNode = parentNode -> _rChild;
   			}
   			else{
   				if(parentNode ->_lChild ==NULL){
   					parentNode ->_lChild = newNode;
   					newNode ->_parent = parentNode;
   					_size++;
   					return;
   				}
   				parentNode = parentNode -> _lChild;
   			}

   		}
   }
   void pop_front() {
   	erase(begin());
   }
   void pop_back() {
   		erase(--end());
   }
   bool erase(iterator pos) {
   		if(pos._node ==NULL) return false;
   		//left right not emqpty
   		if(pos._node ->_lChild != NULL && pos._node ->_rChild != NULL){
   			BSTreeNode<T>* replaceOne = pos.succ(pos._node);

   			if(replaceOne-> _parent == pos._node){
   				pos._node -> _lChild -> _parent = replaceOne;
   				replaceOne ->_lChild = pos._node->_lChild;
   				replaceOne ->_rChild = NULL;
   			}
   			else{

				pos._node -> _lChild -> _parent = replaceOne;
				pos._node -> _rChild -> _parent = replaceOne;

				replaceOne ->_lChild = pos._node->_lChild;
				replaceOne ->_rChild = pos._node->_rChild;

				if(replaceOne ->_parent->_data >= replaceOne->_data)
					replaceOne -> _parent ->_lChild = NULL;
				else
					replaceOne -> _parent ->_rChild = NULL;

			}

				if(pos._node == _root){
					replaceOne ->_parent = NULL;
					_root = replaceOne;
				}
				else{
					replaceOne ->_parent = pos._node->_parent;
					if(replaceOne ->_parent->_data >= replaceOne->_data)
						replaceOne -> _parent ->_lChild = replaceOne;
					else
						replaceOne -> _parent ->_rChild = replaceOne;
				}
				
				//check
				//if(replaceOne ->_lChild==replaceOne) replaceOne->_lChild = NULL;
				//if(replaceOne ->_rChild==replaceOne) replaceOne->_rChild = NULL;
   		}
   		//right empty
   		else if(pos._node ->_lChild != NULL && pos._node ->_rChild == NULL){
   			if(pos._node == _root){
   				_root = pos._node ->_lChild;
   				_root->_parent = NULL;	
   			}
   			else{
   				pos._node->_lChild->_parent = pos._node ->_parent;
   				if(pos._node->_parent->_data >= pos._node->_data)
   					pos._node->_parent->_lChild = pos._node->_lChild;
   				else
   					pos._node ->_parent ->_rChild = pos._node->_lChild;
   			}

   		}
   		//left empty
   		else if(pos._node ->_lChild == NULL && pos._node ->_rChild != NULL){
   			if(pos._node == _root){
   				_root = pos._node -> _rChild;
   				_root->_parent = NULL;	
   			}
   			else{
   				pos._node->_rChild->_parent = pos._node ->_parent;
   				if(pos._node->_parent->_data >= pos._node->_data)
   					pos._node->_parent->_lChild = pos._node->_rChild;
   				else
   					pos._node ->_parent ->_rChild = pos._node->_rChild;
   			}
   		}
   		//left right empty
   		else{
   			if(pos._node == _root)
   				_root = NULL;	
   			else{
   				if(pos._node->_parent->_data >= pos._node->_data)
   					pos._node->_parent->_lChild = NULL;
   				else
   					pos._node ->_parent ->_rChild = NULL;
   			}
   		}
   		delete pos._node;
   		--_size;
   		return true;
   }
   bool erase(const T& x) {
   		if(_root == NULL) return false;
   		iterator pos(begin());
   		iterator _endPos(end());
   		while(pos._node->_data !=x){
   			++pos;
   			if(pos == _endPos) return false;
   		}
   		return erase(pos);

   }
   void clear() { makeEmpty(_root); _root=NULL; _size = 0;}
   void sort(){}
   void print(){}

private:
	BSTreeNode<T>* _root;
	size_t _size;
	void makeEmpty(BSTreeNode<T>* t){
		if(t!=NULL){
			makeEmpty(t->_lChild);
			makeEmpty(t->_rChild);
			delete t;
			t = NULL;
		}
	}

};

#endif // BST_H
