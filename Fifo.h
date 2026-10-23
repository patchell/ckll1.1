#pragma once

class CFifo
{
	CStackItem* m_pHead;
	CStackItem* m_pTail;
	int m_Size;
public:
	CFifo();
	virtual ~CFifo();
	bool Create(int Size);
	void Put(CStackItem* pItem);
	CStackItem* Get();
	bool HasItems() { return m_pHead != nullptr; }
};

