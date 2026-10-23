#pragma once

constexpr auto TOKEN_NUMBER_OF = 20;

class CToken
{
public:
	enum LLRD_Token {
		IDENT = 256,
		STRING,
		NUMBER,
		NONTERMINAL,
		TERMINALS,
		TERMINAL,
		GRAMMAR,
		REPLACED_BY,
		KEYWORD,
		TOKEN,
		EMPTY,
		ENDOFTOKENSTREAM,
		ENDOFTOKENS
	};
	struct STokenStrings {
		LLRD_Token m_Token;
		const char* m_pName;
	};
	static inline STokenStrings TokenNameLUT[TOKEN_NUMBER_OF] = {
		{IDENT,"IDENT"},
		{STRING,"STRING"},
		{NUMBER,"NUMBER"},
		{NONTERMINAL,"NONTERMINAL"},
		{TERMINAL,"TERMINAL"},
		{GRAMMAR,"GRAMMAR"},
		{REPLACED_BY,"->"},
		{KEYWORD,"KEYWORD"},
		{TOKEN,"TOKEN"},
		{EMPTY,"e"},
		{ENDOFTOKENSTREAM,"$"},
		{ENDOFTOKENS,(const char*)0}
	};
	static const char* LookupTokenName(CToken::LLRD_Token Token);
};

