#include "p2Table.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<string>
#include<algorithm>
#include<climits>
using namespace std;

// Implement member functions of class Row and Table here


const int Row::operator[](size_t i)const {
	return _data[i];
}

int& Row::operator[](size_t i) {
	return _data[i];

}

void Row::newArray(int i) {
	_data = new int[i];
	for(int j=0;j<i;j++){
		_data[j]=INT_MAX;
	}

}

const Row& Table::operator[] (size_t i)const {
	return _rows[i];
}

Row& Table::operator[](size_t i) {
	return _rows[i];
}

bool Table::read(const string& csvFile)
{
	stringstream ss;
	fstream fs;
	fs.open(csvFile.c_str(), ios::in);
	if (!fs)
		return false;
	string str;
	getline(fs, str,'\r');
	//cout<<str<<endl;
	size_t i = -1;
	size_t cursor = 0;
	int row = 0;
	size_t col = 0;
	colAmount =count(str.begin(), str.end(), ',')+1;
        //cout<<"col amount "<<colAmount<<endl;
	_rows.resize(row + 1);
	_rows[row].newArray(colAmount);
	while (true) {
		
               // cout<<"start"<<endl;
		i = str.find(",", i + 1);
               // cout<<"find position "<<i<<endl;
               // cout<<"cursorposition "<<cursor<<endl;
                bool avoidDouble=false;
		if (i == string::npos) {
                      //  cout<<"end of col"<<endl;
                        if(cursor==str.size()+1){
			_rows[row][col] = INT_MAX;
			//cout <<"end&empty "<<_rows[row][col]<<endl;
                        avoidDouble=true;
                        }
			else{
                        string temp = str.substr(cursor, str.size()+1);
			ss << temp;
			ss >> _rows[row][col];
			ss.clear();
                       // cout << temp<<" "<<_rows[row][col]<<endl;
                        }
			getline(fs, str,'\r');
			if (fs.eof()){
                                break;
			} 
                      //  cout<<str<<endl;
			
			row++;
			_rows.resize(row + 1);
			_rows[row].newArray(colAmount);
			//for(unsigned int u=0;u<colAmount;u++){
				//cout<<_rows[row][u]<<" ";
			//}
			col = 0;
			i = -1;
                        cursor = 0;
                        continue;

		}

		if (cursor == i&&!avoidDouble) {  //for empty element
			_rows[row][col] = INT_MAX;
                      //  cout <<"empty "<<_rows[row][col]<<endl;
		}
		else {
			string temp = str.substr(cursor, i - cursor);
			ss << temp;
			ss >> _rows[row][col];
                	ss.clear();
                      //  cout << temp<<" "<<_rows[row][col]<<endl;

		}
		cursor = i + 1;
		col++;
	}
	_rows.erase(_rows.end()-1);
	fs.close();
	return true;
} 															

void Table::print()
{

	for (int i = 0 ; (unsigned)i < _rows.size(); i++) {
		for (int j = 0; (unsigned)j < colAmount; j++) {
			if(_rows.at(i)[j]==INT_MAX)
				cout << setw(4) << " ";
			else
				cout << setw(4) << _rows.at(i)[j];
		}
		cout << endl;
	}
}

void Table::sum(size_t i)
{
	int summation = 0;
	for (int j = 0; (unsigned)j < _rows.size(); j++) {
		if(_rows.at(j)[i]==INT_MAX) continue;
		summation += _rows.at(j)[i];
	}
	cout << "The summation of data in column #" << i << " is " << summation << endl;
}

void Table::ave(size_t i)
{
	size_t num =_rows.size();
	float summation = 0;
	for (int j = 0; (unsigned)j < _rows.size(); j++) {
		if(_rows.at(j)[i]==INT_MAX){
			num--;
			continue;
		}
		summation += _rows.at(j)[i];
	}
	float average = summation / num;
	cout << "The average of data in column #" << i << " is " << fixed << setprecision(1) << average << endl;
}

void Table::max(size_t i)
{	
	int maximum;
	if(_rows.at(0)[i]==INT_MAX)  maximum = INT_MIN;
	else maximum = _rows.at(0)[i];
	for (int j = 1; (unsigned)j < _rows.size(); j++) {
		if(_rows.at(j)[i]==INT_MAX) continue;
		if (_rows.at(j)[i] > maximum)
			maximum = _rows.at(j)[i];
	}
	cout << "The maximum of data in column#" << i << " is " << maximum << endl;
}

void Table::min(size_t i)
{
	int minimum = _rows.at(0)[i];
	for (int j = 1; (unsigned)j < _rows.size(); j++) {
		if(_rows.at(j)[i]==INT_MAX) continue;
		if (_rows.at(j)[i] < minimum)
			minimum = _rows.at(j)[i];
	}
	cout << "The minimum of data in column#" << i << " is " << minimum << endl;
}

void Table::countDiff(size_t i)
{
	vector<int>counter;
	for (int j = 0; (unsigned)j < _rows.size(); j++) {
		bool same = false;
		if(_rows.at(j)[i]==INT_MAX)	continue;
		for (int k = 0; (unsigned)k < counter.size(); k++) {
			
			if (_rows.at(j)[i] == counter[k]) {
				same = true;

			}
		}
		if (!same) {
			counter.push_back(_rows.at(j)[i]);
		}
		same = false;
	}
	int num = counter.size();
	cout << "The distinct count of data in column #" << i << " is " << num << endl;
}

void Table::add(int* i)
{
	int temp = _rows.size();
	_rows.resize(temp + 1);
	_rows.at(temp).newArray(colAmount);
	for (int j = 0; (unsigned)j < colAmount; j++) {
		_rows.at(temp)[j] = i[j];
	}
}

