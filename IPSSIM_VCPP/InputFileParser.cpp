#include "InputFileParser.h"
#include <string>
#include "InputFiles.h"
#include "ControlParameters.h"
#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include <fstream>

#include <istream>
#include <streambuf>
#include <string>
#include <limits>
#include <sstream>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>



struct membuf : std::streambuf
{
	membuf(char* begin, char* end) {
		this->setg(begin, begin, end);
	}
};


using namespace std;
InputFileParser* InputFileParser::m_pInstance = NULL;

InputFileParser* InputFileParser::Instance(){
	if (!m_pInstance)
		m_pInstance = new InputFileParser;
	return m_pInstance;
}

InputFileParser::InputFileParser()
{
	cout << "\t Parser object is created." << endl;
}


InputFileParser::~InputFileParser()
{
}
void InputFileParser::findDataSetPositionsInMap(){
	int size = strlen(mapViewOfINPFile);
	char* str_start;
	char* str_end;
	vector<int> datPosStart;
	vector<int> datPosEnd;
	for (int i = 0; i <= size; ++i) {
		if (mapViewOfINPFile[i] == '#') {
			str_start = mapViewOfINPFile + i;
			str_end = mapViewOfINPFile + i + 6;
			string key(str_start, str_end);
			if ((key == "#Start") || (key == "# Data")){
				datPosStart.push_back(i);
			}
		}

	}
	int line = 1;
	for (int i : datPosStart){
		for (int j = 1; j < 512; j++){
			if (mapViewOfINPFile[i + j] == '\n'){
				datPosEnd.push_back(i + j);
				line++;
				break;
			}
		}
	}
		
	if (datPosStart.size() != datPosEnd.size()){
		cout << "Check input file .. There are Errors" << endl;
	}
	

	// pair data Set values
	vector<pair<int, int>> dataSetLocation;
	vector <pair<int, int>> dataSetOmit;
	for (int i = 0; i < datPosStart.size(); i++){
		pair<int, int> p = { datPosStart[i], datPosEnd[i] };
		dataSetLocation.push_back(p);
	}
	dataSetOmit.push_back(dataSetLocation[2]);
	dataSetOmit.push_back(dataSetLocation[4]);
	dataSetOmit.push_back(dataSetLocation[12]);

	string linex;

	//used for check
	/*cout << "Omitted Lines" << endl;
	for (int i = 0; i < 3; i++){
		cout << "Omit first " << dataSetOmit[i].first << " Omit Second " << dataSetOmit[i].second << endl;
		str_start = mapViewOfINPFile + dataSetOmit[i].first;
		str_end = mapViewOfINPFile + dataSetOmit[i].second;
		linex.assign(str_start, str_end);
		cout << linex << endl;
	}*/

	str_start = mapViewOfINPFile;
	
	int dslSize = dataSetLocation.size();
	int lastChar = strlen(mapViewOfINPFile);

	// Used for check
	/*cout << "These Will also be Omitted " << endl;
	for (int i = 0; i < dslSize; i++){
		cout << "data Set first " << dataSetLocation[i].first << " second " << dataSetLocation[i].second << endl;
			str_start = mapViewOfINPFile + dataSetLocation[i].first;
			str_end = mapViewOfINPFile + dataSetLocation[i].second;
			linex.assign(str_start, str_end);
	}*/

	pair<int, int> p = { 0, dataSetLocation[0].first};
	dataSets.push_back(p);
	bool bOmit;
	for (int i = 0; i < dslSize - 1; i++){
		bOmit = false;
		for (pair<int, int> omit : dataSetOmit){
			if ((dataSetLocation[i].second + 1 == omit.first) || (dataSetLocation[i].first == omit.first)){
				bOmit = true;
				break;
			}
		}
		if (!bOmit){
			if (dataSetLocation[i].second + 1 != dataSetLocation[i + 1].first){
				p = { dataSetLocation[i].second + 1, dataSetLocation[i + 1].first - 1 };
			}
			else{
				p = { dataSetLocation[i].second + 1, dataSetLocation[i + 1].first };
			}
			dataSets.push_back(p);
		}
	}

	p = { dataSetLocation[dslSize - 1].second + 1, size - 1 };
	dataSets.push_back(p);
	//cout << "number of Data Sets " << dataSets.size() <<  endl;
	 //used to check dataSets
	//int ix = 1;
	//for (pair<int, int> p : dataSets){
	//	cout << " Data Set Starts at " << p.first << " ends at " << p.second << endl;
	//	str_start = mapViewOfINPFile + p.first;
	//	str_end = mapViewOfINPFile + p.second;
	//	if (str_start != str_end)
	//	linex.assign(str_start, str_end);
	//	/*cout << linex << endl;
	//	ix++;
	//	if (ix == 10)
	//		break;*/
	//}
	
	/*for (int i = 0; i < datPosStart.size(); i++){
		if (i==2 || i==4 || i ==12){
			continue;
		}
		else{
			dataSetPositions.push_back(datPosStart[i]);
		}
	}
*/
}
void InputFileParser::parseLinesFromMap(vector<DataSet *> dataSetContainer){
	char *map = mapViewOfINPFile;
	char* str_start = map;
	char* str_end;
	int line_it=1;
	int size = strlen(map);
	int numberOfDataSets = dataSetPositions.size();
	int dataSetIterator = 0;
	cout << "There are " << numberOfDataSets +1 << " in input File" << endl; // Plus one for Data Set 1 Titles
	ControlParameters* CP = ControlParameters::Instance();
	string ma_string;
	for (int i = 0; i <= size; i++){
		if (map[i] == '\n'){			
			str_end = map + i;
			str_start = map + i + 1;
			line_it++;
		}
	}
	cout << "Input File has " << line_it << " lines" << endl;

	str_start = map;
	linesOfInput.reserve(line_it);
	for (int i = 0; i <= size; i++){
		if (map[i] == '\n'){
			line_it++;
			str_end = map + i;
			ma_string.assign(str_start, str_end);
			linesOfInput.push_back(ma_string);
			str_start = map + i + 1;
			
		}
	}
	
}

vector<pair<int, int>>InputFileParser::getDataSets(){
	return this->dataSets;
}

void InputFileParser::findLinesStartWithDiez(){

	vector<int> das;

	int size = linesOfInput.size();

	for (int i = 0; i < size;i++){
		if (linesOfInput[i][0] == '#')
			das.push_back(i);
	}
	linesOfInput[das[2]].erase();
	das.erase(das.begin() + 2);
	linesOfInput[das[3]].erase();
	das.erase(das.begin() + 3);
	linesOfInput[das[10]].erase();
	das.erase(das.begin() + 10);
	ControlParameters* CP = ControlParameters::Instance();
	//DataSet_1 
	for (vector<string>::iterator it = linesOfInput.begin(); it != linesOfInput.begin() + das[0]; ++it){
	CP->getDataSetMap()["DataSet_1"]->addData(*it);
	}
	
	cout << " Done DataSet_1 " << endl;
	//Rest
	
	

	int sizeData = CP->getDataSets().size();
	cout << sizeData << endl;
	vector<string> * datData;
	for (int i = 1; i < sizeData - 1; i++){
		datData = (CP->getDataSets())[i]->getData();
		int begin = das[i - 1]+1;
		int end = das[i];
		int numLines = end - begin;
		if (numLines != 0){
			cout << numLines << " lines in dataSet" << endl;
			
			
			datData->reserve(numLines);
			for (int j = begin; j < end; j++)
				datData->push_back(linesOfInput[j]);
			
			cout << "Done for " << CP->getDataSets()[i]->getDataSetName() << endl;
		
		}
		else{
			cout << "skipped for " << CP->getDataSets()[i]->getDataSetName() << endl;;
		}
	}
	
	size = das.size();
	int begin = das[size - 1] + 1;
	int end = linesOfInput.size();
	datData = CP->getDataSetMap()["DataSet_22"]->getData();
	int numLines = end - begin;
	datData->reserve(numLines);
	cout << numLines << " lines in dataSet" << endl;
	for (int x = begin; x < end; x++)
		datData->push_back(linesOfInput[x]);
	cout << "Done for " << CP->getDataSetMap()["DataSet_22"]->getDataSetName() << endl;
	
}


void InputFileParser::mapToMemoryINP(InputFiles* inputFiles){
	string inpFile;
	inpFile.append(inputFiles->getInputDirectory());
	inpFile.append(inputFiles->filesToRead["INP"]);

	int ssize = inpFile.length();
	TCHAR *param = new TCHAR[ssize + 1];
	param[ssize] = 0;
	copy(inpFile.begin(), inpFile.end(), param);
	HANDLE hMapFile;
	HANDLE hFile;
	hFile = CreateFile(param,               // file to open
		GENERIC_READ | GENERIC_WRITE,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
		NULL);                 // no attr. template


	if (hFile == INVALID_HANDLE_VALUE)
	{
		
		_tprintf(TEXT("\t Terminal failure: unable to open file \"%s\" for read.\n"), param);
		return;
	}
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("\t Could not create file mapping object (%d).\n"),
			GetLastError());

	}
	mapViewOfINPFile = (char*)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		0);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	cout << "\t Successfully mapped input file." << endl;
}

void InputFileParser::mapToMemoryICS(InputFiles* inputFiles){
	string inpFile;
	inpFile.append(inputFiles->getInputDirectory());
	inpFile.append(inputFiles->filesToRead["ICS"]);

	int ssize = inpFile.length();
	TCHAR *param = new TCHAR[ssize + 1];
	param[ssize] = 0;
	copy(inpFile.begin(), inpFile.end(), param);
	HANDLE hMapFile;
	HANDLE hFile;
	hFile = CreateFile(param,               // file to open
		GENERIC_READ | GENERIC_WRITE,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
		NULL);                 // no attr. template


	if (hFile == INVALID_HANDLE_VALUE)
	{

		_tprintf(TEXT("\t Terminal failure: unable to open file \"%s\" for read.\n"), param);
		return;
	}
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("\t Could not create file mapping object (%d).\n"),
			GetLastError());

	}
	mapViewOfICSFile = (char*)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		0);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	cout << "\t Successfully mapped initial conditions file." << endl;
}
void InputFileParser::mapToMemoryBCS(InputFiles* inputFiles){
	string inpFile;
	inpFile.append(inputFiles->getInputDirectory());
	inpFile.append(inputFiles->filesToRead["BCS"]);

	int ssize = inpFile.length();
	TCHAR *param = new TCHAR[ssize + 1];
	param[ssize] = 0;
	copy(inpFile.begin(), inpFile.end(), param);
	HANDLE hMapFile;
	HANDLE hFile;
	hFile = CreateFile(param,               // file to open
		GENERIC_READ | GENERIC_WRITE,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
		NULL);                 // no attr. template


	if (hFile == INVALID_HANDLE_VALUE)
	{

		_tprintf(TEXT("\t Terminal failure: unable to open file \"%s\" for read.\n"), param);
		return;
	}
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("\t Could not create file mapping object (%d).\n"),
			GetLastError());

	}
	mapViewOfBCSFile = (char*)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		0);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	cout << "\t Successfully mapped BCS file." << endl;


}