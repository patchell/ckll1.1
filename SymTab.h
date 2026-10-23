#pragma once

constexpr auto SYMTAB_SIZE = 101;

class CSymTab
{
	int Hash(const char* name);
	CSet m_NonTerminals;
	CSet m_Terminals;
protected:
	int m_nElements;	//total number of elements in table
	int m_tSize;		//depth of table
	CBucket** m_ppTab;	//pointer to table
public:
	CSymTab() {
		m_ppTab = 0;
		m_nElements = 0;
		m_tSize = 0;
	}
	virtual ~CSymTab();
	BOOL Create(int TableDepth);
	virtual CBin* FindSymbol(const char* name);
	virtual CBin* FindStartSymbol();
	virtual CSymbol* FindFirstPredefinedToken();
	virtual CSymbol* FindNextPredefineToken(CToken::LLRD_Token Value);
	virtual void AddSymbol(CBin* pSym);
	virtual void DelSymbol(CBin* pSym);
	int GetNumElements() { return m_nElements; }
	int GetTableSize() { return m_tSize; }
	CBucket** GetTable() { return m_ppTab; }
	CBucket* GetBucket(int i) { return m_ppTab[i]; }
	int CheckForUnUsedNonTerminals(FILE* pOut);
	int CheckForOrphans(FILE* pOut, CSet* pSetNonTerminals);
	BOOL CheckOrphans_SearchRules(FILE* pOut, CSet* pSetNonTerminals, CLexeme* pLexeme);
	CSet* GetNonTerminalSet() { 
		return &m_NonTerminals; 
	}
	CSet* GetTerminalSet() { 
		return &m_Terminals; 
	}
	BOOL FindOrphanTerminalSymbols();
	//-----------------------------------
	// Debug
	//-----------------------------------
	virtual void PrintTable(FILE* pOut);
	void PrintFirstSets(FILE* pOut);
	void PrintFollowSets(FILE* pOut,
		BOOL bLHS = FALSE,
		BOOL bEOL = TRUE,
		int nIndentSpaces = 0
	);
};

