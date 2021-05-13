#include <iostream>
#include <fstream>
#include <string>

#include "csvReader.hpp"
using namespace std;
typedef  vector<string> stringVector;
template<class InputIterator>
string join (InputIterator first, InputIterator last, string delimited) {
  if (first==last) return "";
  string s = *(first++);
  while (first!=last) {
	s += (delimited + *first);  
    ++first;
  }
  return s;
}


int main(int argc, char **argv) {  
  char * fileName=argv[1]; 
  CSVReader csv;
  csv.open(fileName);
  stringVector cols;
  while(csv.nextCols(cols)){
    cout << join(cols.begin(),cols.end(),"#") <<endl; 
    cols.clear();
  }
  return 1;
}
