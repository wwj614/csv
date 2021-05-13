#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <unordered_map>   // std::unordered_map
#include <vector>          // std::vector
#include <algorithm>
#include <memory>
#include <regex>
#include <exception>


using namespace std;

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
  CSVReader csv(fileName);
  stringVector cols;
  while(csv.nextCols(cols)){
    cout << join(cols.begin(),cols.end(),"#") <<endl; 
    cols.clear();
  }
  return 1;
}
