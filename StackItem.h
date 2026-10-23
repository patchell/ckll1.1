#pragma once

class CStackItem
{
	union m_unionData {
		void* ptrData;
		int intData;
	}m_Value;
	CStackItem* m_pNext;
	CStackItem* m_pPrev;
public:
	enum class ValueType {
		INT,
		pSYMBOL,
		pLEXEME,
		pSET,
		pSETITEM,
		pRULE
	};
	struct StackTypeName {
		ValueType VT;
		const char* Name;
	};
	inline static StackTypeName StackTypeLUT[7] = {
		{ValueType::INT,"INT"},
		{ValueType::pSYMBOL,"pSYMBOL"},
		{ValueType::pLEXEME, "pLEXEME"},
		{ValueType::pSET, "pSET"},
		{ValueType::pSETITEM, "pSETITEM"},
		{ValueType::pRULE,"pRULE"},
		{ValueType(-1),NULL}
	};
private:
	ValueType m_Type;
public:
	CStackItem();
	virtual ~CStackItem();
	BOOL Create(ValueType VT, int IV);
	BOOL Create(ValueType VT, void* pV);
	void SetNext(CStackItem* pSI) {	m_pNext = pSI; }
	CStackItem* GetNext() { return m_pNext; }
	CStackItem* GetPrev() { return m_pPrev; }
	void SetPrev(CStackItem* pSI) { m_pPrev = pSI; }
	BOOL IsType(ValueType Type) {
		BOOL rV = FALSE;
		if (Type == m_Type)
			rV = TRUE;
		return rV;
	}
	ValueType GetType() { return m_Type; }
	int GetIntData() { return m_Value.intData; }
	void* GetPointerData() { return m_Value.ptrData; }
	static const char* LookupValueType(ValueType VT);
	static void CheckStackItem(CStackItem* pSI, ValueType Type);
};

