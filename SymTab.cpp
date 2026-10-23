#include "pch.h"

//***************************************
// Implementation of Hash Table Member
// Functions
//***************************************

//-------------------------------------------
// Constructor
//
// parameter:
//	size......number of Bins in the table
//
// Now, don't ask me why, but I heard that
// it is best to use a prime number for the
// number of bins.  I suspect the reason why
// is that we are doing a MOD opertion in
// the hashing function to make sure the
// index ends up in the table.
//
// And if you are like me, you can't
// remember what the prime number are.
// So, here is a list form 0 to 200.
// 2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
//  31, 37, 41, 43, 47, 53, 59, 61, 67
// 71, 73, 79, 83, 89, 97, 101, 103, 107,
// 109, 113, 127, 131, 137, 139, 149,
// 151, 157, 163, 167, 173, 179, 181,
// 191, 193, 197, 199
//--------------------------------------------

//*****************************************************
// Destructor
//
// destroy HashTab and all of its components
//*****************************************************

CSymTab::~CSymTab()
{
	CBucket* pBucket;
	int i = 0;

	for (i = 0; i < m_tSize; ++i)
	{
		pBucket = m_ppTab[i];
		delete pBucket;
	}
	
	if (m_ppTab) delete[] m_ppTab;
}

BOOL CSymTab::Create(int TableDepth)
{
	m_ppTab = new CBucket * [TableDepth];
	m_tSize = TableDepth;
	for (int i = 0; i < m_tSize; ++i)
		m_ppTab[i] = 0;
	m_nElements = 0;
	GetTerminalSet()->Create("Terminals", "TERMINALS");
	GetNonTerminalSet()->Create("Non Terminals", "NONTERMINALS");
	return TRUE;
}

//*****************************************************
// FindSym
//
// parameter:
//	name....name of object to find
//
//******************************************************

CBin* CSymTab::FindSymbol(const char* name)
{
	CBin* pRV = NULL;

	int Index = Hash(name);	//get index from hash of name
	if(m_ppTab[Index] != 0)
		pRV = m_ppTab[Index]->Find(name);
	return pRV;
}

CBin* CSymTab::FindStartSymbol()
{
	CBucket* pBucket;
	CBin* pBin = 0;
	BOOL Loop;
	int nB = GetNumElements();
	int i;

	for (i = 0; i < nB; ++i)
	{
		pBucket = GetBucket(i);
		if (pBucket)
		{
			pBin = pBucket->GetHead();
			Loop = TRUE;
			while (pBin && Loop)
			{
				if (pBin->IsStartSymbol())
				{
					Loop = FALSE;
					i = 100000;
				}
			}
		}
	}
    return pBin;
}

CSymbol* CSymTab::FindFirstPredefinedToken()
{
	CSymbol* pSymLowestValue = 0;
	CSymbol* pSym;
	CSetMember* pSetMem;

	pSetMem = GetTerminalSet()->GetHead();
	while (pSetMem)
	{
		pSetMem->GetSetMemberSymbol()->Print(LogFile());
		if (pSymLowestValue)
		{
			pSym = pSetMem->GetSetMemberSymbol();
			if (pSym->IsPredefinedToken())
			{
				if (pSym->GetTargetTokenValue() < UINT(pSymLowestValue->GetTargetTokenValue()))
					pSymLowestValue = pSym;
			}
		}
		else
		{
			if (pSetMem->GetSetMemberSymbol()->IsPredefinedToken())
				pSymLowestValue = pSetMem->GetSetMemberSymbol();
		}
		pSetMem = pSetMem->GetNext();
	}
    return pSymLowestValue;
}

CSymbol* CSymTab::FindNextPredefineToken(CToken::LLRD_Token Value)
{
	CSymbol* pSymLowestValue = 0;
	CSymbol* pSym;
	CSetMember* pSetMem;

	pSetMem = GetTerminalSet()->GetHead();
	while (pSetMem)
	{
		if (pSetMem->GetSetMemberSymbol()->IsPredefinedToken())
		{
			if (pSymLowestValue)
			{
				if (pSetMem->GetSetMemberSymbol()->GetTargetTokenValue() > UINT(Value))
				{
					pSym = pSetMem->GetSetMemberSymbol();
					if (pSym->IsPredefinedToken())
					{
						if (pSym->GetTargetTokenValue() < pSymLowestValue->GetTargetTokenValue())
							pSymLowestValue = pSym;
					}
				}
			}
			else
			{
				if (pSetMem->GetSetMemberSymbol()->IsPredefinedToken())
				{
					if (pSetMem->GetSetMemberSymbol()->GetTargetTokenValue() > UINT(Value))
					{
						pSymLowestValue = pSetMem->GetSetMemberSymbol();
					}
				}
			}
		}
		pSetMem = pSetMem->GetNext();
	}
	return pSymLowestValue;
}

//***********************************************************
// AddSym
//
// parameter:
//	pSym....pointer to Bin to add to table
//
//***********************************************************

void CSymTab::AddSymbol(CBin* pSym)
{
	int Index = Hash(pSym->GetName());	//generate index
	if (m_ppTab[Index] == NULL)	//there is NO bucket here
	{
		//------------------------------------
		// Create a new bucket
		//------------------------------------
		m_ppTab[Index] = new CBucket;
		m_ppTab[Index]->Create();
	}
	if (m_ppTab[Index]->GetHead() == NULL)	//nobody home
	{
		m_ppTab[Index]->SetHead(pSym);
		m_ppTab[Index]->SetTail(pSym);
	}
	else
	{
		CBucket* pB = m_ppTab[Index];
		pB->GetTail()->SetNext(pSym);
		pSym->SetPrev(pB->GetTail());
		pB->SetTail(pSym);
	}
	++m_nElements;	//increment number of symbols
}

//*****************************************************
// DelSym
//
// parameter:
//	pSym.....pointer to symbol to remove
//*****************************************************

void CSymTab::DelSymbol(CBin* pSym)
{
	int HashValue = Hash(pSym->GetName());
	fprintf(LogFile(), "Delete Symbol %s  Hash=%d\n", pSym->GetName(), HashValue);
	m_ppTab[HashValue]->Delete(pSym);
}

int CSymTab::CheckForUnUsedNonTerminals(FILE* pOut)
{
	int Unused = 0;
	CSymbol* pSym;
	CBin* pBin;
	int i;

	if (pOut)
		fprintf(pOut, "***************** Check for Undefined Non Terminals **************************\n");
	for (i = 0; i < GetTableSize(); i++)
	{
		if (GetBucket(i))
		{
			pBin = GetBucket(i)->GetHead();
			while (pBin)
			{
				pSym = (CSymbol *) (pBin);
				if (pSym->GetTokenValue() == CToken::LLRD_Token::NONTERMINAL)
				{
					//------------------------------------
					// If there are no rules,
					// then the non-terminal is not
					// defined, or is it?
					//------------------------------------
					if (pSym->GetHead() == NULL)
					{
						if (pOut)
							fprintf(pOut, "  Non Terminal \'%s\' @Line %d is Undefined\n",
								pSym->GetName(),
								pSym->GetLineWhereDefined()
						);
						Unused++;
					}
				}
				pBin = pBin->GetNext();
			}
		}
	}
	if (pOut)
		fprintf(pOut, "***************** End of Check for Undefined Non Terminals *******************\n");
	return Unused;
}

int CSymTab::CheckForOrphans(FILE* pOut, CSet* pSetNonTerminals)
{
	//-------------------------------------------------------
	// CheckForOrphans
	// 
	// Check to see if all members of the set of NonTerminals
	// is actually used in a production
	//-------------------------------------------------------
	int Orphans = 0;
	CSetMember* pSMebr;

	pSMebr = pSetNonTerminals->GetHead();
	while (pSMebr)
	{
		if (pSMebr->GetSetMemberLexeme()->GetLexemeSymbol()->IsNotStartSymbol())
		{
			if (CheckOrphans_SearchRules(
				pOut,
				pSetNonTerminals,
				pSMebr->GetSetMemberLexeme()) == TRUE
				)
			{
				Orphans++;
				fprintf(pOut, "ERROR:Line %d:%s is an Orphan\n",
					pSMebr->GetSetMemberSymbol()->GetLineWhereDefined(),
					pSMebr->GetSetMemberLexeme()->GetName());
			}
		}
		pSMebr = pSMebr->GetNext();
	}
	return Orphans;
}

BOOL CSymTab::CheckOrphans_SearchRules(
	FILE* pOut, 
	CSet* pSetNonTerminals, 
	CLexeme* pLexemeCanidate		//Canidate
)
{
	//---------------------------------------------
	// CheckOrphans_SearchRules
	// 
	// Returns FALSE if the Canidate is an Orphan
	// Returns TRUE if it is not an ORPHAN
	// Seems Backwards ;)
	//---------------------------------------------
	CSetMember* pNonTerminalSetMember;
	CLexeme* pRuleLexeme;
	CRule* pRule;
	BOOL bIsAnOrphan = TRUE;

	pNonTerminalSetMember = pSetNonTerminals->GetHead();
	while (pNonTerminalSetMember && bIsAnOrphan)
	{
		if (pLexemeCanidate->GetLexemeSymbol() == 
			pNonTerminalSetMember->GetSetMemberSymbol()
		)
		{
			//---------------------------------
			// Skip This one
			//---------------------------------
			pNonTerminalSetMember = pNonTerminalSetMember->GetNext();
		}
		else
		{
			//----------------------------------
			// Check to see if the Cannidate
			// is on the RHS of the production
			//----------------------------------
			pRule = pNonTerminalSetMember->GetSetMemberSymbol()->GetHead();
			while (pRule && bIsAnOrphan)
			{
				pRuleLexeme = pRule->GetHead();
	//			pRule->Print(pOut, TRUE, TRUE,  0);
				while (pRuleLexeme && bIsAnOrphan)
				{
					if (pRuleLexeme->GetLexemeSymbol() == pLexemeCanidate->GetLexemeSymbol())
					{
						bIsAnOrphan = FALSE;
					}
					pRuleLexeme = pRuleLexeme->GetNext();
				}
				pRule = pRule->GetNext();
			}
		}
		pNonTerminalSetMember = pNonTerminalSetMember->GetNext();
	}
	return bIsAnOrphan;
}


//*********************************************************
// Hash
//
// parameter
//	name.....pointer to name to Hash
//
//**********************************************************

int CSymTab::Hash(const char* name)
{
	unsigned HashVal = 0;
	unsigned TopBits;
	unsigned char* n;

	n = (unsigned char*)name;
	while (*n)
	{
		HashVal = (HashVal << 12) + *n;
		if ((TopBits = HashVal & 0xff000000) != 0)
			HashVal = ((TopBits >> 24) ^ HashVal) & 0x00ffffff;
		++n;	//next char please...
	}
	return HashVal % m_tSize;	//returns index in table
}


//***************************************************
// PrinTable
//
// paramter:
//	pOut.....output file stream
//
//*****************************************************

BOOL CSymTab::FindOrphanTerminalSymbols()
{
	int i = 0;
	CSymbol* pSym = 0;
	FILE* pOut = LogFile();
	int Orphans = 0;

	if (pOut)
	{
		fprintf(pOut, "***************** Check for Orphan Terminal Symbols **************************\n");
		for (i = 0; i < GetTableSize(); ++i)
		{
			if (GetBucket(i))
			{
				CBin* pBin = GetBucket(i)->GetHead();
				pSym = (CSymbol*)pBin;
				while (pBin)
				{
					if (pSym->GetTokenValue() == CToken::LLRD_Token::TERMINAL)
					{
						if (pSym->GetTerminalSymbolUsedCount() == 0)
						{
							fprintf(pOut, "WARNING:Line %d:%s is an Orphan Terminal Symbol\n",
								pSym->GetLineWhereDefined(),
								pSym->GetName()
							);
							Orphans++;
						}
					}
					pBin = pBin->GetNext();
					pSym = (CSymbol*)pBin;
				}
			}
		}
		fprintf(pOut, "***************** End of Check for Orphan Terminal Symbols = %d *******************\n", Orphans);
	}
	return TRUE;
}

void CSymTab::PrintTable(FILE* pOut)
{
	char* s = new char[512];
	int i;
	int maxStringLen = 0;

	if(pOut) fprintf(pOut, "**************** SymbolTable *****************\n");
	for (i = 0; i < m_tSize; ++i)
	{
		if (m_ppTab[i])
		{
			if (pOut) fprintf(pOut, "------ InDEX %d ------\n", i);
			m_ppTab[i]->Print(pOut);
		}
	}
	if(pOut) fprintf(pOut, "^^^^^^^^^^^^^^^ SymbolTable ^^^^^^^^^^^^^^^^^^^^^^\n");
}

void CSymTab::PrintFirstSets(FILE* pOut)
{
	CSetMember* pSetMemberNonTerminals;

	if (pOut)
	{
		pSetMemberNonTerminals = GetNonTerminalSet()->GetHead();
		while (pSetMemberNonTerminals)
		{
			pSetMemberNonTerminals->GetSetMemberSymbol()->GetFirstSet()->Print(pOut);
			pSetMemberNonTerminals = pSetMemberNonTerminals->GetNext();
		}
	}
}

void CSymTab::PrintFollowSets(
	FILE* pOut, 
	BOOL bLHS, 
	BOOL bEOL, 
	int nIndentSpaces
)
{
	CSetMember* pSetMemberNonTerminals;

	if (pOut)
	{
		pSetMemberNonTerminals = GetNonTerminalSet()->GetHead();
		while (pSetMemberNonTerminals)
		{
			pSetMemberNonTerminals->GetSetMemberSymbol()->GetFollowSet()->Print(pOut);
			pSetMemberNonTerminals = pSetMemberNonTerminals->GetNext();
		}
	}
}
