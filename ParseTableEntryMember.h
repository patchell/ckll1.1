#pragma once

class CParseTableEntryMember
{
	CParseTableEntryMember* m_pNext;
	CParseTableEntryMember* m_pPrev;
	CRule* m_pRule;
public:
	CParseTableEntryMember();
	virtual ~CParseTableEntryMember();
	BOOL Create(CRule *pProduction);
	void AddProduction(CRule* pR) { m_pRule = pR; }
	CRule* GetProduction() { return m_pRule; }
	void SetProduction(CRule* pR) { m_pRule = pR; }
	//----------------------------
	// Linked List Management
	//----------------------------
	CParseTableEntryMember* GetNext() { return  m_pNext; }
	void SetNext(CParseTableEntryMember* pN) { m_pNext = pN; }
	CParseTableEntryMember* GetPrev() { return  m_pPrev; }
	void SetPrev(CParseTableEntryMember* pP) { m_pPrev = pP; }
	//-----------------------------
	// Debug
	//-----------------------------
	void Print(FILE* pOut,
		BOOL bLHS = FALSE,
		BOOL bEOL = TRUE,
		int IndentSpaces = 0
	);
};

