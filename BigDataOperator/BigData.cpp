#define _CRT_SECURE_NO_WARNINGS
#include"BigData.h"
using namespace std;

BigData::BigData(INIT64  value)
	:_value(value)
{
	char buf[128] = { 0 };
	sprintf(buf,"%lld",_value);
	_strData =buf;
}


BigData::BigData(string strData)
	:_value(0)
	, _strData("+0")
{
	char* pData = (char*)strData.c_str();
	while (isspace(*pData))              //���˿հ�
		pData++;
	if ('\0' == *pData)
		return;

	char symbol = *pData;
	if (('+' == *pData) || ('-' == *pData))    //���˷���
		pData++;
	else if (isdigit(*pData))
		symbol = '+';
	else
		return;

	while('0' == *pData)         //����ǰ���0
		pData++;
	if ('\0' == *pData)
		return;

	_strData.resize(strlen(pData)+1);
	_strData[0] = symbol;
	int count = 1;
	while (*pData>='0' && *pData<='9')
	{
		_value = _value * 10 + *pData - '0';
		_strData[count++] = *pData;
		pData++;
	}
	if (*pData)
		_strData.resize(count);

	if (symbol == '-')
		_value = 0 - _value;
}

ostream& operator<<(ostream& os, const BigData& b)
{
	const char *str = b._strData.c_str();
	if ('+' == *str)
		cout << str + 1;
	else
		cout << str;
	return os;
}

bool BigData::isINIT64overflow(const string& strData)
{
	const string max_value= "+9223372036854775807";
	const string min_value= "-9223372036854775808";
	if (strData.size() < max_value.size())
		return false;
	else if (strData.size() == max_value.size())
	{
		if (('+'==strData[0]&&strData<=max_value)\
			||('-'==strData[0]&&strData>=min_value))
			return false;
	}
	return true;
}


BigData BigData::operator+(BigData b)
{
	if (!isINIT64overflow(_strData) && !isINIT64overflow(b._strData))   //�����û���
	{
		if (_strData[0] != b._strData[0])     //���ֱ����ӣ��������
			return _value + b._value;
		else if ('+' == _strData[0] && '+' == b._strData[0])
		{
			INIT64 max_value = 9223372036854775807;
			if (max_value - _value >= b._value)       //����������Ӳ������
			{
				return _value + b._value;
			}
		}
		else if ('-' == _strData[0] && '-' == b._strData[0])
		{
			INIT64 min_value =0-9223372036854775808;
			if (min_value - _value <= b._value)   //����������Ӳ������
			{
				return _value + b._value;
			}
		}
	}

	if (_strData[0] == b._strData[0])      //ͬ�����
		return BigData(Add(_strData, b._strData));
	else                               
	{
		string Left = _strData;       //������ת�����������
		string Right = b._strData;
		Left[0] = '+';
		Right[0] = '+';
		if ('-' == _strData[0])        
			swap(Left,Right);
		return BigData(Sub(Left,Right));
	}

}

BigData BigData::operator-(BigData b)
{
	if (!isINIT64overflow(_strData) && !isINIT64overflow(b._strData))   //�����û���
	{
		if (_strData[0] == b._strData[0])     //ͬ��ֱ��������������
			return _value -b._value;
		else if ('+' == _strData[0] && '-' == b._strData[0])     //����������
		{
			INIT64 max_value = 9223372036854775807;
			if ((max_value - _value) >= (0-b._value))     
			{
				return _value - b._value;
			}
		}
		else if ('-' == _strData[0] && '+' == b._strData[0])      //����������
		{
			INIT64 min_value = 0 - 9223372036854775808;
			if ((min_value - _value) <= (0-b._value))        
			{
				return _value + b._value;
			}
		}
	}

	if (_strData[0]==b._strData[0])      //ͬ�����
		return BigData(Sub(_strData,b._strData));  
	else								//������
	{
		string left = _strData;
		string right = b._strData;

		//��������ת��Ϊ���
		left[0] = '+';
		right[0] = '+';
		string strTemp = Add(left, right);
		if('-' == _strData[0])
			strTemp[0] = '-';
		return BigData(strTemp);
	}
}


BigData BigData::operator * (BigData b)
{
	if ("+0" == _strData || "+0" == b._strData)
		return BigData(0);

	if (!isINIT64overflow(_strData) && !isINIT64overflow(b._strData))
	{
		INIT64 max_value = 9223372036854775807;
		INIT64 min_value = 0 - 9223372036854775808;
		if (_strData[0] == b._strData[0])
		{
			if (('+' == _strData[0] && max_value / _value >= b._value) || \
				('-' == _strData[0] && max_value / _value <= b._value))
				return _value*b._value;
		}
		else
		{
			if (('+' == _strData[0] && min_value / _value <= b._value) || \
				('-' == _strData[0] && min_value / _value >= b._value))
				return BigData(_value*b._value);
		}
	}

	//�ж�����������û��Ϊ����1���������
	if ("+1"==_strData)
		return BigData(b._strData);
	if ("+1" == b._strData)
		return BigData(_strData);
	if ("-1" == _strData)
	{
		string ret = b._strData;
		if ('+' == b._strData[0])
			ret[0] = '-';
		else
			ret[0] = '+';
		return BigData(ret);
	}
	if ("-1" == b._strData)
	{
		string ret =_strData;
		if ('+' ==_strData[0])
			ret[0] = '-';
		else
			ret[0] = '+';
		return BigData(ret);
	}
	return BigData(Mul(_strData, b._strData));
}

BigData BigData::operator/(BigData b)
{
	if ("+0" == b._strData)
	{
		cout << "exception" << endl;
		return BigData("");
	}

	if (!isINIT64overflow(_strData) && !isINIT64overflow(b._strData))
	{
		return _value / b._value;
	}

	if ("+0" == _strData || _strData.size()<b._strData.size() || \
		(_strData.size()==b._strData.size() && strncmp(_strData.c_str()+1,b._strData.c_str()+1,_strData.size()-1)<0))
		return BigData(0);

	if (_strData.size() == b._strData.size() && strncmp(_strData.c_str() + 1, b._strData.c_str() + 1, _strData.size() - 1)==0)
	{
		if (_strData[0]==b._strData[0])
			return BigData(1);
		else
			return BigData(-1);
	}

	if ("+1" == b._strData)
		return BigData(_strData);
	else if ("-1" == b._strData)
	{
		string ret= _strData;
		ret[0] = '+';
		if ('+' == _strData[0])
			ret[0] = '-';
		return BigData(ret);
	}

	return BigData(Div(_strData,b._strData));
}



string BigData::Div(string strLeft, string strRight)
{
	char symbol = '+';
	if (strLeft[0] != strRight[0])
		symbol = '-';
	size_t LSize = strLeft.size();
	size_t RSize = strRight.size();
	char *pLeft = (char*)strLeft.c_str()+1;
	char *pRight = (char*)strRight.c_str()+1;
	size_t dataLen =0;
	string strTemp;
	strTemp.append(1,symbol);

	while ('\0'!=(*(pLeft+dataLen-1)))
	{
		if ('0' == *pLeft)
		{
			pLeft++;
			strTemp.append(1, '0');
			continue;
		}

		if (!isLgtR(pLeft, dataLen, pRight, RSize-1))
		{
			strTemp.append(1,'0');
			dataLen++;
		}
		else
		{
			strTemp.append (1,SubLoop(pLeft, dataLen, pRight, RSize)+'0');
			dataLen++;
		}
	}
	return strTemp;
}

char BigData::SubLoop(char* &pLeft, size_t& dataLen, const char *pRight,const size_t RSize)
{
	char count = 0;
	while (isLgtR(pLeft, dataLen, pRight, RSize-1))
	{
		for (size_t index = 1; index <= dataLen; index++)
		{
			char ret = *(pLeft + dataLen - index) - '0';
			if (RSize>index)
			{
				ret -= (*(pRight + RSize - index-1) - '0');
			}
			if (ret < 0)
			{
				*(pLeft + dataLen - index - 1) -= 1;
				ret += 10;
			}
			*(pLeft + dataLen - index) = ret + '0';
		}
		count++;
		while ('0' == *pLeft&&dataLen>0)
		{
			pLeft++;
			dataLen--;
		}
	}
	return count;
}

//�ж���pLeft��ʼdataLen��������ʾ�����Ĵ�С�ǲ��Ǵ��ڳ���
bool BigData::isLgtR(const char* pLeft,const size_t dataLen, const char *pRight, const size_t RSize)   
{
	if (dataLen > RSize)
		return true;
	else if (dataLen == RSize)
	{
		if (strncmp(pLeft, pRight, dataLen) >= 0)
			return true;
	}
	return false;
}


string BigData::Mul(string strLeft, string strRight)
{
	size_t LSize = strLeft.size();
	size_t RSize = strRight.size();
	
	//�˷���˵�ʱ�򣬱�֤*����ߵ����ĳ���С�ڵ���*���ұߵ���
	if(LSize > RSize)
	{
		swap(LSize,RSize);
		swap(strLeft,strRight);
	}
	char symbol = '+';
	if (strLeft[0] != strRight[0])         //������Ϊ��
		symbol='-';

	string strTemp;
	strTemp.resize(LSize+RSize-1,'0');      //������ˣ������λ���������������λ��֮��
	strTemp[0] = symbol;

	//��Ϊ��������ˣ���������ÿһλ��Ҫ�ͱ����������㣬���Ա���������ѭ��
	for (size_t i = 1; i < LSize;i++)
	{
		if ('0' == strLeft[LSize - i])
			continue;
		char step=0;                         //��¼��λ
		for (size_t j=1;j<RSize;j++)
		{
			char ret = strLeft[LSize-i]-'0';
			ret*=(strRight[RSize-j]-'0');
			ret += step+strTemp[LSize + RSize - j - i] - '0';
			strTemp[LSize + RSize - j - i]= ret%10+'0';
			step = ret / 10;
		}
		strTemp[LSize- i] += step;
	}
	return strTemp;
}

string BigData::Sub(string strLeft, string strRight)      //ͬ�����
{
	size_t LSize = strLeft.size();
	size_t RSize = strRight.size();

	string left = strLeft;
	string right = strRight;
	left[0] = '+';
	right[0] = '+';

	char symbol='+';
	if ((LSize > RSize)||(LSize==RSize && strLeft>=strRight))     //��ߵ������ұߵ�����,���Լ���
	{
		if ('-' == strLeft[0])     //����������
			symbol = '-';
	}
	else            //�ұ�������ߴ󣬼�����
	{
		if ('+' == strLeft[0])   //����������
			symbol = '-';
		swap(left,right);
		swap(LSize,RSize);
	}

	//ͳһת���� a-b��-(a-b)����ʽ,��(a>=b>=0)

	string strTemp;
	strTemp.resize(LSize);
	strTemp[0] = symbol;

	for (size_t index = 1; index < LSize;index++)
	{
		char ret = left[LSize-index]-'0';
		if (RSize>index)
		{
			ret = ret - (right[RSize - index] - '0');
		}
		if (ret < 0)                   //������Ҫ��λ
		{
			left[LSize - index - 1] -= 1;     
			ret += 10;
		}
		strTemp[LSize - index] = ret+'0';
	}
	return strTemp;
}


string BigData::Add(string strLeft,string strRight)
{
	size_t LSize = strLeft.size();
	size_t RSize = strRight.size();
	if (LSize < RSize)                 //Ϊ�˷��㣬���Ǳ�֤+����ߵ�����λ����С�ڼӺ��ұߵ���
	{
		swap(strLeft,strRight);
		swap(LSize,RSize);
	}

	string strTemp;
	strTemp.resize(LSize+1);                 //��������ӣ������λ�����������������λ�������������+1
	strTemp[0] = strLeft[0];                 
	char step = 0;                            //��¼��λ
	for (size_t index = 1; index < LSize;index++)
	{
		char ret = strLeft[LSize - index]-'0';
		ret += step;
		if (RSize>index)
		{
			ret=ret+strRight[RSize-index] - '0';
		}
		step = ret / 10;                     //�����λ���
		strTemp[LSize + 1 - index] = ret%10+'0';  
	}
	strTemp[1] = step+'0';                   //�����λ��λ��ȡ����step��ֵ
	return strTemp;
}


