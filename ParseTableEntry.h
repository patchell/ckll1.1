#pragma once

class CParseTableEntry
{
	char* m_pColName;
	char* m_pRowName;
	CParseTableEntryMember* m_pHead;
	CParseTableEntryMember* m_pTail;
	int m_nMembers;
public:
	CParseTableEntry();
	virtual ~CParseTableEntry();
	BOOL Create();
	void SetColName(char* pColName) {
		m_pColName = pColName;
	}
	void SetRowName(char* pRowName) {
		m_pRowName = pRowName;
	}
	int GetNumMembers() { return m_nMembers; }
	BOOL DoesNotContains(CRule* pRule);
	//--------------------------------------------
	// Manage Entry Members
	//--------------------------------------------
	CParseTableEntryMember* GetHead() { return m_pHead; }
	void SetHead(CParseTableEntryMember* pH) { m_pHead = pH; }
	CParseTableEntryMember* GetTail() { return m_pTail; }
	void SetTail(CParseTableEntryMember* pT) { m_pTail = pT; }
	void AddMember(CParseTableEntryMember* pPTE);
	//------------------ Debug ---------------------
	void Print(FILE* pO, 
		BOOL bLHS = FALSE, 
		BOOL bEOL = TRUE, 
		int nIndentSpaces = 0,
		BOOL bNumberLines = FALSE,
		BOOL bLHSLineNumber = FALSE
	);
};

