#include <iostream>
#include <string>
#include "p2Table.h"
#include<sstream>
#include<cstdlib>
#include<climits>
#include<algorithm>



int main()
{
   Table table;

   // TODO: read in the csv file
   string csvFile;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   if (table.read(csvFile))
      cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   else exit(-1); // csvFile does not exist.

   // TODO read and execute commands
   while (true) {
	   stringstream ss;
	   string str;
	   getline(cin, str);
	   string command;
           size_t par =str.find(" ")+1;
	   command = str.substr(0,str.find(" "));
		if(command=="SUM"){
			command = str.substr(par,string::npos);
			ss<<command;
			int i;
			ss>>i; 
			table.sum(i);
			ss.clear();
		}
		else if(command=="AVE"){
			command = str.substr(par,string::npos);
			ss<<command;
			int i;
			ss>>i;
			table.ave(i);
			ss.clear();
		}
		else if(command=="MIN"){
			command = str.substr(par,string::npos);
			ss<<command;
			int i;
			ss>>i;
			table.min(i);
			ss.clear();
		}
		else if(command=="MAX"){
			command = str.substr(par,string::npos);
			ss<<command;
			int i;
			ss>>i;
			table.max(i);
			ss.clear();
		}
		else if(command=="PRINT"){
			table.print();
		}
		else if(command=="COUNT"){
			command = str.substr(par,string::npos);
			ss<<command;
			int i;
			ss>>i;
			table.countDiff(i);
			ss.clear();
		}
		else if(command=="ADD"){
			int parr;
			int colnum=count(str.begin(),str.end(),' '); //column num
			int* k=new int[colnum];
			string parameter = str.substr(par,string::npos);
			for(int i=0;i<colnum;i++){
				int cursorr = parameter.find(" "); 
				string temp = parameter.substr(0,cursorr);
				parameter = parameter.substr(cursorr+1,string::npos);
				if(temp=="-")
					k[i]=INT_MAX;
				else{
					ss<<temp;
					ss>>parr;
					k[i]=parr;
					ss.clear();
				}
			}
			table.add(k);
		}


   }
}
