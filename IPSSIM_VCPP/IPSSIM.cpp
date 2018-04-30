#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <thread>
#include <string>
#include "ControlParameters.h"
#include "InputFiles.h"
#include "Miscellaneous.h"
#include "InputFileParser.h"
#include <tchar.h>
#include <stdio.h>  
#include <process.h>
#include "Timer.h"
#include "Node.h"
#include "Runnable.h"
#include "Thread.h"
#include "Writer.h"
#include <mutex>
#include <chrono>



mutex writeMutex;


using namespace std;

class Node;



class IPSSIM  {
public:
	IPSSIM(int ID) : myID(ID){} // 1 = LST , 2 = NOD , 3 ELE , 4 OBS;
	virtual void  run() {
		string str = "";
		ControlParameters * CP = ControlParameters::Instance();
		switch (myID){
		case 1: wr = Writer::LSTInstance();  break;
		case 2: wr = Writer::NODInstance();  break;
		case 3: wr = Writer::ELEInstance();  break;
		case 4: wr = Writer::OBSInstance();  break;

		}

		RECHECK:
		if (wr->writeContainer.size() != 0){
				str = wr->writeContainer[0];
				writeMutex.lock();
				wr->writeContainer.pop_front();
				writeMutex.unlock();

				if (myID == 1)
				CP->writeToLSTString(str);
				if (myID == 2)
				CP->writeToNODString(str);
				if (myID == 3)
					CP->writeToELEString(str);
				if (myID == 4)
					CP->writeToOBSString(str);

						
		}
		this_thread::sleep_for(chrono::seconds(1));
			goto RECHECK;
		
		
		//return reinterpret_cast<void*>(myID);
	}
private:
	int myID;
	Writer * wr;


};
/*
public:
IPSSIM(int ID) : myID(ID) {}
virtual void  run() {
string str = "";
ControlParameters * CP = ControlParameters::Instance();
Writer * wr = Writer::Instance();
RECHECK:
if (wr->writeContainer.size() != 0){
str = wr->writeContainer[0];
writeMutex.lock();
CP->writeToLSTString(str);
wr->writeContainer.pop_front();
writeMutex.unlock();
}

goto RECHECK;


//return reinterpret_cast<void*>(myID);
}
private:
int myID;
void *pfi(string str);


};

*/

// Windows console has a standard length of 80 characters for each line.

// Use to convert bytes to MB
#define DIV 1048576
#define WIDTH 7

enum Colors{
	BLACK = 0,
	NAVY = 1,
	DARK_GREEN = 2,
	DARK_CYAN = 3,
	DARK_RED = 4,
	DARK_PINK = 5,
	DARK_YELLOW = 6,
	LIGHT_GRAY = 7,
	DARK_GRAY = 8,
	BRIGHT_BLUE = 9,
	BRIGHT_GREEN = 10,
	BRIGHT_CYAN = 11,
	BRIGHT_RED = 12,
	BRIGHT_PINK = 13,
	BRIGHT_YELLOW = 14,
	BRIGHT_WHITE = 15
};

void main(){
	Writer * logWriter = Writer::LSTInstance(); // Create Writer Class for Logging LST file
	
	// Create Two Timers : Used in Debugging. Timer t -> for elapsed time in a function, Timer gent -> Timer for total elapsed time
	Timer t,gent;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Creating storage object for input files" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	InputFiles* inputFiles = InputFiles::Instance(); // Input Files Object
	inputFiles->getPath();
	Miscellaneous::spacer();

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Printing base directory for input files" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	cout << "\t" << inputFiles->getInputDirectory() << endl;
	Miscellaneous::spacer();

	inputFiles->getFileList();
	Miscellaneous::spacer();

	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	//cout << "IPSSIM is creating required objects and checking files" << endl;
	//Miscellaneous::spacer();
		
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "IPSSIM will check if input files exists." << endl;

	inputFiles->checkInputFiles();


	thread lstWriterThread = thread(&IPSSIM::run, new IPSSIM(1));
	thread nodWriterThread = thread(&IPSSIM::run, new IPSSIM(2));
	thread eleWriterThread = thread(&IPSSIM::run, new IPSSIM(3));
	thread obsWriterThread = thread(&IPSSIM::run, new IPSSIM(4));

	Miscellaneous::spacer();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Preparing to read from input files." << endl;
	//Create controlParameters Object
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	ControlParameters* controlParameters = ControlParameters::Instance();
	Miscellaneous::spacer();


	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Creating Parser Object" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	InputFileParser* inputParser = InputFileParser::Instance();
	Miscellaneous::spacer();
	t.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Mapping INP File" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	inputParser->mapToMemoryINP(inputFiles);
	cout << "\t Mapping input file took " << t << " seconds" << endl;
	Miscellaneous::spacer();
	t.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Mapping ICS File" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	inputParser->mapToMemoryICS(inputFiles);
	cout << "\t Mapping ICS file took " << t << " seconds" << endl;
	Miscellaneous::spacer();
	t.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Creating DataSet..." << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	controlParameters->createDataSets();
	cout << "\t Creating DataSets took " << t << " seconds" << endl;
	Miscellaneous::spacer();

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Finding DataSet Positions in mapped File" << endl;
	t.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	inputParser->findDataSetPositionsInMap();
	cout << "\t Finding DataSet Positions in input file took " << t << " seconds" << endl;

	Miscellaneous::spacer();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Parsing input file into DataSets and setting control parameters" << endl;
	t.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	
	controlParameters->setParameters();
	cout << "\t Parsing datasets and setting control parameters took " << t << " seconds" << endl;

	Miscellaneous::spacer();

	
	// Find the Element each Observation point is in.
	// In components of Observation Points, overwrite node numbers and global coordinates
	// with element numbers and local coordinates
	t.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Finding Observation Points in Elements" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	controlParameters->findObservationElementThread();

	cout << "\t Finding Observation Points " << t << " seconds" << endl;
	Miscellaneous::spacer();

	

	if (UnmapViewOfFile(inputParser->mapViewOfINPFile))
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_RED);
	cout << "Unmapped Memory view of File " << endl;


	Miscellaneous::spacer();

	// IF ITERATIVE SOLVER IS USED, SET UP POINTER ARRAYS IA AND JA THAT SPECIFY
	// MATRIX STRUCTURE IN "SLAP COLUMN FORMAT. DIMENSION NELT GETS SET HERE

	if (controlParameters->getKSOLVP() != 0){
		
	}
	else{
		controlParameters->setNELT(controlParameters->getNN());
		int NDIMIA = 1;
		// Allocate IA here
		

	}

	controlParameters->BANWID();

	// Read BCS Files Here





	// Read INITIAL OR RESTART CONDITIONS HERE
/*	CALL INDAT2(PVEC, UVEC, PM1, UM1, UM2, CS1, CS2, CS3, SL, SR, RCIT, SW, SWT,DSWDP, PBC, IPBC, IPBCT, NREG, QIN,
		DPDTITR, GNUP1, GNUU1, UIN, UBC, QUIN,IBCPBC, IBCUBC, IBCSOP, IBCSOU, IIDPBC, IIDUBC, IIDSOP, IIDSOU, CNUB,RELKT, SWB, RELK, RELKB)              */
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Loading Initial Conditions" << endl;
	controlParameters->loadInitialConditions();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	cout << "\t Initial Conditions are loaded." << endl;



	t.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Creating Nodes" << endl;
	controlParameters->createNodes();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	cout << "\t Nodes Created in " << t << " seconds" << endl;
	Miscellaneous::spacer();

	t.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Creating Elements" << endl;
	controlParameters->createElements();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	cout << "\t Elements Created in " << t << " seconds" << endl;
	Miscellaneous::spacer();


	/*MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx(&statex);


	_tprintf(TEXT("There is  %*ld percent of memory in use.\n"), WIDTH, statex.dwMemoryLoad);
	_tprintf(TEXT("There are %*I64d total Mbytes of physical memory.\n"), WIDTH, statex.ullTotalPhys / DIV);
	_tprintf(TEXT("There are %*I64d free Mbytes of physical memory.\n"), WIDTH, statex.ullAvailPhys / DIV);
	_tprintf(TEXT("There are %*I64d total Mbytes of paging file.\n"), WIDTH, statex.ullTotalPageFile / DIV);
	_tprintf(TEXT("There are %*I64d free Mbytes of paging file.\n"), WIDTH, statex.ullAvailPageFile / DIV);
	_tprintf(TEXT("There are %*I64d total Mbytes of virtual memory.\n"), WIDTH, statex.ullTotalVirtual / DIV);
	_tprintf(TEXT("There are %*I64d free Mbytes of virtual memory.\n"), WIDTH, statex.ullAvailVirtual / DIV);
	_tprintf(TEXT("There are %*I64d free Mbytes of extended memory.\n"), WIDTH, statex.ullAvailExtendedVirtual / DIV);*/
	
	// Determine Maximum Simulation Time and Duration from Time Step Schedule
	if (controlParameters->getISSTRA() == 0){
		
		controlParameters->setTMAX(controlParameters->getTIMESTEPSSchedule()->getSList()[controlParameters->getITMAX()].first);

	}
	else{
		controlParameters->setTMAX(controlParameters->getTSTART());
	}

	controlParameters->setTEMAX();


	controlParameters->setIBCT();

	// Check if Restart

	//
	controlParameters->setTSECP0(controlParameters->getTSEC());
	controlParameters->setTSECU0(controlParameters->getTSEC());
	controlParameters->setTMIN(controlParameters->getTSEC()/60.0);
	controlParameters->setTHOUR(controlParameters->getTSEC() / 3600.0);
	controlParameters->setTDAY(controlParameters->getTHOUR() / 24.0);
	controlParameters->setTWEEK(controlParameters->getTDAY() / 7.0);
	controlParameters->setTMONTH(controlParameters->getTDAY() / 30.4375);
	controlParameters->setTYEAR(controlParameters->getTDAY() / 365.250);


	if (controlParameters->getISSTRA() != 1){
		if (controlParameters->getKTYPE(0) == 3){
		
			//OUTLST3
			controlParameters->OUTLST3(0, 0, 0, 0, 0, 0.0, 0, 0, 0.0);

		}
		else{
			//OUTLST2
		}

	// IF TRANSIENT FLOW, PRINT TO NODEWISE AND OBSERVATION OUTPUT
	// FILES NOW.  (OTHERWISE, WAIT UNTIL STEADY - STATE FLOW SOLUTION IS COMPUTED.)

		if (controlParameters->getISSFLO() == 0){
		controlParameters->OUTNOD();
		controlParameters->setTIME(controlParameters->getTSEC());
		controlParameters->OUTOBS(); /// ADD CODE FOR HERE
		}
	}

	char buff[512];
	// SET SWITCHES AND PARAMETERS FOR SOLUTION WITH STEADY-STATE FLOW 
	if (controlParameters->getISSFLO() != 1){
		goto BEGINTIMESTEP;
	}
	
	controlParameters->setML(1.0);
	controlParameters->setNOUMAT(0);
	controlParameters->setISSFLO(2);
	controlParameters->setITER(0);
	controlParameters->setDLTPM1(controlParameters->getDELTP());
	controlParameters->setDLTUM1(controlParameters->getDELTU());
	controlParameters->setBDELP1(1.0);
	controlParameters->setBDELP(0);
	controlParameters->setBDELU(0);



	if (controlParameters->getISSTRA() != 0){
		_snprintf(buff, sizeof(buff), " TIME STEP %8d OF %8d ", controlParameters->getIT(), controlParameters->getITMAX());
		cout << buff << endl;
	}
	else{
		_snprintf(buff, sizeof(buff), " TIME STEP %8d OF %8d ;    ELAPSED TIME: %+11.4E OF %11.4E [s]", controlParameters->getIT(), controlParameters->getITMAX(), controlParameters->getTSEC(), controlParameters->getTEMAX());
		cout << buff << endl;
	}
	goto BEGINITERATION;

	

BEGINTIMESTEP:
	controlParameters->setIT(controlParameters->getIT() + 1);



BEGINITERATION:


	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << " Total Passed time " << gent << " seconds" << endl;
	lstWriterThread.join();
	nodWriterThread.join();
	eleWriterThread.join();
	obsWriterThread.join();
	int sleepTime = 45000; // in milli seconds
	cout << "Sleeping now for " << sleepTime/1000 << " seconds" << endl;
	Sleep(sleepTime);
}