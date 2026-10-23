#pragma once


class CLexeme
{
	class CSet friend;
	class CSymbol friend;
	CSymbol* m_pLexemeSymbol;
	CLexeme* m_pNext;
	CLexeme* m_pPrev;
public:
	CLexeme();
	virtual ~CLexeme();
	BOOL Create(CSymbol* pSym);
	BOOL Compare(CLexeme* pLex);
	char* GetName();
	void Copy(CLexeme* pLexemeSource);
	CSymbol* GetLexemeSymbol() { return m_pLexemeSymbol; }
	CSet* GetFollowSet(); 
	CSet* GetFirstSet();
	BOOL IsEmpty();
	BOOL IsTerminal();
	BOOL IsNonTerminal();
	BOOL IsNullable();
	BOOL IsNotNullable();
	//----------------------------
	// Link Management
	//----------------------------
	void SetNext(CLexeme* pN) 
	{
		m_pNext = pN; 
	}
	CLexeme* GetNext() { return m_pNext; }
	void SetPrev(CLexeme* pP) { m_pPrev = pP; }
	CLexeme* GetPrev() { return m_pPrev; }
	void Print(FILE* pO, 
		BOOL bLHS = FALSE, 
		BOOL bEOL = TRUE, 
		int nIndentSpaces = 0
	);
};

