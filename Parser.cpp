#include "pch.h"

CParser::CParser()
{
	m_pCurrentNonTerminal = 0;
	m_pCurrentRule = 0;
	m_FirstGrammarSymbol = TRUE;
	m_AutoIncTokenValue = 256;
	m_bLog = false;
}

CParser::~CParser()
{

}

BOOL CParser::Create(FILE* pIn)
{
	m_Lex.Create(pIn);
	return TRUE;
}

void CParser::CloseFiles()
{
	m_Lex.CloseFiles();
}


//**********************************************
// Expect
//
// This function is used to check that we get
// the token that we Expect.
//
// parameters:
//	Lookahead..Current lookahead token
//	token....this is the token we Expect
// return value:
//	reutns the next Lookahead token (>0)
//	reutnrs 0 or negative if we did not get what we Expected
//*********************************************

CToken::LLRD_Token CParser::Expect(CToken::LLRD_Token LookaHeadToken, CToken::LLRD_Token Expected)
{
	if (Accept(LookaHeadToken, Expected))
		LookaHeadToken = GetLexer()->Lex();
	else
	{
		char* pExp = new char[256], * pGot = new char[256];
		CLexer::KeyWord* pKY = 0;

		if (LookaHeadToken >= CToken::LLRD_Token(256))
		{
			pKY = GetLexer()->FindKeyword(LookaHeadToken);
			if (pKY)
				strcpy_s(pGot, 256, pKY->m_Name);
			else
			{
				char* s = new char[256];
				sprintf_s(s, 256, "Unknown Token %d:%s\n",
					LookaHeadToken,
					CToken::LookupTokenName(LookaHeadToken)
				);
				CLexer::Error(stderr, s);
				delete[] s;
				CloseAllFiles();
				exit(-2);
			}
		}
		else
		{
			pGot[0] = int(LookaHeadToken);
			pGot[1] = 0;
		}
		if (Expected >= CToken::LLRD_Token(256))
		{
			pKY = GetLexer()->FindKeyword(Expected);
			if (pKY)
				strcpy_s(pExp, 256, pKY->m_Name);
			else
				fprintf(stderr, "Unknown Token %d\n", Expected);
		}
		else
		{
			pExp[0] = int(Expected);
			pGot[1] = 0;
		}
		fprintf(stderr, "Line %d: Column:%d Unexpected Token:Got %d:%s Expected %d:%s\n",
			GetLexer()->GetLineNumber(),
			GetLexer()->GetColunm(),
			LookaHeadToken,
			pGot,
			Expected,
			pExp
		);
		exit(1);
	}
	return LookaHeadToken;
}

//********************************************
// Accept
//
// This function compares the token you want
// versus the token that is current.  If they
// match, then we get another token.
// If not, then just return.
//
// parameter:
//	Lookahead..The current lookahead token
//	token.....this is the token we want
//
// return value:
//	returns the new token value (>0)
//	returns 0 or negative if we don't get the token we want
//**********************************************

BOOL CParser::Accept(CToken::LLRD_Token Lookahead, CToken::LLRD_Token Expected)
{
	BOOL rv = FALSE;

	if (Expected == Lookahead)
		rv = TRUE;
	return rv;
}

void CParser::Parse()
{
	CToken::LLRD_Token LookaHeadToken;
	//	CRule* pRule;
	CSetMember* pSM, * pSM_EOT;
	//	CLexeme* pLexeme;

	if (m_bLog)
		fprintf(LogFile(), "Start Parsing\n");
	LookaHeadToken = GetLexer()->Lex();
	LookaHeadToken = LLRDgramer(LookaHeadToken);
	if (m_bLog)
	{
		int Lines;
		Lines = GetLexer()->GetLineNumber();
		fprintf(stderr, "We parsed %d Lines of code\n", Lines);
	}
	//------------------------------------------
	// All done, just need to tack $ onto the
	// head grammar rule.
	//------------------------------------------
	fprintf(stderr, "Lines Compiled:%d\n", GetLexer()->GetLineNumber());
	pSM = GetLexer()->GetSymTab()->GetNonTerminalSet()->GetHead();
	pSM_EOT = new CSetMember;
	if (pSM == NULL)
	{
		fprintf(stderr, "Internal Error\nHead of NonTerminal Table is NULL\n");
		CloseAllFiles();
		exit(-1);
	}
	pSM_EOT->Create(CLexer::GetEndOfTokenStream());
	pSM->GetSetMemberSymbol()->GetFollowSet()->AddToSet(pSM_EOT);
//	if(m_bLog) GetLexer()->GetSymTab()->PrintTable(LogFile());
}

//-------------------------------------
// Parsing Methodes
//-------------------------------------

CToken::LLRD_Token CParser::LLRDgramer(CToken::LLRD_Token LookaHeadToken)
{
	//------------------------------------------------
	// LLRDgramer -> Terminals Grammar
	//------------------------------------------------
	LookaHeadToken = Terminals (LookaHeadToken);
	LookaHeadToken = Grammar(LookaHeadToken);
	return LookaHeadToken;
}

CToken::LLRD_Token CParser::Grammar(CToken::LLRD_Token LookaHeadToken)
{
	//------------------------------------------------
	// Grammar	-> GRAMMAR GrammerStmts
	//			-> .
	//			;
	//------------------------------------------------
	switch (LookaHeadToken)
	{
	case CToken::LLRD_Token::GRAMMAR:
		LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token::GRAMMAR);
		LookaHeadToken = GrammarStmts(LookaHeadToken);
		break;
	case CToken::LLRD_Token::EMPTY:
		LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token::EMPTY);
		break;
	default:
		fprintf(stderr, "Expected \'GRAMMAR\' Line:%d\n", GetLexer()->GetLineNumber());
		break;
	}
	return LookaHeadToken;
}

CToken::LLRD_Token CParser::GrammarStmts(CToken::LLRD_Token LookaHeadToken)
{
	//------------------------------------------------
	// GrammarStmts		-> GrammarStmt GrammarStmts_1;
	// GrammarStmts_1	-> ';' GrammarStmt GarammarStmts_1
	//					-> .
	//					;
	//------------------------------------------------
	BOOL Loop = TRUE;

	LookaHeadToken = GrammarStmt(LookaHeadToken);
	while (Loop)
	{
		switch (LookaHeadToken)
		{
		case CToken::LLRD_Token(';'):
			LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token(';'));
			LookaHeadToken = GrammarStmt(LookaHeadToken);
			break;
		default:
			Loop = FALSE;
			break;
		}
	}
	return LookaHeadToken;
}


CToken::LLRD_Token CParser::GrammarStmt(CToken::LLRD_Token LookaHeadToken)
{
	//------------------------------------------------
	// GrammarStmt		-> IDENT RHSide
	//					-> NONTERMINAL RHSide
	//					-> .
	//					;
	//------------------------------------------------
	CSymbol* pSym;
	CStackItem* pSI;
	CSetMember* pSM;

	switch (LookaHeadToken)
	{
	case CToken::LLRD_Token::IDENT:
		//------------------------------------
		// If first defined on LHS
		//------------------------------------
		pSym = GetLexer()->GetSymbol();
		pSym->SetLineWhereDefined(GetLexer()->GetLineNumber());
		GetLexer()->GetSymbol()->SetTokenValue(CToken::LLRD_Token::NONTERMINAL, CSymbol::TokenType::NOT_TOKEN);
		if (m_FirstGrammarSymbol)
		{
			m_FirstGrammarSymbol = FALSE;
			pSym->SetStartSymbol();
		}
		GetLexer()->GetSymTab()->AddSymbol(pSym);
		//--------------------------------------------
		// Add To Non Terminal List (set)
		//--------------------------------------------
		pSM = new CSetMember;
		pSM->Create(pSym);
		GetLexer()->GetSymTab()->GetNonTerminalSet()->AddToSet(pSM);
		LookaHeadToken = Expect(
			LookaHeadToken, 
			CToken::LLRD_Token::IDENT
		);
		pSI = new CStackItem();
		pSI->Create(CStackItem::ValueType::pSYMBOL, pSym);
		GetValueStack()->Push(pSI);
		LookaHeadToken = RHSide(LookaHeadToken);
		break;
	case CToken::LLRD_Token::NONTERMINAL:
		//------------------------------------
		// If first defined on RHS
		//------------------------------------
		pSym = GetLexer()->GetSymbol();
		pSym->SetLineWhereDefined(GetLexer()->GetLineNumber());
		//--------------------------------------------
		// Add To Non Terminal List (set)
		//--------------------------------------------
		pSM = new CSetMember;
		pSM->Create(pSym);
		GetLexer()->GetSymTab()->GetNonTerminalSet()->AddToSet(pSM);
		//-------------------------------------------------------
		LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token::NONTERMINAL);
		pSI = new CStackItem();
		pSI->Create(CStackItem::ValueType::pSYMBOL, pSym);
		GetValueStack()->Push(pSI);
		LookaHeadToken = RHSide(LookaHeadToken);
//		pSym->Print(LogFile());
		break;
	case CToken::LLRD_Token::EMPTY:
		LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token::EMPTY);
		break;
	case CToken::LLRD_Token::TERMINAL:
		fprintf(stderr, "Terminal on LH side Line:%d\n", GetLexer()->GetLineNumber());
	default:
		break;
	}
	return LookaHeadToken;
}

CToken::LLRD_Token CParser::RHSide(CToken::LLRD_Token LookaHeadToken)
{
	//------------------------------------------------
	// RHSide		-> REPLACED_BY Rule RHSide
	//				-> .
	//				;
	//------------------------------------------------
	CRule* pRule;
	CSymbol* pSym;
	CStackItem* pSI;

	switch (LookaHeadToken)
	{
	case CToken::LLRD_Token::REPLACED_BY:
		LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token::REPLACED_BY);
		pSI = GetValueStack()->Look(CStack::StackPosition::STACK_TOP);
		CStackItem::CheckStackItem(pSI, CStackItem::ValueType::pSYMBOL);
		pSym = (CSymbol*)pSI->GetPointerData();
		pRule = new CRule();
		pRule->Create(pSym);
		pSI = new CStackItem;
		pSI->Create(CStackItem::ValueType::pRULE, pRule);
		pSym->AddRule(pRule);
		GetValueStack()->Push(pSI);
		LookaHeadToken = Rule(LookaHeadToken);
		GetValueStack()->Pop();
		LookaHeadToken = RHSide(LookaHeadToken);
		break;
	default:
		break;
	}
	return LookaHeadToken;
}

CToken::LLRD_Token CParser::Rule(CToken::LLRD_Token LookaHeadToken)
{
	//------------------------------------------------
	// Rule			-> IDENT Rule
	//				-> NONTERMINAL Rule
	//				->TERMINAL Rule
	//				-> .
	//				;
	//------------------------------------------------
	CSymbol* pSym;
	CRule* pRule;
	CLexeme* pLexeme;
	CStackItem* pSI;
	BOOL Loop = TRUE;

	while (Loop)		//minimize recursion
	{
		switch (LookaHeadToken)
		{
		case CToken::LLRD_Token::IDENT:
			pSym = GetLexer()->GetSymbol();
			pSym->SetTokenValue(CToken::LLRD_Token::NONTERMINAL, CSymbol::TokenType::NOT_TOKEN);
			pLexeme = new CLexeme;
			pLexeme->Create(pSym);
			pSI = GetValueStack()->Look(CStack::StackPosition::STACK_TOP);
			CStackItem::CheckStackItem(pSI, CStackItem::ValueType::pRULE);
			pRule = (CRule*)pSI->GetPointerData();
			pRule->AddLexeme(pLexeme);
			GetLexer()->GetSymTab()->AddSymbol(pSym);
			LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token::IDENT);
			break;
		case CToken::LLRD_Token::NONTERMINAL:
			pSym = GetLexer()->GetSymbol();
			pLexeme = new CLexeme;
			pLexeme->Create(pSym);
			pSI = GetValueStack()->Look(CStack::StackPosition::STACK_TOP);
			CStackItem::CheckStackItem(pSI, CStackItem::ValueType::pRULE);
			pRule = (CRule*)pSI->GetPointerData();
			pRule->AddLexeme(pLexeme);
			LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token::NONTERMINAL);
			break;
		case CToken::LLRD_Token::TERMINAL:
			pSym = GetLexer()->GetSymbol();
			pLexeme = new CLexeme;
			pLexeme->Create(pSym);
			pSI = GetValueStack()->Look(CStack::StackPosition::STACK_TOP);
			CStackItem::CheckStackItem(pSI, CStackItem::ValueType::pRULE);
			pRule = (CRule*)pSI->GetPointerData();
			pRule->AddLexeme(pLexeme);
			LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token::TERMINAL);
			break;
		case CToken::LLRD_Token::EMPTY:
			pSym = GetLexer()->GetEmpty();
			pLexeme = new CLexeme;
			pLexeme->Create(pSym);
			pSI = GetValueStack()->Look(CStack::StackPosition::STACK_TOP);
			CStackItem::CheckStackItem(pSI, CStackItem::ValueType::pRULE);
			pRule = (CRule*)pSI->GetPointerData();
			pRule->AddLexeme(pLexeme);
			LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token::EMPTY);
			Loop = FALSE;
			break;
		default:
			Loop = FALSE;
			break;
		}
	}
	return LookaHeadToken;
}

//---------------------------------------------------
// Terminal productions
//---------------------------------------------------

CToken::LLRD_Token CParser::Terminals(CToken::LLRD_Token LookaHeadToken)
{
	//-----------------------------------------------
	// Terminals	-> TERMINALS TerminalList
	//				-> .
	//				;
	//-----------------------------------------------
	switch (LookaHeadToken)
	{
	case CToken::LLRD_Token::TERMINALS:
		LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token::TERMINALS);
		LookaHeadToken = TerminalList(LookaHeadToken);
		break;
	}
	return LookaHeadToken;
}

CToken::LLRD_Token CParser::TerminalList(CToken::LLRD_Token LookaHeadToken)
{
	//--------------------------------------------
	// TerminalsList	-> TerminalItem TerminalsList_1 ;
	//	TerminalList_1	-> ',' TerminalItem TerminalList_1
	//					-> .
	//					;
	//--------------------------------------------
	BOOL Loop = TRUE;

	LookaHeadToken = TerminalItem(LookaHeadToken);
	while (Loop)
	{
		switch (LookaHeadToken)
		{
		case CToken::LLRD_Token(','):
			LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token(','));
			LookaHeadToken = TerminalItem(LookaHeadToken);
//			GetLexer()->GetSymTab()->PrintTable(LogFile());
			break;
		default:
			Loop = FALSE;
			break;
		}
	}
	return LookaHeadToken;
}

CToken::LLRD_Token CParser::TerminalItem(CToken::LLRD_Token LookaHeadToken)
{
	//----------------------------------------------
	// TerminalItem		-> STRING OptInit
	//					-> .
	//					;
	//----------------------------------------------
	CSymbol* pSym;
	CStackItem* pSI;
	CSetMember* pSM;

	switch (LookaHeadToken)
	{
	case CToken::LLRD_Token::STRING:
		//---- Create a new symbol
		pSym = new CSymbol;
		pSym->Create(GetLexer()->GetLexbuff(), CSymbol::TokenType::PREDEFINED);
		pSym->SetTokenValue(CToken::TERMINAL, CSymbol::TokenType::PREDEFINED);
		pSym->SetLineWhereDefined(GetLexer()->GetLineNumber());
		//---------- Push Symbol onto value stack -----------------
		pSI = new CStackItem;
		pSI->Create(CStackItem::ValueType::pSYMBOL, pSym);
		GetValueStack()->Push(pSI);
		GetLexer()->GetSymTab()->AddSymbol(pSym);
		//-------- Make set member for Symbol -------------------
		pSM = new CSetMember;
		pSM->Create(pSym);
		//---------- Add symbol to Terminal Set ---------------
		GetLexer()->GetSymTab()->GetTerminalSet()->AddToSet(pSM);
		//-------------------------------------------
		GetLexer()->GetSymTab()->GetTerminalSet()->Print(LogFile());
		pSM = new CSetMember;
		pSM->Create(pSym);
		pSym->GetFirstSet()->AddToSet(pSM);
		pSym->GetFirstSet()->Print(LogFile());
		//-------------------------------------------
		LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token::STRING);
		if (Accept(LookaHeadToken, CToken::LLRD_Token::STRING))
		{
			fprintf(stderr, "Missing , perhaps in Line %d", GetLexer()->GetLineNumber());
			CloseFiles();
			exit(-1);
		}
		LookaHeadToken = OptInit(LookaHeadToken);
		break;
	}
	return LookaHeadToken;
}

CToken::LLRD_Token CParser::OptInit(CToken::LLRD_Token LookaHeadToken)
{
	//------------------------------------------------
	// OptInit	-> '=' NUMBER
	//			-> .
	//			;
	//------------------------------------------------
	CSymbol* pSym;
	CStackItem *pSI;
	int NumberValue;

	switch (LookaHeadToken)
	{
	case CToken::LLRD_Token('='):
		LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token('='));
		pSI = GetValueStack()->Look(CStack::StackPosition::STACK_TOP);
		CStackItem::CheckStackItem(pSI, CStackItem::ValueType::pSYMBOL);
		pSym = (CSymbol*)pSI->GetPointerData();
		NumberValue = GetLexer()->GetNumber();
		pSym->SetTargetTokenValue(NumberValue);
		SetAutoIncTokenValue(NumberValue);
		LookaHeadToken = Expect(LookaHeadToken, CToken::LLRD_Token::NUMBER);
		break;
	default:
		pSI = GetValueStack()->Look(CStack::StackPosition::STACK_TOP);
		CStackItem::CheckStackItem(pSI, CStackItem::ValueType::pSYMBOL);
		pSym = (CSymbol*)pSI->GetPointerData();
		NumberValue = GetAutoIncTokenValue();;
		pSym->SetTargetTokenValue(NumberValue);
		break;
	}
	return LookaHeadToken;
}

void CParser::UnexpectedToken(CToken::LLRD_Token Tokenvalue)
{
	char* s;

	s = new char[256];
	sprintf_s(s, 256, "Unexpected Token %s\n",
		CToken::LookupTokenName(Tokenvalue)
	);
	GetLexer()->Error(stderr, s);
	delete[] s;
	exit(-1);
}
