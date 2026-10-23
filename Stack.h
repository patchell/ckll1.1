#pragma once


class CStack
{
public:
	enum class StackPosition{
		STACK_TOP,
		STACK_$$,
		STACK_$$$
	};
private:
	CStackItem* m_pHead;
	int m_nItems;
public:
	CStack();
	virtual ~CStack();
	BOOL Create();
	virtual void Push(CStackItem* pItem);
	virtual CStackItem* Pop();
	virtual CStackItem* Look(StackPosition Depth);
	virtual CStackItem* GetHead() { return m_pHead; }
	virtual void SetHead(CStackItem* pSI) { m_pHead = pSI; }
};

