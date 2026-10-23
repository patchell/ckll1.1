#include "pch.h"

CMatrix::CMatrix()
{
	//------------------------------------------------------------
	// Constructor
	//------------------------------------------------------------
    m_Cols = 0;		///Columns
	m_Rows = 0;			///Rows
	m_ppProduction = 0;	///create data array
}

CMatrix::~CMatrix()
{
	//-------------------------------------------------------
	// Destructor
	//-------------------------------------------------------------
}

BOOL CMatrix::Create(int Rows, int Cols)
{
	BOOL rV = TRUE;
	int i;

	m_Cols = Cols;
	m_Rows = Rows;

	m_ppProduction = new CParseTableEntry * [Rows * Cols];
	for (i = 0; i < Rows * Cols; ++i)
		m_ppProduction[i] = 0;
	return rV;
}

void CMatrix::Print(FILE *pO,const char *pTitle)	//print matrix
{
    int i,j;

	if (pO)
	{
		fprintf(pO, "--------- %s ---------\n", pTitle);
		fprintf(pO, "Rows=%d  Col=%d\n", m_Cols, m_Rows);
		for (i = 0; i < m_Rows; ++i)	///rows
		{
			for (j = 0; j < m_Cols; ++j)	///columns
				(*this)(i, j)->Print(LogFile());
			fprintf(pO, "\n");
		}
		fprintf(pO, "-------------------------------\n");
	}
}

CParseTableEntry*& CMatrix::operator()(int i,int j)
{
	//------------------------------------------------------
	// Index Operator
	//	This is the indexing operator.  This is
	// what makes this class so cool.  The
	// only bad thing is that we can't use the
	// [] operator.  Still, we get very readable
	// code and none of the problems
	// associated with using A[][] that is built
	// into C++.
	//
	//	parameters:
	//		i...........row
	//		j...........column
    //		returns:
    //			the (i,j) th element of the matrix
	//------------------------------------------------------
	//	m_col.............Number of Columns
	// m_row..............Number of Rows
	//
	// Calculate the index.
	// Index = Column + Row * Nmber of columns
	//------------------------------------------------------
	int maxindex = m_Rows * m_Cols;
	//-------------------------------------------------------
	// To compute the index, we add the
	// column number (j) to the product of
	// row number (i) time the number of
	// columns, which is also the same as
	// the number of elements per row
	//------------------------------------------------------
	int index = j + i* m_Cols;
	if(index >= maxindex) 
		fprintf(stderr,"ERROR:Matrix Index i=%d  j=%d m=%d  n=%d  max=%d Inx=%d\n",
			i,
			j,
			m_Cols, 
			m_Rows,
			maxindex,
			index
		);
	return m_ppProduction[index];
}

const CParseTableEntry*& CMatrix::operator()(int i,int j) const
{
	//------------------------------------------------------------
	// Operator()
    //		parameters:
    //			i........row
    //			j........column
    //		returns:
    //			the (i,j) th element of the matrix
    //---------------------------------------------------------------
	int maxindex = m_Rows * m_Cols;
	int index = j + i* m_Cols;
	if(index >= maxindex) 
		fprintf(stderr,"ERROR:Matrix Index i=%d  j=%d m=%d  n=%d max=%d Index=%d\n",
			i,
			j, 
			m_Cols,
			m_Rows,
			maxindex,index
		);
	return (const CParseTableEntry*&)m_ppProduction[index];
}


