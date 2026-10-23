#include "pch.h"

void CBin::Print(FILE* pOut,
	BOOL bLHS,
	BOOL bEOL,
	int nIndentSpaces
)
{
	if (pOut)
	{
		fprintf(pOut, "CBin::%s  No Data\n", m_pName);
	}
}

