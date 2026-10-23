#include "pch.h"

CLexeme::CLexeme()
{
	m_pNext = 0;
	m_pPrev = 0;
	m_pLexemeSymbol = 0;
}

CLexeme::~CLexeme()
{
}

BOOL CLexeme::Create(CSymbol* pSym)
{
	m_pLexemeSymbol = pSym;
	return TRUE;
}

BOOL CLexeme::Compare(CLexeme* pLex)
{
	BOOL rV;

	rV = GetLexemeSymbol()->Compare(pLex->GetName());
    return rV;
}

char* CLexeme::GetName()
{
	return GetLexemeSymbol()->GetName();
}

void CLexeme::Copy(CLexeme* pLexemeSource)
{
	m_pLexemeSymbol = pLexemeSource->GetLexemeSymbol();
}

CSet* CLexeme::GetFollowSet()
{
	return GetLexemeSymbol()->GetFollowSet();
}

CSet* CLexeme::GetFirstSet()
{
	return GetLexemeSymbol()->GetFirstSet();
}

BOOL CLexeme::IsEmpty()
{
	return GetLexemeSymbol()->IsEmpty();
}

BOOL CLexeme::IsTerminal()
{
	return m_pLexemeSymbol->IsTerminal();
}

BOOL CLexeme::IsNonTerminal()
{
    return GetLexemeSymbol()->IsNonTerminal();
}

BOOL CLexeme::IsNullable()
{
	BOOL rV = FALSE;

	rV = GetLexemeSymbol()->IsNullable();
    return rV;
}

BOOL CLexeme::IsNotNullable()
{
	BOOL rV = FALSE;

	rV = GetLexemeSymbol()->IsNotNullable();
	return rV;
}

void CLexeme::Print(FILE* pO, BOOL bLHS, BOOL bEOL, int nIndentSpaces)
{
	char* s;

	if (pO)
	{
		s = new char[256];
		fprintf(pO, "%s%s",
			IndentString(s, nIndentSpaces),
			GetLexemeSymbol()->GetName());
		if (bEOL)
			fprintf(pO, "\n");
		delete[] s;
	}
}
