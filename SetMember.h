#pragma once

class CSetMember
{
	CSetMember* m_pNext;
	CSetMember* m_pPrev;
	CLexeme* m_pLexeme;
	char SetName[128];
public:
	CSetMember();
	virtual ~CSetMember();
	BOOL Create(CSymbol* pSym);
	void Copy(CSetMember* pSetMemberSource);
	BOOL Compare(CLexeme* pLex);
	BOOL Compare(CSymbol* pLex);
	//---------------------------
	// Link Managment
	//---------------------------
	void SetNext(CSetMember* pN) { m_pNext = pN; }
	CSetMember* GetNext() { return m_pNext; }
	void SetPrev(CSetMember* pP) { m_pPrev = pP; }
	CSetMember* GetPrev() { return m_pPrev; }
	//--------------------------
	// Attributes
	//--------------------------
	CLexeme* GetSetMemberLexeme() { 
		return m_pLexeme; 
	}
	CSymbol* GetSetMemberSymbol() {
		return GetSetMemberLexeme()->GetLexemeSymbol();
	}
	CRule* GetRule(); 
	void SetSetMemberLexem(CLexeme*pL) { m_pLexeme = pL; }
	//--------------------------------------------
	// Debug Methods
	//--------------------------------------------
	int Print(FILE* pOut, 
		int DelimiterChar, 
		BOOL bLHS = FALSE, 
		BOOL bEOL = TRUE, 
		int nIndentSpaces = 0
	);
};

