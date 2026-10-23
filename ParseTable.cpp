#include "pch.h"

CParseTable::CParseTable()
{
	m_ppNonTerminalSymbols = 0;
	m_ppTerminalSymbols = 0;
	m_Rows = 0;
	m_Cols = 0;
	m_pTerminalSet = 0;
	m_pNonTerminalSet = 0;
}

CParseTable::~CParseTable()
{
}

BOOL CParseTable::Create(CSet* pTerm, CSet* pNon)
{
	BOOL rV = TRUE;
	int TableSize;
	int i, j;
	CSetMember* pSM;
	CParseTableEntry* pPTE;

	SetNonTerminalSet(pNon);
	SetTerminalSet(pTerm);
	//------------- Parse Table ------------------
	m_Rows = GetNonTerminalSet()->GetNumberOfMembers();
	m_Cols = GetTerminalSet()->GetNumberOfMembers();
	TableSize = m_Rows * m_Cols;
	m_matParseTable.Create(m_Rows, m_Cols);
	//-------------- Terminal Table --------------------------
	m_ppTerminalSymbols = new CLexeme * [sizeof(CLexeme *) * GetTerminalSet()->GetNumberOfMembers()];
	pSM = GetTerminalSet()->GetHead();
	for (i = 0; i < GetTerminalSet()->GetNumberOfMembers(); ++i)
	{
		m_ppTerminalSymbols[i] = new CLexeme;
		m_ppTerminalSymbols[i]->Create(pSM->GetSetMemberSymbol());
		pSM = pSM->GetNext();
	}
	//---------------- Non Terminal Table ----------------------
	m_ppNonTerminalSymbols = new CLexeme * [sizeof(CLexeme*) * GetNonTerminalSet()->GetNumberOfMembers()];
	pSM = GetNonTerminalSet()->GetHead();
	for (i = 0; i < GetNonTerminalSet()->GetNumberOfMembers(); ++i)
	{
		m_ppNonTerminalSymbols[i] = new CLexeme;
		m_ppNonTerminalSymbols[i]->Create(pSM->GetSetMemberSymbol());
		pSM = pSM->GetNext();
	}
	//----------------------------------------------
	for (i = 0; i < m_Rows; ++i)
	{
		for (j = 0; j < m_Cols; ++j)
		{
			pPTE = new CParseTableEntry;
			pPTE->Create();
			pPTE->SetColName(
				m_ppTerminalSymbols[j]->GetLexemeSymbol()->GetName()
			);
			pPTE->SetRowName(
				m_ppNonTerminalSymbols[i]->GetLexemeSymbol()->GetName()
			);
			m_matParseTable(i, j) = pPTE;
		}
	}

	//----------------------------------------------
	fprintf(stderr, "Terminals    :%d\n", m_Cols);
	fprintf(stderr, "Non Terminals:%d\n", m_Rows);
	return rV;
}

BOOL CParseTable::FillTable(FILE* pOut)
{
	//-----------------------------------------------
	// FillTable
	// 
	//	For each production Y->y of the grammar :
	//	1.	For each terminal 'a' in FIRST(Y), Add 
	//		Y->y to M(Y, a).
	//	2.	If e is in FIRST(y), then for each 
	//		terminal b in FOLLOW(Y), add Y->y into 
	//		M(Y, b).  If e is in FIRST(y) amd $ is in
	//		FOLLOW(Y).Then add Y->y into M(Y, $) as 
	//		well.	
	//-----------------------------------------------
	BOOL bConflicts = FALSE;
	int i;
	CLexeme* pY;	//current Non Terminal

	for (i = 0; i < m_Rows; ++i)
	{
		//------------------------------------
		// Get Left Hand Side of production
		//------------------------------------
		pY = m_ppNonTerminalSymbols[i];
		if(pOut) fprintf(pOut, "------- Non Terminal: %s --------\n", pY->GetName());
		Rule1(pOut, pY);
		Rule2(pOut, pY);
		if(pOut) fprintf(pOut, "----\n");
	}
	Print(pOut);
	return bConflicts;
}

BOOL CParseTable::Rule1(FILE* pOut, CLexeme* pY)
{
	//---------------------------------------------
	// Rule1
	// 
	//	1.	For each terminal 'a' in FIRST(Y), Add 
	//		Y->y to M(Y, a).
	// 
	// parameters:
	//	pY is the left hand side of the production
	//---------------------------------------------
	BOOL rV = TRUE;
	CRule* pProductionRule;
	CParseTableEntryMember* pEntryMember;
	CSet FIRSTy;
	CSetMember* pSetMember;
	int Row, Col;

	pProductionRule = pY->GetLexemeSymbol()->GetHead();
	FIRSTy.Create("TempFirstY", "FIRST");
	if(pOut) 
		fprintf(pOut, "> Rule 1 <\n");
	while (pProductionRule)
	{
		pProductionRule->Print(pOut,TRUE,FALSE,0);
		pProductionRule->FIRST(NULL, pProductionRule->GetHead(), FIRSTy, TRUE);
		FIRSTy.Print(pOut, FALSE, TRUE, 5);
		pSetMember = FIRSTy.GetHead();
		while (pSetMember)
		{
			if(pOut) fprintf(pOut, "BEGIN %s ******\n", pSetMember->GetSetMemberLexeme()->GetName());
			Col = GetTerminalIndex(pSetMember->GetSetMemberLexeme());
			Row = GetNonTerminalIndex(pProductionRule->GetLHS());
			pEntryMember = new CParseTableEntryMember;
			pEntryMember->Create(pProductionRule);
			if (pOut)
				fprintf(pOut, "  By Rule 1 Add ");
			pProductionRule->Print(pOut, TRUE, FALSE, 0);
			if (pOut) 
				fprintf(pOut, " ==> Table(%s, %s)\n",
				GetRowName(Row),
				GetColName(Col)
			);
			m_matParseTable(Row, Col)->AddMember(pEntryMember);
			if(pOut) fprintf(pOut, "END %s ^^^^^^^\n", pSetMember->GetSetMemberLexeme()->GetName());
			pSetMember = pSetMember->GetNext();
		}
		FIRSTy.MakeEmpty();
		pProductionRule = pProductionRule->GetNext();
	}
    return rV;
}

BOOL CParseTable::Rule2(FILE* pOut, CLexeme* pY)
{
	//-----------------------------------------------
	// Rule2
	// 
	//	For each production Y->y of the grammar :
	//	2.	If e is in FIRST(y), then for each 
	//		terminal b in FOLLOW(Y), add Y->y into 
	//		M(Y, b).  If e is in FIRST(y) amd $ is in
	//		FOLLOW(Y).Then add Y->y into M(Y, $) as 
	//		well.
	// parameters:
	//	pY is the left hand side of the production
	//-----------------------------------------------
	BOOL rV = TRUE;
	CRule* pProductionRule;
	CParseTableEntryMember* pEntryMember;
	CSet FIRSTy;
	CSet* pFollowY;
	CSetMember* pSetMember;
	int Row, Col;

	pY->GetFollowSet()->Print(pOut, FALSE, TRUE, 0);
	m_pTerminalSet->Print(pOut, FALSE, TRUE, 0);
	pProductionRule = pY->GetLexemeSymbol()->GetHead();
	FIRSTy.Create("TempFirstY", "FIRST");
	if(pOut) fprintf(pOut, "############< Rule 2 >############\n");
	while (pProductionRule)
	{
		if(pOut) fprintf(pOut, "--------");
		pProductionRule->Print(pOut, TRUE, FALSE, 0);
		if(pOut) fprintf(pOut, "--------\n");
		pProductionRule->FIRST(
			pOut, 
			pProductionRule->GetHead(), 
			FIRSTy,
			TRUE
		);
		if (FIRSTy.Contains(pOut, CLexer::GetEmpty()))
		{
			pFollowY = pProductionRule->GetLHS()->GetFollowSet();
			pSetMember = pFollowY->GetHead();
			while (pSetMember)
			{
				pSetMember->Print(pOut, ' ', FALSE, TRUE, 5);
				Col = GetTerminalIndex(pSetMember->GetSetMemberLexeme());
				Row = GetNonTerminalIndex(pProductionRule->GetLHS());
				pEntryMember = new CParseTableEntryMember;
				pEntryMember->Create(pProductionRule);
				if (pOut)
					fprintf(pOut, "  By Rule 2a Add ");
				pProductionRule->Print(pOut, TRUE, FALSE, 0);
				if (pOut) 
					fprintf(pOut, " ==> Table(%s, %s)\n",
						GetRowName(Row),
						GetColName(Col)
					);
				m_matParseTable(Row, Col)->AddMember(pEntryMember);
				pSetMember = pSetMember->GetNext();
			}
			if (pFollowY->Contains(pOut, CLexer::GetEndOfTokenStream()))
			{
				Col = GetTerminalIndex(CLexer::GetEndOfTokenStream());
				Row = GetNonTerminalIndex(pProductionRule->GetLHS());
				pEntryMember = new CParseTableEntryMember;
				pEntryMember->Create(pProductionRule);
				if(pOut) fprintf(pOut, "  By Rule 2b Add ");
				pProductionRule->Print(pOut, TRUE, FALSE);
				if(pOut) fprintf(pOut, " ==> Table(%s, %s)\n",
					GetRowName(Row),
					GetColName(Col)
				);
				m_matParseTable(Row, Col)->AddMember(pEntryMember);
			}
		}
		pSetMember = FIRSTy.GetHead();
		FIRSTy.Print(pOut, FALSE, TRUE, 2);
		while (pSetMember)
		{
			pSetMember->Print(pOut, ' ', FALSE, TRUE, 5);
			pSetMember = pSetMember->GetNext();
		}
		pProductionRule = pProductionRule->GetNext();
	}
	return rV;
}

int CParseTable::GetTerminalIndex(CLexeme* pLex)
{
	int Index = -1;
	int i;
	BOOL bLoop;

	for (i = 0, bLoop = TRUE; i < m_Cols && bLoop; ++i)
	{
		if (pLex->Compare(m_ppTerminalSymbols[i]))
		{
			bLoop = FALSE;
			Index = i;
		}
	}
	if (Index < 0)
	{
		fprintf(stderr, "Could not find Terminal %s\n", pLex->GetName());
		exit(-1);
	}
	return Index;
}

int CParseTable::GetTerminalIndex(CSymbol* pSym)
{
	int Index = -1;
	int i;
	BOOL bLoop;

	for (i = 0, bLoop = TRUE; i < m_Cols && bLoop; ++i)
	{
		if (pSym == m_ppTerminalSymbols[i]->GetLexemeSymbol())
		{
			bLoop = FALSE;
			Index = i;
		}
	}
	if (Index < 0)
	{
		fprintf(stderr, "Could not find Terminal %s\n", pSym->GetName());
		exit(-1);
	}
	return Index;
}


int CParseTable::GetNonTerminalIndex(CSymbol* pSym)
{
	int Index = -1;
	int i;
	BOOL bLoop;

	for (i = 0, bLoop = TRUE; i < m_Rows && bLoop; ++i)
	{
		if (pSym->Compare(m_ppNonTerminalSymbols[i]->GetLexemeSymbol()))
		{
			bLoop = FALSE;
			Index = i;
		}
	}
	if (Index < 0)
	{
		fprintf(stderr, "Could not find Non Terminal %s\n", pSym->GetName());
		exit(-1);
	}
	return Index;
}

int CParseTable::GetNumberOfEntries()
{
	int TotalEntries = 0;
	int i, j;
	int ni, nj;
	CParseTableEntry* pEntry;

	ni = GetRows();
	nj = GetCols();
	for (i = 0; i < ni; ++i)
	{
		for (j = 0; j < nj; ++j)
		{
			pEntry = m_matParseTable(i, j);
			if (pEntry->GetNumMembers() > 0)
			{
				++TotalEntries;
			}
		}
	}
	return TotalEntries;
}

int CParseTable::CheckForConflicts(FILE* pOut)
{
	int TotalConflicts = 0;
	int i, j;
	int ni, nj;
	CParseTableEntry* pEntry;

	ni = GetRows();
	nj = GetCols();
	for (i = 0; i < ni; ++i)
	{
		for (j = 0; j < nj; ++j)
		{
			pEntry = m_matParseTable(i, j);
			if (pEntry->GetNumMembers() > 1)
			{
				if(pOut) fprintf(pOut, "%d(\'%s\',\'%s\')\n",
					pEntry->GetNumMembers(),
					m_ppNonTerminalSymbols[i]->GetName(),
					m_ppTerminalSymbols[j]->GetName()
				);
				m_matParseTable(i, j)->Print(pOut, TRUE, TRUE, 10,TRUE, TRUE);
				TotalConflicts += pEntry->GetNumMembers();
			}
		}
	}
	return TotalConflicts;
}

char* CParseTable::GetRowName(int Row)
{
	return m_ppNonTerminalSymbols[Row]->GetLexemeSymbol()->GetName();
}

char* CParseTable::GetColName(int Col)
{
	return 	m_ppTerminalSymbols[Col]->GetLexemeSymbol()->GetName();
	;
}

void CParseTable::Print(FILE* pO)
{
	int i, j;
	BOOL bNumberLines = FALSE;

	if (pO)
		fprintf(pO, "xxxxxxxxxxxx Parse Table xxxxxxxxxxxxxxxxx\n");
	for (i = 0; i < m_Rows; i++)
	{
		for (j = 0; j < m_Cols; j++)
		{
			if (m_matParseTable(i, j)->GetNumMembers())
			{
				if(pO) fprintf(pO, "(\'%s\',\'%s\')\n",
					m_ppNonTerminalSymbols[i]->GetName(),
					m_ppTerminalSymbols[j]->GetName()
				);
				if (pO)
				{
					if (m_matParseTable(i, j)->GetNumMembers() > 1)
						bNumberLines = TRUE;
					m_matParseTable(i, j)->Print(pO, TRUE, TRUE, 5, bNumberLines);
				}
			}
		}
	}
}
