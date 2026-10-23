#include "pch.h"

CRecDecParGen::CRecDecParGen():CParser()
{
	m_pLog = 0;
}

CRecDecParGen::~CRecDecParGen()
{
}

BOOL CRecDecParGen::Create(FILE* pIn,  FILE* pLog)
{
	m_pLog = pLog;
	CParser::Create(pIn);
	GetNullableSet()->Create("Nullables","NULLABLES");
	GetNotNullables()->Create("NOT_Nullables", "NOTNULLABLE");
	return TRUE;
}

void CRecDecParGen::CloseFiles()
{
	fclose(m_pLog);
	CParser::CloseFiles();
}

BOOL CRecDecParGen::Run()
{
	CSetMember* pDollarSign;
	CSetMember* pEmptySymbol;;
	int UndefinedNonTerminals;
	int Orphans;
	int Recursions;
	FILE* pLog;
	CSymTab* pSymTab = 0;
	pLog = NULL;
	pLog = LogFile();

	//-----------------------------------
	// Parse the Grammar
	//-----------------------------------
	Parse();
	//if(pLog)
	//	fprintf(pLog, "*************  Print Out Grammar Structure************\n");
	//if(pLog)
	//	PrintGrammar(LogFile());
	//------------------------------------
	// Check for Undefined Non Terminals
	//------------------------------------
	UndefinedNonTerminals = GetLexer()->GetSymTab()->CheckForUnUsedNonTerminals(stderr);
	fprintf(stderr, "Undefined Non Terminals:%d\n", UndefinedNonTerminals);
	if (UndefinedNonTerminals)
	{
		CloseAllFiles();
		exit(10);
	}
	//---------------------------------------
	// Check for Orphans
	//---------------------------------------
/*	Orphans = GetLexer()->GetSymTab()->CheckForOrphans(stderr, GetLexer()->GetSymTab()->GetNonTerminalSet());
	fprintf(stderr, "Total Orphans = %d\n", Orphans);
	if (Orphans)
	{
		CloseAllFiles();
		exit(11);
	}*/
	pSymTab = GetLexer()->GetSymTab();
	pSymTab->FindOrphanTerminalSymbols();
	//----------------------------------------------------------------
	//------------------- Check for Left recursion -------------------
	//----------------------------------------------------------------
	Recursions = CheckForDirectRecursion(stderr, GetLexer()->GetSymTab()->GetNonTerminalSet());
	fprintf(stderr, "Total Left Recursions = %d\n", Recursions);
	if (Recursions)
	{
		CloseAllFiles();
		exit(12);
	}
	//----------------------------
	// Add $ and Epsilon to terminal list
	//----------------------------
	pEmptySymbol = new CSetMember;
	pEmptySymbol->Create(CLexer::GetEmpty());
	GetLexer()->GetSymTab()->GetTerminalSet()->AddToSet(pEmptySymbol);
	pDollarSign = new CSetMember;
	pDollarSign->Create(CLexer::GetEndOfTokenStream());
	GetLexer()->GetSymTab()->GetTerminalSet()->AddToSet(pDollarSign);
	GetLexer()->GetSymTab()->PrintTable(0);
	//-----------------------------------
	// Print the set of Non Terminals
	//-----------------------------------
	GetLexer()->GetSymTab()->GetTerminalSet()->Print(LogFile(), FALSE, TRUE, 0);
	GetLexer()->GetSymTab()->GetNonTerminalSet()->Print(LogFile(), FALSE, TRUE, 0);
	//------------------------------------
	// Create Nullable Set
	//------------------------------------
	fprintf(LogFile(), "*********** Build Nullable Set **********\n");
	CreateNullableSet(NULL);
	GetNullableSet()->Print(NULL,FALSE,TRUE,0);
	fprintf(LogFile(), "****************Create NOT nullable set ************************\n");
	CreateNotNullablesSet(NULL);
	//----------------------------------------------
	// Create First Set
	//----------------------------------------------
	fprintf(LogFile(), "----- Calc First Sets------\n");
	CreateFirstSets(0);
	GetLexer()->GetSymTab()->PrintFirstSets(LogFile());
	//---------------------------------------
	// Create Follow Sets
	//---------------------------------------
	fprintf(LogFile(), "----- Calc Follow Sets------\n");
	CreateFollowSets(NULL);
	fprintf(LogFile(), "------------ Follow Sets ---------------\n");
	GetLexer()->GetSymTab()->PrintFollowSets(LogFile(), FALSE, TRUE, 0);
	//--------------------------------------
	// Create Parse Table
	//--------------------------------------
	fprintf(LogFile(), "---------------- Create Parse Table --------------\n");
	GetParseTable()->Create(
		GetLexer()->GetSymTab()->GetTerminalSet(), 
		GetLexer()->GetSymTab()->GetNonTerminalSet()
	);
	GetParseTable()->FillTable(NULL);
	fprintf(LogFile(), "Table Entries = %d\n", 
		GetParseTable()->GetNumberOfEntries());
	//--------------------------------------------
	// Check For Conflicts
	//--------------------------------------------
	fprintf(LogFile(), "------------ Check For Conflicts-------------\n");
	fprintf(stderr , "Conflicts = %d\n",
		GetParseTable()->CheckForConflicts(stderr)
	);
	return TRUE;
}

int CRecDecParGen::CheckForDirectRecursion(FILE* pO, CSet* pNonTerminals)
{
	int DirectRecursionCount = 0;
	CSetMember* pSetMember;
	CSetMember* pSetMemLHS;
	CRule* pRule;
	CRule* pLHSrules;
	CLexeme* pLHSRuleLexeme;

	pO = stderr;
	pSetMemLHS = pNonTerminals->GetHead();
	while (pSetMemLHS)
	{
		//---------------------------------------
		// Scan Through all non terminals
		// pSetMember
		//---------------------------------------
		pSetMember = pNonTerminals->GetHead();
		while (pSetMember)
		{
			if (pSetMemLHS != pSetMember)	// if equal, skip
			{
				//--------------------------------------------
				// Indirect Recursion
				//--------------------------------------------
				pRule = pSetMember->GetRule();
				while (pRule)
				{
					if (pRule->GetHead() == NULL)
					{
						fprintf(stderr, "ERROR: Rule with No Lexemes\n");
						fprintf(stderr, "Missing '.' Perhaps?");
						pRule->Print(pO, TRUE, TRUE, 0, TRUE);
						exit(14);
					}
					//--------------------------------------
					// Check to see if pSetMemLHS is in
					// the left most lexemes of the
					// rules for pSetMember
					//--------------------------------------
					if (pRule->GetHead()->GetLexemeSymbol() == 
						pSetMemLHS->GetSetMemberSymbol()
					)
					{
						//------------------------------------
						// Now check to see if pSetMember is
						// in any of the rules for for
						// pSetMemLHS
						//------------------------------------
						pLHSrules = pSetMemLHS->GetRule();
						while (pLHSrules)
						{
							pLHSRuleLexeme = pLHSrules->GetHead();
							while (pLHSRuleLexeme)
							{
								if (pLHSRuleLexeme->GetLexemeSymbol() ==
									pSetMember->GetSetMemberSymbol()
									)
								{
									//-------------------------------
									// We have an Indirect Recursion
									//-------------------------------
									if(pO) fprintf(pO, "-----------------------\n");
									if (pO) fprintf(pO, "Warning:Possible Indirect Left Recursion\n");
									pRule->Print(pO, TRUE, TRUE, 5, TRUE);
									pLHSrules->Print(pO, TRUE, TRUE, 5, TRUE);
								}
								pLHSRuleLexeme = pLHSRuleLexeme->GetNext();
							}
							pLHSrules = pLHSrules->GetNext();
						}
					}
					pRule = pRule->GetNext();
				}
			}
			else
			{
				//----------------------------------
				// Direct Recursion
				//----------------------------------
				pRule = pSetMemLHS->GetSetMemberSymbol()->GetHead();
				if (pRule->GetLHS() ==
					pRule->GetHead()->GetLexemeSymbol()
				)
				{
					if (pO) fprintf(pO, "---------------------\n");
					if (pO) fprintf(pO, "Error:Direct Left Recursion\n");
					pSetMember->GetRule()->GetHead()->GetLexemeSymbol()->Print(pO);
					pRule->GetHead()->GetLexemeSymbol()->Print(pO);
					pRule->Print(pO, TRUE, TRUE, 5, TRUE);
					DirectRecursionCount++;
				}
			}
			pSetMember = pSetMember->GetNext();
		}
		pSetMemLHS = pSetMemLHS->GetNext();
	}
    return DirectRecursionCount;
}


void CRecDecParGen::CreateFirstSets(FILE* pOut)
{
	//------------------------------------
	// CreateFirstSets:
	//	1.	If X is a terminal then 
	//			First(X) is just X!
	//	2.	If there is a Production
	//			 X ? ? then add ? to 
	//			first(X)
	//	3.	If there is a Production 
	//			X ? Y1Y2..Yk then add 
	//			first(Y1Y2..Yk) to 
	//			first(X)
	//	4.	First(Y1Y2..Yk) is either
	//		1.	First(Y1) (if First(Y1) 
	//				doesn't contain ?)
	//		2.	OR(if First(Y1) does 
	//			contain ?) then 
	//			First(Y1Y2..Yk) is 
	//			everything in First(Y1) 
	//			< except for ? > as well 
	//			as everything in First(Y2..Yk)
	//		3.	If First(Y1) First(Y2)..
	//			First(Yk) all contain ? 
	//			then add e to 
	//			First(Y1Y2..Yk) as well.
	//------------------------------------
	BOOL bChanged;
	CSetMember* pNonTerminalSetMember;
	CSymbol* pSymLHS;	//left hand side of production
	CRule* pRule;
	CSet* pFIRST_X;
	int Iterration = 1;
	BOOL bDebugTag = FALSE;

	if (pOut) fprintf(pOut, "******* Create First Set Ver 2 ********\n");
	do
	{
		if (pOut) fprintf(pOut, "###### First Set Iteration #%d\n", Iterration++);
		bChanged = FALSE;
		pNonTerminalSetMember = GetLexer()->GetSymTab()->GetNonTerminalSet()->GetHead();
		while (pNonTerminalSetMember)
		{
			if (strcmp("AddExpr_1", pNonTerminalSetMember->GetSetMemberSymbol()->GetName()) == 0)
				bDebugTag = TRUE;
			else
				bDebugTag = FALSE;
			if (pOut) fprintf(pOut, ">%s ------------< Iteration %d >----------\n", 
				pNonTerminalSetMember->GetSetMemberSymbol()->GetName(),
				Iterration
			);
			if (pOut) fprintf(pOut, "FIRST(%s)\n", pNonTerminalSetMember->GetSetMemberSymbol()->GetName());
			pSymLHS = pNonTerminalSetMember->GetSetMemberSymbol();
			pRule = pSymLHS->GetHead();
			pFIRST_X = pSymLHS->GetFirstSet();
			while (pRule)
			{
				if (pOut) fprintf(pOut, "%s\t", pSymLHS->GetName());
				pRule->Print(pOut);
				bChanged |= pRule->FIRST(
					pOut,
					pRule->GetHead(),
					pSymLHS->GetFirstSetRef(),
					bDebugTag
				);
				pSymLHS->GetFirstSetRef().Print(pOut, FALSE, TRUE);
				pRule = pRule->GetNext();
			}
			pNonTerminalSetMember = pNonTerminalSetMember->GetNext();
		}
		if(pOut) fprintf(pOut, "###### Iteration #%d  DONE!\n", Iterration);
	} while (bChanged);
}

void CRecDecParGen::CreateFollowSets(FILE* pOut)
{
	//-------------------------------------------
	// CreateFollowSets
	// 
	// 
	//	1.	First put $(the end of input 
	//		marker) in Follow(S) (S is the
	//		 start symbol)
	//	2.	If there is a production A->aBb,
	//		 (where a can be a whole string)
	//		 then everything in FIRST(b) 
	//		except for e is placed in FOLLOW(B).
	//	3.	If there is a production A->aB, then
	//		 everything in FOLLOW(A) is in FOLLOW(B)
	//	4.	If there is a production A->aBb,
	//		 where FIRST(b) contains e, then 
	//		everything in FOLLOW(A) is in 
	//		FOLLOW(B)	//
	//-------------------------------------------
	CSymbol* pSym;
	CSetMember* pNonTerminalSetMember;
	int Iteration = 0;
	BOOL bChanged = FALSE;
	int LoopCount;
	int Itteration = 0;
	//----------------------------------------------
	// Rule #1 is NOT needed (ha-ha)
	//----------------------------------------------
	//---------------------------------------------
	// Find the FOLLOW set for all of the other non
	// terminal symbols.
	//---------------------------------------------
	LoopCount = 100;
	do
	{
		if(pOut) fprintf(pOut, "######### Follow Set Itteration #%d\n", ++Iteration);
		bChanged = FALSE;
		pNonTerminalSetMember = GetLexer()->GetSymTab()->GetNonTerminalSet()->GetHead();
		while (pNonTerminalSetMember)
		{
			pSym = pNonTerminalSetMember->GetSetMemberSymbol();
			bChanged |= ApplyFollowRulesTo(pOut, pSym);
			pNonTerminalSetMember = pNonTerminalSetMember->GetNext();
		}
	} while (bChanged && --LoopCount);	//end of do loop
}

BOOL CRecDecParGen::ApplyFollowRulesTo(FILE* pOut, CSymbol* pSym)
{
	//-------------------------------------------------
	// ApplyFollowRulesTo
	// 
	// pSym is a Non Terminal symbol.  We use this
	// symbol to go through all of the productions
	// looking for a production that the FOLLOW SET
	// rules can be applied to.
	//-------------------------------------------------
	BOOL bChanged = FALSE;
	CSymbol* pProductionSymbol;
	CSetMember* pNonTerminalSetMember;
	CRule* pRule;

	pNonTerminalSetMember = GetLexer()->GetSymTab()->GetNonTerminalSet()->GetHead();
	if(pOut) fprintf(pOut, "------------Looking For %s ---------------\n", pSym->GetName());
	while (pNonTerminalSetMember)
	{
		pProductionSymbol = pNonTerminalSetMember->GetSetMemberSymbol();
		pRule = pProductionSymbol->GetHead();
		while (pRule)
		{
			bChanged |= pRule->DoesThisRuleHave(pOut, pSym);
			pRule = pRule->GetNext();
		}
		pNonTerminalSetMember = pNonTerminalSetMember->GetNext();
	}
	return bChanged;
}

void CRecDecParGen::CreateNullableSet(FILE* pOut)
{
	//-----------------------------------------------
	// CreateNullableSet
	//-----------------------------------------------
	CSymbol* pSymLHS;
	CRule* pRule;
	CSetMember* pMember;
	CSetMember* pNTLHS;
	int Iteration = 0;
	BOOL bChanged = FALSE;
	BOOL bNull = FALSE;

	if(pOut) fprintf(pOut,"----- Generate Nullables-------\n");
	do
	{
		bChanged = FALSE;
		pNTLHS = GetLexer()->GetSymTab()->GetNonTerminalSet()->GetHead();
		while (pNTLHS)
		{
			bNull = FALSE;
			pSymLHS = pNTLHS->GetSetMemberSymbol();
			pRule = pSymLHS->GetHead();
			while (pRule && !bNull)
			{
				pRule->Print(pOut, TRUE, TRUE, 0);
				bNull |= pRule->DoesThisRuleHaveEpsilon();
				if (bNull)
				{
					pSymLHS->SetNullable(TRUE);
					pMember = new CSetMember;
					pMember->Create(pSymLHS);
					bChanged |= GetNullableSet()->AddToSet(pMember);
				}
				bNull = FALSE;
				pRule = pRule->GetNext();
			}
			pNTLHS = pNTLHS->GetNext();
		}
		if(pOut) fprintf(pOut, "************ Nullable Iteration %d *********\n", ++Iteration);
		if (Iteration > 100)
			bChanged = FALSE;
	} while (bChanged);
	if(pOut) fprintf(pOut, "----- Done Generate Nullables-------\n");
}


void CRecDecParGen::CreateNotNullablesSet(FILE* pOut)
{
	CBucket* pBucket;
	CBin* pBin;
	CSymbol* pSym;
	CSetMember* pMember;
	int i;


	for (i = 0; i < GetLexer()->GetSymTab()->GetTableSize(); ++i)
	{
		pBucket = GetLexer()->GetSymTab()->GetBucket(i);
		if (pBucket)
		{
			pBin = pBucket->GetHead();
			while (pBin)
			{
				pSym = (CSymbol*)pBin;
				if (pSym->IsNotNullable() && pSym->IsNonTerminal())
				{
					pMember = new CSetMember;
					pMember->Create(pSym);
					GetNotNullables()->AddToSet(pMember);
				}
				pBin = pBin->GetNext();
			}
		}
	}
}

void CRecDecParGen::PrintGrammar(FILE* pOut)
{
	CSymTab* pSymTab = 0;
	CSet* pNonTerm;
	CSetMember* pNonTermSetMember;

	pNonTerm = GetLexer()->GetSymTab()->GetNonTerminalSet();
	pNonTermSetMember = pNonTerm->GetHead();
	while (pNonTermSetMember)
	{
		if (pNonTermSetMember->GetSetMemberSymbol()->IsNonTerminal())
		{
			pNonTermSetMember->GetSetMemberSymbol()->PrintProduction(pOut, TRUE, TRUE, 0);
		}
		pNonTermSetMember = pNonTermSetMember->GetNext();
	}
}

