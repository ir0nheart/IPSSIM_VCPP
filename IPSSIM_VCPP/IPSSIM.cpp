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
#include <fstream>
#include <chrono>

#define VIENNACL_HAVE_EIGEN



mutex writeMutex;


using namespace std;
using namespace Eigen;

class Node;

class IPSSIM  {
public:
	IPSSIM(int ID) : myID(ID)
	{
		switch (myID)
		{
		case 1 :
			cout << "LST FileWriter Thread is created and ready.." << endl;
			break;
		case 2:
			cout << "NOD FileWriter Thread is created and ready.." << endl;
			break;
		case 3:
			cout << "ELE FileWriter Thread is created and ready.." << endl;
			break;
		case 4:
			cout << "OBS FileWriter Thread is created and ready.." << endl;
			break;
		case 5:
			cout << "IA FileWriter Thread is created and ready.." << endl;
			break;
		}
	} // 1 = LST , 2 = NOD , 3 ELE , 4 OBS;
	virtual void  run() {
		string str = "";
		ControlParameters * CP = ControlParameters::Instance();
		switch (myID){
		case 1: wr = Writer::LSTInstance();  break;
		case 2: wr = Writer::NODInstance();  break;
		case 3: wr = Writer::ELEInstance();  break;
		case 4: wr = Writer::OBSInstance();  break;
		case 5: wr = Writer::IAInstance(); break;

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
				if (myID == 5)
					CP->writeToIAString(str);

						
		}
		this_thread::sleep_for(chrono::seconds(1));
			goto RECHECK;
	}
private:
	int myID;
	Writer * wr;
};

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
	Timer mainTimer,tempTimer;
	// Use for Advance The Code
	//double vol = 123213;
	//Node N = Node(213);
	//N.setVOLTry(&vol);
	//cout << N.getVolTry() << endl;
	//vol = 1213;
	//cout << N.getVolTry() << endl;

	
	// Create object for storing input and output file names.
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Creating storage object for input files" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	InputFiles* inputFiles = InputFiles::Instance(); // Creates input-output storage object and sets path for current working directory
	Miscellaneous::spacer();

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Printing base directory for input files" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	cout << "\t" << inputFiles->getInputDirectory() << endl;
	Miscellaneous::spacer();


	inputFiles->getFileList();
	Miscellaneous::spacer();
		
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "IPSSIM will check if input files exists." << endl;

	inputFiles->checkInputFiles();

	Miscellaneous::spacer();
	thread lstWriterThread = thread(&IPSSIM::run, new IPSSIM(1));
	thread nodWriterThread = thread(&IPSSIM::run, new IPSSIM(2));
	thread eleWriterThread = thread(&IPSSIM::run, new IPSSIM(3));
	thread obsWriterThread = thread(&IPSSIM::run, new IPSSIM(4));
	thread iaWriterThread = thread(&IPSSIM::run, new IPSSIM(5));

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
	tempTimer.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Mapping INP File" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	inputParser->mapToMemoryINP(inputFiles);
	cout << "\t Mapping input file took " << tempTimer << " seconds" << endl;
	Miscellaneous::spacer();
	tempTimer.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Mapping ICS File" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	inputParser->mapToMemoryICS(inputFiles);
	cout << "\t Mapping ICS file took " << tempTimer << " seconds" << endl;
	Miscellaneous::spacer();
	tempTimer.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Creating DataSet..." << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	controlParameters->createDataSets();
	cout << "\t Creating DataSets took " << tempTimer << " seconds" << endl;
	Miscellaneous::spacer();

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Finding DataSet Positions in mapped File" << endl;
	tempTimer.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	inputParser->findDataSetPositionsInMap();
	cout << "\t Finding DataSet Positions in input file took " << tempTimer << " seconds" << endl;
	Miscellaneous::spacer();


	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Parsing input file into DataSets and setting control parameters" << endl;
	tempTimer.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	controlParameters->setParameters();
	cout << "\t Parsing datasets and setting control parameters took " << tempTimer << " seconds" << endl;
	Miscellaneous::spacer();



	
	// Find the Element each Observation point is in.
	// In components of Observation Points, overwrite node numbers and global coordinates
	// with element numbers and local coordinates
	tempTimer.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Finding Observation Points in Elements" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	controlParameters->findObservationElementThread();

	cout << "\t Finding Observation Points " << tempTimer << " seconds" << endl;
	Miscellaneous::spacer();

	if (UnmapViewOfFile(inputParser->mapViewOfINPFile))
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_RED);
	cout << "Unmapped Memory view of File " << endl;

	// Release DataSets
	controlParameters->freeDataSets();
	Miscellaneous::spacer();

	// IF ITERATIVE SOLVER IS USED, SET UP POINTER ARRAYS IA AND JA THAT SPECIFY
	// MATRIX STRUCTURE IN "SLAP COLUMN FORMAT. DIMENSION NELT GETS SET HERE

	if (controlParameters->getKSOLVP() != 0){
		controlParameters->alpayPTR();
	}
	else{
		controlParameters->setNELT(controlParameters->getNN());
		int NDIMIA = 1;
		// Allocate IA here
	}

	controlParameters->BANWID();

	controlParameters->createSolverMatrix();

	// Read BCS Files Here
	if (inputFiles->getBcsDef()){
		inputParser->mapToMemoryBCS(inputFiles);
		controlParameters->loadBCS();
	}

	// Read INITIAL OR RESTART CONDITIONS HERE
/*	CALL INDAT2(PVEC, UVEC, PM1, UM1, UM2, CS1, CS2, CS3, SL, SR, RCIT, SW, SWT,DSWDP, PBC, IPBC, IPBCT, NREG, QIN,
		DPDTITR, GNUP1, GNUU1, UIN, UBC, QUIN,IBCPBC, IBCUBC, IBCSOP, IBCSOU, IIDPBC, IIDUBC, IIDSOP, IIDSOU, CNUB,RELKT, SWB, RELK, RELKB)              */
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Loading Initial Conditions" << endl;
	controlParameters->loadInitialConditions();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	cout << "\t Initial Conditions are loaded." << endl;

	if (UnmapViewOfFile(inputParser->mapViewOfINPFile)){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_RED);
		cout << "Unmapped Memory view of ICS File " << endl;
	}

	tempTimer.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Creating Nodes" << endl;
	controlParameters->createNodes();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	cout << "\t Nodes Created in " << tempTimer << " seconds" << endl;
	Miscellaneous::spacer();

	tempTimer.reset();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << "Creating Elements" << endl;
	controlParameters->createElements();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_GREEN);
	cout << "\t Elements Created in " << tempTimer << " seconds" << endl;
	Miscellaneous::spacer();

	// Determine Maximum Simulation Time and Duration from Time Step Schedule
	if (controlParameters->getISSTRA() == 0){
		
		controlParameters->setTMAX(controlParameters->getTIMESTEPSSchedule()->getSList()[controlParameters->getITMAX()].first);

	}
	else{
		controlParameters->setTMAX(controlParameters->getTSTART());
	}

	controlParameters->setTEMAX();
	controlParameters->setIT(controlParameters->getITRST());

	if (controlParameters->getIT() == 0){
		controlParameters->setDELTLC(controlParameters->getDELT());
	}
	else{
		double TMITM1, TMIT;
		TMITM1 = controlParameters->getTIMESTEPSSchedule()->getSList()[0].first;
		TMIT = controlParameters->getTIMESTEPSSchedule()->getSList()[1].first;
		controlParameters->setDELT( TMITM1 - TMIT);
		controlParameters->setDELTLC(controlParameters->getDELT());
		controlParameters->setTSEC(TMIT);
		
	}
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
	
	controlParameters->setML(1);
	controlParameters->setNOUMAT(0);
	controlParameters->setISSFLO(2);
	controlParameters->setITER(0);
	controlParameters->setDLTPM1(controlParameters->getDELTP());
	controlParameters->setDLTUM1(controlParameters->getDELTU());
	controlParameters->setBDELP1(1.0);
	controlParameters->setBDELP(0);
	controlParameters->setBDELU(0);



	if (controlParameters->getISSTRA() != 0){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_BLUE);
		_snprintf(buff, sizeof(buff), " TIME STEP %8d OF %8d ", controlParameters->getIT(), controlParameters->getITMAX());
		cout << buff << endl;
	}
	else{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_BLUE);
		_snprintf(buff, sizeof(buff), " TIME STEP %8d OF %8d ;    ELAPSED TIME: %+11.4E OF %11.4E [s]", controlParameters->getIT(), controlParameters->getITMAX(), controlParameters->getTSEC(), controlParameters->getTEMAX());
		cout << buff << endl;
	}
	goto BEGINITERATION;

	

BEGINTIMESTEP:
	controlParameters->setIT(controlParameters->getIT() + 1);
	controlParameters->setITREL(controlParameters->getIT() - controlParameters->getITRST());
	controlParameters->setITBCS(controlParameters->getIT());
	controlParameters->setITER(0);
	controlParameters->setML(0);
	controlParameters->setNOUMAT(0);

	// remove followign two lines
	if (controlParameters->getOnceP() && (controlParameters->getITREL() > 2)){
		if ((((controlParameters->getIT() - 1) % controlParameters->getNPCYC()) != 0) && !controlParameters->getBCSFL(controlParameters->getIT() - 1) && !controlParameters->getBCSFL(controlParameters->getIT()) && controlParameters->getITRMAX() == 1){
			controlParameters->setNOUMAT(1);
		}
	}

	if (controlParameters->getIT() == 1 && controlParameters->getISSFLO() == 2)
		goto INCREMENT;

	if (((controlParameters->getIT() % controlParameters->getNPCYC()) != 0) && !controlParameters->getBCSFL(controlParameters->getIT())){
		controlParameters->setML(2);
	}
	if (((controlParameters->getIT() % controlParameters->getNUCYC()) != 0) && !controlParameters->getBCSTR(controlParameters->getIT())){
		controlParameters->setML(1);
	}

INCREMENT:

	controlParameters->setTSECM1(controlParameters->getTSEC());
	controlParameters->setTSEC(controlParameters->getTIMESTEPSSchedule()->getSList()[controlParameters->getIT()].first);
	controlParameters->setTMIN(controlParameters->getTSEC() / 60.0);
	controlParameters->setTHOUR(controlParameters->getTSEC() / 3600.0);
	controlParameters->setTDAY(controlParameters->getTHOUR() / 24.0);
	controlParameters->setTWEEK(controlParameters->getTDAY() / 7.0);
	controlParameters->setTMONTH(controlParameters->getTDAY() / 30.4375);
	controlParameters->setTYEAR(controlParameters->getTDAY() / 365.250);
	controlParameters->setDELTM1(controlParameters->getDELT());
	controlParameters->setDELT(controlParameters->getTSEC() - controlParameters->getTSECM1());
	controlParameters->setRELCHG(abs(controlParameters->getDELT() - controlParameters->getDELTLC()) / controlParameters->getDELTLC());

	if (controlParameters->getRELCHG() > 1e-14){
		controlParameters->setDELTLC(controlParameters->getDELT());
		controlParameters->setNOUMAT(0);
	}

	if (controlParameters->getISSTRA() != 0){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_BLUE);
		_snprintf(buff, sizeof(buff), " TIME STEP %8d OF %8d ", controlParameters->getIT(), controlParameters->getITMAX());
		cout << buff << endl;

	}
	else{
		controlParameters->setTELAPS(controlParameters->getTSEC() - controlParameters->getTSTART());
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_BLUE);
		_snprintf(buff, sizeof(buff), " TIME STEP %8d OF %8d ;  ELAPSED TIME: %11.4E OF %11.4E [s]", controlParameters->getIT(), controlParameters->getITMAX(), controlParameters->getTELAPS(), controlParameters->getTEMAX());
		cout << buff << endl;
	}

	if ((controlParameters->getML() - 1) < 0){
		controlParameters->setDLTPM1(controlParameters->getDELTP());
		controlParameters->setDLTUM1(controlParameters->getDELTU());
		controlParameters->setDELTP(controlParameters->getTSEC() - controlParameters->getTSECP0());
		controlParameters->setDELTU(controlParameters->getTSEC() - controlParameters->getTSECU0());
		controlParameters->setTSECP0(controlParameters->getTSEC());
		controlParameters->setTSECU0(controlParameters->getTSEC());
	}
	else if ((controlParameters->getML() - 1) == 0){
		controlParameters->setDLTPM1(controlParameters->getDELTP());
		controlParameters->setDELTP(controlParameters->getTSEC() - controlParameters->getTSECP0());
		controlParameters->setTSECP0(controlParameters->getTSEC());
	}
	else{
		controlParameters->setDLTUM1(controlParameters->getDELTU());
		controlParameters->setDELTU(controlParameters->getTSEC() - controlParameters->getTSECU0());
		controlParameters->setTSECU0(controlParameters->getTSEC());
	}

	controlParameters->setBDELP((controlParameters->getDELTP() / controlParameters->getDLTPM1())*0.5);
	controlParameters->setBDELU((controlParameters->getDELTU() / controlParameters->getDLTUM1())*0.5);
	controlParameters->setBDELP1(controlParameters->getBDELP() + 1.0);
	controlParameters->setBDELU1(controlParameters->getBDELU() + 1.0);

BEGINITERATION:


	controlParameters->setITER(controlParameters->getITER() + 1);

	if (controlParameters->getITRMAX() != 1){
		cout << "    NON-LINEARITY ITERATION " << controlParameters->getITER() << endl;
	}

	if ((controlParameters->getML() - 1) < 0){

		controlParameters->setITERPARAMS1();

		/// CALL BC

		if (controlParameters->getITER() <= 2){
			controlParameters->setITERPARAMS2();
		}

		if (controlParameters->getITER() > 1)
			goto LEND;

			controlParameters->setITERPARAMS3();
		goto LEND;
	}

		LEND:
	
	Node * node;

	controlParameters->setITERPARAMS();
	//if (controlParameters->getML() == 0){
	//	for (int i = 1; i <= controlParameters->getNN(); i++){
	//		node = controlParameters->getNodeContainer()[i];
	//		node->setPITER(controlParameters->getBDELP1()*node->getPVEC() - controlParameters->getBDELP()*node->getPM1());
	//		node->setUITER(controlParameters->getBDELU1()*node->getUVEC() - controlParameters->getBDELU()*node->getUM1());
	//		node->setRCITM1(node->getRCIT());
	//		node->setRCIT(controlParameters->getRHOW0() + controlParameters->getDRWDU()*(node->getUITER() - controlParameters->getURHOW0()));
	//		node->setPM1(node->getPVEC());
	//	}
	//}

	if (controlParameters->getML() > 0){
		if (controlParameters->getITER() == 1){
			for (int i = 1; i <= controlParameters->getNN(); i++){
				node = controlParameters->getNodeContainer()[i];
				node->setUITER(controlParameters->getBDELU1()*node->getUVEC() - controlParameters->getBDELU()*node->getUM1());
			}
		}
		else{
			for (int i = 1; i <= controlParameters->getNN(); i++){
				node = controlParameters->getNodeContainer()[i];
				node->setUITER(node->getUVEC());
			}

		}

		if (controlParameters->getNOUMAT() == 1)
			goto FHE;
		if (controlParameters->getITER() > 1)
			goto SH;
		for (int i = 1; i <= controlParameters->getNN(); i++){
			node = controlParameters->getNodeContainer()[i];
			node->setDPDTITR((node->getPVEC() - node->getPM1()) / controlParameters->getDELTP());
			node->setQINITR(node->getQIN());
			node->setPITER(node->getPVEC());
			node->setPVEL(node->getPVEC());
			node->setRCITM1(node->getRCIT());
		}

		// CALL BC
		
	FHE:
		for (int i = 1; i <= controlParameters->getNN(); i++){
			node = controlParameters->getNodeContainer()[i];
			node->setUM2(node->getUM1());
			node->setUM1(node->getUVEC());
			node->setCNUBM1(node->getCNUB());
		}

		
	}
SH:

	//C.....INITIALIZE ARRAYS WITH VALUE OF ZERO                               SUTRA........42100
	//	MATDIM = NELT*NCBI                                                   SUTRA........42200
	//	IF(ML - 1) 3000, 3000, 3300                                            SUTRA........42300
	//	3000 CALL ZERO(PMAT, MATDIM, 0.0D0)                                       SUTRA........42400
	//	CALL ZERO(PVEC, NNVEC, 0.0D0)                                        SUTRA........42500                                                            !CNUB initial zero added here.!!!
	//	CALL ZERO(VOL, NN, 0.0D0)                                            SUTRA........42600
	//	IF(ML - 1) 3300, 3400, 3300                                            SUTRA........42700
	//	3300 IF(NOUMAT) 3350, 3350, 3375                                          SUTRA........42800
	//	3350 CALL ZERO(UMAT, MATDIM, 0.0D0)                                       SUTRA........42900
	//	3375 CALL ZERO(UVEC, NNVEC, 0.0D0)                                        SUTRA........43000
	//	ccc      CALL ZERO(CNUB, NN, 0.0D0)                                          !alden 110111 commented out
	//	3400 CONTINUE                                                           SUTRA........43100


	if (controlParameters->getITER() == 1 && controlParameters->getIBCT() != 4)
		controlParameters->BCTIME();

	if (controlParameters->getITER() == 1 && inputFiles->getBcsDef()){
		controlParameters->set_bcs(true);
		controlParameters->BCSTEP();
	}

	if ((controlParameters->getML() != 1) && (controlParameters->getME() == -1) && (controlParameters->getNOUMAT() == 0) && (controlParameters->getADSMOD() != "'NONE'"))
		controlParameters->ADSORB();


	if (controlParameters->getNOUMAT() == 0){
		if (controlParameters->getKTYPE(0) == 3){
			controlParameters->ELEMN3();
		}
		else{
			controlParameters->ELEMN2();
		}
		
	}
	//controlParameters->printToFile(string("beforeNodal.txt"));

	controlParameters->NODAL();
	//controlParameters->printToFile(string("beforeBC.txt"));
	controlParameters->BC();

	controlParameters->solveTimeStep();

	if (controlParameters->getIGOI() > 0 && controlParameters->getITER() == controlParameters->getITRMAX())
		controlParameters->setISTOP(-1);
	if (controlParameters->getIGOI() > 0 && controlParameters->getISTOP() == 0 && controlParameters->getIERR() == 0)
		goto BEGINITERATION;

	if (controlParameters->getIERR() == 0){
		if (controlParameters->getISTORE() != 0 && (controlParameters->getISTOP() != 0 || (controlParameters->getIT() % controlParameters->getISTORE() == 0)))
		{
			cout << " OUTRST" << endl;
		}
		if (controlParameters->getISTOP() == 0)
			goto BEGINTIMESTEP;
	}


	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_YELLOW);
	cout << " Total Passed time " << mainTimer << " seconds" << endl;
	lstWriterThread.join();
	nodWriterThread.join();
	eleWriterThread.join();
	obsWriterThread.join();
	int sleepTime = 45000; // in milli seconds
	cout << "Sleeping now for " << sleepTime/1000 << " seconds" << endl;
	Sleep(sleepTime);
}