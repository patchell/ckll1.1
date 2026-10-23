#ifndef PCH_H
#define PCH_H


#define INDENT(S,I) int _i_;for(_i_=0; _i_ < I;++_i_) S[_i_]=' ';S[_i_]=0


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <Windows.h>

#include "Token.h"
#include "StackItem.h"
#include "Stack.h"
#include "Fifo.h"
#include "Value.h"
#include "Bin.h"
#include "Bucket.h"
#include "Lexeme.h"
#include "Rule.h"
#include "ParseTableEntryMember.h"
#include "ParseTableEntry.h"
#include "Matrix.h"
#include "SetMember.h"
#include "Set.h"
#include "symbol.h"
#include "SymTab.h"
#include "Lexer.h"
#include "ParseTable.h"
#include "parser.h"
#include "RecDecParGen.h"

extern 	CRecDecParGen Parser;
extern FILE* LogFile();
extern const char* TrueFalse(BOOL b);
extern char* IndentString(char* s, int nSpaces, char FillChar = ' ');
extern void CloseAllFiles();
extern CRecDecParGen* GetParser();


#endif // PCH_H
