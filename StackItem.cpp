#include "pch.h"

CStackItem::CStackItem()
{
	m_pNext = 0;
	m_Value.intData = 0;
	m_Value.ptrData = 0;
	m_Type = ValueType(-1);
}

CStackItem::~CStackItem()
{
}

BOOL CStackItem::Create(ValueType VT, int IV)
{
	BOOL rV = TRUE;
	m_Type = VT;
	m_Value.intData = IV;
	return rV;
}

BOOL CStackItem::Create(ValueType VT, void* pV)
{
	BOOL rV = TRUE;
	m_Type = VT;
	m_Value.ptrData = pV;
	return rV;
}

const char* CStackItem::LookupValueType(ValueType VT)
{
	const char* pString = 0;
	int i = 0;

	while (StackTypeLUT[i].Name && !pString)
	{
		if (VT == StackTypeLUT[i].VT)
			pString = StackTypeLUT[i].Name;
		++i;
	}
	return pString;
}

void CStackItem::CheckStackItem(CStackItem* pSI, ValueType Type)
{
	char* s;

	if (pSI == NULL && !pSI->IsType(Type))
	{
		s = new char[256];
		sprintf_s(s, 256, "Top of Stack NULL or Not %s Type",
			CStackItem::LookupValueType(CStackItem::ValueType::pSYMBOL)
		);
		CLexer::Error(stderr, s);
		delete[]s;
		exit(-2);
	}
}
