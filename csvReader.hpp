#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>         
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
CSVReader() {
  _quota='"';
  _colsep=',';
  _rowsep='\n';
  _buf=nullptr;
  _is=&cin;
}

CSVReader(const char quota, const char colsep,const char rowsep)
  : _quota(quota)
  , _colsep(colsep)
  , _rowsep(rowsep) {
  _buf=nullptr;
  _is=&cin;
}

~CSVReader() {
  if (_buf) delete []_buf;
  if (_is && _is != &cin) delete _is;
}

void open(const char *filename) {
  _buf=new char[_blockLen*2+1];
  _buf[_blockLen*2]='\n';               // 防止行长超过_blockLen，检查"配对
  _buf2=_buf+_blockLen;
  if (filename[0]!='-')
    _is=new ifstream(filename,ifstream::binary);
  _is->read(_buf,_blockLen*2);          // 初始化两块数据，保证第一块的数据完整
  _dataBegin=_buf;                      // 新行头 
  _dataEnd=_buf+_is->gcount();          // 块中有效数据尾+1
  _isEnd=false;                         
}

bool nextCols(stringVector& cols) {
  if (_isEnd) return false;             // 行已去完  
  char *colBegin=_dataBegin;
  char *p=colBegin;
  char *pp=p-1;
  bool outQuota=true;
  while (*p) {
    if ( (*p ==_quota) && (*pp != '\\') ) {
      outQuota=!outQuota;
    }
    else if ( (*p ==_colsep) && (*pp != '\\') && outQuota ) {  //列结束
      *p='\0';      
      cols.push_back(string(colBegin));
      colBegin=p+1;
    }
    else if (*p ==_rowsep)  { 
      if ((*pp != '\\') && outQuota) {                         //行结束 
        *p='\0';         
        break;           
      } else {
        *p=' ';
        if (*pp == '\\') *pp=' ';  // 去掉列内回车
      }
    }
    p++;
    pp++;
  } 
  if ((p-_dataBegin) > _blockLen) {
    throw ("line too large, check quota pair!");
  }
  if (*pp=='\r') *pp='\0';      //去掉windows的CR
  cols.push_back(string(colBegin));   //最后一列
  _dataBegin=p+1;                     //下一行头
  if( _dataBegin >= _buf2) {                    //第一块读完 
    memcpy(_buf, _buf2, _blockLen);             //第二块移动到第一块
    _dataBegin -= _blockLen;              //调整数据范围
    _dataEnd -= _blockLen;                //_dataEnd为有效数据尾+1，csv有数据时，在第二块头
    if(!_is->eof()) {                     //从_is中补第二块数据    
      _is->read(_dataEnd,_blockLen);
      _dataEnd += _is->gcount();          
      *_dataEnd='\0';                     //补完数据后，给nextCols置尾标记 
    }
  }
  _isEnd=(_dataBegin>=_dataEnd);
  return true;
}

private:
  char _quota;
  char _colsep;
  char _rowsep;
  static const int _blockLen = 1<<20;  //1M
  char *_buf; 
  char *_buf2; 
  istream *_is;
  char *_dataBegin;
  char *_dataEnd;    
  bool _isEnd;
};
