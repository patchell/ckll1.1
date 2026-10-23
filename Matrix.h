#pragma once

///----------------------------------------------------
/// CMatrix
///
/// Implementation of a two
/// dimensional complex number
/// matrix
///-----------------------------------------------------

class CMatrix
{
	int m_Cols;	//number of columns was m_m
	int m_Rows;	//number of rows was m_n
	CParseTableEntry** m_ppProduction;	// pointer to matrix members array
public:
	CMatrix();
	virtual ~CMatrix();
	BOOL Create(int Rows, int Cols);
	//Indexing operations
	CParseTableEntry*& operator()(int i,int j);
	const CParseTableEntry*& operator()(int i,int j) const;

	void Print(FILE *pO,const char *pT);	//print matrix
};
