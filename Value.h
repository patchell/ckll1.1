#pragma once

class CValue : public CStackItem
{
	int m_IntData;
public:
	CValue();
	virtual ~CValue();
	int GetData() { return m_IntData; }
	void SetData(int V) { m_IntData = V; }
};

