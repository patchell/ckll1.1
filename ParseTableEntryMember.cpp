#include "pch.h"

CParseTableEntryMember::CParseTableEntryMember()
{
	m_pNext = 0;
	m_pPrev = 0;
	m_pRule = 0;
}

CParseTableEntryMember::~CParseTableEntryMember()
{
}

BOOL CParseTableEntryMember::Create(CRule* pRule)
{
	BOOL rV = TRUE;

	SetProduction(pRule);
	return rV;
}

void CParseTableEntryMember::Print(FILE* pO, 
	BOOL bLHS, 
	BOOL bEOL, 
	int nIndentSpaces
)
{
	if (pO)
	{
		GetProduction()->Print(pO, bLHS, bEOL, nIndentSpaces);
	}
}
