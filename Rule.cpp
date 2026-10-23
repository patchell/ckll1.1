#include "pch.h"

CRule::CRule()
{
	m_pHead = 0;
	m_pTail = 0;
	m_pNext = 0;
	m_pPrev = 0;
	m_nLexemes = 0;
	m_pLHS = 0;
	m_LineWhereDefined = 0;
}

CRule::~CRule()
{
}

BOOL CRule::Create(CSymbol* pLHS)
{
	m_pLHS = pLHS;
	return TRUE;
}


BOOL CRule::FIRST(
	FILE* pOut, 
	CLexeme *pY1, 
	CSet& FirstSet,
	BOOL dbTag
)
{
	//-----------------------------------
	//	1.	If X is a terminal then 
	//			First(X) is just X!
	//	2.	If there is a Production (i.e. Nullable)
	//			 X -> e then add e to 
	//			first(X)
	//	3.	If there is a Production 
	//			X -> Y1Y2..Yk then add 
	//			first(Y1Y2..Yk) to 
	//			first(X)
	//	4.	First(Y1Y2..Yk) is either
	//		1.	First(Y1) (if First(Y1) 
	//				doesn't contain epsilon)
	//		2.	OR(if First(Y1) does 
	//			contain epsilon) then 
	// 
	//			First(Y1Y2..Yk) is 
	//			everything in First(Y1) 
	//			< except for epsilon > 
	//			as well as everything in First(Y2..Yk)
	//		3.	If First(Y1) First(Y2)..
	//			First(Yk) all contain epsion 
	//			then add epsilon to 
	//			First(Y1Y2..Yk) as well.
	//-----------------------------------
	BOOL bChanged = FALSE;
	CLexeme* pLexeme = GetHead();
	int n = GetNumLexemes();


	if (pY1->IsEmpty())	//Rule 2.
	{
		if (dbTag && pOut)
		{
			fprintf(pOut, "             Rule 2:");
		}
		bChanged = FirstSet.Union(pOut,CLexer::GetEmpty()->GetFirstSet());
		if (dbTag && pOut)
		{
			fprintf(pOut, "%s\n", bChanged?"Changed":"Not Changed");
		}
	}
	else if (pY1->GetFirstSet()->DoesNotContain(pOut, CLexer::GetEmpty()))
	{
		//Rule 4.1
		if (dbTag && pOut)
		{
			fprintf(pOut, "             Rule 4.1\n");
		}
		bChanged = FirstSet.Union(pOut, pY1->GetFirstSet());
		if (dbTag && pOut)
		{
			fprintf(pOut, "Rule 4.1:%s\n", bChanged ? "Changed" : "Not Changed");
		}
	}
	else
	{
		//Rule 4.2
		if (dbTag && pOut)
		{
			fprintf(pOut, "             Rule 4.2");
		}
		if (pY1->GetFirstSet()->Contains(pOut, CLexer::GetEmpty()))
		{
			bChanged = FIRST_Y1Y2__Yk(pOut, pY1, FirstSet);
		}
		if (dbTag && pOut)
		{
			fprintf(pOut, "%s\n", bChanged ? "Changed" : "Not Changed");
		}
	}
	//Rule 4.3
	if (DoAllLexemesContainEpsilong())
		FirstSet.Union(pOut, CLexer::GetEmpty()->GetFirstSet());
	return bChanged;
}


BOOL CRule::FIRST_Y1Y2__Yk(
	FILE* pOut,
	CLexeme* pY1, 
	CSet& FirstSet
)
{
	//-------------------------------------
	//	4.	First(Y1Y2..Yk) is either
	//		2.	OR(if First(Y1) does 
	//			contain epsilon) then 
	//			First(Y1Y2..Yk) is 
	//			everything in First(Y1) 
	//			< except for epsilon > as well 
	//			as everything in First(Y2..Yk)
	//-------------------------------------
	BOOL bChanged = FALSE;

	if (pY1->GetNext() == 0)
	{
		//---------------------------------
		// End of the chain, from here we
		// will be only returning.
		//---------------------------------
		if(pOut) fprintf(pOut, "End of Y1Y2...Yk Chain\n");
	}
	else
	{
		//-------------------------------------------
		// There are still more Lexemes
		//-------------------------------------------
		bChanged = FIRST_Y1Y2__Yk(
			pOut, 
			pY1->GetNext(), 
			FirstSet
		);
		bChanged |= FirstSet.UnionExceptEmpty(pY1->GetFirstSet());
	}
	return bChanged;
}

BOOL CRule::FOLLOW(FILE* pOut)
{
	//	1.	First put $(the end of input 
	//		marker) in Follow(S) (S is the
	//		 start symbol)
	//	2.	If there is a production A->aBb,
	//		 (where a can be a whole string)
	//		 then everything in FIRST(b) 
	//		except for e is placed in FOLLOW(B).
	//		NOTE:We are looking for a NON Terminal
	//		followed by "something", which can be
	//		either a Terminal or NON Terminal
	//	3.	If there is a production A->aB, then
	//		 everything in FOLLOW(A) is in FOLLOW(B)
	//		NOTE:B is a Non Terminal
	//	4.	If there is a production A->aBb,
	//		 where FIRST(b) contains e, then 
	//		everything in FOLLOW(A) is in 
	//		FOLLOW(B)
	//		NOTE:We have a NON Terminal followed
	//		by another NON Terminal that has
	//		e in its first set
	//-------------------------------------------
	BOOL bChanged = FALSE;
	CLexeme* pLast, * pNextToLast;

	pLast = GetTail();
	pNextToLast = pLast->GetPrev();
	if (pNextToLast &&
		pNextToLast->IsNonTerminal() &&
		pLast->GetFirstSet()->Contains(pOut, CLexer::GetEmpty())
	)
	{
		//------------
		// Rule #4
		//------------
		if(pOut) fprintf(pOut, "Apply Rule 4\n");
//		bChanged = FOLLOW_RULE4();
		if(pOut) fprintf(pOut, "----------\n");
	}
	else if (pNextToLast && 
		pNextToLast->IsNonTerminal()
	)
	{
		//------------
		// Rule #2
		//------------
		if(pOut) fprintf(pOut, "Apply Rule 2\n");
//		bChanged = FOLLOW_RULE2();
		if(pOut) fprintf(pOut, "----------\n");
	}
	else if (pLast->IsNonTerminal())
	{
		//------------
		// Rule #3
		//------------
		if(pOut) fprintf(pOut, "Apply Rule 3\n");
//		bChanged = FOLLOW_RULE3();
		if(pOut) fprintf(pOut, "----------\n");
	}
	if(pOut) fprintf(pOut, "Changed?:%s\n", bChanged ? "TRUE" : "FALSE");
	return bChanged;
}

BOOL CRule::FOLLOW_RULE2(FILE* pOut, CSymbol* pA, CSymbol* pB, CSymbol *pb)
{
	//-------------------------------------------
	//	2.	If there is a production A->aBb,
	//		 (where a can be a whole string)
	//		 then everything in FIRST(b) 
	//		except for e is placed in FOLLOW(B).
	//-------------------------------------------
	BOOL bChanged;

	bChanged = pB->GetFollowSet()->UnionExceptEmpty(pb->GetFirstSet());
	return bChanged;
}

BOOL CRule::FOLLOW_RULE3(FILE* pOut, CSymbol *pA, CSymbol* pB)
{
	//-------------------------------------------
	//	3.	If there is a production A->aB, then
	//		 everything in FOLLOW(A) is in 
	//		FOLLOW(B)
	//-------------------------------------------
	BOOL bChanged;

	bChanged = pB->GetFollowSet()->Union(pOut, pA->GetFollowSet());
	return bChanged;
}

BOOL CRule::FOLLOW_RULE4(
	FILE* pOut, 
	CSymbol* pA, 
	CSymbol* pB, 
	CSymbol *pb
)
{
	//-------------------------------------------
	//	4.	If there is a production A->aBb,
	//		 where FIRST(b) contains e, then 
	//		everything in FOLLOW(A) is in 
	//		FOLLOW(B)	
	//-------------------------------------------
	BOOL bChanged;

	bChanged = pB->GetFollowSet()->Union(pOut, pA->GetFollowSet());
	return bChanged;
}

BOOL CRule::DoAllLexemesContainEpsilong()
{
	BOOL bAllLexemesContainEpsilong = TRUE;
	CLexeme* pLexeme = GetHead();

	while (pLexeme && bAllLexemesContainEpsilong)
	{
		bAllLexemesContainEpsilong = pLexeme->GetLexemeSymbol()->DoesThisProductionHaveEpsilon();
		pLexeme = pLexeme->GetNext();
	}
	return bAllLexemesContainEpsilong;
}

BOOL CRule::DoesThisRuleHaveEpsilon()
{
	//--------------------------------------
	// Epsilon had damn well be the first
	// Lexeme in the rule.
	//--------------------------------------
	BOOL bRuleHasEpsilon = TRUE;
	CLexeme* pLexeme;
	CSymbol* pEpsilon;

	pEpsilon = CLexer::GetEmpty();
	pLexeme = GetHead();
	if (!pLexeme)
		CLexer::Error(stderr, "Lexeme is NULL");
	if (pLexeme->GetLexemeSymbol() != pEpsilon)
		bRuleHasEpsilon = FALSE;
	return bRuleHasEpsilon;
}

BOOL CRule::DoesThisRuleHave(FILE* pOut, CSymbol* pSym)
{
	BOOL bYes = FALSE;
	CLexeme* pLexemeLast;
	CLexeme* pLexemeNextToLast;
	int Count = 0;

	pLexemeLast = GetTail();
	pLexemeNextToLast = pLexemeLast->GetPrev();
	if (pLexemeNextToLast)
	{
		if (pLexemeNextToLast->GetLexemeSymbol() == pSym)
		{
			if(pOut) fprintf(pOut, "\t%s", GetLHS()->GetName());
			Print(pOut);
			if (pLexemeNextToLast->IsNonTerminal())
			{
				if (pLexemeLast->GetFirstSet()->Contains(pOut, CLexer::GetEmpty()))
				{
					if(pOut) fprintf(pOut, "Rule 2 3 4:%s", GetLHS()->GetName());
					Print(pOut);
					bYes |= FOLLOW_RULE2(
						pOut,
						GetLHS(),
						pLexemeNextToLast->GetLexemeSymbol(),
						pLexemeLast->GetLexemeSymbol()
					);
					bYes |= FOLLOW_RULE3(
						pOut,
						GetLHS(),
						pLexemeLast->GetLexemeSymbol()
					);
					bYes |= FOLLOW_RULE4(
						pOut,
						GetLHS(), 
						pLexemeNextToLast->GetLexemeSymbol(),
						pLexemeLast->GetLexemeSymbol()
					);
				}
				else 
				{
					if(pOut) fprintf(pOut, "Rule 2 3:%s", GetLHS()->GetName());
					Print(pOut);
					bYes = FOLLOW_RULE2(
						pOut,
						GetLHS(),
						pLexemeNextToLast->GetLexemeSymbol(),
						pLexemeLast->GetLexemeSymbol()
					);
					bYes |= FOLLOW_RULE3(
						pOut,
						GetLHS(),
						pLexemeLast->GetLexemeSymbol()
					);
				}
			}
			else if (pLexemeLast->GetLexemeSymbol() == pSym)
			{
				if(pOut) fprintf(pOut, "Rule 3:%s", GetLHS()->GetName());
				Print(pOut);
				bYes = FOLLOW_RULE3(pOut, GetLHS(), pLexemeLast->GetLexemeSymbol());
			}
		}
	}
	else if (pLexemeLast->GetLexemeSymbol() == pSym)
	{
		if(pOut) fprintf(pOut, "Rule 3:%s", GetLHS()->GetName());
		Print(pOut);
		FOLLOW_RULE3(pOut, GetLHS(), pLexemeLast->GetLexemeSymbol());
	}
	return bYes;
}

BOOL CRule::RuleTotallyNullable()
{
	BOOL rV = FALSE;
	CLexeme* pLexeme = GetHead();
	int n = this->GetNumLexemes();
	int count = 0;

	while (pLexeme)
	{
		if (pLexeme->IsNullable())
			count++;
		pLexeme = pLexeme->GetNext();
	}
	if (count == n)
		rV = TRUE;
	return rV;
}

BOOL CRule::IsRuleEmpty()
{
	BOOL rV;
	
	rV = GetHead()->IsEmpty();
	return rV;
}

void CRule::Error(FILE* pOut, const char* pES)
{
	fprintf(pOut, "In Production %s Line%d : %s\n",
		GetLHS()->GetName(),
		GetLineWhereDefined(),
		pES
	);
}

void CRule::Print(FILE* pOut, BOOL bLHS, BOOL bEOL, int nIndentSpaces, BOOL bLineNumber)
{
	CLexeme* pL = GetHead();
	char* s = new char[256];
	if (pOut)
	{
		if (bLineNumber)
		{
			fprintf(pOut, "%sLine-%d:",
				IndentString(s, nIndentSpaces),
				GetLHS()->GetLineWhereDefined()
			);
			nIndentSpaces = 0;
		}
		if (bLHS)
			fprintf(pOut, "%s%s ->",
				IndentString(s, nIndentSpaces),
				GetLHS()->GetName());
		else
			fprintf(pOut, "%s ->", IndentString(s, nIndentSpaces));
		if (pL)
		{
			while (pL)
			{
				pL->Print(pOut, FALSE, FALSE, 1);
				pL = pL->GetNext();
			}
		}
		else
		{
			fprintf(pOut, "?? Null Lexeme: BAD! ??");
		}
		if (bEOL)
			fprintf(pOut, "\n");

	}
	delete[] s;
}

int CRule::AddLexeme(CLexeme* pRO)
{
	if (GetTail())
	{
		GetTail()->SetNext(pRO);
		pRO->SetPrev(GetTail());
		SetTail(pRO);
	}
	else
	{
		SetTail(pRO);
		SetHead(pRO);
	}
	return IncNumLexemes();
}
