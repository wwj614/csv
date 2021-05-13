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
���г�С�ڿ鳤��ʱ�������黺�峤�ȱ�֤һ�������������ġ�
���п絽�ڶ���ʱ���ƶ��ڶ������ݵ���һ�飬�ٶ���ڶ�������
���ж���\0ʱ��ʾ���һ��
���п���ڶ���ʱ���г��ѳ����鳤�ȣ�����
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
  _buf[_blockLen*2]='\n';               // ��ֹ�г�����_blockLen�����"���
  _buf2=_buf+_blockLen;
  if (filename[0]!='-')
    _is=new ifstream(filename,ifstream::binary);
  _is->read(_buf,_blockLen*2);          // ��ʼ���������ݣ���֤��һ�����������
  _dataBegin=_buf;                      // ����ͷ 
  _dataEnd=_buf+_is->gcount();          // ������Ч����β+1
  _isEnd=false;                         
}

bool nextCols(stringVector& cols) {
  if (_isEnd) return false;             // ����ȥ��  
  char *colBegin=_dataBegin;
  char *p=colBegin;
  char *pp=p-1;
  bool outQuota=true;
  while (*p) {
    if ( (*p ==_quota) && (*pp != '\\') ) {
      outQuota=!outQuota;
    }
    else if ( (*p ==_colsep) && (*pp != '\\') && outQuota ) {  //�н���
      *p='\0';      
      cols.push_back(string(colBegin));
      colBegin=p+1;
    }
    else if (*p ==_rowsep)  { 
      if ((*pp != '\\') && outQuota) {                         //�н��� 
        *p='\0';         
        break;           
      } else {
        *p=' ';
        if (*pp == '\\') *pp=' ';  // ȥ�����ڻس�
      }
    }
    p++;
    pp++;
  } 
  if ((p-_dataBegin) > _blockLen) {
    throw ("line too large, check quota pair!");
  }
  if (*pp=='\r') *pp='\0';      //ȥ��windows��CR
  cols.push_back(string(colBegin));   //���һ��
  _dataBegin=p+1;                     //��һ��ͷ
  if( _dataBegin >= _buf2) {                    //��һ����� 
    memcpy(_buf, _buf2, _blockLen);             //�ڶ����ƶ�����һ��
    _dataBegin -= _blockLen;              //�������ݷ�Χ
    _dataEnd -= _blockLen;                //_dataEndΪ��Ч����β+1��csv������ʱ���ڵڶ���ͷ
    if(!_is->eof()) {                     //��_is�в��ڶ�������    
      _is->read(_dataEnd,_blockLen);
      _dataEnd += _is->gcount();          
      *_dataEnd='\0';                     //�������ݺ󣬸�nextCols��β��� 
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
