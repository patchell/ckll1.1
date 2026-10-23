#include "pch.h"

CStack::CStack()
{
	m_pHead = 0;
	m_nItems = 0;
}

CStack::~CStack()
{
	CStackItem* pSI;

	while (m_pHead)
	{
		pSI = Look(StackPosition::STACK_$$);
		delete m_pHead;
		m_pHead = pSI;
	}
}

BOOL CStack::Create()
{
	BOOL rV = TRUE;
	return rV;
}

void CStack::Push(CStackItem* pItem)
{
	++m_nItems;
	pItem->SetNext(GetHead());
	SetHead(pItem);
}

CStackItem* CStack::Pop()
{
	CStackItem* pRV = 0;

	if (GetHead())
	{
		--m_nItems;
		pRV = GetHead();
		SetHead(GetHead()->GetNext());
	}
	else
	{
		CLexer::Error(stderr, "Attempt to Pop Stack with no elements");
		exit(-2);
	}
	return pRV;
}

CStackItem* CStack::Look(CStack::StackPosition Depth)
{
	int i = 0;
	int D = int(Depth);
	CStackItem* pRV = 0;;
	BOOL Loop = TRUE;

	pRV = GetHead();
	if (pRV == 0)
	{
		CLexer::Error(stderr, "Attempt to Read Stack with no elements");
		exit(-2);
	}
	else if (D > m_nItems)
	{
		char* s = new char[256];
		sprintf_s(s, 256, "Stack Depth=%d < Requested Iten=%d", m_nItems, Depth);
		CLexer::Error(stderr, s);
		delete[] s;
		exit(-2);
	}
	else
	{
		while (pRV && Loop)
		{
			if (i++ == D)
			{
				Loop = FALSE;
			}
			else if (pRV == 0)
			{
				Loop = FALSE;
				exit(-2);
			}
			else
			{
				pRV = pRV->GetNext();
			}
		}
	}
	return pRV;
}
