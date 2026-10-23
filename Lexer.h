#pragma once

constexpr auto EPSILON = 238;
constexpr auto LEX_BUFFER_SIZE = 4096;

class CLexer
{
public:
	struct KeyWord {
		CToken::LLRD_Token m_TokenID;	//token value
		const char* m_Name;	//token name
	};
private:
	static inline KeyWord KeyWords[] = {
		{CToken::LLRD_Token::REPLACED_BY, "REPLACED_BY"},
		{CToken::LLRD_Token::TOKEN,"TOKEN"},
		{CToken::LLRD_Token::GRAMMAR, "GRAMMAR"},
		{CToken::LLRD_Token::KEYWORD,"KEYWORD"},
		{CToken::LLRD_Token::TERMINAL,"TERMINAL"},
		{CToken::LLRD_Token::TERMINALS,"TERMINALS"},
		{CToken::LLRD_Token::ENDOFTOKENS,"$"},
	};
	CSymTab m_SymbolTable;		// Table of Symbols
	FILE* m_pInputFile;			// Grammar Input File
	char m_aLexBuff[LEX_BUFFER_SIZE];	// Input character buffer
	int m_LexBuffIndex;					// Index into the input buffer
	static inline int m_Line = 1;		// Current line number
	static inline int m_Col = 0;		// Current column number
	int m_Number;
	CSymbol* m_pLexSymbol;				// Current symbol being lexed
	inline static CSymbol m_EmptySymbol;
	inline static CSymbol m_EndOfTokenStream;
	CFifo m_UngetBuffer;
public:
	CLexer();
	virtual ~CLexer();
	BOOL Create(FILE* pIn);
	void CloseFiles();
	static void Error(FILE* pO, const char* pErrorString);
	int LexGet();
	void LexUnGet(int Value);
	BOOL IsValidNumber(int c);
	BOOL IsValidNameChar(int c);
	BOOL IsWhiteSpace(int c);
    BOOL CheckTokens(CToken::LLRD_Token Last, CToken::LLRD_Token Current);
	BOOL CheckSymbols(CSymbol* pLast, CSymbol* pNew);
    CToken::LLRD_Token Lex();
	static CSymbol* GetEmpty() { return &m_EmptySymbol; }
	static CSymbol* GetEndOfTokenStream() {
		return &m_EndOfTokenStream;
	}
	//------------------------------------------
	// Symbol Table Methods
	//------------------------------------------
	CBin* LookupSymbol(const char* pName);
	CSymTab* GetSymTab() { return &m_SymbolTable; }
	//------------------------------------------
	// Keyword Lookup Methods
	//------------------------------------------
	KeyWord* FindKeyword(CToken::LLRD_Token KeywordToken);
	CToken::LLRD_Token LookupKeyword(const char* pKW);
	//--------------------------------------
	// Symbol Methods
	//--------------------------------------
	CSymbol* GetSymbol() { return m_pLexSymbol; }
	int GetNumber() { return m_Number; }
	char* GetLexbuff() { return m_aLexBuff; }
	int GetLineNumber() { return m_Line; }
	int GetColunm() { return m_Col; }
};

