#pragma once

constexpr auto KINDOF_RULE_MIXED = 0;
constexpr auto KINDOF_RULE_ALL_TERMINALS = 1;
constexpr auto KINDOF_RULE_ALL_NONTERMINALS = 2;
constexpr auto TOKEN_TYPE_POSTDEFINED = 1;
constexpr auto TOKEN_TYPE_PREDEFINED = 2;

class CRecDecParGen :public CParser
{
	CParseTable m_PaarseTable;
	CSet m_NotNullables;
	CSet m_Nullables;
	FILE* m_pLog;

public:
	CRecDecParGen();
	virtual ~CRecDecParGen();
	BOOL Create(FILE* pIn, FILE* pLog);
	void CloseFiles();
	BOOL Run();
	int CheckForDirectRecursion(FILE* pO, CSet* pNonTerminals);
	void CreateFirstSets(FILE* pOut);
	void CreateFollowSets(FILE* pOut);
	BOOL ApplyFollowRulesTo(FILE* pOut, CSymbol* pSym);
	void CreateNullableSet(FILE* pOut);
	void CreateNotNullablesSet(FILE* pOut);
	CSet* GetNotNullables() { return &m_NotNullables; }
	CSet* GetNullableSet(){return &m_Nullables; }
	CParseTable* GetParseTable() { return &m_PaarseTable; }
	CSymTab* GetSymTab() {
		return GetLexer()->GetSymTab();
	}
	//-----------------------------
	// Debug Methods
	//-----------------------------
	void PrintGrammar(FILE* pOut);
	FILE* GetLogFile() { return m_pLog; }
};

