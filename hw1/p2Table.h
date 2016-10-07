#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>
#include<climits>

using namespace std;

class Row
{
public:
	const int operator[] (size_t i) const; // TODO
	int& operator[] (size_t i); // TODO
	void newArray(int i);

private:
   int  *_data;
};

class Table
{
public:
	const Row& operator[] (size_t i)const;
	Row& operator[](size_t i);
	bool read(const string&);//unDONE
	void print();
	void sum(size_t i);
	void ave(size_t i);
	void max(size_t i);
	void min(size_t i);
	void countDiff(size_t i);
	void add(int*);

private:
	vector<Row> _rows;
	size_t colAmount;
};
#endif // P2_TABLE_H
