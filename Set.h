#pragma once

class CSet
{
	CSetMember* m_pHead;
	CSetMember* m_pTail;
	CSet* m_pNext;
	CSet* m_pPrev;
	int m_nNumberOfMembers;
	char m_aName[128];
public:
	CSet();
	virtual ~CSet();
	BOOL Create(const char* pName, const char* pClassName);
	void SetName(const char* pName, const char* pClassName);
	const char* GetName() {
		return m_aName;
	}
	BOOL Copy(CSet* pSet);
	BOOL CopyExceptEmpty(CSet* pSet);
	BOOL Union(CSet* pSetB, CSet& SetC);
	BOOL Union(FILE* pOut, CSet* pSetA);
	BOOL UnionExceptEmpty(CSet* pSetA);
	BOOL Intersection(CSet* pSetB, CSet& SetC);
	BOOL IsMemberOf(CSet* pSetB);
	int GetNumberOfMembers() { return m_nNumberOfMembers; }
    BOOL IsNotAlreadyInSet(CSetMember* pSetMember);
    BOOL IsAlreadyInSet(CSetMember* pSetMembr);
	BOOL Contains(FILE*pOut, CSymbol* pSym);
	BOOL Contains(FILE* pOut, CLexeme* pSym);
	BOOL DoesNotContain(FILE* pOut, CSymbol* pSym);
	//-----------------------------
	// Membership Coordinator
	//-----------------------------
	BOOL AddToSet(CSetMember* pNewObj);
	void RemoveFromSet(CSetMember* pObj);
	void MakeEmpty();
	void SetHead(CSetMember* pSM) { m_pHead = pSM; }
	CSetMember* GetHead() { return m_pHead; }
	void SetTail(CSetMember* pSM) { m_pTail = pSM; }
	CSetMember* GetTail() { return m_pTail; }
	//-----------------------------------------
	// Link Management
	//-----------------------------------------
	void SetNext(CSet* pN) { m_pNext = pN; }
	CSet* GetNext() { return m_pNext; }
	void SetPrev(CSet* pP) { m_pPrev = pP; }
	CSet* GetPrev() { return m_pPrev; }
	//----------------------------------
	// Debug Methods
	//----------------------------------
	void Print(FILE* pOut, 
		BOOL bLHS = FALSE, 
		BOOL bEOL = TRUE, 
		int nIndentSpaces = 0
	);
};