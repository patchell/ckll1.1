#include "pch.h"

CRecDecParGen Parser;	//BARF a global

CRecDecParGen* GetParser()
{
	return &Parser;
}

void CloseAllFiles()
{
	Parser.CloseFiles();
}

FILE* LogFile()
{
	return Parser.GetLogFile();
}

char* IndentString(char *s, int nSpaces, char FillChar)
{
	int i = 0;
	while (nSpaces--)
		s[i++] = FillChar;
	s[i] = 0;
	return s;
}

const char* TrueFalse(BOOL b)
{
	const char* s;
	if (b)
		s = "TRUE";
	else
		s = "FALSE";
	return s;
}
int main(int argc, char *argv[])
{
	FILE* pIN;
	FILE* pLog;
	char aLogFile[256];

	fprintf(stderr, "CkLL1 Ver 1.1.3\n");
	fprintf(stderr, "March  24, 2025\n");
	if (argc == 1)
	{
		fprintf(stderr, "Missing Source File\n");
		exit(-1);
	}
	fopen_s(&pIN,argv[1], "r");
	if (pIN == NULL)
	{
		printf("Cannot Open Input File %s\n", argv[1]);
		exit(1);
	}
	sprintf_s(aLogFile, 256, "%s_Log.txt", argv[1]);
	fopen_s(&pLog, aLogFile, "w");
	if (pLog == NULL)
	{
		fprintf(stderr, "Could not open %s as the Log File\n", aLogFile);
		fclose(pIN);
		exit(1);
	}
	fprintf(stderr, "InFile: %s\nLog File %s\n",
		argv[1],
		aLogFile
	);

	Parser.Create(pIN, pLog);
	Parser.Run();
	printf("Done\n");
	Parser.CloseFiles();
	return 0;
}
