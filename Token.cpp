#include "pch.h"

const char* CToken::LookupTokenName(CToken::LLRD_Token Token)
{
	int i;
	BOOL Loop = TRUE;
	const char* pRV = 0;

	for (i = 0; Loop && (i < TOKEN_NUMBER_OF); i++)
	{
		if (Token == TokenNameLUT[i].m_Token)
		{
			pRV = TokenNameLUT[i].m_pName;
			Loop = FALSE;
		}
	}
	return pRV;
}
