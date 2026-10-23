#pragma once

class CLexer;

class CSymbol: public CBin
{
//	class CLexer friend;
public:
	enum class TokenType {
		NOT_TOKEN,
		PREDEFINED,
		POSTDEFINED
	};
private:
	inline static const char* TokenTypeStrings[3] = {
		"NOT TOKEN",
		"PRE DEFINED",
		"POST DEFINED"
	};
	//---------------------------------------
	// Things That Define Tokens
	//---------------------------------------
	TokenType m_TokenType;
	CToken::LLRD_Token m_TokenValue;	//token value for this parser
	UINT m_TerminalSymbolUsedCount;	//how many times this terminal symbol was used in the grammar
	UINT m_TargetTokenValue;	//token value for target parser
	char m_aTokenName[256];
	//---------------------------------------
	// Symbol Flags
	//---------------------------------------
	BOOL m_IsEmpty;
	BOOL m_IsNullable;
	BOOL m_StartSymbol;
	BOOL m_EndOfTokenStream;
	CRule* m_pHead;
	CRule* m_pTail;
	CSet m_FirstSet;
	CSet m_FollowSet;
	int m_nTotalRules;
	int m_LineNumber;		//line where defined
public:
	CSymbol() {
		int i;
		m_TokenType = TokenType::NOT_TOKEN;
		m_TerminalSymbolUsedCount = 0;
		m_IsEmpty = FALSE;
		m_IsNullable = FALSE;
		m_StartSymbol = FALSE;
		m_EndOfTokenStream = FALSE;
		m_TokenValue = CToken::LLRD_Token(0);
		m_TargetTokenValue = 0;
		m_pHead = 0;
		m_pTail = 0;
		m_nTotalRules = 0;
		m_LineNumber = 0;
		for (i = 0; i < 256; ++i)
		{
			m_aTokenName[i] = 0;
		}
	}
	virtual ~CSymbol() {}
	BOOL Create(const char* pName, TokenType Toktype);
	virtual BOOL Compare(const char* name);
	virtual BOOL Compare(CSymbol* pSym);
	char* GetTokenString();
	int GetNumberOfRules() { return m_nTotalRules; }
	CSymbol* GetNext() { return (CSymbol*)CBin::GetNext(); }
	BOOL DoesThisProductionHaveEpsilon();
	CSymbol::TokenType GetTokenType() { return m_TokenType; }
	BOOL IsPredefinedToken() {
		BOOL rV = FALSE;
		if (m_TokenType == TokenType::PREDEFINED)
			rV = TRUE;
		return rV;
	}
	//----------------------------
	// Debug Stuff
	//----------------------------
	virtual void PrintProduction(
		FILE* pOut, 
		BOOL bLHS = FALSE, 
		BOOL bEOL = TRUE, 
		int nIndentSpaces = 0
	);
	virtual void Print(FILE* pOut, 
		BOOL bLHS = FALSE, 
		BOOL bEO = TRUE, 
		int nIndentSpaces = 0
	);
	//-----------------------------
	// Accessor Methods
	//-----------------------------
	void IncrementTerminalSymbolUsedCount() { m_TerminalSymbolUsedCount++; }
	UINT GetTerminalSymbolUsedCount() const { return m_TerminalSymbolUsedCount; }
	UINT GetTargetTokenValue() {
		return m_TargetTokenValue;
	}
	void SetTargetTokenValue(UINT v) { m_TargetTokenValue = v; }
	int GetLineWhereDefined() { return m_LineNumber; }
	void SetLineWhereDefined(int ln) { m_LineNumber = ln; }
	CToken::LLRD_Token GetTokenValue() { return m_TokenValue; }
	void SetTokenValue(CToken::LLRD_Token TV, TokenType eTokenType) {
		m_TokenValue = TV;
		m_TokenType = eTokenType;

	}
	BOOL IsEmpty() { return m_IsEmpty; }
	BOOL IsNotEmpty() { return !m_IsEmpty; }
	void SetNullable(BOOL bF);
	BOOL IsNullable() { return m_IsNullable; }
	BOOL IsNotNullable() { return !m_IsNullable; }
	void SetStartSymbol() { m_StartSymbol = TRUE; }
	virtual BOOL IsStartSymbol() { return m_StartSymbol; }
	virtual BOOL IsNotStartSymbol() { return !m_StartSymbol; }
	void SetEndOfTokenStream() { m_EndOfTokenStream = TRUE; }
	BOOL IsEndOfTokenSteam() {
		return m_EndOfTokenStream;
	}
	static const char* GetTokenTypeString(TokenType tt) {
		return TokenTypeStrings[int(tt)];
	}
	void SetTokenName(const char* pName) {
		strcpy_s(m_aTokenName, 256, pName);
	}
	char* GetTokenName() { return m_aTokenName; }
public:
	void SetEmpty(BOOL IE) { m_IsEmpty = IE; }
	BOOL IsTerminal();
	BOOL IsNonTerminal();
	//-----------------------------
	// Rule Management
	//-----------------------------
	CRule* GetHead() { return m_pHead; }
	void SetHead(CRule* pH) { m_pHead = pH; }
	CRule* GetTail() { return m_pTail; }
	void SetTail(CRule* pT) { m_pTail = pT; }
	void AddRule(CRule* pRule);
	//----------------------------
	// Set Managment
	//----------------------------
	CSet* GetFirstSet() {
		return &m_FirstSet; 
	}
	CSet& GetFirstSetRef() {
		return m_FirstSet; 
	}
	CSet* GetFollowSet() { return &m_FollowSet; }
};

