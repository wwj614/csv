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
typedef  vector<string> stringVector;
/**************************************************************************
当行长小于块长度时，用两块缓冲长度保证一行数据是完整的。
当行跨到第二块时，移动第二块数据到第一块，再读入第二块数据
当行读到\0时表示最后一行
当行跨过第二块时，行长已超过块长度，报错
***************************************************************************/
class CSVReader {
public:
CSVReader(const char *filename) {
  _buf=new char[_blockLen*2+1];
  _buf[_blockLen*2]='\n';               // 防止行长超过_blockLen，检查"配对
  _buf2=_buf+_blockLen;
  if (filename[0]=='-')
    _is=&cin;
  else
    _is=new ifstream(filename,ifstream::binary);
  _is->read(_buf,_blockLen*2);
  _dataBegin=_buf;
  _dataEnd=_buf+_is->gcount();    
  _rowBegin=_buf;
  _isEnd=false;  
}
~CSVReader() {
  if (_buf) delete []_buf;
  if (_is != &cin) delete _is;
}
bool nextCols(stringVector& cols) {
  if (_isEnd) return false;  
  char *colBegin=_rowBegin;
  char *p=colBegin;
  char *pp=p-1;
  bool outQuota=true;
  while (*p) {
    if ( (*p =='"') && (*pp != '\\') ) {
      outQuota=!outQuota;
    }
    else if ( (*p ==',') && (*pp != '\\') && outQuota ) {  //列结束
      *p='\0';      
      cols.push_back(string(colBegin));
      colBegin=p+1;
    }
    else if (*p =='\n')  { 
      if ((*pp != '\\') && outQuota) { //行结束 
        *p='\0';
        break;           
      } else {
        *p=' ';
        if (*pp == '\\') *pp=' ';  // 拼行
      }
    }
    p++;
    pp++;
  } 
  if ((p-_rowBegin) > _blockLen) {
    throw ("line too large, check quota pair!");
  }
  if (*pp=='\r') *pp='\0';      //去掉windows的CR
  cols.push_back(string(colBegin));   //最后一列
  _rowBegin=p+1;  
  if( _rowBegin >= _buf2) //第一块读完 _dataBegin为buf头
    nextBlock();
  _isEnd=(_rowBegin>=_dataEnd);
  return true;
}

void nextBlock() {
  memcpy(_buf, _buf2, _blockLen); //第二块移动到第一块
  _rowBegin -= _blockLen;
  _dataEnd -= _blockLen;              //_dataEnd为有效数据尾+1，csv有数据时，在第二块头
  if(!_is->eof()) {                    //从fin中补第二块数据    
    _is->read(_dataEnd,_blockLen);
    int cnt=_is->gcount();
    _dataEnd += cnt;                  //补完数据后 正好文件尾   在第二块头
                                      //           还有数据     在第二块尾+1
    if (cnt<_blockLen) {              //           文件结束     在第二块中一个位置       
      *_dataEnd='\0';                 //给nextCols置尾标记                
    }
  }
}

private:
  static const int _blockLen = 1<<20;  //1M
  char *_buf; 
  char *_buf2; 
  istream *_is;
  char *_dataBegin;
  char *_dataEnd;    
  char *_rowBegin;
  bool _isEnd;
};

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
