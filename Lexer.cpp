#include "pch.h"

CLexer::CLexer()
{
	int i;

	for (i = 0; i < LEX_BUFFER_SIZE; ++i)
		m_aLexBuff[i] = 0;
	m_pInputFile = 0;
	m_LexBuffIndex = 0;
	m_Number = 0;
	m_pLexSymbol = 0;
}


CLexer::~CLexer()
{
//	if (m_pEmbeddedCodeBuffer) delete[] m_pEmbeddedCodeBuffer;
}	

BOOL CLexer::Create(FILE* pIn)
{
//	char Epsilon[3] = { char(207),char(181 ), 0};
	char Epsilon[2] = { 'e', 0};
	char Dollar[2] = { '$',0 };
	CRule* pRule;
	CLexeme* pLexeme;
	CSetMember* pSM;

	m_pInputFile = pIn;
	m_SymbolTable.Create(SYMTAB_SIZE);

	//--------------------------------
	// Create the empty (epsilon)
	// symbol
	//--------------------------------
	m_EmptySymbol.Create(Epsilon, CSymbol::TokenType::POSTDEFINED);
	m_EmptySymbol.SetEmpty(TRUE);
	m_EmptySymbol.SetNullable(TRUE);
	m_EmptySymbol.SetTokenValue(CToken::LLRD_Token::TERMINAL, CSymbol::TokenType::POSTDEFINED);
	pLexeme = new CLexeme;
	pLexeme->Create(&m_EmptySymbol);
	pSM = new CSetMember;
	pSM->Create(&m_EmptySymbol);
	pRule = new CRule;
	pRule->Create(&m_EmptySymbol);
	pRule->AddLexeme(pLexeme);
	m_EmptySymbol.AddRule(pRule);
	m_EmptySymbol.GetFirstSet()->AddToSet(pSM);

	//----------------------------------
	// Create the End of Token Stream
	// symbol - $
	//----------------------------------
	m_EndOfTokenStream.Create(Dollar, CSymbol::TokenType::POSTDEFINED);
	m_EndOfTokenStream.SetEmpty(FALSE);
	m_EndOfTokenStream.SetTokenValue(CToken::LLRD_Token::TERMINAL, CSymbol::TokenType::POSTDEFINED);
	m_EndOfTokenStream.SetEndOfTokenStream();
	m_EndOfTokenStream.SetNullable(FALSE);
	pLexeme = new CLexeme;
	pLexeme->Create(&m_EndOfTokenStream);
	pRule = new CRule;
	pRule->Create(&m_EndOfTokenStream);
	pRule->AddLexeme(pLexeme);
	m_EndOfTokenStream.AddRule(pRule);
	return TRUE;
}

void CLexer::CloseFiles()
{
	fclose(m_pInputFile);
}

void CLexer::Error(FILE* pO, const char* pErrorString)
{
	fprintf(pO, "Line %d  Col %d Error:%s\n", m_Line, m_Col, pErrorString);
	CloseAllFiles();
}

int CLexer::LexGet()
{
	int c = 0;

	if (m_UngetBuffer.HasItems())
	{
		c = m_UngetBuffer.Get()->GetIntData();
	}
	else
	{
		if (m_pInputFile)
			c = fgetc(m_pInputFile);
		else
			c = EOF;
	}
	m_Col++;
	return c;
}

void CLexer::LexUnGet(int Value)
{
	CStackItem* pSI = new CStackItem;
	pSI->Create(CStackItem::ValueType::INT, Value);
	m_UngetBuffer.Put(pSI);
}

BOOL CLexer::IsValidNumber(int c)
{
	BOOL IsValid = FALSE;

	switch (c)
	{
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		IsValid = TRUE;
		break;
	}
	return IsValid;
}

BOOL CLexer::IsValidNameChar(int c)
{
	BOOL IsValid = FALSE;

	if (isalnum(c) || c == '_')
		IsValid = TRUE;
	return IsValid;;
}

BOOL CLexer::IsWhiteSpace(int c)
{
	BOOL IsValid = FALSE;

	switch (c)
	{
	case '\n':
		m_Line++;
		m_Col = 0;
	case '\r':
	case '\t':
	case ' ':
		IsValid = TRUE;
		break;;
	}
	return IsValid;
}

BOOL CLexer::CheckTokens(CToken::LLRD_Token Last, CToken::LLRD_Token Current)
{
	BOOL rV = TRUE;
	int t = (int)Current;
	int l = (int)Last;
	if (l == '.')
	{
		if (t == ';')
			rV = FALSE;
	}
	return rV;
}

BOOL CLexer::CheckSymbols(CSymbol* pLast, CSymbol* pNew)
{
	BOOL rV = FALSE;

	if (pLast)
	{
		if (pLast == pNew)
		{
			fprintf(stderr, "WARNING missing ; ???");
			rV = TRUE;
		}
	}
	return rV;
}

CToken::LLRD_Token CLexer::Lex()
{
	BOOL Loop = TRUE;
	BOOL auxLoop = TRUE;
	int c;
	CToken::LLRD_Token TokenValue = CToken::LLRD_Token(0);
	CToken::LLRD_Token LastToken = CToken::LLRD_Token(0);
	CSymbol* pLastSym = 0;

//	if (m_Line >= 285)
//		printf("Boo-Boo Line:%d\n", m_Line);
	m_LexBuffIndex = 0;
	m_aLexBuff[1] = 0;
	m_pLexSymbol = 0;
	while (Loop)
	{
		c = LexGet();
		m_aLexBuff[0] = c;

		switch (c)
		{
		case EOF:
			TokenValue = CToken::LLRD_Token(EOF);
			Loop = FALSE;
			break;
		case '\n':	//white space
			m_Col = 0;
			m_Line++;
			break;
		case '\t':	//more white space
			m_Col += 4;
			break;
		case '\r':	
			break;
		case ' ':
			break;
		case '/':
			c = LexGet();
			if (c == '/')
			{
				//eat characters until the EOL
				do
				{
					c = LexGet();
				} while (c != '\n');
				LexUnGet(c);
			}
			else
			{
				LexUnGet('/');
				LexUnGet(c);
			}
			break;
		case '0':case '1':case '2':case '3':case '4':
		case '5':case '6':case '7':case '8':case '9':
			while (IsValidNumber(c))
			{
				m_aLexBuff[m_LexBuffIndex++] = c;
				c = LexGet();
			}
			m_aLexBuff[m_LexBuffIndex] = 0;
			m_Number = atoi(m_aLexBuff);
			Loop = FALSE;
			TokenValue = CToken::LLRD_Token::NUMBER;
			LexUnGet(c);
			break;
		case '-':
			c = LexGet();
			if (c == '>')
			{
				m_aLexBuff[1] = c;
				m_aLexBuff[2] = 0;
				TokenValue = CToken::LLRD_Token::REPLACED_BY;
				Loop = 0;
			}
			else
			{
				LexUnGet(c);
				TokenValue = CToken::LLRD_Token('-');
				Loop = FALSE;
			}
			break;
		case '"':	//String
			auxLoop = TRUE;
			while (auxLoop)
			{
				c = LexGet();
				if (c == '\\')	//is it the escape char?
				{
					c = LexGet();
					m_aLexBuff[m_LexBuffIndex++] = c;
				}
				else if (c == '"')
				{
					auxLoop = FALSE;
					m_aLexBuff[m_LexBuffIndex] = 0;
					TokenValue = CToken::LLRD_Token::STRING;
				}
				else if (c == '\n')
				{
					Error(stderr, "ERROR: End of line inside STRING");
					CloseFiles();
					exit(-3);
				}
				else
					m_aLexBuff[m_LexBuffIndex++] = c;
			}
			Loop = FALSE;
			break;
		case '.':	//empty rule
			TokenValue = CToken::LLRD_Token::EMPTY;
			m_pLexSymbol = GetEmpty();
			Loop = FALSE;
			break;
		case '$':
			m_pLexSymbol = GetEndOfTokenStream();
			TokenValue = CToken::LLRD_Token::ENDOFTOKENSTREAM;
			Loop = FALSE;
			break;
		case '=':	//misc tokens
		case ';':
		case ',':
			TokenValue = CToken::LLRD_Token(c);
			Loop = FALSE;
			break;
		case '\'':	// Target Terminal Token
			m_LexBuffIndex = 0;
			auxLoop = TRUE;
			while (auxLoop)
			{
				c = LexGet();
				if (c == '\'')
				{
					m_aLexBuff[m_LexBuffIndex] = 0;
					auxLoop = FALSE;
				}
				else if (c == '\n')
				{
					Error(stderr, "End of Line in Token");
					exit(-3);
				}
				else if (c == EOF)
				{
					Error(stderr, "End of File in Token");
					exit(-3);
				}
				else
				{
					m_aLexBuff[m_LexBuffIndex++] = c;
				}
			}
			m_pLexSymbol = (CSymbol*)LookupSymbol(m_aLexBuff);
			if (m_pLexSymbol)
			{
				TokenValue = CToken::LLRD_Token(m_pLexSymbol->GetTokenValue());
				if (TokenValue == CToken::LLRD_Token::NONTERMINAL)
				{
					//------------------------------
					// Sometbing wrong if this is
					// actually a Non Terminal
					//------------------------------
					fprintf(stderr, "ERROR Line:%d Col:%d  Using %s as a Terminal\n",
						m_Line,
						m_Col,
						m_pLexSymbol->GetName()
					);
					exit(-4);
				}
				else if (TokenValue == CToken::LLRD_Token::TERMINAL)
				{
					m_pLexSymbol->IncrementTerminalSymbolUsedCount();
				}
				Loop = FALSE;
			}
			else
			{
				CSetMember* pSM;
				//-----------------------------------------
				// We have a new terminal for the target
				// grammar, add it to the
				// symbol table and terminal set
				//-----------------------------------------
				m_pLexSymbol = new CSymbol;
				m_pLexSymbol->Create(m_aLexBuff, CSymbol::TokenType::POSTDEFINED);
				m_pLexSymbol->SetTokenValue(
					CToken::LLRD_Token::TERMINAL, 
					CSymbol::TokenType::POSTDEFINED
				);
				m_pLexSymbol->SetTargetTokenValue(m_aLexBuff[0]);
				GetSymTab()->AddSymbol(m_pLexSymbol);
				pSM = new CSetMember;
				pSM->Create(m_pLexSymbol);
				m_pLexSymbol->GetFirstSet()->AddToSet(pSM);
				pSM = new CSetMember;
				pSM->Create(m_pLexSymbol);
				GetSymTab()->GetTerminalSet()->AddToSet(pSM);
				TokenValue = CToken::LLRD_Token::TERMINAL;
				Loop = FALSE;
			}
			break;
		default:	//Keywords and Identifiers and non terminals
			m_aLexBuff[m_LexBuffIndex++] = c;
			auxLoop = TRUE;
			while (auxLoop)
			{
				c = LexGet();
				if (IsValidNameChar(c))
				{
					m_aLexBuff[m_LexBuffIndex++] = c;
				}
				else
				{
					auxLoop = FALSE;
					m_aLexBuff[m_LexBuffIndex] = 0;
					LexUnGet(c);
				}
			}	//END OF collecting characters for word

			//---------------------------------
			// First check to see if it is a
			// Keyword
			//---------------------------------
			TokenValue = LookupKeyword(m_aLexBuff);
			if (int(TokenValue))
			{
				//--------------------------
				// Keyword has matched
				//--------------------------
				Loop = 0;
			}
			else
			{
				//-------------------------------------
				// Is it an Identifier?
				//-------------------------------------
				m_pLexSymbol = (CSymbol*)LookupSymbol(m_aLexBuff);
				if (m_pLexSymbol)
				{
					TokenValue = m_pLexSymbol->GetTokenValue();
					Loop = FALSE;
				}
				else
				{
					//-------------------------------------
					// Identifier is New/Undefined
					//-------------------------------------
					m_pLexSymbol = new CSymbol;
					m_pLexSymbol->Create(m_aLexBuff, CSymbol::TokenType::NOT_TOKEN);
					m_pLexSymbol->SetLineWhereDefined(GetLineNumber());
					TokenValue = CToken::LLRD_Token::IDENT;
					m_pLexSymbol->SetTokenValue(CToken::LLRD_Token::IDENT, CSymbol::TokenType::PREDEFINED);
					Loop = FALSE;
				}
			}
			break;	// end of default:
		}	// End of switch(c)
	}
	if (!CheckTokens(LastToken, TokenValue))
		fprintf(stderr, "WARNING . is not followed by ;\n");
	LastToken = TokenValue;
	CheckSymbols(pLastSym, m_pLexSymbol);
	pLastSym = m_pLexSymbol;
//	fprintf(LogFile(), "Lex::%s\n", m_aLexBuff);
	return TokenValue;
}

CBin* CLexer::LookupSymbol(const char* pName)
{
	CBin* pSym = 0;

	pSym = m_SymbolTable.FindSymbol(pName);
	return pSym;
}

CToken::LLRD_Token CLexer::LookupKeyword(const char* pKeyword)
{
	int i;
	CToken::LLRD_Token KeywordToken = CToken::LLRD_Token(0);
	BOOL Loop = TRUE;

	for (i = 0; Loop && KeyWords[i].m_TokenID != CToken::LLRD_Token::ENDOFTOKENS; ++i)
	{
		if (strcmp(pKeyword, KeyWords[i].m_Name) == 0)
		{
			Loop = FALSE;
			KeywordToken = KeyWords[i].m_TokenID;
		}
	}
	return KeywordToken;
}

CLexer::KeyWord* CLexer::FindKeyword(CToken::LLRD_Token KeywordToken)
{
	KeyWord* pKeyword = 0;
	BOOL Loop = TRUE;
	int i = 0;

	while (Loop)
	{
		if (KeyWords[i].m_TokenID != CToken::LLRD_Token::ENDOFTOKENS)
		{
			if (KeyWords[i].m_TokenID == KeywordToken)
			{
				pKeyword = (KeyWord*)&KeyWords[i];
				Loop = 0;
			}
			else
				++i;
		}
		else
			Loop = 0;
	}
	return pKeyword;
}


