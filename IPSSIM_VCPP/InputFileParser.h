#ifndef _INPUT_FILE_PARSER
#define _INPUT_FILE_PARSER
#include <process.h>
#include <vector>
#include "DataSet.h"
#include <iterator>
#include <algorithm>
#pragma once
using namespace std;
class InputFiles;
class DataSet;
class ControlParameters;
class InputFileParser
{
public:
	static InputFileParser* Instance();
	void mapToMemoryINP(InputFiles* inputFiles);
	char* mapViewOfINPFile;
	void mapToMemoryICS(InputFiles* inputFiles);
	char* mapViewOfICSFile;
	void mapToMemoryBCS(InputFiles* inputFiles);
	char* mapViewOfBCSFile;
	void parseLinesFromMap(vector<DataSet *> dataSetContainer);
	void findDataSetPositionsInMap();
	void findLinesStartWithDiez();
	vector<pair<int, int>>getDataSets();
private:
	vector<pair<int, int>>dataSets;
	InputFileParser();
	InputFileParser(InputFileParser const&){};
	InputFileParser& operator = (InputFileParser const&){};
	virtual ~InputFileParser();
	static InputFileParser* m_pInstance;
	vector <int> dataSetPositions;
	vector <string> linesOfInput;
};

#endif