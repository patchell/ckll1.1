#pragma once

class CParser
{
	bool m_bLog;
	CStack m_ValueStack;
	CSymbol* m_pCurrentNonTerminal;
	CRule* m_pCurrentRule;
	CLexer m_Lex;
	BOOL m_FirstGrammarSymbol;
	int m_AutoIncTokenValue;
public:
	CParser();
	virtual ~CParser();
	BOOL Create(FILE* pIn);
	void CloseFiles();
	virtual CLexer* GetLexer() { return &m_Lex; }
	CToken::LLRD_Token Expect(CToken::LLRD_Token Lookahead, CToken::LLRD_Token Expected);
	BOOL Accept(CToken::LLRD_Token Lookahead, CToken::LLRD_Token Expected);
	CSymbol* GetCurrentNonTerminal() { return m_pCurrentNonTerminal; }
	void SetCurrentNonTerminal(CSymbol* pSym) { m_pCurrentNonTerminal = pSym; }
	virtual CRule* GetCurrentRule() { return m_pCurrentRule; }
	virtual void Parse();
	void SetAutoIncTokenValue(int v) { m_AutoIncTokenValue = v; }
	int GetAutoIncTokenValue() {
		return ++m_AutoIncTokenValue;
	}
	CStack* GetValueStack() { return &m_ValueStack; }
	//-------------------------------------
	// Parsing Methodes
	//-------------------------------------
private:
	CToken::LLRD_Token LLRDgramer(CToken::LLRD_Token LookaHeadToken);
	CToken::LLRD_Token Grammar(CToken::LLRD_Token LookaHeadToken);
	CToken::LLRD_Token GrammarStmts(CToken::LLRD_Token LookaHeadToken);
	CToken::LLRD_Token GrammarStmt(CToken::LLRD_Token LookaHeadToken);
	CToken::LLRD_Token RHSide(CToken::LLRD_Token LookaHeadToken);
	CToken::LLRD_Token Rule(CToken::LLRD_Token LookaHeadToken);
	CToken::LLRD_Token Terminals(CToken::LLRD_Token LookaHeadToken);
	CToken::LLRD_Token TerminalList(CToken::LLRD_Token LookaHeadToken);
	CToken::LLRD_Token TerminalItem(CToken::LLRD_Token LookaHeadToken);
	CToken::LLRD_Token OptInit(CToken::LLRD_Token LookaHeadToken);
	void UnexpectedToken(CToken::LLRD_Token Tokenvalue);
};

