#include "InputFiles.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <thread>
#include <chrono>
#include <iomanip>
#include "Miscellaneous.h"
#include "Layer.h"
#include "ControlParameters.h"


using namespace std;

enum Colors{BLACK=0,
			NAVY=1,
			DARK_GREEN=2,
			DARK_CYAN=3,
			DARK_RED=4,
			DARK_PINK=5,
			DARK_YELLOW=6,
			LIGHT_GRAY=7,
			DARK_GRAY=8,
			BRIGHT_BLUE=9,
			BRIGHT_GREEN=10,
			BRIGHT_CYAN=11,
			BRIGHT_RED=12,
			BRIGHT_PINK=13,
			BRIGHT_YELLOW=14,
			BRIGHT_WHITE=15};

InputFiles* InputFiles::m_pInstance = NULL;


vector<char> convStringToCharVector(const std::string &s)
{
	return std::vector<char>(s.begin(), s.end());
}

InputFiles* InputFiles::Instance(){
	if (!m_pInstance)
		m_pInstance = new InputFiles;
	return m_pInstance;
}

InputFiles::InputFiles()
{
	cout << " \t Storage object for required input files is created." << endl;
}


InputFiles::~InputFiles()
{
}

void InputFiles::exitOnError(){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_WHITE);
	cout << "Terminating program in 10 seconds" << endl;
	for (int i = 10; i >0; i--){
		cout << i << " seconds to terminate the program." << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
	cout << "Byee..." << endl;
 	exit(1);
}

void InputFiles::getPath()
{
	char result[MAX_PATH];
	GetModuleFileNameA(0, result, MAX_PATH);
	string strPath(result);
	strPath = strPath.substr(0, strPath.find_last_of("\\"));
	strPath.append("\\");
	this->inputDir = strPath;
	
}

string InputFiles::getInputDirectory(){
	return this->inputDir;
}

void InputFiles::getFileList()
{	
	string mline;
	const char * del = " "; // CSV file delimiter
	
	string fileList;
	fileList.append(this->inputDir);
	fileList.append("SUTRA.FIL");
	ifstream simulationFileList;


	simulationFileList.open(fileList.c_str());
	if (simulationFileList.fail()){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_RED);
		cerr << "Error: " << strerror(errno) << "\n" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
		cout << "Please make sure SUTRA.FIL is in your work directory \n" << endl;
		exitOnError();
		
	}
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Printing list of files defined in SUTRA.FIL file" << endl;

	int itLine = 0;

	while (getline(simulationFileList, mline)){  // This argument gets a line from file and saves in mline
		// Example Line of SUTRA.FIL
		// FILETYPE MODULENO(FORTRAN REQUIRES) 'FILENAME'
		// INP    50 'GT_SUM_GT_LW.inp'

		//convert mline String to char vector and push terminating character
		vector<char> lineBuffer(mline.begin(), mline.end());
		lineBuffer.push_back('\0');



		string filetype, filename; // two strings for storing required information filetype and filename, in c++ we don't need module number
		filetype = string(strtok(lineBuffer.data(), del));  // parse the line and save filetype 
		stoi(strtok(NULL, del)); // eliminate module number from line
		filename = string(strtok(NULL, del)); // readfilename and save it. note that filename given like 'filename'


		string delimiter = "'"; // for parsing filename we use a different delimiter
		size_t pos = 0; 
		string actFileName;
		while ((pos = filename.find(delimiter)) != std::string::npos) {
			actFileName = filename.substr(0, pos);
			filename.erase(0, pos + delimiter.length());
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_PINK);
		cout << "\t" <<filetype;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_WHITE);
		cout << "      file is :   ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_CYAN);
		cout << actFileName << endl; // print to console filetype and actual file name.
		inputFileList[filetype] = actFileName;
		itLine += 1;
	}
	string bcsfilename = inputFileList["BCS"];
	string inpfilename = inputFileList["INP"];
	string icsfilename = inputFileList["ICS"];
	if (inpfilename.empty()){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_RED);
		cout << "ERROR!!" << endl;
		cout << "INP input file for problem is not defined in SUTRA.FIL file." << endl;
		exitOnError();
	}
	filesToRead["INP"]=inpfilename;
	if (icsfilename.empty()){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_RED);
		cout << "ERROR!!" << endl;
		cout << "ICS input file for problem is not defined in SUTRA.FIL file." << endl;
		exitOnError();
	}
	filesToRead["ICS"] = icsfilename;

	if (!bcsfilename.empty()){
		filesToRead["BCS"] = bcsfilename;
		

	}
	else{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_RED);
		cout << "WARNING !!" << endl;
		cout << "BCS input file for Boundary Conditions is not defined in SUTRA.FIL file." << endl;
	}
	
	filesToWrite["LST"] = inputFileList["LST"];
	filesToWrite["RST"] = inputFileList["RST"];
	filesToWrite["NOD"] = inputFileList["NOD"];
	filesToWrite["ELE"] = inputFileList["ELE"];
	filesToWrite["OBS"] = inputFileList["OBS"];
	filesToWrite["SMY"] = inputFileList["SMY"];

	Miscellaneous::spacer();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "List of files to read as follows " << endl;
	
	int i = 1;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	unordered_map<string, string>::iterator it = filesToRead.begin();
	while (it != filesToRead.end())
	{
		cout <<"\t"<< i << " -- " << it->second << endl;
		it++;
		i++;
	}
	
	Miscellaneous::spacer();
	i = 1;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "List of files to write as follows " << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	it = filesToWrite.begin();
	while (it != filesToWrite.end())
	{
		cout << "\t" << i << " -- " << it->second << endl;
		it++;
		i++;
	}
	


}

void InputFiles::printInputFilesToLST(){
	string logLine = "";
	Writer * logWriter = Writer::LSTInstance();
	logLine.append(string(11, ' ') + "F I L E   U N I T   A S S I G N M E N T S\n\n");
	logLine.append(string(13, ' ') + "INPUT UNITS : \n");
	logLine.append(string(14, ' ') + "INP FILE (MAIN INPUT)              ASSIGNED TO ");
	logLine.append(filesToRead["INP"]);
	logLine.append("\n");
	logLine.append(string(14, ' ') + "ICS FILE (INITIAL CONDITIONS)      ASSIGNED TO ");
	logLine.append(filesToRead["ICS"]);
	logLine.append("\n");
	if (filesToRead.size() > 2){
		logLine.append(string(14, ' ') + "BCS FILE (TIME-VAR. BND. COND.)    ASSIGNED TO ");
		logLine.append(filesToRead["BCS"]);
		logLine.append("\n");
	}
	logLine.append("\n");
	logLine.append(string(13, ' ') + "OUTPUT UNITS:\n");
	logLine.append(string(14, ' ') + "SMY FILE (RUN SUMMARY)              ASSIGNED TO ");
	logLine.append(filesToWrite["SMY"]);
	logLine.append("\n");


	logLine.append(string(14, ' ') + "LST FILE (GENERAL OUTPUT)           ASSIGNED TO ");
	logLine.append(filesToWrite["LST"]);
	logLine.append("\n");



	if (filesToWrite["RST"] != ""){
		logLine.append(string(14, ' ') + "RST FILE (RESTART DATA)             ASSIGNED TO ");
		logLine.append(filesToWrite["LST"]);
		logLine.append("\n");
	}
	if (filesToWrite["NOD"] != ""){
		logLine.append(string(14, ' ') + "NOD FILE (NODEWISE OUTPUT)          ASSIGNED TO ");
		logLine.append(filesToWrite["NOD"]);
		logLine.append("\n");
	}
	if (filesToWrite["ELE"] != ""){
		logLine.append(string(14, ' ') + "ELE FILE (VELOCITY OUTPUT)          ASSIGNED TO ");
		logLine.append(filesToWrite["ELE"]);
		logLine.append("\n");

	}

	if (filesToWrite["OBS"] != ""){
		logLine.append(string(14, ' ') + "OBS FILE (OBSERVATION OUTPUT)       ASSIGNED TO ");
		logLine.append(filesToWrite["OBS"]);
		logLine.append("\n");
	}

	if (filesToWrite["OBC"] != ""){
		logLine.append(string(14, ' ') + "OBC FILE (OBSERVATION OUTPUT)       ASSIGNED TO ");
		logLine.append(filesToWrite["OBC"]);
		logLine.append("\n");
	}

	if (filesToWrite["BCOF"] != ""){
		logLine.append(string(14, ' ') + "BCOF FILE (BND. COND. OUTPUT)       ASSIGNED TO ");
		logLine.append(filesToWrite["BCOF"]);
		logLine.append("\n");
	}
	if (filesToWrite["BCOS"] != ""){
		logLine.append(string(14, ' ') + "BCOS FILE (BND. COND. OUTPUT)       ASSIGNED TO ");
		logLine.append(filesToWrite["BCOS"]);
		logLine.append("\n");
	}
	if (filesToWrite["BCOP"] != ""){
		logLine.append(string(14, ' ') + "BCOP FILE (BND. COND. OUTPUT)       ASSIGNED TO ");
		logLine.append(filesToWrite["BCOP"]);
		logLine.append("\n");
	}
	if (filesToWrite["BCOU"] != ""){
		logLine.append(string(14, ' ') + "BCOU FILE (BND. COND. OUTPUT)       ASSIGNED TO ");
		logLine.append(filesToWrite["BCOU"]);
		logLine.append("\n");
	}

	logLine.append("\n              NAMES FOR OBS AND OBC FILES WILL BE GENERATED AUTOMATICALLY FROM THE BASE NAMES LISTED ABOVE AND SCHEDULE NAMES");
	logLine.append("\n              LISTED LATER IN THIS FILE.");
	logWriter->writeContainer.push_back(logLine);
}
void InputFiles::checkInputFiles(){
	unordered_map<string, string>::iterator it = filesToRead.begin();
	while (it != filesToRead.end()){
	string dale;
	dale.append(this->inputDir);
	dale.append(it->second);
	ifstream fin(dale.c_str());
	
	if (fin.fail())
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_RED);
		cout << "Failed: " << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_WHITE);
		cout << dale  << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_CYAN);
		cout << setw(80) << right << " file is not in the current working directory." << endl;
		exitOnError();
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	cout << "Success :" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_WHITE);
	cout << dale << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << setw(80) << right << " file is in the current working directory." << endl;
	it++;
	}
}

void InputFiles::readPropsINP(){
	string mline;
	const char * del = " "; // CSV file delimiter
	ControlParameters* cp=ControlParameters::Instance();

	string propsFile;
	propsFile.append(this->inputDir);
	propsFile.append("props.inp");
	ifstream propsFileStream;


	propsFileStream.open(propsFile.c_str());
	if (propsFileStream.fail()){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_RED);
		cerr << "Error: " << strerror(errno) << "\n" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
		cout << "Please make sure 'props.inp' is in your work directory \n" << endl;
		exitOnError();

	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_BLUE);
	cout << "Printing defined properties" << endl;
	// ignore title line
	getline(propsFileStream, mline);
	// read line where PSTAR, GCONST, TEMP , SMWH and TimeStep Divide defined
	getline(propsFileStream, mline);
	vector<char> lineBuffer(mline.begin(), mline.end());
	lineBuffer.push_back('\0'); // terminating character
	cp->setPSTAR(stod(strtok(lineBuffer.data(), del)));
	cp->setGCONST(stod(strtok(NULL, del)));
	cp->setTEMP(stod(strtok(NULL, del)));
	cp->setSMWH(stod(strtok(NULL, del)));
	cp->setTimeStepDivide(stoi(strtok(NULL, del)));
	cp->setWaterTable(stod(strtok(NULL, del)));
 
	//remove number of layers tab
	getline(propsFileStream, mline);

	lineBuffer.clear();
	getline(propsFileStream, mline);
	lineBuffer = convStringToCharVector(mline);
	lineBuffer.push_back('\0');

	int nnLayers = stoi(strtok(lineBuffer.data(), del));
	cp->setNumberOfLayers(nnLayers);
	
	//remove Layer definition header line
	getline(propsFileStream, mline);

	for (int i = 0; i < nnLayers; i++) {
		lineBuffer.clear();
		getline(propsFileStream, mline);
		lineBuffer = convStringToCharVector(mline);
		lineBuffer.push_back('\0');
		int layNo = stoi(strtok(lineBuffer.data(), del));
		double unitWeight = stod(strtok(NULL, del));
		double topLayer = stod(strtok(NULL, del));
		double bottomLayer = stod(strtok(NULL, del));
		int satCond = 0;

		if (abs(bottomLayer) >= abs(cp->getWaterTable())){
			satCond = -1;
		}
		if (abs(cp->getWaterTable()) <= abs(topLayer)){
			satCond = 1;
		}
		if ((abs(cp->getWaterTable()) > abs(topLayer)) && (abs(cp->getWaterTable()) < abs(bottomLayer))){
			satCond = 0;
		}

		Layer* lay = new Layer(layNo, unitWeight, topLayer, bottomLayer,satCond);
		
		cp->addLayer(lay);

	}
	

}