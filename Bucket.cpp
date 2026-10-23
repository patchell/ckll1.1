#include "pch.h"


CBucket::~CBucket()
{
	CBin* pBin;

	pBin = m_pHead;

	while (pBin)
	{
		m_pHead = pBin->GetNext();
		delete pBin;
		pBin = m_pHead;
	}
}

void CBucket::Add(CBin* pSym)
{
	if (GetTail())
	{
		GetTail()->SetNext(pSym);
		pSym->SetPrev(GetTail());
		SetTail(pSym);
	}
	else
	{
		SetTail(pSym);
		SetHead(pSym);
	}
}

CBin* CBucket::Find(const char* pName)
{
	CBin* pBin = 0;
	BOOL loop = TRUE;

	pBin = GetHead();
	while (pBin && loop)
	{
		if (pBin->Compare(pName))
			loop = FALSE;
		else
			pBin = pBin->GetNext();
	}
	return pBin;
}

void CBucket::Print(FILE* pOut, BOOL bLHS, BOOL bEOL, int nIndentSpaces)
{
	CBin* pBin;

	if (pOut)
	{
		pBin = GetHead();
		while (pBin)
		{
			pBin->Print(pOut);
			pBin = pBin->GetNext();
		}
	}
}

void CBucket::Delete(CBin* pSym)
{
	Unlink(pSym);
	delete pSym;
}

void CBucket::Unlink(CBin* pSym)
{
	if (m_pHead == pSym)
	{
		m_pHead = pSym->GetNext();
		m_pHead->SetPrev(0);
	}
	else if (m_pTail == pSym)
	{
		m_pTail = pSym->GetPrev();
		m_pTail->SetNext(0);
	}
	else
	{
		pSym->GetNext()->SetPrev(pSym->GetPrev());
		pSym->GetPrev()->SetNext(pSym->GetNext());
	}
}

