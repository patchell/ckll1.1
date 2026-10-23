#include "pch.h"

CSetMember::CSetMember()
{
	m_pLexeme = 0;
	m_pNext = 0;
	m_pPrev = 0;
	memset(SetName, 0, sizeof(SetName));
}

CSetMember::~CSetMember()
{
}

BOOL CSetMember::Create(CSymbol* pSym)
{
	CLexeme* pLxme;

	pLxme = new CLexeme;
	pLxme->Create(pSym);
	SetSetMemberLexem(pLxme);
	return TRUE;
}

void CSetMember::Copy(CSetMember* pSetMemberSource)
{
	CLexeme* pLexeme = new CLexeme;
	pLexeme->Create(pSetMemberSource->GetSetMemberLexeme()->GetLexemeSymbol());
	SetSetMemberLexem(pLexeme);
}

BOOL CSetMember::Compare(CLexeme* pLex)
{
	BOOL bLexemsAgree;
	bLexemsAgree = GetSetMemberLexeme()->Compare(pLex);
    return bLexemsAgree;
}

BOOL CSetMember::Compare(CSymbol* pSym)
{
	BOOL bSymsAgree;

	bSymsAgree = GetSetMemberSymbol()->Compare(pSym);
	return bSymsAgree;
}

CRule* CSetMember::GetRule()
{
	return GetSetMemberLexeme()->GetLexemeSymbol()->GetHead();
}

int CSetMember::Print(FILE* pOut, int DelimiterChar, BOOL bLHS, BOOL bEOL, int nIndentSpaces)
{
	int NumCharsPrinted = 0;
	char* pName;

	if (pOut)
	{
		pName = GetSetMemberLexeme()->GetLexemeSymbol()->GetName();
		fprintf(pOut, "%c\'%s\'", 
			DelimiterChar, 
			pName
		);
		NumCharsPrinted = int(strlen(GetSetMemberLexeme()->GetLexemeSymbol()->GetName()));
		NumCharsPrinted += nIndentSpaces + 3;
		if (bEOL)
			fprintf(pOut, "\n");
	}
	return NumCharsPrinted;
}
