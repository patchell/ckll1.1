#include "pch.h"


CFifo::CFifo()
{
	m_Size = 0;
	m_pHead = nullptr;
	m_pTail = nullptr;
}

CFifo::~CFifo()
{}

bool CFifo::Create(int Size)
{
	return true;
}

void CFifo::Put(CStackItem* pItem)
{
	//-------------------------------------
	// Put the item at the head of the fifo
	//-------------------------------------
	if (m_pHead == nullptr)
	{
		m_pHead = pItem;
		m_pTail = pItem;
		++m_Size;
	}
	else
	{
		pItem->SetNext(m_pHead);
		m_pHead->SetPrev(pItem);
		m_pHead = pItem;
		++m_Size;
	}
}

CStackItem* CFifo::Get()
{
	//-------------------------------------
	// Get the item at the tail of the fifo
	//-------------------------------------
	CStackItem* pRV = nullptr;

	if (m_pTail)
	{
		pRV = m_pTail;
		m_pTail = m_pTail->GetPrev();
		if (m_pTail)
			m_pTail->SetNext(nullptr);
		else
			m_pHead = nullptr;
		m_Size--;
	}
	return pRV;
}
