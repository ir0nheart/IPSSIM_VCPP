#include "ControlParameters.h"
#include <iostream>
#include <fstream>
#include <mutex>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>  


using namespace std;
mutex mtx;
int foundObs = 0;


void increment_count()
{
	mtx.lock();
	foundObs = foundObs + 1;
	mtx.unlock();
}

void print_obs_thread_info(int tid, int step, int rem){
	lock_guard<mutex>guard(mtx);
	cout << "Thread " << tid + 1 << " has " << step - 1 << " steps with a buff size of 1000 and additional step with size " << rem << endl;
}
void print_start_end(int start,int end,int tid){
	mtx.lock();
	cout << " Thread : " << tid << " will check elements from " << start << " to " << end << endl;
	mtx.unlock();
}
int get_count()
{
	int c;
	mtx.lock();
	c = foundObs;
	mtx.unlock();
	return (c);
}

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
const string ControlParameters::SOLWRD[] = { "'DIRECT'", "'CG'", "'GMRES'", "'ORTHOMIN'" };
const string ControlParameters::SOLNAM[] = { "BANDED GAUSSIAN ELIMINATION (DIRECT)",
"IC-PRECONDITIONED CONJUGATE GRADIENT",
"ILU-PRECONDITIONED GMRES",
"ILU-PRECONDITIONED ORTHOMIN" };
 
ControlParameters* ControlParameters::m_cInstance = NULL;

unordered_map<string, DataSet *> ControlParameters::getDataSetMap(){
	return this->dataSetMap;
}

void ControlParameters::createDataSets(){
	char * dataSets[] = { "DataSet_1","DataSet_2A","DataSet_2B","DataSet_3","DataSet_4","DataSet_5","DataSet_6",
		"DataSet_7A","DataSet_7B","DataSet_7C","DataSet_8ABC","DataSet_8D","DataSet_8E_9_10_11","DataSet_12_13_14A",
		"DataSet_14B","DataSet_15A","DataSet_15B","DataSet_17","DataSet_18","DataSet_19","DataSet_20","DataSet_22"};
	for (int i = 0; i < 22; i++){
		DataSet* dataSet = new DataSet(dataSets[i]);
		dataSetMap[dataSets[i]] = dataSet;
		dataSetContainer.push_back(dataSet);
	}
}



bool is_file_exist(string fileName)
{
	ifstream infile(fileName);
	return infile.good();
}

void ControlParameters::setPSTAR(double PSTAR){
	this->PSTAR = PSTAR;
}

double ControlParameters::getPSTAR(){
	return this->PSTAR;
}


void ControlParameters::setGCONST(double GCONST){
	this->GCONST = GCONST;
}

double ControlParameters::getGCONST(){
	return this->GCONST;
}

void ControlParameters::setTEMP(double TEMP){
	this->TEMP = TEMP;
}

double ControlParameters::getTEMP(){
	return this->TEMP;
}

void ControlParameters::setSMWH(double SMWH){
	this->SMWH = SMWH;
}

double ControlParameters::getSMWH(){
	return this->SMWH;
}


void ControlParameters::setTimeStepDivide(int TimeStepDivide){
	this->timeStepDivide = TimeStepDivide;
}

int ControlParameters::getTimeStepDivide(){
	return this->timeStepDivide;
}

void ControlParameters::setNumberOfLayers(int nnLayers){
	this->numberOfLayers = nnLayers;
}

int ControlParameters::getNumberOfLayers(){
	return this->numberOfLayers;
}

ControlParameters* ControlParameters::Instance(){
	if (!m_cInstance)
		m_cInstance = new ControlParameters;
	return m_cInstance;
}

void ControlParameters::addToListOfSchedules(Schedule * sch){
	this->listOfSchedules.push_back(sch);
}
void ControlParameters::addToListOfOBS(ObservationPoints * obs){
	this->observationPointsContainer.push_back(obs);
}
void ControlParameters::addToNodeContainer(Node * aNode){
	int nodeNumber = aNode->getNodeNumber();
	this->nodeContainer.push_back(aNode);
}
void ControlParameters::addToElementContainer(Element * anElement){
	this->elementContainer.push_back(anElement);
}
vector<Schedule *> ControlParameters::getListOfSchedules(){
	return this->listOfSchedules;
}

ControlParameters::ControlParameters()
{
	isNewRun = true;

}


ControlParameters::~ControlParameters()
{
}


void ControlParameters::setTITLE1(string str){
	TITLE1 = str;
	
}

void ControlParameters::setTITLE2(string str){
	TITLE2 = str;
}

void ControlParameters::addLayer(Layer* lay){
	layers.push_back(lay);
}
//void ControlParameters::addDataSet(string dataSet,string dataSetName){
//	DataSet *dataSetx = new DataSet(dataSetName);
//	dataSetx->setRawData(dataSet);
//	dataSetMap[dataSetName] = dataSetx;
//	dataSetContainer.push_back(dataSetx);
//	cout << "DataSet " << dataSetx->getDataSetName() << " added to container" << endl;
//}

vector<DataSet*> ControlParameters::getDataSets(){
	return dataSetContainer;

}
//void ControlParameters::parseDataSet_8D(){}
//void ControlParameters::parseDataSet_14B(){}
//void ControlParameters::parseDataSet_15B(){}
//void ControlParameters::parseDataSet_19(){}
//void ControlParameters::parseDataSet_22(){}

void ControlParameters::setParameters(){
	Timer t;
	Writer * logWriter = Writer::Instance();
	string logLine = "";
	Miscellaneous::banner();
	/*for (DataSet* x : dataSetContainer){
		x->parseAllLines();
	}
	cout << " parsing all data finished in " << t << " seconds" << endl;*/
	// Parse Data Set 1
	dataSetMap["DataSet_1"]->parseAllLines();

	// Parse Data Set 2A
	dataSetMap["DataSet_2A"]->parseAllLines();
	logLine.append(string(133, '*'));
	logLine.append("\n\n");
	logLine.append(Miscellaneous::setLSTLine("* * * * *   I N D U C E D   P A R T I A L   S A T U R A T I O N   S I M U L A T I O N   * * * * *"));
	logLine.append("\n\n"); 
	logLine.append(string(133, '*'));
	logLine.append("\n\n\n\n\n");
	logWriter->writeContainer.push_back(logLine);
	logLine.clear();

	// Parse Data Set 2B
	dataSetMap["DataSet_2B"]->parseAllLines();

	logLine.append(string(133, '-') +"\n\n");
	logLine.append(Miscellaneous::setLSTLine(TITLE1));
	logLine.append("\n\n");
	logLine.append(Miscellaneous::setLSTLine(TITLE2));
	logLine.append("\n\n");
	logLine.append(string(133, '-') + "\n\n\n\n\n");
	logWriter->writeContainer.push_back(logLine);
	logLine.clear();

	// Print List of Files to LST file
	InputFiles* inputFiles = InputFiles::Instance();
	inputFiles->printInputFilesToLST();
	// Read Simulation Properties
	inputFiles->readPropsINP();

	logLine.append("\n\n\n\n\n");
	logLine.append(string(11, ' ') + "D E F I N E D   L A Y E R   I N F O R M A T I O N\n\n");
	logLine.append(string(13, ' ') + "LAYER NO \t UNIT WEIGHT \t TOP OF THE LAYER \t BOTTOM OF THE LAYER\n");
	logLine.append(string(13, ' ') + "-------- \t ----------- \t ---------------- \t -------------------\n");


	for (Layer* lay : layers){
		char buff[512];
		_snprintf(buff, sizeof(buff), "             %8d \t %10.1f \t    %9.2f    \t    %9.2f   \n", lay->getLayerNumber(),lay->getLayerUnitWeight(),lay->getLayerTop(),lay->getLayerBottom());
		logLine.append(buff);
	}
	logLine.append("\n\n\n\n");
	logWriter->writeContainer.push_back(logLine);
	logLine.clear();

	
	// ParseData Set 3
	dataSetMap["DataSet_3"]->parseAllLines();

	
	// ParseData Set 4
	dataSetMap["DataSet_4"]->parseAllLines();
	
	logLine.append(string(11, ' ') + "S I M U L A T I O N   M O D E   O P T I O N S\n\n");
	if ((ISSTRA == 1) && (ISSFLO != 1)){
		exitOnError("INP-4-5");
	}

	if (IUNSAT == 1){
		logLine.append(string(11, ' ') + "- ALLOW UNSATURATED AND SATURATED FLOW: UNSATURATED PROPERTIES ARE USER-PROGRAMMED IN SUBROUTINE U N S A T\n");
	}
	else if (IUNSAT == 0){
		logLine.append(string(11, ' ') + "- ASSUME SATURATED FLOW ONLY\n");
	}

	if (ISSFLO == 1){
		logLine.append(string(11, ' ') + "- ASSUME STEADY-STATE FLOW FIELD CONSISTENT WITH INITIAL CONCENTRATION CONDITIONS\n");
	}
	else if (ISSFLO == 0){
		logLine.append(string(11, ' ') + "- ALLOW TIME-DEPENDENT FLOW FIELD\n");
	}

	if (ISSTRA == 1){
		logLine.append(string(11, ' ') + "- ASSUME STEADY-STATE TRANSPORT\n");
	}
	else if (ISSTRA == 0){
		logLine.append(string(11, ' ') + "- ALLOW TIME-DEPENDENT TRANSPORT\n");
	}

	if (IREAD == -1){
		logLine.append(string(11, ' ') + "- WARM START - SIMULATION IS TO BE CONTINUED FROM PREVIOUSLY-STORED DATA");
	}
	else if (IREAD == 1){
		logLine.append(string(11, ' ') + "- COLD START - BEGIN NEW SIMULATION\n");
	}

	if (ISTORE > 0){
		logLine.append(string(11, ' ') + "- STORE RESULTS AFTERE EVERY " + to_string(ISTORE) + " TIME STEPS IN RESTART FILE AS BACKUP AND FOR USE IN A SIMULATION RESTART\n");
	}
	else if (ISTORE == 0){
		logLine.append(string(11, ' ') + "- DO NOT STORE RESULTS FOR USE IN A RESTART OF SIMULATION\n");
	}
	logLine.append("\n\n\n");
	logWriter->writeContainer.push_back(logLine);
	logLine.clear();

	logLine.append(string(11, ' ') + "S I M U L A T I O N   C O N T R O L   N U M B E R S\n\n");
	char buff[512];
	_snprintf(buff, sizeof(buff), "           %8d \t NUMBER OF NODES IN FINITE-ELEMENT MESH\n",NN);
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %8d \t NUMBER OF ELEMENTS IN MESH\n\n", NE);
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %8d \t EXACT NUMBER OF NODES IN MESH AT WHICH PRESSURE IS A SPECIFIED CONSTANT OR FUNCTION OF TIME\n",NPBC);
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %8d \t EXACT NUMBER OF NODES IN MESH AT WHICH SOLUTE CONCENTRATION IS A SPECIFIED CONSTANT OR FUNCTION OF TIME\n\n", NUBC);
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %8d \t EXACT NUMBER OF NODES IN MESH AT WHICH FLUID INFLOW OR OUTFLOW IS A SPECIFIED CONSTANT OR FUNCTION OF TIME\n", NSOP);
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %8d \t EXACT NUMBER OF NODES AT WHICH A SOURCE OR SINK OF SOLUTE MASS IS A SPECIFIED CONSTANT OR FUNCTION OF TIME\n\n", NSOU);
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %8d \t EXACT NUMBER OF NODES AT WHICH PRESSURE AND CONCENTRATION WILL BE OBSERVED\n\n\n\n\n", NOBS);
	logLine.append(buff);
	logWriter->writeContainer.push_back(logLine);
	logLine.clear();

	// ParseData Set 5
	dataSetMap["DataSet_5"]->parseAllLines();

	logLine.append(string(11, ' ') + "N U M E R I C A L   C O N T R O L   D A T A\n\n");
	_snprintf(buff, sizeof(buff), "           %1.8f \t 'UPSTREAM WEIGHTING FACTOR'\n", UP);
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %1.4e \t SPECIFIED PRESSURE BOUNDARY CONDITION FACTOR'\n", GNUP);
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %1.4e \t SPECIFIED CONCENTRATION BOUNDARY CONDITION FACTOR'\n\n\n\n\n", GNUU);
	logLine.append(buff);
	logWriter->writeContainer.push_back(logLine);
	logLine.clear();


	// Data Set 6:
	dataSetMap["DataSet_6"]->parseAllLines();

	
	//INPUT DATASET 7A : ITERATION CONTROLS FOR RESOLVING NONLINEARITIES
	dataSetMap["DataSet_7A"]->parseAllLines();
	
	//INPUT DATASET 7b : MATRIX EQUATION SOLVER CONTROLS FOR PRESSURE
	dataSetMap["DataSet_7B"]->parseAllLines();
	

	//INPUT DATASET 7c : MATRIX EQUATION SOLVER CONTROLS FOR TRANSPORT
	dataSetMap["DataSet_7C"]->parseAllLines();

	// SOME ALLOCATION IT MAY NOT NECESSARY FOR THIS PROGRAM

	// INPUT DATASETS 8 to 15
	// INPUT DATASET 8A:  OUTPUT CONTROLS AND OPTIONS FOR LST FILE and SCREEN
	dataSetMap["DataSet_8ABC"]->parseAllLines();

	// INPUT DATA  SET 8E: OUTPUT CONTROLS AND OPTIONS FOR BCOF,BCOP,BCOS and BCOU FILES
	dataSetMap["DataSet_8D"]->parseAllLines();

	// INPUT DATASET 8 E:
	dataSetMap["DataSet_8E_9_10_11"]->parseAllLines();
	// INPUT DATA SET 12 : PRODUCTION OF ENERGY OR SOLUTE MASS
	dataSetMap["DataSet_12_13_14A"]->parseAllLines();

	// INPUT DATA SET 14b : NODEWISE DATA
	//dataSetMap["DataSet_14B"]->parseAllLines();
	parseDataSet_14B();
	// INPUT 15 A
	dataSetMap["DataSet_15A"]->parseAllLines();
	

	//INPUT 15B - Element DATA
//	dataSetMap["DataSet_15B"]->parseAllLines();
	parseDataSet_15B();


	// INPUT DATA SETS 17&18 (SOURCES OF FLUID MASS AND ENERGY OR SOLUTE MASS)
	if (NSOP - 1 > 0){
		dataSetMap["DataSet_17"]->parseAllLines();
	}
	if (NSOU - 1 > 0){
		dataSetMap["DataSet_18"]->parseAllLines();
	}

	// DATASETS 19 &20 (SPECIFIED P AND U BOUNDARY CONDITIONS
	if (NPBC > 0){
		//dataSetMap["DataSet_19"]->parseAllLines();
		Bound("DataSet_19");
	}
	if (NUBC > 0){
		//dataSetMap["DataSet_20"]->parseAllLines();
		Bound("DataSet_20");
	}

	t.reset();
	dataSetMap["DataSet_22"]->parseAllLines();	
	cout << "Incidence Map: " << t <<" seconds" << endl;
}

void ControlParameters::setsSutra(string str){ this->sSutra = str; }
void ControlParameters::setvVersion(string str){ this->vVersion = str; }
void ControlParameters::setvVersionNum(string str){ this->vVersionNum = str; }
void ControlParameters::setsimulationType(string str){ this->simulationType = str; }
void ControlParameters::setsTransport(string str){ this->sTransport = str; }
void ControlParameters::setME(int ME){ this->ME = ME; }
void ControlParameters::setMSHSTR(string str){
	this->MSHSTR = str;
}
void ControlParameters::setKTYPE(int i, int val){
	this->KTYPE[i] = val;
}
int ControlParameters::getKTYPE(int i){
	return this->KTYPE[i];
}
string ControlParameters::getMSHSTR(){
	return this->MSHSTR;
}
void ControlParameters::setNE(int val){ this->NE = val;
elementRegion = new int[NE + 1];
elementNodes = new int*[NE + 1];
elALMAX = new double[NE + 1];
elALMID = new double[NE + 1];
elALMIN = new double[NE + 1];
elATMAX = new double[NE + 1];
elATMIN = new double[NE + 1];
elATMID = new double[NE + 1];
elPMAX = new double[NE + 1];
elPMID = new double[NE + 1];
elPMIN = new double[NE + 1];
elANGLE1 = new double[NE + 1];
elANGLE2 = new double[NE + 1];
elANGLE3 = new double[NE + 1];

elementContainer.reserve(NE+1);
Timer t;
elementContainer.push_back(new Element()); // an empty Element
for (int i = 1; i <= NE; i++){
	elementContainer.push_back(new Element(i));
}
cout << NE << " elements created in " << t << "seconds " << endl;

}
void ControlParameters::setNPBC(int val){ this->NPBC = val; }
void ControlParameters::setNUBC(int val){ this->NUBC = val; }
void ControlParameters::setNSOP(int val){ this->NSOP = val; }
void ControlParameters::setNSOU(int val){ this->NSOU = val; }
void ControlParameters::setNOBS(int val){ this->NOBS = val; }
int ControlParameters::getNE(){ return this->NE; }
int ControlParameters::getNPBC(){ return this->NPBC; }
int ControlParameters::getNUBC(){ return this->NUBC; }
int ControlParameters::getNSOP(){ return this->NSOP; }
int ControlParameters::getNSOU(){ return this->NSOU; }
int ControlParameters::getNOBS(){ return this->NOBS; }
int ControlParameters::getNN1(){ return this->NN1; }
int ControlParameters::getNN2(){ return this->NN2; }
int ControlParameters::getNN3(){ return this->NN3; }
int ControlParameters::getNN(){ return this->NN; }
void ControlParameters::setNN1(int val){ this->NN1 = val; }
void ControlParameters::setNN2(int val){ this->NN2 = val; }
void ControlParameters::setNN3(int val){ this->NN3 = val; }

deque<string> ControlParameters::getWriteContainer(){
	return this->writeContainer;
}
void ControlParameters::addToWriteContainer(string s){
	this->writeContainer.push_back(s);
}

void ControlParameters::setNN(int val){ this->NN = val;
nodePorosity = new double[NN + 1];
nodeX = new double[NN + 1];
nodeY = new double[NN + 1];
nodeZ = new double[NN + 1];
nodeRegion = new int[NN + 1];
nodeSOP = new double[NN + 1];
nodePBC = new double[NN + 1]{};
nodeUBC = new double[NN + 1]{};
nodeQIN = new double[NN + 1]{};
nodeUIN = new double[NN + 1]{};
nodeQUIN = new double[NN + 1]{};
nodePVEC = new double[NN + 1]{};
nodeUVEC = new double[NN + 1]{};

nodeContainer.reserve(NN+1);

Timer t;
nodeContainer.push_back(new Node()); // an empty Node
for (int i = 1; i <= NN; i++){

	nodeContainer.push_back(new Node(i));
	
}
cout << NN << " nodes created in " << t << "seconds " << endl;
}
string ControlParameters::getsSutra(){ return this->sSutra; }
string ControlParameters::getvVersion(){ return this->vVersion; }
string ControlParameters::getvVersionNum(){ return this->vVersionNum; }
string ControlParameters::getsimulationType(){ return this->simulationType; }
string ControlParameters::getsTransport(){ return this->sTransport; }
int ControlParameters::getME(){ return this->ME; }

void ControlParameters::setElementParametersThread(){
	int size = elementContainer.size() - 1;
	int div = size / 8;
	int rem = size - 7 * div;
	
	thread t[8];
	for (int i = 0; i < 8; i++){
		if (i == 7){
			int begin = i * div+1;
			int end = (i)*div + rem;
			t[i] = thread(&ControlParameters::setElementParameters, this, begin, end, i + 1);
			print_start_end(begin, end, i);
		}
		else{
			int begin = i * div + 1;
			int end = (i+1)*div ;
			t[i] = thread(&ControlParameters::setElementParameters, this, begin, end, i + 1);
			print_start_end(begin, end, i);
		}
	}

	for (int i = 0; i < 8; i++){
		t[i].join();
	}

}

void ControlParameters::setElementParameters(int b, int e, int tid){
	if (KTYPE[0] == 3){
	for (vector<Element *>::iterator it = elementContainer.begin() + b; it != elementContainer.begin()+e; ++it){
		(*it)->ROTMATTENSYM();
		
	}
	cout << " Thread " << tid << " ended. " << endl;
	}
	else if (KTYPE[0] == 2){
		for (vector<Element *>::iterator it = elementContainer.begin() + b; it != elementContainer.begin()+e; ++it){
			(*it)->setPERMS2D();
		}
		cout << " Thread " << tid << " ended. " << endl;
	}
}

void ControlParameters::setTSTART(double val){ this->TSTART = val; }
double ControlParameters::getTSTART(){ return this->TSTART; }
void ControlParameters::setUP(double val){ this->UP = val; }
void ControlParameters::setGNUP(double val){ this->GNUP = val; }
void ControlParameters::setGNUU(double val){ this->GNUU = val; }
double ControlParameters::getUP(){ return this->UP; }
double ControlParameters::getGNUP(){ return this->GNUP; }
double ControlParameters::getGNUU(){ return this->GNUU; }
void ControlParameters::setNSCH(int val){ this->NSCH = val; }
void ControlParameters::setNSCHAU(int val){ this->NSCHAU = val; }
void ControlParameters::setNPCYC(int val){ this->NPCYC = val; }
void ControlParameters::setNUCYC(int val){ this->NUCYC = val; }
int ControlParameters::getNSCH(){ return this->NSCH; }
int ControlParameters::getNSCHAU(){ return this->NSCHAU; }
int ControlParameters::getNPCYC(){ return this->NPCYC; }
int ControlParameters::getNUCYC(){ return this->NUCYC; }
void ControlParameters::setCUNSAT(string str){ this->CUNSAT = str; }
void ControlParameters::setCSSFLO(string str){ this->CUNSAT = str; }
void ControlParameters::setCSSTRA(string str){ this->CUNSAT = str; }
void ControlParameters::setCREAD(string str){ this->CUNSAT = str; }
void ControlParameters::setIUNSAT(int val){ this->IUNSAT = val; }
void ControlParameters::setISSFLO(int val){ this->ISSFLO = val; }
void ControlParameters::setISSTRA(int val){ this->ISSTRA = val; }
void ControlParameters::setIREAD(int val){ this->IREAD = val; }
void ControlParameters::setISTORE(int val){ this->ISTORE = val; }
string ControlParameters::getCUNSAT(){ return this->CUNSAT; }
string ControlParameters::getCSSFLO(){ return this->CSSFLO; }
string ControlParameters::getCSSTRA(){ return this->CSSTRA; }
string ControlParameters::getCREAD(){ return this->CREAD; }
int ControlParameters::getIUNSAT(){ return this->IUNSAT; }
int ControlParameters::getISSFLO(){ return this->ISSFLO; }
int ControlParameters::getISSTRA(){ return this->ISSTRA; }
int ControlParameters::getIREAD(){ return this->IREAD; }
int ControlParameters::getISTORE(){ return this->ISTORE; }

void ControlParameters::setITRMAX(int val){ this->ITRMAX = val; }
void ControlParameters::setRPMAX(double val){ this->RPMAX = val; }
void ControlParameters::setRUMAX(double val){ this->RUMAX = val; }
int ControlParameters::getITRMAX(){ return this->ITRMAX; }
double ControlParameters::getRPMAX(){ return this->RPMAX; }
double ControlParameters::getRUMAX(){ return this->RUMAX; }

void ControlParameters::setCSOLVP(string str){ this->CSOLVP = str; }
void ControlParameters::setCSOLVU(string str){ this->CSOLVU = str; }
void ControlParameters::setITRMXP(int val){ this->ITRMXP = val; }
void ControlParameters::setTOLP(double val){ this->TOLP = val; }
void ControlParameters::setITRMXU(int val){ this->ITRMXU = val; }
void ControlParameters::setTOLU(double val){ this->TOLU = val; }
string ControlParameters::getCSOLVP(){ return this->CSOLVP; }
string ControlParameters::getCSOLVU(){ return this->CSOLVU; }
int ControlParameters::getITRMXP(){ return this->ITRMXP; }
int ControlParameters::getITRMXU(){ return this->ITRMXU; }
double ControlParameters::getTOLP(){ return this->TOLP; }
double ControlParameters::getTOLU(){ return this->TOLU; }
void ControlParameters::setITOLP(int val){ this->ITOLP = val; }
void ControlParameters::setITOLU(int val){ this->ITOLU = val; }
int ControlParameters::getITOLP(){ return this->ITOLP; }
int ControlParameters::getITOLU(){ return this->ITOLU; }
void ControlParameters::setKSOLVP(int val){ this->KSOLVP = val; }
void ControlParameters::setKSOLVU(int val){ this->KSOLVU = val; }
int ControlParameters::getKSOLVP(){ return this->KSOLVP; }
int ControlParameters::getKSOLVU(){ return this->KSOLVU; }
void ControlParameters::setNSAVEP(int val){ this->NSAVEP = val; }
void ControlParameters::setNSAVEU(int val){ this->NSAVEU = val; }
int ControlParameters::getNSAVEU(){ return this->NSAVEU; }
void ControlParameters::setNOBSN(int val){ this->NOBSN = val; }
void ControlParameters::setNBCN(int val){ this->NBCN = val; }
int ControlParameters::getNOBSN(){ return this->NOBSN; }
int ControlParameters::getNBCN(){ return this->NBCN; }
void ControlParameters::setNNNX(int val){ this->NNNX = val; }
void ControlParameters::setNDIMJA(int val){ this->NDIMJA = val; }
void ControlParameters::setNNVEC(int val){ this->NNVEC = val; }
int ControlParameters::getNNVEC(){return this->NNVEC; }
int ControlParameters::getNDIMJA(){ return this->NDIMJA; }
int ControlParameters::getNNNX(){ return this->NNNX; }
int ControlParameters::getNSAVEP(){ return this->NSAVEP; }

void ControlParameters::setN48(int val){ this->N48 = val; }
void ControlParameters::setNIN(int val){ this->NIN = val; }
void ControlParameters::setNEX(int val){ this->NEX = val; }
int ControlParameters::getN48(){ return this->N48; }
int ControlParameters::getNIN(){ return this->NIN; }
int ControlParameters::getNEX(){ return this->NEX; }

void ControlParameters::setNPRINT(int val){ this->NPRINT = val; }
void ControlParameters::setCNODAL(string str){ this->CNODAL = str; }
void ControlParameters::setCELMNT(string str){ this->CELMNT = str; }
void ControlParameters::setCINCID(string str){ this->CINCID = str; }
void ControlParameters::setCPANDS(string str){ this->CPANDS = str; }
void ControlParameters::setCVEL(string str){ this->CVEL = str; }
void ControlParameters::setCCORT(string str){ this->CCORT = str; }
void ControlParameters::setCBUDG(string str){ this->CBUDG = str; }
void ControlParameters::setCSCRN(string str){ this->CSCRN = str; }
void ControlParameters::setCPAUSE(string str){ this->CPAUSE = str; }
int ControlParameters::getNPRINT(){ return this->NPRINT; }
string ControlParameters::getCNODAL(){ return this->CNODAL; }
string ControlParameters::getCELMNT(){ return this->CELMNT; }
string ControlParameters::getCINCID(){ return this->CINCID; }
string ControlParameters::getCPANDS(){ return this->CPANDS; }
string ControlParameters::getCVEL(){ return this->CVEL; }
string ControlParameters::getCCORT(){ return this->CCORT; }
string ControlParameters::getCBUDG(){ return this->CBUDG; }
string ControlParameters::getCSCRN(){ return this->CSCRN; }
string ControlParameters::getCPAUSE(){ return this->CPAUSE; }
void ControlParameters::setKNODAL(int val){ this->KNODAL = val; }
void ControlParameters::setKELMNT(int val){ this->KELMNT = val; }
void ControlParameters::setKINCID(int val){ this->KINCID = val; }
void ControlParameters::setKPANDS(int val){ this->KPANDS = val; }
void ControlParameters::setKVEL(int val){ this->KVEL = val; }
void ControlParameters::setKCORT(int val){ this->KCORT = val; }
void ControlParameters::setKBUDG(int val){ this->KBUDG = val; }
void ControlParameters::setKSCRN(int val){ this->KSCRN = val; }
void ControlParameters::setKPAUSE(int val){ this->KPAUSE = val; }

int ControlParameters::getKNODAL(){ return this->KNODAL; }
int ControlParameters::getKELMNT(){ return this->KELMNT; }
int ControlParameters::getKINCID(){ return this->KINCID; }
int ControlParameters::getKPANDS(){ return this->KPANDS; }
int ControlParameters::getKVEL(){ return this->KVEL; }
int ControlParameters::getKCORT(){ return this->KCORT; }
int ControlParameters::getKBUDG(){ return this->KBUDG; }
int ControlParameters::getKSCRN(){ return this->KSCRN; }
int ControlParameters::getKPAUSE(){ return this->KPAUSE; }
vector<ObservationPoints *> ControlParameters::getObservationPointsContainer(){ return this->observationPointsContainer; }
string ControlParameters::getVERN(){ return this->VERN; }
void ControlParameters::setNCOLPR(int val){ this->NCOLPR = val; }
void ControlParameters::setLCOLPR(int val){ this->LCOLPR = val; }
int ControlParameters::getNCOLPR(){ return this->NCOLPR; }
int ControlParameters::getLCOLPR(){ return this->LCOLPR; }
void ControlParameters::setNCOLMX(int val){ this->NCOLMX = val; }
void ControlParameters::setLCOLMX(int val){ this->LCOLMX = val; }
int ControlParameters::getNCOLMX(){ return this->NCOLMX; }
int ControlParameters::getLCOLMX(){ return this->LCOLMX; }
void ControlParameters::addToNCOL(string str){ this->NCOL.push_back(str); }
vector<string> ControlParameters::getNCOL(){ return this->NCOL; }
void ControlParameters::addToLCOL(string str){ this->LCOL.push_back(str); }
vector<string> ControlParameters::getLCOL(){ return this->LCOL; }


void ControlParameters::setNBCFPR(int val){ this->NBCFPR = val; }
void ControlParameters::setNBCSPR(int val){ this->NBCSPR = val; }
void ControlParameters::setNBCPPR(int val){ this->NBCPPR = val; }
void ControlParameters::setNBCUPR(int val){ this->NBCUPR = val; }
void ControlParameters::setCINACT(string str){ this->CINACT = str; }
int ControlParameters::getNBCFPR(){ return this->NBCFPR; }
int ControlParameters::getNBCSPR(){ return this->NBCSPR; }
int ControlParameters::getNBCPPR(){ return this->NBCPPR; }
int ControlParameters::getNBCUPR(){ return this->NBCUPR; }
string ControlParameters::getCINACT(){ return this->CINACT; }
int ControlParameters::getKINACT(){ return this->KINACT; }
void ControlParameters::setKINACT(int val){ this->KINACT = val; }

void ControlParameters::setCOMPFL(double val){ this->COMPFL = val; }
void ControlParameters::setCW(double val){ this->CW = val; }
void ControlParameters::setSIGMAW(double val){ this->SIGMAW = val; }
void ControlParameters::setRHOW0(double val){ this->RHOW0 = val; }
void ControlParameters::setURHOW0(double val){ this->URHOW0 = val; }
void ControlParameters::setDRWDU(double val){ this->DRWDU = val; }
void ControlParameters::setVISC0(double val){ this->VISC0 = val; }
double ControlParameters::getCOMPFL(){ return this->COMPFL; }
double ControlParameters::getCW(){ return this->CW; }
double ControlParameters::getSIGMAW(){ return this->SIGMAW; }
double ControlParameters::getRHOW0(){ return this->RHOW0; }
double ControlParameters::getURHOW0(){ return this->URHOW0; }
double ControlParameters::getDRWDU(){ return this->DRWDU; }
double ControlParameters::getVISC0(){ return this->VISC0; }

void  ControlParameters::setCOMPMA(double val){ this->COMPMA = val; }
void  ControlParameters::setCS(double val){ this->CS = val; }
void  ControlParameters::setSIGMAS(double val){ this->SIGMAS = val; }
void  ControlParameters::setRHOS(double val){ this->RHOS = val; }
double ControlParameters::getCOMPMA(){ return this->COMPMA; }
double ControlParameters::getCS(){ return this->CS; }
double ControlParameters::getSIGMAS(){ return this->SIGMAS; }
double ControlParameters::getRHOS(){ return this->RHOS; }

void ControlParameters::setPRODF0(double val){ this->PRODF0 = val; }
void ControlParameters::setPRODS0(double val){ this->PRODS0 = val; }
void ControlParameters::setPRODF1(double val){ this->PRODF1 = val; }
void ControlParameters::setPRODS1(double val){ this->PRODS1 = val; }
double ControlParameters::getPRODF0(){ return this->PRODF0; }
double ControlParameters::getPRODS0(){ return this->PRODS0; }
double ControlParameters::getPRODF1(){ return this->PRODF1; }
double ControlParameters::getPRODS1(){ return this->PRODS1; }

void ControlParameters::setGRAVX(double val){ this->GRAVX = val; }
double ControlParameters::getGRAVX(){ return this->GRAVX; }
void ControlParameters::setGRAVY(double val){ this->GRAVY = val; }
double ControlParameters::getGRAVY(){ return this->GRAVY; }
void ControlParameters::setGRAVZ(double val){ this->GRAVZ = val; }
double ControlParameters::getGRAVZ(){ return this->GRAVZ; }

void ControlParameters::setSCALX(double val){ this->SCALX = val; }
void ControlParameters::setSCALY(double val){ this->SCALY = val; }
void ControlParameters::setSCALZ(double val){ this->SCALZ = val; }
void ControlParameters::setPORFAC(double val){ this->PORFAC= val; }
double ControlParameters::getSCALX(){ return this->SCALX; }
double ControlParameters::getSCALY(){ return this->SCALY; }
double ControlParameters::getSCALZ(){ return this->SCALZ; }
double ControlParameters::getPORFAC(){ return this->PORFAC; }
vector<Node *> ControlParameters::getNodeContainer(){ return this->nodeContainer; }
vector<Element *> ControlParameters::getElementContainer(){ return this->elementContainer; }
void ControlParameters::setPMAXFA(double val){ this->PMAXFA = val; }
void ControlParameters::setPMIDFA(double val){ this->PMIDFA = val; }
void ControlParameters::setPMINFA(double val){ this->PMINFA = val; }
void ControlParameters::setANG1FA(double val){ this->ANG1FA = val; }
void ControlParameters::setANG2FA(double val){ this->ANG2FA = val; }
void ControlParameters::setANG3FA(double val){ this->ANG3FA = val; }
void ControlParameters::setALMAXF(double val){ this->ALMAXF = val; }
void ControlParameters::setALMIDF(double val){ this->ALMIDF = val; }
void ControlParameters::setALMINF(double val){ this->ALMINF = val; }
void ControlParameters::setATMXF(double val){ this->ATMXF = val; }
void ControlParameters::setATMDF(double val){ this->ATMDF = val; }
void ControlParameters::setATMNF(double val){ this->ATMNF = val; }
void ControlParameters::setANGFAC(double val){ this->ANGFAC = val; }
double ControlParameters::getANGFAC(){ return this->ANGFAC; }

double ControlParameters::getPMAXFA(){ return this->PMAXFA; }
double ControlParameters::getPMIDFA(){ return this->PMIDFA; }
double ControlParameters::getPMINFA(){ return this->PMINFA; }
double ControlParameters::getANG1FA(){ return this->ANG1FA; }
double ControlParameters::getANG2FA(){ return this->ANG2FA; }
double ControlParameters::getANG3FA(){ return this->ANG3FA; }
double ControlParameters::getALMAXF(){ return this->ALMAXF; }
double ControlParameters::getALMIDF(){ return this->ALMIDF; }
double ControlParameters::getALMINF(){ return this->ALMINF; }
double ControlParameters::getATMXF(){ return this->ATMXF; }
double ControlParameters::getATMDF(){ return this->ATMDF; }
double ControlParameters::getATMNF(){ return this->ATMNF; }
void ControlParameters::setITMAX(int val){ this->ITMAX = val; }
int ControlParameters::getITMAX(){ return this->ITMAX; }



void ControlParameters::Source(string key){
	      //CALL SOURCE(QIN, UIN, IQSOP, QUIN, IQSOU, IQSOPT, IQSOUT,IBCSOP, IBCSOU)
	//SUBROUTINE SOURCE(QIN, UIN, IQSOP, QUIN, IQSOU, IQSOPT, IQSOUT, IBCSOP, IBCSOU)
	string logLine = "";
	Writer * logWriter = Writer::Instance();
	char buff[512];
	
	string line;
	vector <string> dataPos;
	int NSOPI = NSOP - 1;
	int NSOUI = NSOU - 1;

	IQSOPT = 1;
	IQSOUT = 1;
	string compare = key;
	if (key == "DataSet_17"){
		logLine.append("\n\n\n\n           F L U I D   S O U R C E   D A T A\n\n\n\n");
		logLine.append("           **** NODES AT WHICH FLUID INFLOWS OR OUTFLOWS ARE SPECIFIED ****\n\n");
		logLine.append("                            DEFAULT FLUID     DEFAULT CONCENTRATION\n");
		logLine.append("                      INFLOW(+)/OUTFLOW(-)       OF INFLOWING FLUID\n");
		logLine.append("            NODE       (FLUID MASS/SECOND)  (MASS SOLUTE/MASS WATER)\n\n");
		int IQCP;
		double QINC;
		double UINC;

		pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[17];
		int size = dataSetPosition.second - dataSetPosition.first;
		char * map = InputFileParser::Instance()->mapViewOfINPFile;
		vector<string> lines;
		string line;
		vector<char> lineBuffer;
		char* str_start = map + dataSetPosition.first;
		char * str_end;
		Node * aNode;
		const char * del = " ";
		for (int j = dataSetPosition.first; j < dataSetPosition.second; j++){
			if (map[j] == '\r'){
				str_end = map + j;
				lineBuffer.assign(str_start, str_end);
				str_start = map + j + 1;
				lineBuffer.push_back('\0');
				IQCP = stoi(strtok(lineBuffer.data(), del));
				if (IQCP != 0){
					if (IQCP > 0){
						QINC = stod(strtok(lineBuffer.data(), del));
						if (QINC > 0){
							UINC = stod(strtok(lineBuffer.data(), del));
						}
						else{
							UINC = 0.0;
						}
					}
					else{
						QINC = 0.0;
						UINC = 0.0;
						IQSOPT = -1;
					}

					IQSOP.push_back(IQCP);
					int nn = abs(IQCP);
					nodeQIN[nn] = QINC;
					nodeUIN[nn] = UINC;
				
					if (IQCP > 0){
						if (QINC > 0){
							_snprintf(buff, sizeof(buff), "       %9d      %+20.13e      %20.13e\n", IQCP, QINC, UINC);
							logLine.append(buff);
						}
						else{
							_snprintf(buff, sizeof(buff), "       %9d      %+20.13e\n", IQCP, QINC);
							logLine.append(buff);

						}
					}
					else{
						_snprintf(buff, sizeof(buff), "       %9d\n", IQCP);
						logLine.append(buff);
					}

				}
			}
			

		}
		
		if (IQSOPT != -1){
			logLine.append("\n           SPECIFICATIONS MADE IN (OPTIONAL) BCS INPUT FILES TAKE PRECEDENCE OVER THE");
			logLine.append("\n           DEFAULT VALUES LISTED ABOVE AND ANY VALUES SET IN SUBROUTINE BCTIME.\n");
		}
		else{
			logLine.append("\n\n            TIME-DEPENDENT FLUID SOURCE/SINK OR INFLOW CONCENTRATION");
			logLine.append("\n            SET IN SUBROUTINE BCTIME IS INDICATED BY NEGATIVE NODE NUMBER\n");
		}
		logWriter->writeContainer.push_back(logLine);
		logLine.clear();

	}
	else if (key == "DataSet_18"){
		logLine.append("\n\n\n\n           S O L U T E   S O U R C E   D A T A\n");
		logLine.append("\n\n\n           **** NODES AT WHICH FLUID SOURCES OR SINKS OF SOLUTE MASS ARE SPECIFIED ****\n");
		logLine.append("                            DEFAULT SOLUTE\n");
		logLine.append("                            SOURCE(+)/SINK(-)\n");
		logLine.append("            NODE           (SOLUTE MASS/SECOND)\n");
		int IQCU;
		double QUINC;
		pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[18];
		int size = dataSetPosition.second - dataSetPosition.first;
		char * map = InputFileParser::Instance()->mapViewOfINPFile;
		vector<string> lines;
		string line;
		vector<char> lineBuffer;
		char* str_start = map + dataSetPosition.first;
		char * str_end;
		Node * aNode;
		const char * del = " ";
		for (int j = dataSetPosition.first; j < dataSetPosition.second; j++){
			if (map[j] == '\r'){
				str_end = map + j;
				lineBuffer.assign(str_start, str_end);
				str_start = map + j + 1;
				lineBuffer.push_back('\0');
				IQCU = stoi(strtok(lineBuffer.data(), del));
				if (IQCU != 0){
					if (IQCU > 0){
						QUINC = stod(strtok(lineBuffer.data(), del));
					}
					else{
						QUINC = 0.0;
						IQSOUT = -1;
					}
					IQSOU.push_back(IQCU);
					int nn = abs(IQCU);
					nodeQUIN[nn] = QUINC;
				

					if (IQCU > 0){
						_snprintf(buff, sizeof(buff), "       %9d      %+20.13e\n", IQCU,QUINC);
						logLine.append(buff);
					}
					else{
						_snprintf(buff, sizeof(buff), "       %9d\n", IQCU);
						logLine.append(buff);
					}


				}
			}
		}

		
		if (IQSOUT != -1){
			logLine.append("\n           SPECIFICATIONS MADE IN (OPTIONAL) BCS INPUT FILES TAKE PRECEDENCE OVER THE\n");
			logLine.append("           DEFAULT VALUES LISTED ABOVE AND ANY VALUES SET IN SUBROUTINE BCTIME.\n");
		}
		else{
			logLine.append("\n            TIME-DEPENDENT SOLUTE SOURCE/SINK SET IN\n");
			logLine.append("            SUBROUTINE BCTIME IS INDICATED BY NEGATIVE NODE NUMBER\n");
		}
		logWriter->writeContainer.push_back(logLine);
		logLine.clear();
	}
	else{
		exitOnError("INP-17_18-1");
	}

	//if (NSOPI != 0){ //if (NSOPI != 0)
	//	DataSet * dataSet = dataSetMap["DataSet_17"];
	//	
	//	cout << "\n\n\n\n           F L U I D   S O U R C E   D A T A" << endl;
	//	cout << "\n\n\n           **** NODES AT WHICH FLUID INFLOWS OR OUTFLOWS ARE SPECIFIED ****" << endl;
	//	cout << "                            DEFAULT FLUID     DEFAULT CONCENTRATION" << endl;
	//	cout << "                      INFLOW(+)/OUTFLOW(-)       OF INFLOWING FLUID" << endl;
	//	cout << "            NODE       (FLUID MASS/SECOND)  (MASS SOLUTE/MASS WATER" << endl;

	//
	//	int IQCP;
	//	double QINC;
	//	double UINC;
	//	
	//	vector<string> aContainer = *dataSet->getData();
	//	for (string x : aContainer){
	//		dataPos.clear();
	//		boost::split(dataPos, x, boost::is_any_of(" "), boost::token_compress_on);
	//		// IQCP Node Number
	//		// QINC Pressure
	//		// UINC Concentration
	//		IQCP = stoi(dataPos.at(0));

	//		if (IQCP != 0){
	//			if (IQCP > 0){
	//				QINC = stod(dataPos.at(1));
	//				if (QINC > 0){
	//					UINC = stod(dataPos.at(2));
	//				}
	//				else{
	//					UINC = 0.0;
	//				}
	//			}
	//			else{
	//				QINC = 0.0;
	//				UINC = 0.0;
	//				IQSOPT = -1;
	//			}

	//			IQSOP.push_back(IQCP);
	//			int nn = abs(IQCP);
	//			Node* nod = nodeContainer[nn];
	//			nod->setQIN(QINC);
	//			nod->setUIN(UINC);
	//			if (IQCP > 0){
	//				cout << "       " << IQCP << "      " << QINC << "      " << UINC << endl;
	//			}
	//			else{
	//				cout << "       " << IQCP << endl;
	//			}

	//		}
	//	}
	//	cout << "            TIME-DEPENDENT FLUID SOURCE/SINK OR INFLOW CONCENTRATION" << endl;
	//	cout << "            SET IN SUBROUTINE BCTIME IS INDICATED BY NEGATIVE NODE NUMBER" << endl;
	//	if (IQSOPT != -1){
	//		cout << "           SPECIFICATIONS MADE IN (OPTIONAL) BCS INPUT FILES TAKE PRECEDENCE OVER THE" << endl;
	//		cout << "           DEFAULT VALUES LISTED ABOVE AND ANY VALUES SET IN SUBROUTINE BCTIME." << endl;
	//	}

	//}



	/*if (NSOUI != 0){
		DataSet * dataSet = dataSetMap["DataSet_18"];
		
		cout << "\n\n\n\n           S O L U T E   S O U R C E   D A T A" << endl;
		cout << "\n\n\n           **** NODES AT WHICH FLUID SOURCES OR SINKS OF SOLUTE MASS ARE SPECIFIED ****" << endl;
		cout << "                            DEFAULT SOLUTE"<< endl;
		cout << "                            SOURCE(+)/SINK(-)" << endl;
		cout << "            NODE           (SOLUTE MASS/SECOND)" << endl;

		int IQCU;
		double QUINC;
		vector<string> aContainer = *dataSet->getData();
		for (string x : aContainer){
			dataPos.clear();
			boost::split(dataPos, x, boost::is_any_of(" "), boost::token_compress_on);
			IQCU = stoi(dataPos.at(0));
			if (IQCU != 0){
				if (IQCU > 0){
					QUINC = stod(dataPos.at(1));
					}
				else{
					QUINC = 0.0;
					IQSOUT = -1;
				}
				IQSOU.push_back(IQCU);
				int nn = abs(IQCU);
				Node* nod = nodeContainer[nn];
				nod->setQUIN(QUINC);

				if (IQCU > 0){
					cout << "       " << IQCU << "      " << QUINC << endl;
				}
				else{
					cout << "       " << IQCU << endl;
				}
			}

		}
		cout << "            TIME-DEPENDENT SOLUTE SOURCE/SINK SET IN" << endl;
		cout << "            SUBROUTINE BCTIME IS INDICATED BY NEGATIVE NODE NUMBER" << endl;
		if (IQSOUT != -1){
			cout << "           SPECIFICATIONS MADE IN (OPTIONAL) BCS INPUT FILES TAKE PRECEDENCE OVER THE" << endl;
			cout << "           DEFAULT VALUES LISTED ABOVE AND ANY VALUES SET IN SUBROUTINE BCTIME." << endl;
		}

	}*/

	

}
void ControlParameters::Bound(string key){
          //CALL BOUND(IPBC, PBC, IUBC, UBC, IPBCT, IUBCT, IBCPBC, IBCUBC, GNUP1, GNUU1)
	//SUBROUTINE BOUND(IPBC, PBC, IUBC, UBC, IPBCT, IUBCT, IBCPBC, IBCUBC, GNUP1, GNUU1)
	string logLine = "";
	Writer * logWriter = Writer::Instance();
	char buff[512];


	if (key =="DataSet_19"){
		logLine.append("\n\n\n\n           S P E C I F I E D   P R E S S U R E   D A T A\n");
		logLine.append("\n\n\n           **** NODES AT WHICH PRESSURES ARE SPECIFIED ****\n\n");
		logLine.append("                (AS WELL AS SOLUTE CONCENTRATION OF ANY\n");
		logLine.append("                 FLUID INFLOW WHICH MAY OCCUR AT THE POINT\n");
		logLine.append("                 OF SPECIFIED PRESSURE)\n\n");
		logLine.append("            NODE          DEFAULT PRESSURE     DEFAULT CONCENTRATION\n\n\n");
		pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[19];
		int size = dataSetPosition.second - dataSetPosition.first;
		char * map = InputFileParser::Instance()->mapViewOfINPFile;
		vector<string> lines;
		string line;
		vector<char> lineBuffer;
		char* str_start = map + dataSetPosition.first;
		char * str_end;
		Node * aNode;
		int IDUM; // NodeNumber at which Boundary Pressure is defined
		int IDUMA; // ABSOLUTE value of IPU
		int IPU = 0; // Counter for defined boundary Conditions
		IPBCT = 1;
		const char * del = " ";
		for (int j = dataSetPosition.first; j < dataSetPosition.second; j++){
			if (map[j] == '\r'){
				str_end = map + j;
				lineBuffer.assign(str_start, str_end);
				str_start = map + j + 1;
				lineBuffer.push_back('\0');
				IDUM = stoi(strtok(lineBuffer.data(), del));
				if (IDUM != 0){
					IDUMA= abs(IDUM);
					if (IDUMA > NN){
						exitOnError("INP-19-1");

					}
					if(IPU == NPBC){
						break;
					}

					if (IDUM > 0){
						//aNode = nodeContainer[IDUM];
						nodePBC[IDUMA] = stod(strtok(lineBuffer.data(), del));
						nodeUBC[IDUMA] = stod(strtok(lineBuffer.data(), del));
						_snprintf(buff, sizeof(buff), "       %9d      %+20.13e      %+20.13e\n", IDUMA, nodePBC[IDUMA], nodeUBC[IDUMA]);
						logLine.append(buff);
						IPU++;
					}
					else if (IDUM < 0){
						_snprintf(buff, sizeof(buff), "       %9d\n", IDUM);
						logLine.append(buff);
						IPBCT = -1;
						IPU++;
					}	
				}else if (IPU == NPBC){
					if (IPBCT != -1){
						logLine.append("\n           SPECIFICATIONS MADE IN (OPTIONAL BCS INPUT FILES TAKE PRECEDENCE OVER THE\n");
						logLine.append("           DEFAULT VALUES LISTED ABOVE AND ANY VALUES SET IN SUBROUTINE BCTIME.\n");
					}
					else{
						logLine.append("\n            TIME-DEPENDENT SPECIFIED PRESSURE OR INFLOW CONCENTRATION\n");
						logLine.append("            SET-IN SUBROUTINE BCTIME IS INDICATED BY NEGATIVE NODE NUMBER\n");
						logLine.append("\n           SPECIFICATIONS MADE IN (OPTIONAL BCS INPUT FILES TAKE PRECEDENCE OVER THE\n");
						logLine.append("           DEFAULT VALUES LISTED ABOVE AND ANY VALUES SET IN SUBROUTINE BCTIME.\n");
					}
				}
				else{
					exitOnError("INP-3,19-1");
				}
			}
		}

		logWriter->writeContainer.push_back(logLine);
		logLine.clear();
	}
	else if (key == "DataSet_20"){
		logLine.append("\n\n\n\n           S P E C I F I E D   C O N C E N T R A T I O N   D A T A\n");
		logLine.append("\n\n\n           **** NODES AT WHICH SOLUTE CONCENTRATIONS ARE SPECIFIED ****\n");
		logLine.append("                TO BE INDEPENDENT OF LOCAL FLOWS AND FLUID SOURCES\n");
		logLine.append("            NODE     DEFAULT CONCENTRATION\n\n\n");
		pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[20];
		int size = dataSetPosition.second - dataSetPosition.first;
		char * map = InputFileParser::Instance()->mapViewOfINPFile;
		vector<string> lines;
		string line;
		vector<char> lineBuffer;
		char* str_start = map + dataSetPosition.first;
		char * str_end;
		Node * aNode;
		int IDUM; // NodeNumber at which Boundary Pressure is defined
		int IDUMA; // ABSOLUTE value of IPU
		int IPU = 0; // Counter for defined boundary Conditions
		IUBCT = 1;
		const char * del = " ";
		for (int j = dataSetPosition.first; j < dataSetPosition.second; j++){
			if (map[j] == '\r'){
				str_end = map + j;
				lineBuffer.assign(str_start, str_end);
				str_start = map + j + 1;
				lineBuffer.push_back('\0');
				IDUM = stoi(strtok(lineBuffer.data(), del));
				if (IDUM != 0){
					IDUMA = abs(IDUM);
					if (IDUMA > NN){
						exitOnError("INP-19-1");

					}
					if (IPU == NUBC + NPBC){
						break;
					}

					if (IDUM > 0){
						
						nodeUBC[IDUMA]= stod(strtok(lineBuffer.data(), del));
						_snprintf(buff, sizeof(buff), "       %9d      %+20.13e\n", IDUMA, nodeUBC[IDUMA]);
						logLine.append(buff);

						IPU++;
					}
					else if (IDUM < 0){
						_snprintf(buff, sizeof(buff), "       %9d\n", IDUM);
						logLine.append(buff);
						IUBCT = -1;
						IPU++;
					}
				}else if (IPU == NUBC){
					if (IUBCT != -1){
						logLine.append("\n           SPECIFICATIONS MADE IN (OPTIONAL BCS INPUT FILES TAKE PRECEDENCE OVER THE\n");
						logLine.append("           DEFAULT VALUES LISTED ABOVE AND ANY VALUES SET IN SUBROUTINE BCTIME.\n");
					}
					else{
						logLine.append("\n            TIME-DEPENDENT SPECIFIED CONCENTRATIONS USER-PROGRAMMED IN\n");
						logLine.append("            SUBROUTINE BCTIME IS INDICATED BY NEGATIVE NODE NUMBER\n");
						logLine.append("\n           SPECIFICATIONS MADE IN (OPTIONAL BCS INPUT FILES TAKE PRECEDENCE OVER THE\n");
						logLine.append("           DEFAULT VALUES LISTED ABOVE AND ANY VALUES SET IN SUBROUTINE BCTIME.\n");
					}
				}
				else{
					exitOnError("INP-3,20-1");
				}
			}
		}
		logWriter->writeContainer.push_back(logLine);
		logLine.clear();
	}
}

void ControlParameters::Connec(){
	string logLine = "";
	Writer * logWriter = Writer::Instance();
	char buff[512];

	if (this->KINCID == 0){
		logLine.append("\n\n\n\n           M E S H   C O N N E C T I O N   D A T A\n\n");
		logLine.append("                 PRINTOUT OF NODAL INCIDENCES CANCELLED.\n");
	}
	if (this->KINCID == 1){
		logLine.append("\n\n\n\n           M E S H   C O N N E C T I O N   D A T A\n\n\n");
		logLine.append("            **** NODAL INCIDENCES ****\n\n");
	}

	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[21];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<char> lineBuffer;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	int whereat = 0;
	const char * del = " ";
	for (int j = dataSetPosition.first; j < dataSetPosition.second; j++){
		if (map[j] == '\r'){
			str_end = map + j;
			line.assign(str_start, str_end);
			str_start = map + j + 1;
			lineBuffer.push_back('\0');
			whereat = j + 1;
			break;
		}
	}
	if (line != "'INCIDENCE'"){
		exitOnError("INP-22-1");
	}
	int ElNo;
	int nodeNum;
	for (int j = whereat; j < dataSetPosition.second; j++){
		if (map[j] == '\r'){
			str_end = map + j - 1;
			lineBuffer.assign(str_start, str_end);
			lineBuffer.push_back('\0');
			str_start = map + j + 2;

			if (N48 == 8){
				int * elNodes = new int[8];
				ElNo = stoi(strtok(lineBuffer.data(), del));
			
				for (int j = 0; j < 8; j++){
					elNodes[j]= stoi(strtok(NULL, del));
				}
				elementNodes[ElNo] = elNodes;
				if (KINCID == 1){
					_snprintf(buff, sizeof(buff), "           ELEMENT %9d     NODES AT :       CORNERS ***** %9d %9d %9d %9d %9d %9d %9d %9d *****\n", ElNo, elNodes[0], elNodes[1], elNodes[2], elNodes[3], elNodes[4], elNodes[5], elNodes[6], elNodes[7]);
					logLine.append(buff);
				}
			}
			else if (N48 == 4){
				int * elNodes = new int[4];
				ElNo = stoi(strtok(lineBuffer.data(), del));
				for (int j = 0; j < 4; j++){
					elNodes[j] = stoi(strtok(NULL, del));
				}
				elementNodes[ElNo] = elNodes;
				if (KINCID == 1){
					_snprintf(buff, sizeof(buff), "           ELEMENT %9d     NODES AT :       CORNERS ***** %9d %9d %9d %9d *****\n", ElNo, elNodes[0], elNodes[1], elNodes[2], elNodes[3]);
					logLine.append(buff);
				}
			}
			else{
				exitOnError("INP-22-2");
			}
		}
	}
	
	logWriter->writeContainer.push_back(logLine);
	logLine.clear();
}
void ControlParameters::exitOnError(string ERRCOD, vector<string>CHERR){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BRIGHT_WHITE);
	vector<string> outError;
	if (ERRCOD == "INP-2A-1"){
		outError.push_back("The first word of SIMULA is not 'SUTRA'."); // change to IPSSIM
		outError.push_back("In dataset 2A of the main input file, the first word");
		outError.push_back("of the variable SIMULA must be 'SUTRA'.");
		outError.push_back(" ");
		outError.push_back("Example of a valid dataset 2A:");
		outError.push_back("'SUTRA SOLUTE TRANSPORT'");
		
	}
	else if (ERRCOD == "INP-2A-4"){
		outError.push_back("Unsupported SUTRA Version " + CHERR.at(0));
		outError.push_back("Input files from SUTRA version " + CHERR.at(0));
		outError.push_back("are not supported by version " + CHERR.at(1));
	}
	else if (ERRCOD == "INP-2A-3"){
		outError.push_back("Only Solute Transport simulation supported in IPSSIM");
		outError.push_back("In dataset 2A of the main input file, the fourth word must be 'SOLUTE'");
	}
	else if (ERRCOD == "INP-2B-1"){
		outError.push_back("The first Word of MSHSTR is not '2D' or '3D'.");
		outError.push_back("In dataset 2B of the main input file, the first word");
		outError.push_back("of the variable MSHSTR must be '2D' or '3D'.");
		outError.push_back("Example of a valid dataset 2B:");
		outError.push_back("'3D REGULAR MESH'  10  20  30");
	}
	else if (ERRCOD == "INP-2B-4"){
		outError.push_back("The second word of MSHSTR is not 'IRREGULAR', or 'REGULAR'");
		outError.push_back("In dataset 2B of the main input file, the second word");
		outError.push_back("of the variable MSHSTR must be 'IRREGULAR', or 'REGULAR'.\n");
		outError.push_back("Example of a valid dataset 2B:");
		outError.push_back("'3D REGULAR MESH'  10  20  30");
		
	}
	else if (ERRCOD == "INP-4-1"){
		outError.push_back("The first word of CUNSAT is not 'SATURATED' or");
		outError.push_back("'UNSATURATED'.");
		outError.push_back("In dataset 4 of the main input file, the first word");
		outError.push_back("of the variable CUNSAT must be 'SATURATED' or");
		outError.push_back("'UNSATURATED'.");
		outError.push_back("\n");
		outError.push_back("Example of a valid dataset 4:");
		outError.push_back("'SATURATED FLOW' 'STEADY FLOW' 'TRANSIENT TRANSPORT' 'COLD' 10");
	}
	else if (ERRCOD == "INP-4-2"){
		outError.push_back("The first word of CSSFLO is not 'STEADY' or");
		outError.push_back("'TRANSIENT'.");
		outError.push_back("In dataset 4 of the main input file, the first word");
		outError.push_back("of the variable CSSFLO must be 'TRANSIENT' or");
		outError.push_back("'STEADY'.");
		outError.push_back("\n");
		outError.push_back("Example of a valid dataset 4:");
		outError.push_back("'SATURATED FLOW' 'STEADY FLOW' 'TRANSIENT TRANSPORT' 'COLD' 10");
	}
	else if (ERRCOD == "INP-4-3"){
		outError.push_back("The first word of CSSTRA is not 'STEADY' or 'TRANSIENT'.");
		outError.push_back("In dataset 4 of the main input file, the first word");
		outError.push_back("of the variable CSSTRA must be 'TRANSIENT' or");
		outError.push_back("'STEADY'.");
		outError.push_back("\n");
		outError.push_back("Example of a valid dataset 4:");
		outError.push_back("'SATURATED FLOW' 'STEADY FLOW' 'TRANSIENT TRANSPORT' 'COLD' 10");
	}
	else if (ERRCOD == "INP-4-4"){
		outError.push_back("The first word of CREAD is not 'COLD' or");
		outError.push_back("'WARM'.");
		outError.push_back("In dataset 4 of the main input file, the first word");
		outError.push_back("of the variable CREAD must be 'COLD' or");
		outError.push_back("'WARM'.");
		outError.push_back("\n");
		outError.push_back("Example of a valid dataset 4:");
		outError.push_back("'SATURATED FLOW' 'STEADY FLOW' 'TRANSIENT TRANSPORT' 'COLD' 10");
	}
	else if (ERRCOD == "INP-6-11"){
		outError.push_back("Invalid user - defined schedule name, ");
		outError.push_back(CHERR.at(0));
		outError.push_back("\nSchedule names 'STEP_0', 'STEP_1', and 'STEPS_1&UP' ");
		outError.push_back("are reserved by IPSSIM and may not be used to name a user-defined schedule");

	}
	else if (ERRCOD == "INP-7B&C-1"){
		outError.push_back("Unrecognized solver name.");
		outError.push_back("In datasets 7B&C, valid solver selections are:");
		for (int i = 0; i < 4; i++){
			outError.push_back(SOLWRD[0] + "-->" + SOLNAM[0]);
		};
		outError.push_back("Note that solver selections for P and U must be both DIRECT or both iterative.");

	}
	else if (ERRCOD == "INP-7B&C-2"){
		outError.push_back("Solver selections for P and U are not both DIRECT or both iterative.");
	}
	else if (ERRCOD == "INP-7B&C-3"){
		outError.push_back("Invalid selection of the CG solver.");
	}
	else if (ERRCOD == "INP-22-1"){
		outError.push_back("Line 1 of the element incidence does not begin with the word 'INCIDENCE' ");
	}
	else if (ERRCOD == "INP-22-2"){
		outError.push_back("vala vala CIDENCE' ");
	}
	for (string a : outError)
		cout << a << endl;

	cout << "\nTerminating program in 10 seconds" << endl;
	for (int i = 100; i >0; i--){
		cout << i << " seconds to terminate the program." << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
	cout << "Byee..." << endl;
	exit(1);
}

void ControlParameters::writeToLSTString(string str){
	string filename = InputFiles::Instance()->getInputDirectory() + "\\" + InputFiles::Instance()->filesToWrite["LST"];
	ofstream outfile;
	if (!isNewRun){//is_file_exist(filename)){
		outfile.open(filename, std::ios::app);
	
			outfile << str << endl;
			
	}
	else{
		outfile.open(filename, std::ios_base::out);
		
			outfile << str << endl;
			isNewRun = false;
	}

}
void ControlParameters::popWriteContainer(){
	writeContainer.pop_front();
}
void ControlParameters::parseDataSet_14B(){
	string logLine = "";
	Writer * logWriter = Writer::Instance();
	char buff[512];
	Timer gent, t;
	
	if (getKTYPE(0) == 3){
		const char * del = " ";
	
		// NodeNumber
		pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[14];
		int size = dataSetPosition.second - dataSetPosition.first;
		char * map = InputFileParser::Instance()->mapViewOfINPFile;
		vector<string> lines;
		vector<char> lineBuffer;
		char* str_start = map + dataSetPosition.first;
		char * str_end;
		int i;
		int NROLD=1;
		NRTEST = 1;
		//Node * nNode;
		for (int j = dataSetPosition.first; j < dataSetPosition.second; j++){
			if (map[j] == '\r'){
				str_end = map + j - 1;
				lineBuffer.assign(str_start, str_end);
				str_start = map + j + 2;
				lineBuffer.push_back('\0');

			

				i = stoi(strtok(lineBuffer.data(), del));
				nodeRegion[i] = stoi(strtok(NULL, del));
				nodeX[i] = stod(strtok(NULL, del))*SCALX;
				nodeY[i] = stod(strtok(NULL, del))*SCALY;
				nodeZ[i] = stod(strtok(NULL, del))*SCALZ;
				nodePorosity[i] = stod(strtok(NULL, del))*PORFAC;
				nodeSOP[i] = (1 - nodePorosity[i])*COMPMA + nodePorosity[i] * COMPFL;

				if (i>1 && nodeRegion[i] != NROLD){
					NROLD = nodeRegion[i];
					NRTEST++;
				}
				/*nNode = nodeContainer[i];

				nNode->setNREG(stoi(strtok(NULL, del)));

				nNode->setXCoord(stod(strtok(NULL, del))*SCALX);

				nNode->setYCoord(stod(strtok(NULL, del))*SCALY);

				nNode->setZCoord(stod(strtok(NULL, del))*SCALZ);

				nNode->setPorosity(stod(strtok(NULL, del))*PORFAC);

				nNode->setSOP(1 - nNode->getPorosity()*COMPMA + nNode->getPorosity() * COMPFL);*/

	
			}

			//cout << t << endl;
			////boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
			//NodeNum = stoi(strtok(mat.data(), del));
			//NREG = stoi(strtok(NULL, del));
			//XCoord = stod(strtok(NULL, del));
			//YCoord = stod(strtok(NULL, del));
			//ZCoord = stod(strtok(NULL, del));
			//Porosity = stod(strtok(NULL, del));
			//XCoord = XCoord * SCALXx;
			//YCoord = YCoord * SCALYx;
			//ZCoord = ZCoord * SCALZx;
			//Porosity = Porosity * PORFACx;
			//SOP = (1.0 - Porosity)*COMPMAx + Porosity*COMPFLx;
			//aNode->setParameters(NREG, XCoord, YCoord, ZCoord, Porosity, SOP);

			//IF(I.GT.1.AND.NREG(II).NE.NROLD) NRTEST = NRTEST + 1                   INDAT1.......82000
			//NROLD = NREG(II)                                                     INDAT1.......82100
			//nodeContainer.push_back(aNode);
		}
		if (KNODAL == 0){
			logLine.append("\n\n\n\n           N O D E   I N F O R M A T I O N\n\n");
			logLine.append("                PRINTOUT OF NODE COORDINATES AND POROSITIES CANCELLED\n");
			logLine.append("\n                SCALE FACTORS:\n");
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     X-SCALE\n", SCALX);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     Y-SCALE\n", SCALY);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     Z-SCALE\n", SCALZ);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     POROSITY FACTOR\n", PORFAC);
			logLine.append(buff);
			
		}

		if (IUNSAT == 1 && KNODAL == 0 && NRTEST != 1){

			logLine.append(string(33, ' ') + "MORE THAN ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE NODES\n");
		}

		if (IUNSAT == 1 && KNODAL == 0 && NRTEST == 1){

			logLine.append(string(33, ' ') + "ONLY ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE NODES\n");
		}

		if (KNODAL == 1 && IUNSAT != 1){
			logLine.append("\n\n\n\n           N O D E   I N F O R M A T I O N\n\n");
			logLine.append("           NODE       X                Y                Z               POROSITY\n\n");
			for (int i = 1; i < sizeof(nodeX); i++){
				_snprintf(buff, sizeof(buff), "         %9d   %+14.5e   %+14.5e   %14.5e      %9.5f\n", i,nodeX[i],nodeY[i],nodeZ[i],nodePorosity[i]);
				logLine.append(buff);
			}

		}
		if (KNODAL == 1 && IUNSAT == 1){
			logLine.append("\n\n\n\n           N O D E   I N F O R M A T I O N\n\n");
			logLine.append("           NODE   REGION       X                Y                Z               POROSITY\n\n");
			for (int i = 1; i < sizeof(nodeX); i++){
				_snprintf(buff, sizeof(buff), "         %9d   %6d   %+14.5e   %+14.5e   %14.5e      %9.5f\n", i, nodeRegion[i],nodeX[i], nodeY[i], nodeZ[i], nodePorosity[i]);
				logLine.append(buff);
			}

		}
		logWriter->writeContainer.push_back(logLine);
		logLine.clear();
		// Check unsaturated


	}
	if (KTYPE[0] == 2){

		int NROLD = 0;
		NRTEST = 1;

		const char * del = " ";

		// NodeNumber
		pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[14];
		int size = dataSetPosition.second - dataSetPosition.first;
		char * map = InputFileParser::Instance()->mapViewOfINPFile;
		vector<string> lines;
		vector<char> lineBuffer;
		char* str_start = map + dataSetPosition.first;
		char * str_end;
		int i;
		//Node * nNode;
		for (int j = dataSetPosition.first; j < dataSetPosition.second; j++){
			if (map[j] == '\r'){
				str_end = map + j - 1;
				lineBuffer.assign(str_start, str_end);
				str_start = map + j + 2;
				lineBuffer.push_back('\0');



				i = stoi(strtok(lineBuffer.data(), del));
				nodeRegion[i] = stoi(strtok(NULL, del));
				nodeX[i] = stod(strtok(NULL, del))*SCALX;
				nodeY[i] = stod(strtok(NULL, del))*SCALY;
				nodeZ[i] = stod(strtok(NULL, del))*SCALZ;
				nodePorosity[i] = stod(strtok(NULL, del))*PORFAC;
				nodeSOP[i] = (1 - nodePorosity[i])*COMPMA + nodePorosity[i] * COMPFL;
				if (i>1 && nodeRegion[i]!=NROLD){
					NROLD = nodeRegion[i];
					NRTEST++;
				}

			}
		}

		if (getKNODAL() == 0){
			logLine.append("\n\n\n\n           N O D E   I N F O R M A T I O N\n\n");
			logLine.append("                PRINTOUT OF NODE COORDINATES AND POROSITIES CANCELLED\n");
			logLine.append("\n                SCALE FACTORS:\n");
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     X-SCALE\n", SCALX);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     Y-SCALE\n", SCALY);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     THICKNESS FACTOR\n", SCALZ);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     POROSITY FACTOR\n", PORFAC);
			logLine.append(buff);
		}

		if (IUNSAT == 1 && KNODAL == 0 && NRTEST != 1){

			logLine.append(string(33, ' ') + "MORE THAN ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE NODES\n");
		}

		if (IUNSAT == 1 && KNODAL == 0 && NRTEST == 1){

			logLine.append(string(33, ' ') + "ONLY ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE NODES\n");
		}

		if (KNODAL == 1 && IUNSAT != 1){
			logLine.append("\n\n\n\n           N O D E   I N F O R M A T I O N\n\n");
			logLine.append("           NODE       X                Y                 THICKNESS               POROSITY\n\n");
			for (int i = 1; i < sizeof(nodeX); i++){
				_snprintf(buff, sizeof(buff), "         %9d   %+14.5e   %+14.5e   %14.5e      %9.5f\n", i, nodeX[i], nodeY[i], nodeZ[i], nodePorosity[i]);
				logLine.append(buff);
			}

		}
		if (KNODAL == 1 && IUNSAT == 1){
			logLine.append("\n\n\n\n           N O D E   I N F O R M A T I O N\n\n");
			logLine.append("           NODE   REGION       X                Y                 THICKNESS               POROSITY\n\n");
			for (int i = 1; i < sizeof(nodeX); i++){
				_snprintf(buff, sizeof(buff), "         %9d   %6d   %+14.5e   %+14.5e   %14.5e      %9.5f\n", i, nodeRegion[i], nodeX[i], nodeY[i], nodeZ[i], nodePorosity[i]);
				logLine.append(buff);
			}

		}
		logWriter->writeContainer.push_back(logLine);
		logLine.clear();
		
	}

}
//
//void ControlParameters::parseDataSet_14B(){
//	DataSet * dataSet = dataSetMap["DataSet_14B"];
//	vector<string> * Data = dataSet->getData();
//	string dataLine;
//	Timer gent, t;
//	vector<char>lineBuffer;
//	string::iterator begin;
//	string::iterator end;
//	Node * nNode;
//	if (getKTYPE(0) == 3){
//		const char * del = " ";
//
//		// NodeNumber
//		int i;
//		for (vector<string>::iterator it = Data->begin(); it != Data->end(); ++it){
//			//cout << *it << endl;
//			/*begin = (*it).begin();
//			end = (*it).end();*/
//			//copy(begin,end,lineBuffer);
//			lineBuffer.assign((*it).begin(), (*it).end());
//			lineBuffer.push_back('\0');
//			i = stoi(strtok(lineBuffer.data(), del));
//			nNode = nodeContainer[i];
//
//			nNode->setNREG(stoi(strtok(NULL, del)));
//
//			nNode->setXCoord(stod(strtok(NULL, del))*SCALX);
//
//			nNode->setYCoord(stod(strtok(NULL, del))*SCALY);
//
//			nNode->setZCoord(stod(strtok(NULL, del))*SCALZ);
//
//			nNode->setPorosity(stod(strtok(NULL, del))*PORFAC);
//
//			nNode->setSOP(1 - nNode->getPorosity()*COMPMA + nNode->getPorosity() * COMPFL);
//
//			//cout << t << endl;
//			////boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
//			//NodeNum = stoi(strtok(mat.data(), del));
//			//NREG = stoi(strtok(NULL, del));
//			//XCoord = stod(strtok(NULL, del));
//			//YCoord = stod(strtok(NULL, del));
//			//ZCoord = stod(strtok(NULL, del));
//			//Porosity = stod(strtok(NULL, del));
//			//XCoord = XCoord * SCALXx;
//			//YCoord = YCoord * SCALYx;
//			//ZCoord = ZCoord * SCALZx;
//			//Porosity = Porosity * PORFACx;
//			//SOP = (1.0 - Porosity)*COMPMAx + Porosity*COMPFLx;
//			//aNode->setParameters(NREG, XCoord, YCoord, ZCoord, Porosity, SOP);
//
//			//IF(I.GT.1.AND.NREG(II).NE.NROLD) NRTEST = NRTEST + 1                   INDAT1.......82000
//			//NROLD = NREG(II)                                                     INDAT1.......82100
//			//nodeContainer.push_back(aNode);
//		}
//		if (KNODAL == 0){
//			cout << "\n\n\n\n           N O D E   I N F O R M A T I O N" << endl;
//			cout << "\n                PRINTOUT OF NODE COORDINATES AND POROSITIES CANCELLED" << endl;
//			cout << "\n                SCALE FACTORS:" << endl;
//			cout << "\n                                 " << SCALX << "     X-SCALE" << endl;
//			cout << "\n                                 " << SCALY << "     Y-SCALE" << endl;
//			cout << "\n                                 " << SCALZ << "     Z-SCALE" << endl;
//			cout << "\n                                 " << PORFAC << "     POROSITY FACTOR" << endl;
//		}
//		// Check unsaturated
//
//		// INPUT DATASETS 15A & 15B
//
//
//		cout << "Extracting Nodes took: " << gent << " seconds" << endl;
//	}
//	if (KTYPE[0] == 2){
//
//		int NROLD = 0;
//		int NRTEST = 1;
//
//		const char * del = " ";
//		vector<char>lineBuffer;
//
//		int i; // NodeNumber
//		for (int i = 1; i <= NN + 1; i++){
//			/*dataLine.assign(Data[i - 1].begin(), Data[i - 1].end());
//			t.reset();
//			vector<char>lineBuffer(dataLine.begin(), dataLine.end());
//			lineBuffer.push_back('\0');*/
//
//			Node * nNode = nodeContainer[i];
//			i = stoi(strtok(lineBuffer.data(), del));
//			nNode->setNREG(stoi(strtok(NULL, del)));
//			nNode->setXCoord(stod(strtok(NULL, del))*getSCALX());
//			nNode->setYCoord(stod(strtok(NULL, del))*getSCALY());
//			nNode->setZCoord(0.0);
//			nNode->setPorosity(stod(strtok(NULL, del))*getPORFAC());
//			nNode->setSOP(1 - nNode->getPorosity()*getCOMPMA() + nNode->getPorosity() * getCOMPFL());
//
//
//			/*if (NREG != NROLD){
//			NROLD = NREG;
//			NRTEST++;
//			}
//			*/
//			//IF(I.GT.1.AND.NREG(II).NE.NROLD) NRTEST = NRTEST + 1                   INDAT1.......82000
//			//NROLD = NREG(II)                                                     INDAT1.......82100
//
//		}
//
//		if (getKNODAL() == 0){
//			cout << "\n\n\n\n           N O D E   I N F O R M A T I O N" << endl;
//			cout << "\n                PRINTOUT OF NODE COORDINATES AND POROSITIES CANCELLED" << endl;
//			cout << "\n                SCALE FACTORS:" << endl;
//			cout << "\n                                 " << getSCALX() << "     X-SCALE" << endl;
//			cout << "\n                                 " << getSCALY() << "     Y-SCALE" << endl;
//			cout << "\n                                 " << getSCALZ() << "     Z-SCALE" << endl;
//			cout << "\n                                 " << getPORFAC() << "     POROSITY FACTOR" << endl;
//		}
//
//		// Check Unsaturated
//
//		cout << "Extracting Nodes took: " << t << " seconds" << endl;
//	}
//
//}
void ControlParameters::parseDataSet_15B(){
	string logLine = "";
	Writer * logWriter = Writer::Instance();
	char buff[512];
	int i;
	DataSet * dataSet = dataSetMap["DataSet_15B"];
	vector<string> * Data = dataSet->getData();
	string dataLine;
	Timer gent, t;
	vector<char>lineBuffer;
	string::iterator begin;
	string::iterator end;
	//Element * anElement;
	const char * del = " ";
	LRTEST = 1;
	if (KTYPE[0] == 3){
		int LROLD = 1;
	
		pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[16];
		int size = dataSetPosition.second - dataSetPosition.first;
		char * map = InputFileParser::Instance()->mapViewOfINPFile;
		vector<string> lines;
		vector<char> lineBuffer;
		char* str_start = map + dataSetPosition.first;
		char * str_end;
		int i;
		for (int j = dataSetPosition.first; j < dataSetPosition.second; j++){
			if (map[j] == '\r'){
				str_end = map + j - 1;
				lineBuffer.assign(str_start, str_end);
				str_start = map + j + 2;
				lineBuffer.push_back('\0');
				i = stoi(strtok(lineBuffer.data(), del));
				elementRegion[i] = stoi(strtok(NULL, del));
				elPMAX[i] = stod(strtok(NULL, del))*PMAXFA;
				elPMID[i] = stod(strtok(NULL, del))*PMIDFA;
				elPMIN[i] = stod(strtok(NULL, del))*PMINFA;
				elANGLE1[i] = stod(strtok(NULL, del))*ANG1FA;
				elANGLE2[i] = stod(strtok(NULL, del))*ANG2FA;
				elANGLE3[i] = stod(strtok(NULL, del))*ANG3FA;
				elALMAX[i] = stod(strtok(NULL, del))*ALMAXF;
				elALMID[i] = stod(strtok(NULL, del))*ALMIDF;
				elALMIN[i] = stod(strtok(NULL, del))*ALMINF;
				elATMAX[i] = stod(strtok(NULL, del))*ATMXF;
				elATMID[i] = stod(strtok(NULL, del))*ATMDF;
				elATMIN[i] = stod(strtok(NULL, del))*ATMNF;

				

				if (i > 1 && elementRegion[i] != LROLD){
					LRTEST = LRTEST + 1;
					LROLD = elementRegion[i];
				}


				if (KELMNT == 1 && IUNSAT != 1){

					_snprintf(buff, sizeof(buff), "         %9d  %+14.5e  %+14.5e  %+14.5e       %+11.4e    %+11.4e    %+11.4e    %+11.4e    %+11.4e    %+11.4e    %+11.4e    %+11.4e    %+11.4e\n", i, elPMAX[i], elPMID[i], elPMIN[i], elANGLE1[i], elANGLE2[i], elANGLE3[i], elALMAX[i], elALMID[i], elALMIN[i], elATMAX[i], elATMID[i], elATMIN[i]);
					logLine.append(buff);

				}

				if (KELMNT == 1 && IUNSAT == 1){

					_snprintf(buff, sizeof(buff), "         %9d    %5d  %+14.5e  %+14.5e  %+14.5e       %+11.4e    %+11.4e    %+11.4e    %+11.4e    %+11.4e    %+11.4e    %+11.4e    %+11.4e    %+11.4e\n", i, elementRegion[i],elPMAX[i], elPMID[i], elPMIN[i], elANGLE1[i], elANGLE2[i], elANGLE3[i], elALMAX[i], elALMID[i], elALMIN[i], elATMAX[i], elATMID[i], elATMIN[i]);
					logLine.append(buff);

				}


			}






		}

		if (KELMNT == 0){
			logLine.append("\n\n\n\n E L E M E N T   I N F O R M A T I O N\n\n");
			logLine.append("                PRINTOUT OF ELEEMENT PERMEABILITIES AND DISPERSIVITIES CANCELLED.\n\n");
			logLine.append("                SCALE FACTORS:\n");
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     MAXIMUM PERMEABILITY FACTOR\n", PMAXFA);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     MIDDLE PERMEABILITY FACTOR\n", PMIDFA);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     MINIMUM PERMEABILITY FACTOR\n", PMINFA);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     ANGLE1 FACTOR\n", ANG1FA);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     ANGLE2 FACTOR\n", ANG2FA);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     ANGLE3 FACTOR\n", ANG3FA);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MAX-PERM DIRECTION\n", ALMAXF);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MID-PERM DIRECTION\n", ALMIDF);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MIN-PERM DIRECTION\n", ALMINF);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     FACTOR FOR TRANSVERSE DISPERSIVITY IN MAX-PERM DIRECTION\n", ATMXF);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     FACTOR FOR TRANSVERSE DISPERSIVITY IN MID-PERM DIRECTION\n", ATMDF);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     FACTOR FOR TRANSVERSE DISPERSIVITY IN MIN-PERM DIRECTION\n", ATMNF);
			logLine.append(buff);
		}
		if (IUNSAT == 1 && KELMNT == 0 && LRTEST != 1)
			logLine.append("MORE THAN ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE ELEMENTS.\n");
		if (IUNSAT == 1 && KELMNT == 0 && LRTEST == 0)
			logLine.append("ONLY ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE ELEMENTS.\n");

		
		logWriter->writeContainer.push_back(logLine);
		logLine.clear();


	}
	if (KTYPE[0] == 2){
		int LRTEST = 1;
		int LROLD = 0;
		pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[16];
		int size = dataSetPosition.second - dataSetPosition.first;
		char * map = InputFileParser::Instance()->mapViewOfINPFile;
		vector<string> lines;
		vector<char> lineBuffer;
		char* str_start = map + dataSetPosition.first;
		char * str_end;
		int i;
		for (int j = dataSetPosition.first; j < dataSetPosition.second; j++){
			if (map[j] == '\r'){
				str_end = map + j - 1;
				lineBuffer.assign(str_start, str_end);
				str_start = map + j + 2;
				lineBuffer.push_back('\0');
				i = stoi(strtok(lineBuffer.data(), del));
				elementRegion[i] = stoi(strtok(NULL, del));
				elPMAX[i] = stod(strtok(NULL, del))*PMAXFA;
				elPMIN[i] = stod(strtok(NULL, del))*PMINFA;
				elANGLEX[i] = stod(strtok(NULL, del))*ANGFAC;
				elALMAX[i] = stod(strtok(NULL, del))*ALMAXF;
				elALMIN[i] = stod(strtok(NULL, del))*ALMINF;
				elATMAX[i] = stod(strtok(NULL, del))*ATMXF;
				elATMIN[i] = stod(strtok(NULL, del))*ATMNF;
				if (i > 1 && elementRegion[i] != LROLD){
					LRTEST = LRTEST + 1;
					LROLD = elementRegion[i];
				}


				if (KELMNT == 1 && IUNSAT != 1){
					
						_snprintf(buff, sizeof(buff), "         %9d  %+14.5e  %+14.5e         %+11.4e    %+11.4e    %+11.4e    %+11.4e    %+11.4e    \n", i, elPMAX[i], elPMIN[i], elANGLEX[i], elALMAX[i], elALMIN[i], elATMAX[i], elATMIN[i]);
						logLine.append(buff);
					
				}

				if (KELMNT == 1 && IUNSAT == 1){
					
						_snprintf(buff, sizeof(buff), "         %9d    %5d  %+14.5e  %+14.5e         %+11.4e    %+11.4e    %+11.4e    %+11.4e    %+11.4e    \n", i, elementRegion[i], elPMAX[i], elPMIN[i], elANGLEX[i], elALMAX[i], elALMIN[i], elATMAX[i], elATMIN[i]);
						logLine.append(buff);
					
				}

			}

		}

		if (KELMNT == 0){
			logLine.append("\n\n\n\n E L E M E N T   I N F O R M A T I O N\n\n");
			logLine.append("                PRINTOUT OF ELEEMENT PERMEABILITIES AND DISPERSIVITIES CANCELLED.\n\n");
			logLine.append("                SCALE FACTORS:\n");
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     MAXIMUM PERMEABILITY FACTOR\n", PMAXFA);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     MINIMUM PERMEABILITY FACTOR\n", PMINFA);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     ANGLE FROM +X TO MAXIMUM DIRECTION FACTOR\n", ANGFAC);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MAX-PERM DIRECTION\n", ALMAXF);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MIN-PERM DIRECTION\n", ALMINF);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     FACTOR FOR TRANSVERSE DISPERSIVITY IN MAX-PERM DIRECTION\n", ATMXF);
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "                                 %+15.4e     FACTOR FOR TRANSVERSE DISPERSIVITY IN MIN-PERM DIRECTION\n", ATMNF);
			logLine.append(buff);
		}
		if (IUNSAT == 1 && KELMNT == 0 && LRTEST != 1)
			logLine.append("MORE THAN ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE ELEMENTS.\n");
		if (IUNSAT == 1 && KELMNT == 0 && LRTEST == 0)
			logLine.append("ONLY ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE ELEMENTS.\n");

		

		logWriter->writeContainer.push_back(logLine);
		logLine.clear();


	}
}
//void ControlParameters::parseDataSet_15B(){
//	cout << " Parsing (*Data) Set 15B" << endl;
//	int i;
//	DataSet * dataSet = dataSetMap["DataSet_15B"];
//	vector<string> * Data = dataSet->getData();
//	string dataLine;
//	Timer gent, t;
//	vector<char>lineBuffer;
//	string::iterator begin;
//	string::iterator end;
//	Element * anElement;
//	const char * del = " ";
//	if (KTYPE[0] == 3){
//
//		int LRTEST = 1;
//		clock_t t;
//		t = clock();
//		cout << NE << " Elements in Data Set " << endl;
//		for (vector<string>::iterator it = Data->begin(); it != Data->end(); ++it){
//			begin = (*it).begin();
//			end = (*it).end();
//			//copy(begin,end,lineBuffer);
//			lineBuffer.assign(begin, end);
//			lineBuffer.push_back('\0');
//			i = stoi(strtok(lineBuffer.data(), del));
//			anElement = elementContainer[i];
//			anElement->setLREG(stoi(strtok(NULL, del)));
//			anElement->setPMAX(stod(strtok(NULL, del))*PMAXFA);
//			anElement->setPMID(stod(strtok(NULL, del))*PMIDFA);
//			anElement->setPMIN(stod(strtok(NULL, del))*PMINFA);
//			anElement->setANGLE1(stod(strtok(NULL, del))*ANG1FA);
//			anElement->setANGLE2(stod(strtok(NULL, del))*ANG2FA);
//			anElement->setANGLE3(stod(strtok(NULL, del))*ANG3FA);
//			anElement->setALMAX(stod(strtok(NULL, del))*ALMAXF);
//			anElement->setALMID(stod(strtok(NULL, del))*ALMIDF);
//			anElement->setALMIN(stod(strtok(NULL, del))*ALMINF);
//			anElement->setATMAX(stod(strtok(NULL, del))*ATMXF);
//			anElement->setATMID(stod(strtok(NULL, del))*ATMDF);
//			anElement->setATMIN(stod(strtok(NULL, del))*ATMNF);
//			anElement->setPANGS();
//			anElement->ROTMAT3D();
//			anElement->TENSYM3D();
//
//
//
//
//
//			//IF(I.GT.1.AND.NREG(II).NE.NROLD) NRTEST = NRTEST + 1                   INDAT1.......82000
//			//NROLD = NREG(II)                                                     INDAT1.......82100
//
//
//		}
//
//		if (KELMNT == 0){
//			cout << "\n\n\n\n E L E M E N T   I N F O R M A T I O N" << endl;
//			cout << "                PRINTOUT OF ELEEMENT PERMEABILITIES AND DISPERSIVITIES CANCELLED." << endl;
//			cout << "                SCALE FACTORS:" << endl;
//			cout << "                                 " << PMAXFA << "     MAXIMUM PERMEABILITY FACTOR" << endl;
//			cout << "                                 " << PMIDFA << "     MIDDLE PERMEABILITY FACTOR" << endl;
//			cout << "                                 " << PMINFA << "     MINIMUM PERMEABILITY FACTOR" << endl;
//			cout << "                                 " << ANG1FA << "     ANGLE1 FACTOR" << endl;
//			cout << "                                 " << ANG2FA << "     ANGLE2 FACTOR" << endl;
//			cout << "                                 " << ANG3FA << "     ANGLE3 FACTOR" << endl;
//			cout << "                                 " << ALMAXF << "     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MAX-PERM DIRECTION" << endl;
//			cout << "                                 " << ALMIDF << "     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MID-PERM DIRECTION" << endl;
//			cout << "                                 " << ALMINF << "     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MIN-PERM DIRECTION" << endl;
//			cout << "                                 " << ATMXF << "     FACTOR FOR TRANSVERSE DISPERSIVITY IN MAX-PERM DIRECTION" << endl;
//			cout << "                                 " << ATMDF << "     FACTOR FOR TRANSVERSE DISPERSIVITY IN MID-PERM DIRECTION" << endl;
//			cout << "                                 " << ATMNF << "     FACTOR FOR TRANSVERSE DISPERSIVITY IN MIN-PERM DIRECTION" << endl;
//
//		}
//		if (IUNSAT == 1 && KELMNT == 0 && LRTEST != 1)
//			cout << "MORE THAN ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE ELEMENTS." << endl;
//		if (IUNSAT == 1 && KELMNT == 0 && LRTEST == 0)
//			cout << "ONLY ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE ELEMENTS." << endl;
//
//		t = clock() - t;
//		cout << "Extracting Elements took: " << ((float)t) / CLOCKS_PER_SEC << " seconds" << endl;
//	}
//	if (KTYPE[0] == 2){
//		int LRTEST = 1;
//		clock_t t;
//		t = clock();
//		int LROLD = 0;
//		for (vector<string>::iterator it = Data->begin(); it != Data->end(); ++it){
//			begin = (*it).begin();
//			end = (*it).end();
//			//copy(begin,end,lineBuffer);
//			lineBuffer.assign(begin, end);
//			lineBuffer.push_back('\0');
//			i = stoi(strtok(lineBuffer.data(), del));
//			anElement = elementContainer[i];
//			anElement->setLREG(stoi(strtok(NULL, del)));
//			anElement->setPMAX(stod(strtok(NULL, del))*PMAXFA);
//
//			anElement->setPMIN(stod(strtok(NULL, del))*PMINFA);
//
//
//			anElement->setANGLEX(stod(strtok(NULL, del))*ANGFAC);
//			anElement->setALMAX(stod(strtok(NULL, del))*ALMAXF);
//
//			anElement->setALMIN(stod(strtok(NULL, del))*ALMINF);
//			anElement->setATMAX(stod(strtok(NULL, del))*ATMXF);
//
//			anElement->setATMIN(stod(strtok(NULL, del))*ATMNF);
//
//			anElement->setPERMS2D();
//			/*EleNum = stoi(dataPos.at(0));
//			LREG = stoi(dataPos.at(1));
//			PMAX = stod(dataPos.at(2));
//			PMIN = stod(dataPos.at(4));
//			ANGLEX = stod(dataPos.at(4));
//			ALMAX = stod(dataPos.at(8));
//			ALMIN = stod(dataPos.at(10));
//			ATMAX = stod(dataPos.at(11));
//			ATMIN = stod(dataPos.at(13));
//
//			PMAX = PMAX*PMAXFA;
//			PMIN = PMIN*PMINFA;
//			ALMAX = ALMAX*ALMAXF;
//			ALMIN = ALMIN*ALMINF;
//			ATMAX = ATMAX*ATMXF;
//			ATMIN = ATMIN*ATMNF;
//			ANGLEX = ANGLEX*ANGFAC;*/
//
//			/*	if (LREG != LROLD){
//			LRTEST++;
//			LROLD = LREG;
//			}*/
//
//
//
//
//			//IF(I.GT.1.AND.NREG(II).NE.NROLD) NRTEST = NRTEST + 1                   INDAT1.......82000
//			//NROLD = NREG(II)                                                     INDAT1.......82100
//
//
//		}
//
//		if (KELMNT == 0){
//			cout << "\n\n\n\n E L E M E N T   I N F O R M A T I O N" << endl;
//			cout << "                PRINTOUT OF ELEEMENT PERMEABILITIES AND DISPERSIVITIES CANCELLED." << endl;
//			cout << "                SCALE FACTORS:" << endl;
//			cout << "                                 " << PMAXFA << "     MAXIMUM PERMEABILITY FACTOR" << endl;
//			cout << "                                 " << PMINFA << "     MINIMUM PERMEABILITY FACTOR" << endl;
//			cout << "                                 " << ANGFAC << "     ANGLE FROM +X TO MAXIMUM DIRECTION FACTOR" << endl;
//			cout << "                                 " << ALMAXF << "     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MAX-PERM DIRECTION" << endl;
//			cout << "                                 " << ALMINF << "     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MIN-PERM DIRECTION" << endl;
//			cout << "                                 " << ATMXF << "     FACTOR FOR TRANSVERSE DISPERSIVITY IN MAX-PERM DIRECTION" << endl;
//			cout << "                                 " << ATMNF << "     FACTOR FOR TRANSVERSE DISPERSIVITY IN MIN-PERM DIRECTION" << endl;
//
//		}
//		if (IUNSAT == 1 && KELMNT == 0 && LRTEST != 1)
//			cout << "MORE THAN ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE ELEMENTS." << endl;
//		if (IUNSAT == 1 && KELMNT == 0 && LRTEST == 0)
//			cout << "ONLY ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE ELEMENTS." << endl;
//
//		t = clock() - t;
//		cout << "Extracting Elements took: " << ((float)t) / CLOCKS_PER_SEC << " seconds" << endl;
//
//	}
//}
void ControlParameters::findObservationElementThread(){
	int size = observationPointsContainer.size();
	int div = size / 8;
	int rem = size - 7 * div;
	thread t[8];
	int begin = 0;
	int end;
	for (int i = 0; i < 8; i++){
		if (i == 7){
			end = i*div + rem-1;
			t[i] = thread(&ControlParameters::findObservationElement, this, begin, end, i + 1);

		}
		else{
			end = (i + 1)*div;			
			t[i] = thread(&ControlParameters::findObservationElement, this, begin, end, i + 1);
			begin = end + 1;

		}
	}

	for (int i = 0; i < 8; i++){
		t[i].join();
	}
	/*int size = elementContainer.size() - 1;

	int div = size / 8;
	int rem = size - 7 * div;
	
	thread t[8];


		for (int i = 0; i < 8; i++){
			if (i == 7){
				int begin = (i*div) + 1;
				int end = i*div + rem;
				t[i] = thread(&ControlParameters::findObservationElement, this, begin, end, i + 1);
	
			}
			else{
				int begin = i * div + 1;
				int end = (i + 1)*div;
				t[i] = thread(&ControlParameters::findObservationElement, this, begin, end, i + 1);

			}
		}

		for (int i = 0; i < 8; i++){
			t[i].join();
		}
		*/
	
	//findObservationElement(0, 0, 0);


	/*std::thread t1(&ControlParameters::findObservationElement,this, 1, div);
	std::thread t2(&ControlParameters::findObservationElement,this, div + 1, 2 * div);
	std::thread t3(&ControlParameters::findObservationElement,this, 2 * div + 1, 3 * div);
	std::thread t4(&ControlParameters::findObservationElement,this, 3 * div + 1, rem);
	t1.join();
	t2.join();
	t3.join();
	t4.join();*/

}

void ControlParameters::findObservationElement(int b, int e, int tid){
	
	if (N48 == 8){
		bool found;
		for (vector<ObservationPoints *>::iterator itx = observationPointsContainer.begin()+b; itx != observationPointsContainer.begin()+e; ++itx){
		found = false;
			for (int i = 1; i <= NE; i++){
				found = FINDL3ver(i, elementNodes[i], (*itx));
				if (found){
					cout << "Obs Pt. " << (*itx)->getOBSNAM() << " is found in " << (*itx)->getObsElement() << " by thread " << tid << endl;
					break;
				}
			}
			if (!found){
				cout << " Could not find obs point" << endl;
			}
		}
			/*for (vector<Element *>::iterator it = elementContainer.begin() + b; it != elementContainer.begin() + e; ++it){
				if ((*itx)->getObsElement() != -1)
					break;

				if (get_count() == NOBS)
					break;

				found = FINDL3((*it)->getElementNumber(), (*it)->getCornersX(), (*it)->getCornersY(), (*it)->getCornersZ(), (*itx));

				if (found){
					cout << "Observation Point " << (*itx)->getOBSNAM() << " is found in element " << (*itx)->getObsElement() << " by " << tid << endl;
					increment_count();
					break;
				}

			}
			
			if (get_count() == NOBS)
			break;

				
		}
	
		cout << " Thread " << tid << " finished." << endl; */
	}
	else if (N48 == 4){
		
	}
	else{
		cout << "Error while finding Observation Point Elements " << endl;
		exitOnError("");
	}
	cout << " Thread " << tid << " finished." << endl;
}

bool ControlParameters::FINDL3(int ElNo, vector<double>X, vector<double>Y, vector<double>Z,ObservationPoints * obs){//ObservationPoints * obs
	double EPSILON = 0.001;
	double TOL = 0.001;
	double ITRMAX = 25;
	double OPE = 1 + EPSILON;
	double AX, BX, CX, DX, EX, FX, GX, HX, AY, BY, CY, DY, EY, FY, GY, HY, AZ, BZ, CZ, DZ, EZ, FZ, GZ, HZ;
	double F10, F20, F30, FP11, FP12, FP13, FP21, FP22, FP23, FP31, FP32, FP33;
	double S11, S12, S13, CF12, CF34, CF43, CF56, DETXSI, DETETA, DETZET, DETERM, DELXSI, DELETA, DELZET;
	double tXSI, tETA, tZET;
	tXSI = tETA = tZET = 0.0;
	bool found = false;
	// Calculate Coefficients
	AX = +X[0] + X[1] + X[2] + X[3] + X[4] + X[5] + X[6] + X[7];
	BX = -X[0] + X[1] + X[2] - X[3] - X[4] + X[5] + X[6] - X[7];//........5000
	CX = -X[0] - X[1] + X[2] + X[3] - X[4] - X[5] + X[6] + X[7];//........5100
	DX = -X[0] - X[1] - X[2] - X[3] + X[4] + X[5] + X[6] + X[7];//........5200
	EX = +X[0] - X[1] + X[2] - X[3] + X[4] - X[5] + X[6] - X[7];//........5300
	FX = +X[0] - X[1] - X[2] + X[3] - X[4] + X[5] + X[6] - X[7];//........5400
	GX = +X[0] + X[1] - X[2] - X[3] - X[4] - X[5] + X[6] + X[7];//........5500
	HX = -X[0] + X[1] - X[2] + X[3] + X[4] - X[5] + X[6] - X[7];//........5600
	AY = +Y[0] + Y[1] + Y[2] + Y[3] + Y[4] + Y[5] + Y[6] + Y[7];//........5700
	BY = -Y[0] + Y[1] + Y[2] - Y[3] - Y[4] + Y[5] + Y[6] - Y[7];//........5800
	CY = -Y[0] - Y[1] + Y[2] + Y[3] - Y[4] - Y[5] + Y[6] + Y[7];//........5900
	DY = -Y[0] - Y[1] - Y[2] - Y[3] + Y[4] + Y[5] + Y[6] + Y[7];//........6000
	EY = +Y[0] - Y[1] + Y[2] - Y[3] + Y[4] - Y[5] + Y[6] - Y[7];//........6100
	FY = +Y[0] - Y[1] - Y[2] + Y[3] - Y[4] + Y[5] + Y[6] - Y[7];//........6200
	GY = +Y[0] + Y[1] - Y[2] - Y[3] - Y[4] - Y[5] + Y[6] + Y[7];//........6300
	HY = -Y[0] + Y[1] - Y[2] + Y[3] + Y[4] - Y[5] + Y[6] - Y[7];//........6400
	AZ = +Z[0] + Z[1] + Z[2] + Z[3] + Z[4] + Z[5] + Z[6] + Z[7];//........6500
	BZ = -Z[0] + Z[1] + Z[2] - Z[3] - Z[4] + Z[5] + Z[6] - Z[7];//........6600
	CZ = -Z[0] - Z[1] + Z[2] + Z[3] - Z[4] - Z[5] + Z[6] + Z[7];//........6700
	DZ = -Z[0] - Z[1] - Z[2] - Z[3] + Z[4] + Z[5] + Z[6] + Z[7];//........6800
	EZ = +Z[0] - Z[1] + Z[2] - Z[3] + Z[4] - Z[5] + Z[6] - Z[7];//........6900
	FZ = +Z[0] - Z[1] - Z[2] + Z[3] - Z[4] + Z[5] + Z[6] - Z[7];//........7000
	GZ = +Z[0] + Z[1] - Z[2] - Z[3] - Z[4] - Z[5] + Z[6] + Z[7];//........7100
	HZ = -Z[0] + Z[1] - Z[2] + Z[3] + Z[4] - Z[5] + Z[6] - Z[7];//........7200

	for (int i = 0; i < ITRMAX; i++){
		F10 = AX - 8.*obs->getXOBS() + BX*tXSI + CX*tETA + DX*tZET + EX*tXSI*tETA + FX*tXSI*tZET + GX*tETA*tZET + HX*tXSI*tETA*tZET;
		F20 = AY - 8.*obs->getYOBS() + BY*tXSI + CY*tETA + DY*tZET + EY*tXSI*tETA + FY*tXSI*tZET + GY*tETA*tZET + HY*tXSI*tETA*tZET;
		F30 = AZ - 8.*obs->getZOBS() + BZ*tXSI + CZ*tETA + DZ*tZET + EZ*tXSI*tETA + FZ*tXSI*tZET + GZ*tETA*tZET + HZ*tXSI*tETA*tZET;
		FP11 = BX + EX*tETA + FX*tZET + HX*tETA*tZET;
		FP12 = CX + EX*tXSI + GX*tZET + HX*tXSI*tZET;
		FP13 = DX + FX*tXSI + GX*tETA + HX*tXSI*tETA;
		FP21 = BY + EY*tETA + FY*tZET + HY*tETA*tZET;
		FP22 = CY + EY*tXSI + GY*tZET + HY*tXSI*tZET;
		FP23 = DY + FY*tXSI + GY*tETA + HY*tXSI*tETA;
		FP31 = BZ + EZ*tETA + FZ*tZET + HZ*tETA*tZET;
		FP32 = CZ + EZ*tXSI + GZ*tZET + HZ*tXSI*tZET;
		FP33 = DZ + FZ*tXSI + GZ*tETA + HZ*tXSI*tETA;
		S11 = FP22*FP33 - FP32*FP23;	
		S12 = FP21*FP33 - FP31*FP23;
		S13 = FP21*FP32 - FP31*FP22;
		CF12 = -F20*FP33 + F30*FP23;
		CF34 = -F20*FP32 + F30*FP22;
		CF43 = -CF34;
		CF56 = -F30*FP21 + F20*FP31;
		DETERM = FP11*S11 - FP12*S12 + FP13*S13;
		DETXSI = -F10*S11 - FP12*CF12 + FP13*CF34;
		DELXSI = DETXSI / DETERM;
		if (abs(DELXSI) >= TOL){
			tXSI = tXSI + DELXSI;
			continue;
		}

		DETETA = FP11*CF12 + F10*S12 + FP13*CF56;
		DELETA = DETETA / DETERM;
		if (abs(DELETA) >= TOL){
			tETA = tETA + DELETA;
			continue;
		}
		DETZET = FP11*CF43 - FP12*CF56 - F10*S13;
		DELZET = DETZET / DETERM;

		if (abs(DELZET) >= TOL){
			tZET = tZET + DELZET;
			continue;
		}

		/*tXSI = tXSI + DELXSI;
		tETA = tETA + DELETA;
		tZET = tZET + DELZET;*/
		if ((abs(DELXSI) < TOL) && (abs(DELETA) < TOL) && (abs(DELZET) < TOL)){
			if ((abs(tXSI) <= OPE) && (abs(tETA) <= OPE) && (abs(tZET) <= OPE)){
				obs->setObsElement(ElNo);
				obs->setXSI(tXSI);
				obs->setETA(tETA);
				obs->setZET(tZET);
			
				found = true;
				break;
			}
		}
	}

	return found;
}

bool ControlParameters::FINDL3ver(int ElNo, int* elNodes, ObservationPoints * obs){
	double EPSILON = 0.001;
	double TOL = 0.001;
	double ITRMAX = 25;
	double OPE = 1 + EPSILON;
	double AX, BX, CX, DX, EX, FX, GX, HX, AY, BY, CY, DY, EY, FY, GY, HY, AZ, BZ, CZ, DZ, EZ, FZ, GZ, HZ;
	double F10, F20, F30, FP11, FP12, FP13, FP21, FP22, FP23, FP31, FP32, FP33;
	double S11, S12, S13, CF12, CF34, CF43, CF56, DETXSI, DETETA, DETZET, DETERM, DELXSI, DELETA, DELZET;
	double tXSI, tETA, tZET;
	tXSI = tETA = tZET = 0.0;
	bool found = false;
	// Calculate Coefficients
	double X1, X2, X3, X4, X5, X6, X7, X8;
	double Y1, Y2, Y3, Y4, Y5, Y6, Y7, Y8;
	double Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8;
	X1 = nodeX[elNodes[0]];
	X2 = nodeX[elNodes[1]];
	X3 = nodeX[elNodes[2]];
	X4 = nodeX[elNodes[3]];
	X5 = nodeX[elNodes[4]];
	X6 = nodeX[elNodes[5]];
	X7 = nodeX[elNodes[6]];
	X8 = nodeX[elNodes[7]];
	Y1 = nodeY[elNodes[0]];
	Y2 = nodeY[elNodes[1]];
	Y3 = nodeY[elNodes[2]];
	Y4 = nodeY[elNodes[3]];
	Y5 = nodeY[elNodes[4]];
	Y6 = nodeY[elNodes[5]];
	Y7 = nodeY[elNodes[6]];
	Y8 = nodeY[elNodes[7]];
	Z1 = nodeZ[elNodes[0]];
	Z2 = nodeZ[elNodes[1]];
	Z3 = nodeZ[elNodes[2]];
	Z4 = nodeZ[elNodes[3]];
	Z5 = nodeZ[elNodes[4]];
	Z6 = nodeZ[elNodes[5]];
	Z7 = nodeZ[elNodes[6]];
	Z8 = nodeZ[elNodes[7]];


	AX = +X1 + X2 + X3 + X4 + X5 + X6 + X7 + X8;
	BX = -X1 + X2 + X3 - X4 - X5 + X6 + X7 - X8;//........5000
	CX = -X1 - X2 + X3 + X4 - X5 - X6 + X7 + X8;//........5100
	DX = -X1 - X2 - X3 - X4 + X5 + X6 + X7 + X8;//........5200
	EX = +X1 - X2 + X3 - X4 + X5 - X6 + X7 - X8;//........5300
	FX = +X1 - X2 - X3 + X4 - X5 + X6 + X7 - X8;//........5400
	GX = +X1 + X2 - X3 - X4 - X5 - X6 + X7 + X8;//........5500
	HX = -X1 + X2 - X3 + X4 + X5 - X6 + X7 - X8;//........5600
	AY = +Y1 + Y2 + Y3 + Y4 + Y5 + Y6 + Y7 + Y8;//........5700
	BY = -Y1 + Y2 + Y3 - Y4 - Y5 + Y6 + Y7 - Y8;//........5800
	CY = -Y1 - Y2 + Y3 + Y4 - Y5 - Y6 + Y7 + Y8;//........5900
	DY = -Y1 - Y2 - Y3 - Y4 + Y5 + Y6 + Y7 + Y8;//........6000
	EY = +Y1 - Y2 + Y3 - Y4 + Y5 - Y6 + Y7 - Y8;//........6100
	FY = +Y1 - Y2 - Y3 + Y4 - Y5 + Y6 + Y7 - Y8;//........6200
	GY = +Y1 + Y2 - Y3 - Y4 - Y5 - Y6 + Y7 + Y8;//........6300
	HY = -Y1 + Y2 - Y3 + Y4 + Y5 - Y6 + Y7 - Y8;//........6400
	AZ = +Z1 + Z2 + Z3 + Z4 + Z5 + Z6 + Z7 + Z8;//........6500
	BZ = -Z1 + Z2 + Z3 - Z4 - Z5 + Z6 + Z7 - Z8;//........6600
	CZ = -Z1 - Z2 + Z3 + Z4 - Z5 - Z6 + Z7 + Z8;//........6700
	DZ = -Z1 - Z2 - Z3 - Z4 + Z5 + Z6 + Z7 + Z8;//........6800
	EZ = +Z1 - Z2 + Z3 - Z4 + Z5 - Z6 + Z7 - Z8;//........6900
	FZ = +Z1 - Z2 - Z3 + Z4 - Z5 + Z6 + Z7 - Z8;//........7000
	GZ = +Z1 + Z2 - Z3 - Z4 - Z5 - Z6 + Z7 + Z8;//........7100
	HZ = -Z1 + Z2 - Z3 + Z4 + Z5 - Z6 + Z7 - Z8;//........7200


	//AX = +nodeX[elNodes[0]] + nodeX[elNodes[1]] + nodeX[elNodes[2]] + nodeX[elNodes[3]] + nodeX[elNodes[4]] + nodeX[elNodes[5]] + nodeX[elNodes[6]] + nodeX[elNodes[7]];
	//BX = -nodeX[elNodes[0]] + nodeX[elNodes[1]] + nodeX[elNodes[2]] - nodeX[elNodes[3]] - nodeX[elNodes[4]] + nodeX[elNodes[5]] + nodeX[elNodes[6]] - nodeX[elNodes[7]];//........5000
	//CX = -nodeX[elNodes[0]] - nodeX[elNodes[1]] + nodeX[elNodes[2]] + nodeX[elNodes[3]] - nodeX[elNodes[4]] - nodeX[elNodes[5]] + nodeX[elNodes[6]] + nodeX[elNodes[7]];//........5100
	//DX = -nodeX[elNodes[0]] - nodeX[elNodes[1]] - nodeX[elNodes[2]] - nodeX[elNodes[3]] + nodeX[elNodes[4]] + nodeX[elNodes[5]] + nodeX[elNodes[6]] + nodeX[elNodes[7]];//........5200
	//EX = +nodeX[elNodes[0]] - nodeX[elNodes[1]] + nodeX[elNodes[2]] - nodeX[elNodes[3]] + nodeX[elNodes[4]] - nodeX[elNodes[5]] + nodeX[elNodes[6]] - nodeX[elNodes[7]];//........5300
	//FX = +nodeX[elNodes[0]] - nodeX[elNodes[1]] - nodeX[elNodes[2]] + nodeX[elNodes[3]] - nodeX[elNodes[4]] + nodeX[elNodes[5]] + nodeX[elNodes[6]] - nodeX[elNodes[7]];//........5400
	//GX = +nodeX[elNodes[0]] + nodeX[elNodes[1]] - nodeX[elNodes[2]] - nodeX[elNodes[3]] - nodeX[elNodes[4]] - nodeX[elNodes[5]] + nodeX[elNodes[6]] + nodeX[elNodes[7]];//........5500
	//HX = -nodeX[elNodes[0]] + nodeX[elNodes[1]] - nodeX[elNodes[2]] + nodeX[elNodes[3]] + nodeX[elNodes[4]] - nodeX[elNodes[5]] + nodeX[elNodes[6]] - nodeX[elNodes[7]];//........5600
	//AY = +nodeY[elNodes[0]] + nodeY[elNodes[1]] + nodeY[elNodes[2]] + nodeY[elNodes[3]] + nodeY[elNodes[4]] + nodeY[elNodes[5]] + nodeY[elNodes[6]] + nodeY[elNodes[7]];//........5700
	//BY = -nodeY[elNodes[0]] + nodeY[elNodes[1]] + nodeY[elNodes[2]] - nodeY[elNodes[3]] - nodeY[elNodes[4]] + nodeY[elNodes[5]] + nodeY[elNodes[6]] - nodeY[elNodes[7]];//........5800
	//CY = -nodeY[elNodes[0]] - nodeY[elNodes[1]] + nodeY[elNodes[2]] + nodeY[elNodes[3]] - nodeY[elNodes[4]] - nodeY[elNodes[5]] + nodeY[elNodes[6]] + nodeY[elNodes[7]];//........5900
	//DY = -nodeY[elNodes[0]] - nodeY[elNodes[1]] - nodeY[elNodes[2]] - nodeY[elNodes[3]] + nodeY[elNodes[4]] + nodeY[elNodes[5]] + nodeY[elNodes[6]] + nodeY[elNodes[7]];//........6000
	//EY = +nodeY[elNodes[0]] - nodeY[elNodes[1]] + nodeY[elNodes[2]] - nodeY[elNodes[3]] + nodeY[elNodes[4]] - nodeY[elNodes[5]] + nodeY[elNodes[6]] - nodeY[elNodes[7]];//........6100
	//FY = +nodeY[elNodes[0]] - nodeY[elNodes[1]] - nodeY[elNodes[2]] + nodeY[elNodes[3]] - nodeY[elNodes[4]] + nodeY[elNodes[5]] + nodeY[elNodes[6]] - nodeY[elNodes[7]];//........6200
	//GY = +nodeY[elNodes[0]] + nodeY[elNodes[1]] - nodeY[elNodes[2]] - nodeY[elNodes[3]] - nodeY[elNodes[4]] - nodeY[elNodes[5]] + nodeY[elNodes[6]] + nodeY[elNodes[7]];//........6300
	//HY = -nodeY[elNodes[0]] + nodeY[elNodes[1]] - nodeY[elNodes[2]] + nodeY[elNodes[3]] + nodeY[elNodes[4]] - nodeY[elNodes[5]] + nodeY[elNodes[6]] - nodeY[elNodes[7]];//........6400
	//AZ = +nodeZ[elNodes[0]] + nodeZ[elNodes[1]] + nodeZ[elNodes[2]] + nodeZ[elNodes[3]] + nodeZ[elNodes[4]] + nodeZ[elNodes[5]] + nodeZ[elNodes[6]] + nodeZ[elNodes[7]];//........6500
	//BZ = -nodeZ[elNodes[0]] + nodeZ[elNodes[1]] + nodeZ[elNodes[2]] - nodeZ[elNodes[3]] - nodeZ[elNodes[4]] + nodeZ[elNodes[5]] + nodeZ[elNodes[6]] - nodeZ[elNodes[7]];//........6600
	//CZ = -nodeZ[elNodes[0]] - nodeZ[elNodes[1]] + nodeZ[elNodes[2]] + nodeZ[elNodes[3]] - nodeZ[elNodes[4]] - nodeZ[elNodes[5]] + nodeZ[elNodes[6]] + nodeZ[elNodes[7]];//........6700
	//DZ = -nodeZ[elNodes[0]] - nodeZ[elNodes[1]] - nodeZ[elNodes[2]] - nodeZ[elNodes[3]] + nodeZ[elNodes[4]] + nodeZ[elNodes[5]] + nodeZ[elNodes[6]] + nodeZ[elNodes[7]];//........6800
	//EZ = +nodeZ[elNodes[0]] - nodeZ[elNodes[1]] + nodeZ[elNodes[2]] - nodeZ[elNodes[3]] + nodeZ[elNodes[4]] - nodeZ[elNodes[5]] + nodeZ[elNodes[6]] - nodeZ[elNodes[7]];//........6900
	//FZ = +nodeZ[elNodes[0]] - nodeZ[elNodes[1]] - nodeZ[elNodes[2]] + nodeZ[elNodes[3]] - nodeZ[elNodes[4]] + nodeZ[elNodes[5]] + nodeZ[elNodes[6]] - nodeZ[elNodes[7]];//........7000
	//GZ = +nodeZ[elNodes[0]] + nodeZ[elNodes[1]] - nodeZ[elNodes[2]] - nodeZ[elNodes[3]] - nodeZ[elNodes[4]] - nodeZ[elNodes[5]] + nodeZ[elNodes[6]] + nodeZ[elNodes[7]];//........7100
	//HZ = -nodeZ[elNodes[0]] + nodeZ[elNodes[1]] - nodeZ[elNodes[2]] + nodeZ[elNodes[3]] + nodeZ[elNodes[4]] - nodeZ[elNodes[5]] + nodeZ[elNodes[6]] - nodeZ[elNodes[7]];//........7200

	for (int i = 0; i < ITRMAX; i++){
		F10 = AX - 8.*obs->getXOBS() + BX*tXSI + CX*tETA + DX*tZET + EX*tXSI*tETA + FX*tXSI*tZET + GX*tETA*tZET + HX*tXSI*tETA*tZET;
		F20 = AY - 8.*obs->getYOBS() + BY*tXSI + CY*tETA + DY*tZET + EY*tXSI*tETA + FY*tXSI*tZET + GY*tETA*tZET + HY*tXSI*tETA*tZET;
		F30 = AZ - 8.*obs->getZOBS() + BZ*tXSI + CZ*tETA + DZ*tZET + EZ*tXSI*tETA + FZ*tXSI*tZET + GZ*tETA*tZET + HZ*tXSI*tETA*tZET;
		FP11 = BX + EX*tETA + FX*tZET + HX*tETA*tZET;
		FP12 = CX + EX*tXSI + GX*tZET + HX*tXSI*tZET;
		FP13 = DX + FX*tXSI + GX*tETA + HX*tXSI*tETA;
		FP21 = BY + EY*tETA + FY*tZET + HY*tETA*tZET;
		FP22 = CY + EY*tXSI + GY*tZET + HY*tXSI*tZET;
		FP23 = DY + FY*tXSI + GY*tETA + HY*tXSI*tETA;
		FP31 = BZ + EZ*tETA + FZ*tZET + HZ*tETA*tZET;
		FP32 = CZ + EZ*tXSI + GZ*tZET + HZ*tXSI*tZET;
		FP33 = DZ + FZ*tXSI + GZ*tETA + HZ*tXSI*tETA;
		S11 = FP22*FP33 - FP32*FP23;
		S12 = FP21*FP33 - FP31*FP23;
		S13 = FP21*FP32 - FP31*FP22;
		CF12 = -F20*FP33 + F30*FP23;
		CF34 = -F20*FP32 + F30*FP22;
		CF43 = -CF34;
		CF56 = -F30*FP21 + F20*FP31;
		DETERM = FP11*S11 - FP12*S12 + FP13*S13;
		DETXSI = -F10*S11 - FP12*CF12 + FP13*CF34;
		DELXSI = DETXSI / DETERM;
		if (abs(DELXSI) >= TOL){
			tXSI = tXSI + DELXSI;
			continue;
		}

		DETETA = FP11*CF12 + F10*S12 + FP13*CF56;
		DELETA = DETETA / DETERM;
		if (abs(DELETA) >= TOL){
			tETA = tETA + DELETA;
			continue;
		}
		DETZET = FP11*CF43 - FP12*CF56 - F10*S13;
		DELZET = DETZET / DETERM;

		if (abs(DELZET) >= TOL){
			tZET = tZET + DELZET;
			continue;
		}

		/*tXSI = tXSI + DELXSI;
		tETA = tETA + DELETA;
		tZET = tZET + DELZET;*/
		if ((abs(DELXSI) < TOL) && (abs(DELETA) < TOL) && (abs(DELZET) < TOL)){
			if ((abs(tXSI) <= OPE) && (abs(tETA) <= OPE) && (abs(tZET) <= OPE)){
				obs->setObsElement(ElNo);
				obs->setXSI(tXSI);
				obs->setETA(tETA);
				obs->setZET(tZET);
				
				found = true;
				break;
			}
		}
	}

	return found;
}
bool ControlParameters::FINDL2(int ElNo, vector<double>X, vector<double>Y, ObservationPoints * obs){
	double EPSILON = 0.001;
	double TOL = 0.001;
	double ITRMAX = 25;
	double OPE = 1 + EPSILON;
	double AX, BX, CX, DX, AY, BY, CY, DY;
	double F10, F20, FP11, FP12, FP21, FP22;
	double DETXSI, DETETA, DETERM, DELXSI, DELETA;
	double tXSI = 0.0, tETA = 0.0;
	bool found = false;

	AX = X[0] + X[1] + X[2] + X[3];
	BX = -X[0] + X[1] + X[2] - X[3];
	CX = -X[0] - X[1] + X[2] + X[3];
	DX = X[0] - X[1] + X[2] - X[3];
	AY = Y[0] + Y[1] + Y[2] + Y[3];
	BY = -Y[0] + Y[1] + Y[2] - Y[3];
	CY = -Y[0] - Y[1] + Y[2] + Y[3];
	DY = Y[0] - Y[1] + Y[2] - Y[3];

	for (int i = 0; i < ITRMAX; i++){

		F10 = AX - 4 * obs->getXOBS() + BX*tXSI + CX*tETA + DX*tXSI*tETA;
		F20 = AY - 4 * obs->getYOBS() + BY*tXSI + CY*tETA + DY*tXSI*tETA;
		FP11 = BX + DX*tETA;
		FP12 = CX + DX*tXSI;
		FP21 = BY + DY*tETA;
		FP22 = CY + DY*tXSI;

		DETXSI = -F10 * FP22 + F20*FP12;
		DETETA = -F20*FP11 + F10*FP21;
		DETERM = FP11*FP22 - FP12*FP21;
		DELETA = DETETA / DETERM;
		DELXSI = DETXSI / DETERM;
		if ((abs(DELXSI) < TOL) && (abs(DELETA)< TOL)){
			if (abs(tXSI) <= OPE && abs(tETA) <= OPE){
				obs->setObsElement(ElNo);
				obs->setXSI(tXSI);
				obs->setETA(tETA);
				found = true;
				break;
			}
		}
	}
	return found;
}

void ControlParameters::createNodes(){
	for (int i = 1; i <= NN; i++){
		nodeContainer[i]->setNREG(nodeRegion[i]);
		nodeContainer[i]->setXCoord(nodeX[i]);
		nodeContainer[i]->setYCoord(nodeY[i]);
		nodeContainer[i]->setZCoord(nodeZ[i]);
		nodeContainer[i]->setPorosity(nodePorosity[i]);
		nodeContainer[i]->setSOP(nodeSOP[i]);
		nodeContainer[i]->setPBC(nodePBC[i]);
		nodeContainer[i]->setUBC(nodeUBC[i]);
		nodeContainer[i]->setQIN(nodeQIN[i]);
		nodeContainer[i]->setUIN(nodeUIN[i]);
		nodeContainer[i]->setQUIN(nodeQUIN[i]);
		nodeContainer[i]->setPVEC(nodePVEC[i]);
		nodeContainer[i]->setUVEC(nodeUVEC[i]);
	}

}

void ControlParameters::createElements(){
	if (KTYPE[0] == 3){
		for (int i = 1; i <= NE; i++){
			elementContainer[i]->setLREG(elementRegion[i]);
			elementContainer[i]->setALMAX(elALMAX[i]);
			elementContainer[i]->setATMAX(elATMAX[i]);
			elementContainer[i]->setPMAX(elPMAX[i]);
			elementContainer[i]->setPMID(elPMID[i]);
			elementContainer[i]->setATMID(elATMID[i]);
			elementContainer[i]->setALMID(elALMID[i]);
			elementContainer[i]->setPMIN(elPMIN[i]);
			elementContainer[i]->setALMIN(elALMIN[i]);
			elementContainer[i]->setATMIN(elATMIN[i]);
			elementContainer[i]->setANGLE1(elANGLE1[i]);
			elementContainer[i]->setANGLE2(elANGLE2[i]);
			elementContainer[i]->setANGLE3(elANGLE3[i]);
			elementContainer[i]->setElementNodes(elementNodes[i]);
			elementContainer[i]->ROTMATTENSYM();
		}
	}
	else if (KTYPE[0] == 2){
		for (int i = 1; i <= NE; i++){
			elementContainer[i]->setLREG(elementRegion[i]);
			elementContainer[i]->setALMAX(elALMAX[i]);
			elementContainer[i]->setATMAX(elATMAX[i]);
			elementContainer[i]->setPMAX(elPMAX[i]);
			elementContainer[i]->setPMIN(elPMIN[i]);
			elementContainer[i]->setALMIN(elALMIN[i]);
			elementContainer[i]->setATMIN(elATMIN[i]);
			elementContainer[i]->setANGLEX(elANGLEX[i]);
			elementContainer[i]->setElementNodes(elementNodes[i]);
		}
	}
}

void ControlParameters::setNELT(int val){ this->NELT = val; }
int ControlParameters::getNELT(){ return this->NELT; }

void ControlParameters::BANWID(){
	string logLine = "";
	Writer * logWriter = Writer::Instance();
	char buff[512];

	logLine.append("\n\n\n\n           **** MESH ANALYSIS ****\n\n");
	int NDIF = 0;
	int IELO = 0;
	int IEHI = 0;
	for (int i = 1; i <= NE; i++){
		IELO = elementNodes[i][0];
		IEHI = elementNodes[i][0];
		for (int j = 1; j < N48; j++){
			if (elementNodes[i][j] < IELO) IELO = elementNodes[i][j];
			if (elementNodes[i][j] > IEHI) IEHI = elementNodes[i][j];
		}
		if ((IEHI - IELO) > NDIF){
			NDIF = IEHI - IELO;
			LEM = i;
		}
	}

	NB = 2 * NDIF + 1;
	NBHALF = NDIF + 1;
	NBI = NB;

	_snprintf(buff, sizeof(buff), "\n\n             MAXIMUM FULL BANDWIDTH %9d, WAS CALCULATED IN ELEMENT %9d\n", NB, LEM);
	logLine.append(buff);
	logWriter->writeContainer.push_back(logLine); 
	logLine.clear();

}

void ControlParameters::loadInitialConditions(){
	char * map = InputFileParser::Instance()->mapViewOfICSFile;
	char * start_str = map;
	char * end_str;
	int size = strlen(map);
	vector<char>lineBuffer;
	int whereat;
	const char* del = " ";
	
	if (IREAD == -1 || IREAD == 0){ // WARM START
		double DUM;
		double DELTP, DELTU;
		string CPUNI, CUUNI;
		for (int j = 0; j < size; j++){
			if (map[j] == '\r'){
				end_str = map + j - 1;
				lineBuffer.assign(start_str, end_str);
				lineBuffer.push_back('\0');
				start_str = map + j + 2;
				DUM = stod(strtok(lineBuffer.data(), del));
				DELTP = stod(strtok(NULL, del));
				DELTU = stod(strtok(NULL, del));
				ITRST = stoi(strtok(NULL, del));
				DUM = stod(strtok(NULL, del));
				whereat = j+2;
				break;
			}
		}
		if (ITRST >= ITRMAX){
			exitOnError("ICS-1-1");
		}

		/*bool neglectStr = false;
		for (int i = whereat; i < size; i++){
			if (map[i] == '#'){
				start_str = map + i;
				neglectStr = true;
			}
			if (neglectStr && (map[i] == '\r')){
				end_str = map + i - 1;
				start_str = map + i + 2;
				whereat = i + 2;
				break;
			}
		}*/

		for (int j = whereat; j < size; j++){
			if (map[j] == '\r'){
				end_str = map + j;
				lineBuffer.assign(start_str, end_str);
				lineBuffer.push_back('\0');
				start_str = map + j + 2;
				CPUNI = strtok(lineBuffer.data(), del);
				whereat = j + 2;
				break;
			}
		}
		if (CPUNI != "'NONUNIFORM"){
			exitOnError("ICS-2-2");
		}

		int lineCounter = 0;
		for (int j = whereat; j < size; j++){
			if (map[j] == '\r'){
				lineCounter++;

				end_str = map + j - 1;
				lineBuffer.assign(start_str, end_str);
				lineBuffer.push_back('\0');
				start_str = map + j + 2;
				if (lineCounter > NN){
					whereat = j + 2;
					break;
				}
				nodePVEC[lineCounter] = stod(strtok(lineBuffer.data(), del));
			}
		}
			

		/*neglectStr = false;
		for (int i = whereat; i < size; i++){
			if (map[i] == '#'){
				start_str = map + i;
				neglectStr = true;
			}
			if (neglectStr && (map[i] == '\r')){
				end_str = map + i - 1;
				start_str = map + i + 2;
				whereat = i + 2;
				break;
			}
		}*/

		for (int j = whereat; j < size; j++){
			if (map[j] == '\r'){
				end_str = map + j;
				lineBuffer.assign(start_str, end_str);
				lineBuffer.push_back('\0');
				start_str = map + j + 2;
				CUUNI = strtok(lineBuffer.data(), del);
				whereat = j + 2;
				break;
			}
		}
		if (CUUNI != "'NONUNIFORM"){
			exitOnError("ICS-3-2");
		}

		lineCounter = 0;
		for (int j = whereat; j < size; j++){
			if (map[j] == '\r'){
				lineCounter++;

				end_str = map + j - 1;
				lineBuffer.assign(start_str, end_str);
				lineBuffer.push_back('\0');
				start_str = map + j + 2;
				if (lineCounter > NN){
					whereat = j + 2;
					break;
				}
				nodeUVEC[lineCounter] = stod(strtok(lineBuffer.data(), del));
			}
		}
		 // Here Initializations for WARM START need to figure out


	}
	else if (IREAD == 1){ // COLD START
		double DUM;
		string CPUNI,CUUNI;
		double PUNI,UUNI;
		ITRST = 0;
	// READ PRESSURE
		for (int j = 0; j < size; j++){
			if (map[j] == '\r'){
				end_str = map + j - 1;
				lineBuffer.assign(start_str, end_str);
				lineBuffer.push_back('\0');
				start_str = map + j + 2;
				DUM = stod(strtok(lineBuffer.data(), del));
				whereat = j+2;
				break;
			}
		}

		bool neglectStr =false;
		for (int i = whereat; i < size; i++){
			if (map[i] == '#'){
				start_str = map + i;
				neglectStr = true;
			}
			if (neglectStr && (map[i] == '\r')){
				end_str = map + i - 1;
				start_str = map + i + 2;
				whereat = i+2;
				break;
			}
		}

		for (int j = whereat; j < size; j++){
			if (map[j] == '\r'){
				end_str = map + j;
				lineBuffer.assign(start_str, end_str);
				lineBuffer.push_back('\0');
				start_str = map + j + 2;
				CPUNI = strtok(lineBuffer.data(), del);
				whereat = j+2;
				break;
			}
		}

		if (CPUNI == "'UNIFORM'"){
			for (int j = whereat; j < size; j++){
				if (map[j] == '\r'){
					end_str = map + j - 1;
					lineBuffer.assign(start_str, end_str);
					lineBuffer.push_back('\0');
					start_str = map + j + 2;
					PUNI = stod(strtok(lineBuffer.data(), del));
					whereat = j+2;
					break;
				}
			}
			for (int i = 1; i <= NN; i++){
				nodePVEC[i] = PUNI;
			}
		}
		else if (CPUNI == "'NONUNIFORM'"){
			int lineCounter = 0;
			for (int j = whereat; j < size; j++){
				if (map[j] == '\r'){
					lineCounter++;
				
					end_str = map + j - 1;
					lineBuffer.assign(start_str, end_str);
					lineBuffer.push_back('\0');
					start_str = map + j + 2;
					if (lineCounter > NN){
						whereat = j+2;
						break;
					}

				/*	cout << lineCounter << " "<< stod(strtok(lineBuffer.data(), del)) << endl;*/
					nodePVEC[lineCounter] = stod(strtok(lineBuffer.data(), del));	


				}
			}

		}
		else{
			exitOnError("ICS-2-1");
		}

		// READ CONCENTRATION
		neglectStr = false;
		for (int i = whereat; i < size; i++){
			if (map[i] == '#'){
				start_str = map + i;
				neglectStr = true;
			}
			if (neglectStr && (map[i] == '\r')){
				end_str = map + i - 1;
				start_str = map + i + 2;
				whereat = i+2;
				break;
			}
		}

		for (int j = whereat; j < size; j++){
			if (map[j] == '\r'){
				end_str = map + j;
				lineBuffer.assign(start_str, end_str);
				lineBuffer.push_back('\0');
				start_str = map + j + 2;
				CUUNI = strtok(lineBuffer.data(), del);
				whereat = j+2;
				break;
			}
		}

		if (CUUNI == "'UNIFORM'"){
			for (int j = whereat; j < size; j++){
				if (map[j] == '\r'){
					end_str = map + j - 1;
					lineBuffer.assign(start_str, end_str);
					lineBuffer.push_back('\0');
					start_str = map + j + 2;
					UUNI = stod(strtok(lineBuffer.data(), del));
					whereat = j+2;
					break;
				}
			}
			for (int i = 1; i <= NN; i++){
				nodeUVEC[i] = UUNI;
			}
		}
		else if (CUUNI == "'NONUNIFORM'"){
			int lineCounter = 0;
			for (int j = whereat; j < size; j++){
				if (map[j] == '\r'){
					lineCounter++;

					end_str = map + j - 1;
					lineBuffer.assign(start_str, end_str);
					lineBuffer.push_back('\0');
					start_str = map + j + 2;
					if (lineCounter > NN){
						whereat = j+2;
						break;
					}
					nodeUVEC[lineCounter] = stod(strtok(lineBuffer.data(), del));
				}
			}

		}
		else{
			exitOnError("ICS-3-1");
		}

		


	}
	else{
		cout << " ERROR IN ICS FILE" << endl;
	}
}

TimeCycleSch * ControlParameters::getTIMESTEPSSchedule(){
	Schedule * TS = nullptr;
	for (Schedule * sc : listOfSchedules){
		if (sc->getScheduleName() == "TIME_STEPS"){
			TS = sc;
			break;
		}
	}
	return (TimeCycleSch*)TS;
}

void ControlParameters::setTFINISH(double val){ this->TFINISH = val; }
double ControlParameters::getTFINISH(){ return this->TFINISH; }
void ControlParameters::setNRTEST(int val){ this->NRTEST = val; }
int ControlParameters::getNRTEST(){ return this->NRTEST; }
void ControlParameters::setLRTEST(int val){ this->LRTEST = val; }
int ControlParameters::getLRTEST(){ return this->LRTEST; }

void ControlParameters::setIQSOPT(int val){ this->IQSOPT = val; }
void ControlParameters::setIQSOUT(int val){ this->IQSOUT = val; }
void ControlParameters::setIPBCT(int val){ this->IPBCT = val; }
void ControlParameters::setIUBCT(int val){ this->IUBCT = val; }
int ControlParameters::getIQSOPT(){ return this->IQSOPT; }
int ControlParameters::getIQSOUT(){ return this->IQSOUT; }
int ControlParameters::getIPBCT(){ return this->IPBCT; }
int ControlParameters::getIUBCT(){ return this->IUBCT; }

void ControlParameters::setIBCT(){ this->IBCT = IQSOPT+IQSOUT+IPBCT+IUBCT; }
int ControlParameters::getIBCT(){ return this->IBCT; }

void ControlParameters::setTMAX(double val){ this->TMAX = val; }
double ControlParameters::getTMAX(){ return this->TMAX; }

void ControlParameters::setTEMAX(){ this->TEMAX = TMAX-TSTART; }
double ControlParameters::getTEMAX(){ return this->TEMAX; }

void ControlParameters::setTSEC(double val){ this->TSEC = val; }
void ControlParameters::setTSECP0(double val){ this->TSECP0 = val; }
void ControlParameters::setTSECU0(double val){ this->TSECU0 = val; }
void ControlParameters::setTSECM1(double val){ this->TSECM1 = val; }
void ControlParameters::setTMIN(double val){ this->TMIN = val; }
void ControlParameters::setTHOUR(double val){ this->THOUR = val; }
void ControlParameters::setTDAY(double val){ this->TDAY = val; }
void ControlParameters::setTWEEK(double val){ this->TWEEK = val; }
void ControlParameters::setTMONTH(double val){ this->TMONTH = val; }
void ControlParameters::setTYEAR(double val){ this->TYEAR = val; }
void ControlParameters::setDELT(double val){ this->DELT = val; }
void ControlParameters::setDELTM1(double val){ this->DELTM1 = val; }
double ControlParameters::getTSEC(){ return this->TSEC; }
double ControlParameters::getTSECP0(){ return this->TSECP0; }
double ControlParameters::getTSECU0(){ return this->TSECU0; }
double ControlParameters::getTSECM1(){ return this->TSECM1; }
double ControlParameters::getTMIN(){ return this->TMIN; }
double ControlParameters::getTHOUR(){ return this->THOUR; }
double ControlParameters::getTDAY(){ return this->TDAY; }
double ControlParameters::getTWEEK(){ return this->TWEEK; }
double ControlParameters::getTMONTH(){ return this->TMONTH; }
double ControlParameters::getTYEAR(){ return this->TYEAR; }
double ControlParameters::getDELT(){ return this->DELT; }
double ControlParameters::getDELTM1(){ return this->DELTM1; }


/*
C     SUBROUTINE        O  U  T  L  S  T  3        SUTRA VERSION 2.2     OUTLST3........100
C                                                                        OUTLST3........200
C *** PURPOSE :                                                          OUTLST3........300
C ***  TO PRINT PRESSURE AND TEMPERATURE OR CONCENTRATION                OUTLST3........400
C ***  SOLUTIONS AND TO OUTPUT INFORMATION ON TIME STEP, ITERATIONS,     OUTLST3........500
C ***  SATURATIONS, AND FLUID VELOCITIES FOR 3D PROBLEMS.                OUTLST3........600
C ***  OUTPUT IS TO THE LST FILE.                                        OUTLST3........700
C                                                                        OUTLST3........800
SUBROUTINE OUTLST3(ML,ISTOP,IGOI,IERRP,ITRSP,ERRP,                 OUTLST3........900
1   IERRU,ITRSU,ERRU,PVEC,UVEC,VMAG,VANG1,VANG2,SW,SWT,SWB)         OUTLST3.......1000!SWT and SWB added
IMPLICIT DOUBLE PRECISION (A-H,O-Z)                                OUTLST3.......1100
DIMENSION PVEC(NNVEC),UVEC(NNVEC),VMAG(NE),VANG1(NE),VANG2(NEX),   OUTLST3.......1200
1   SW(NN),SWT(NN),SWB(NN),RELK(NN),RELKB(NN),RELKT(NN)             OUTLST3.......1300!SWT(NN)and SWB(NN) added
DIMENSION KTYPE(2)                                                 OUTLST3.......1400
COMMON /CONTRL/ GNUP,GNUU,UP,DTMULT,DTMAX,ME,ISSFLO,ISSTRA,ITCYC,  OUTLST3.......1500
1   NPCYC,NUCYC,NPRINT,NBCFPR,NBCSPR,NBCPPR,NBCUPR,IREAD,           OUTLST3.......1600
2   ISTORE,NOUMAT,IUNSAT,KTYPE                                      OUTLST3.......1700
COMMON /DIMS/ NN,NE,NIN,NBI,NCBI,NB,NBHALF,NPBC,NUBC,              OUTLST3.......1800
1   NSOP,NSOU,NBCN,NCIDB                                            OUTLST3.......1900
COMMON /DIMX/ NWI,NWF,NWL,NELT,NNNX,NEX,N48                        OUTLST3.......2000
COMMON /DIMX2/ NELTA,NNVEC,NDIMIA,NDIMJA                           OUTLST3.......2100
COMMON /FUNITS/ K00,K0,K1,K2,K3,K4,K5,K6,K7,K8,K9,                 OUTLST3.......2200
1   K10,K11,K12,K13,KAUX                                                 OUTLST3.......2300
COMMON /ITERAT/ RPM,RPMAX,RUM,RUMAX,ITER,ITRMAX,IPWORS,IUWORS      OUTLST3.......2400
COMMON /KPRBCS/ KINACT                                             OUTLST3.......2500
COMMON /KPRINT/ KNODAL,KELMNT,KINCID,KPLOTP,KPLOTU,                OUTLST3.......2600
1   KPANDS,KVEL,KCORT,KBUDG,KSCRN,KPAUSE                            OUTLST3.......2700
COMMON /SOLVI/ KSOLVP,KSOLVU,NN1,NN2,NN3                           OUTLST3.......2800
COMMON /TIMES/ DELT,TSEC,TMIN,THOUR,TDAY,TWEEK,TMONTH,TYEAR,       OUTLST3.......2900
1   TMAX,DELTP,DELTU,DLTPM1,DLTUM1,IT,ITBCS,ITRST,ITMAX,TSTART      OUTLST3.......3000
C                                                                        OUTLST3.......3100
C.....OUTPUT MAJOR HEADINGS FOR CURRENT TIME STEP                        OUTLST3.......3200
ITREL = IT - ITRST                                                 OUTLST3.......3300
IF(ITREL.GT.0.OR.ISSFLO.EQ.2.OR.ISSTRA.EQ.1) GOTO 100              OUTLST3.......3400
WRITE(K3,60)                                                       OUTLST3.......3500
60 FORMAT('1'////11X,'I N I T I A L   C O N D I T I O N S',           OUTLST3.......3600
1             /11X,'___________________________________')           OUTLST3.......3700
IF(IREAD.EQ.-1) WRITE(K3,65) ITRST                                 OUTLST3.......3800
65 FORMAT(//11X,'INITIAL CONDITIONS RETRIEVED FROM A RESTART',        OUTLST3.......3900
1   ' FILE (WARM START)'                                            OUTLST3.......4000
2   /11X,'THAT WAS SAVED AT THE END OF TIME STEP ',I8,' OF THE ',   OUTLST3.......4100
3        'ORIGINAL SIMULATION.')                                    OUTLST3.......4200
GOTO 500                                                           OUTLST3.......4300
C                                                                        OUTLST3.......4400
100 WRITE(K3,350) IT                                                   OUTLST3.......4500
350 FORMAT('1'//11X,'RESULTS FOR TIME STEP ',I8/                       OUTLST3.......4600
1   11X,'_______ ___ ____ ____ ________')                           OUTLST3.......4700
C                                                                        OUTLST3.......4800
IF(ITRMAX.GT.1) THEN                                               OUTLST3.......4900
IF(IGOI.EQ.0) THEN                                              OUTLST3.......5000
WRITE(K3,355) ITER                                           OUTLST3.......5100
ELSE                                                            OUTLST3.......5200
WRITE(K3,356) ITER                                           OUTLST3.......5300
END IF                                                          OUTLST3.......5400
355    FORMAT(/11X,'NON-LINEARITY ITERATIONS CONVERGED AFTER ',I5,     OUTLST3.......5500
1      ' ITERATIONS')                                               OUTLST3.......5600
356    FORMAT(/11X,'NON-LINEARITY ITERATIONS  N O T  CONVERGED',       OUTLST3.......5700
1      ' AFTER ',I5,' ITERATIONS')                                  OUTLST3.......5800
WRITE(K3,450) RPM,IPWORS,RUM,IUWORS                             OUTLST3.......5900
450    FORMAT(11X,'MAXIMUM P CHANGE FROM PREVIOUS ITERATION ',         OUTLST3.......6000
1      1PE14.5,' AT NODE ',I9/11X,'MAXIMUM U CHANGE FROM PREVIOUS', OUTLST3.......6100
2      ' ITERATION ',1PE14.5,' AT NODE ',I9)                        OUTLST3.......6200
END IF                                                             OUTLST3.......6300
C                                                                        OUTLST3.......6400
IF ((ML.EQ.0).OR.(ML.EQ.1)) THEN                                   OUTLST3.......6500
IF (KSOLVP.EQ.0) THEN                                           OUTLST3.......6600
WRITE(K3,452)                                                OUTLST3.......6700
ELSE IF (IERRP.EQ.0) THEN                                       OUTLST3.......6800
WRITE(K3,455) ITRSP, ERRP                                    OUTLST3.......6900
ELSE                                                            OUTLST3.......7000
WRITE(K3,456) ITRSP, ERRP                                    OUTLST3.......7100
END IF                                                          OUTLST3.......7200
END IF                                                             OUTLST3.......7300
IF ((ML.EQ.0).OR.(ML.EQ.2)) THEN                                   OUTLST3.......7400
IF (ML.EQ.2) WRITE(K3,*) ' '                                    OUTLST3.......7500
IF (KSOLVU.EQ.0) THEN                                           OUTLST3.......7600
WRITE(K3,453)                                                OUTLST3.......7700
ELSE IF (IERRU.EQ.0) THEN                                       OUTLST3.......7800
WRITE(K3,457) ITRSU, ERRU                                    OUTLST3.......7900
ELSE                                                            OUTLST3.......8000
WRITE(K3,458) ITRSU, ERRU                                    OUTLST3.......8100
END IF                                                          OUTLST3.......8200
END IF                                                             OUTLST3.......8300
452 FORMAT(/11X,'P-SOLUTION COMPUTED USING DIRECT SOLVER')             OUTLST3.......8400
453 FORMAT(11X,'U-SOLUTION COMPUTED USING DIRECT SOLVER')              OUTLST3.......8500
455 FORMAT(/11X,'P-SOLUTION CONVERGED AFTER ',I5,' MATRIX'             OUTLST3.......8600
1   ' SOLVER ITERATIONS; ESTIMATED ERROR ',1PE14.5)                 OUTLST3.......8700
456 FORMAT(/11X,'P-SOLUTION  F A I L E D  AFTER ',I5,' MATRIX'         OUTLST3.......8800
1   ' SOLVER ITERATIONS; ESTIMATED ERROR ',1PE14.5)                 OUTLST3.......8900
457 FORMAT(11X,'U-SOLUTION CONVERGED AFTER ',I5,' MATRIX'              OUTLST3.......9000
1   ' SOLVER ITERATIONS; ESTIMATED ERROR ',1PE14.5)                 OUTLST3.......9100
458 FORMAT(11X,'U-SOLUTION  F A I L E D  AFTER ',I5,' MATRIX'          OUTLST3.......9200
1   ' SOLVER ITERATIONS; ESTIMATED ERROR ',1PE14.5)                 OUTLST3.......9300
C                                                                        OUTLST3.......9400
500 IF(IT.EQ.0.AND.ISSFLO.EQ.2) GOTO 680                               OUTLST3.......9500
IF(ISSTRA.EQ.1) GOTO 800                                           OUTLST3.......9600
WRITE(K3,550) DELT,TSEC,TMIN,THOUR,TDAY,TWEEK,                     OUTLST3.......9700
1   TMONTH,TYEAR                                                    OUTLST3.......9800
550 FORMAT(///11X,'TIME INCREMENT :',T27,1PE15.4,' SECONDS'//11X,      OUTLST3.......9900
1   'TIME AT END',3X,T27,1PE15.4,' SECONDS',/11X,'OF STEP:',6X,T27, OUTLST3......10000
2   1PE15.4,' MINUTES'/T27,1PE15.4,' HOURS'/T27,1PE15.4,' DAYS'     OUTLST3......10100
3   /T27,1PE15.4,' WEEKS'/T27,1PE15.4,' MONTHS'/T27,1PE15.4,        OUTLST3......10200
4   ' YEARS')                                                       OUTLST3......10300
C                                                                        OUTLST3......10400
C.....OUTPUT PRESSURES FOR TRANSIENT FLOW SOLUTION (AND, POSSIBLY,       OUTLST3......10500
C        SATURATION AND VELOCITY)                                        OUTLST3......10600
IF(ML.EQ.2.AND.ISTOP.GE.0) GOTO 700                                OUTLST3......10700
IF(ISSFLO.GT.0) GOTO 700                                           OUTLST3......10800
IF (KPANDS.EQ.1) THEN                                              OUTLST3......10900
WRITE(K3,650) (I,CUTSML(PVEC(I)),I=1,NN)                        OUTLST3......11000
650    FORMAT(///11X,'P  R  E  S  S  U  R  E'                          OUTLST3......11100
1      //2X,5(6X,'NODE',16X)/(2X,5(1X,I9,1X,1PE15.8)))              OUTLST3......11200
WRITE(K3,651) (I,CUTSML(SWT(I)),I=1,NN)                        OUTLST3......11300! ASk about SW(I) again IF(IUNSAT.NE.0) deleted adn SW(I) changed to SWT(I)
651    FORMAT(///11X,'S  A  T  U  R  A  T  I  O  N'                    OUTLST3......11400
1      //2X,5(6X,'NODE',16X)/(2X,5(1X,I9,1X,1PE15.8)))              OUTLST3......11500
END IF                                                             OUTLST3......11600
IF(KVEL.EQ.1.AND.ITREL.GT.0) THEN                                  OUTLST3......11700
WRITE(K3,655) (L,CUTSML(VMAG(L)),L=1,NE)                        OUTLST3......11800
WRITE(K3,656) (L,CUTSML(VANG1(L)),L=1,NE)                       OUTLST3......11900
WRITE(K3,657) (L,CUTSML(VANG2(L)),L=1,NE)                       OUTLST3......12000
END IF                                                             OUTLST3......12100
655 FORMAT(///11X,'F  L  U  I  D     V  E  L  O  C  I  T  Y'//         OUTLST3......12200
1   11X,'M A G N I T U D E   AT CENTROID OF ELEMENT'//              OUTLST3......12300
2   2X,5(3X,'ELEMENT',16X)/(2X,5(1X,I9,1X,1PE15.8)))                OUTLST3......12400
656 FORMAT(///11X,'F  L  U  I  D     V  E  L  O  C  I  T  Y'//         OUTLST3......12500
1   11X,'A N G L E 1   AT CENTROID OF ELEMENT, IN DEGREES FROM ',   OUTLST3......12600
2   '+X-AXIS TO PROJECTION OF FLOW DIRECTION IN XY-PLANE'//         OUTLST3......12700
3   2X,5(3X,'ELEMENT',16X)/(2X,5(1X,I9,1X,1PE15.8)))                OUTLST3......12800
657 FORMAT(///11X,'F  L  U  I  D     V  E  L  O  C  I  T  Y'//         OUTLST3......12900
1   11X,'A N G L E 2   AT CENTROID OF ELEMENT, IN DEGREES FROM ',   OUTLST3......13000
2   'XY-PLANE TO FLOW DIRECTION'//                                  OUTLST3......13100
3   2X,5(3X,'ELEMENT',16X)/(2X,5(1X,I9,1X,1PE15.8)))                OUTLST3......13200
C     END IF                                                             OUTLST3......13300
GOTO 700                                                           OUTLST3......13400
C                                                                        OUTLST3......13500
C.....OUTPUT PRESSURES FOR STEADY-STATE FLOW SOLUTION                    OUTLST3......13600
680 IF (KPANDS.EQ.1) THEN                                              OUTLST3......13700
WRITE(K3,690) (I,CUTSML(PVEC(I)),I=1,NN)                        OUTLST3......13800
690    FORMAT(///11X,'S  T  E  A  D  Y  -  S  T  A  T  E     ',        OUTLST3......13900
1      'P  R  E  S  S  U  R  E'//2X,5(6X,'NODE',16X)                OUTLST3......14000
2      /(2X,5(1X,I9,1X,1PE15.8)))                                   OUTLST3......14100
WRITE(K3,651) (I,CUTSML(SWT(I)),I=1,NN)                        OUTLST3......14200!Ask about SW(I) again
END IF                                                             OUTLST3......14300
GOTO 1000                                                          OUTLST3......14400
C                                                                        OUTLST3......14500
C.....OUTPUT CONCENTRATIONS OR TEMPERATURES FOR                          OUTLST3......14600
C        TRANSIENT TRANSPORT SOLUTION                                    OUTLST3......14700
700 IF(ML.EQ.1.AND.ISTOP.GE.0) GOTO 1000                               OUTLST3......14800
IF (KCORT.EQ.1) THEN                                               OUTLST3......14900
IF(ME) 720,720,730                                              OUTLST3......15000
720    WRITE(K3,725) (I,CUTSML(UVEC(I)),I=1,NN)                        OUTLST3......15100
725    FORMAT(///11X,'C  O  N  C  E  N  T  R  A  T  I  O  N'           OUTLST3......15200
1      //2X,5(6X,'NODE',16X)/(2X,5(1X,I9,1X,1PE15.8)))              OUTLST3......15300
GOTO 900                                                        OUTLST3......15400
730    WRITE(K3,735) (I,CUTSML(UVEC(I)),I=1,NN)                        OUTLST3......15500
735    FORMAT(///11X,'T  E  M  P  E  R  A  T  U  R  E'                 OUTLST3......15600
1      //2X,5(6X,'NODE',16X)/(2X,5(1X,I9,1X,1PE15.8)))              OUTLST3......15700
GOTO 900                                                        OUTLST3......15800
END IF                                                             OUTLST3......15900
C                                                                        OUTLST3......16000
C.....OUTPUT CONCENTRATIONS OR TEMPERATURES FOR                          OUTLST3......16100
C        STEADY-STATE TRANSPORT SOLUTION                                 OUTLST3......16200
800 IF (KCORT.EQ.1) THEN                                               OUTLST3......16300
IF(ME) 820,820,830                                              OUTLST3......16400
820    WRITE(K3,825) (I,CUTSML(UVEC(I)),I=1,NN)                        OUTLST3......16500
825    FORMAT(///11X,'S  T  E  A  D  Y  -  S  T  A  T  E     ',        OUTLST3......16600
1      'C  O  N  C  E  N  T  R  A  T  I  O  N'                      OUTLST3......16700
2      //2X,5(6X,'NODE',16X)/(2X,5(1X,I9,1X,1PE15.8)))              OUTLST3......16800
GOTO 900                                                        OUTLST3......16900
830    WRITE(K3,835) (I,CUTSML(UVEC(I)),I=1,NN)                        OUTLST3......17000
835    FORMAT(///11X,'S  T  E  A  D  Y  -  S  T  A  T  E     ',        OUTLST3......17100
1      'T  E  M  P  E  R  A  T  U  R  E'                            OUTLST3......17200
2      //2X,5(6X,'NODE',16X)/(2X,5(1X,I9,1X,1PE15.8)))              OUTLST3......17300
END IF                                                             OUTLST3......17400
C                                                                        OUTLST3......17500
C.....OUTPUT VELOCITIES FOR STEADY-STATE FLOW SOLUTION                   OUTLST3......17600
900 IF(ISSFLO.NE.2.OR.IT.NE.1.OR.KVEL.NE.1) GOTO 1000                  OUTLST3......17700
WRITE(K3,925) (L,CUTSML(VMAG(L)),L=1,NE)                           OUTLST3......17800
WRITE(K3,950) (L,CUTSML(VANG1(L)),L=1,NE)                          OUTLST3......17900
WRITE(K3,951) (L,CUTSML(VANG2(L)),L=1,NE)                          OUTLST3......18000
925 FORMAT(///11X,'S  T  E  A  D  Y  -  S  T  A  T  E     ',           OUTLST3......18100
1   'F  L  U  I  D     V  E  L  O  C  I  T  Y'//                    OUTLST3......18200
2   11X,'M A G N I T U D E   AT CENTROID OF ELEMENT'//              OUTLST3......18300
3   2X,5(3X,'ELEMENT',16X)/(2X,5(1X,I9,1X,1PE15.8)))                OUTLST3......18400
950 FORMAT(///11X,'S  T  E  A  D  Y  -  S  T  A  T  E     ',           OUTLST3......18500
1   'F  L  U  I  D     V  E  L  O  C  I  T  Y'//                    OUTLST3......18600
2   11X,'A N G L E 1   AT CENTROID OF ELEMENT, IN DEGREES FROM ',   OUTLST3......18700
3   '+X-AXIS TO PROJECTION OF FLOW DIRECTION IN XY-PLANE'//         OUTLST3......18800
4   2X,5(3X,'ELEMENT',16X)/(2X,5(1X,I9,1X,1PE15.8)))                OUTLST3......18900
951 FORMAT(///11X,'S  T  E  A  D  Y  -  S  T  A  T  E     ',           OUTLST3......19000
1   'F  L  U  I  D     V  E  L  O  C  I  T  Y'//                    OUTLST3......19100
2   11X,'A N G L E 2   AT CENTROID OF ELEMENT, IN DEGREES FROM ',   OUTLST3......19200
3   'XY-PLANE TO FLOW DIRECTION'//                                  OUTLST3......19300
4   2X,5(3X,'ELEMENT',16X)/(2X,5(1X,I9,1X,1PE15.8)))                OUTLST3......19400
C                                                                        OUTLST3......19500
1000 RETURN                                                             OUTLST3......19600
C                                                                        OUTLST3......19700
END                                                                OUTLST3......19800

*/

void ControlParameters::OUTLST3(int ML, int ISTOP,int IGOI, int IERRP,int ITRSP,double ERRP, int IERRU,int ITRSU,double ERRU ){
	//args ML, ISTOP, IGOI, IERRP, ITRSP, ERRP, IERRU, ITRSU, ERRU, PVEC, UVEC, VMAG, VANG1, VANG2, SW, SWT, SWB
	string logLine = "";
	Writer * logWriter = Writer::Instance();
	char buff[512];

	ITREL = IT - ITRST;

	if (ITREL > 0 || ISSFLO == 2 || ISSTRA == 1)
		goto HUNDRED;

	logLine.append("\n\n\n\n" + string(11, ' ') + "I N I T I A L   C O N D I T I O N S\n" + string(11, ' ') + "___________________________________\n\n");
	if (IREAD == -1){
		logLine.append("\n\n" + string(11, ' ') + "INITIAL CONDITIONS RETRIEVED FROM A RESTART FILE(WARM START)\n");
		_snprintf(buff, sizeof(buff), "           THAT WAS SAVED AT THE END OF TIME STEP %8d OF THE ORIGINAL SIMULATION", ITRST);
		logLine.append(buff);
	}
	goto FIVEHUNDRED;


HUNDRED:
	_snprintf(buff, sizeof(buff), "\n\n           RESULTS FOR TIME STEP %8d\n           _______ ___ ____ ____ ________\n", IT);
	logLine.append(buff);

	if (ITRMAX > 1){
		if (IGOI == 0){
			_snprintf(buff, sizeof(buff), "\n           NON-LINEARITY ITERATIONS CONVERGED AFTER %5d ITERATIONS\n", ITER);
			logLine.append(buff);
		}
		else{
			_snprintf(buff, sizeof(buff), "\n           NON-LINEARITY ITERATIONS N O T CONVERGED AFTER %5d ITERATIONS\n", ITER);
			logLine.append(buff);
		}
		_snprintf(buff, sizeof(buff), "\n           MAXIMUM P CHANGE FROM PREVIOUS ITERATION %+14.5e AT NODE %9d\n", RPM, IPWORS);
		logLine.append(buff);
		_snprintf(buff, sizeof(buff), "           MAXIMUM U CHANGE FROM PREVIOUS ITERATION %+14.5e AT NODE %9d\n", RUM, IUWORS);
		logLine.append(buff);
	}

	if (ML == 0 || ML == 1){
		if (KSOLVP == 0){
			logLine.append("\n" + string(11, ' ') + "P-SOLUTION COMPUTED USING DIRECT SOLVER");
		}
		else if (IERRP == 0){
			_snprintf(buff, sizeof(buff), "           P-SOLUTION CONVERGED AFTER %5d MATRIX SOLVER ITERATIONS; ESTIMATED ERROR %+14.5e\n", ITRSP, ERRP);
			logLine.append(buff);
		}
		else
		{
			_snprintf(buff, sizeof(buff), "           P-SOLUTION F A I L E D AFTER %5d MATRIX SOLVER ITERATIONS; ESTIMATED ERROR %+14.5e\n", ITRSP, ERRP);
			logLine.append(buff);
		}
	}
	if (ML == 0 || ML == 2){
		if (ML == 2){ logLine.append("\n"); }
		if (KSOLVU == 0){
			logLine.append("\n" + string(11, ' ') + "U-SOLUTION COMPUTED USING DIRECT SOLVER");
		}
		else if (IERRU == 0){
			_snprintf(buff, sizeof(buff), "           U-SOLUTION CONVERGED AFTER %5d MATRIX SOLVER ITERATIONS; ESTIMATED ERROR %+14.5e\n", ITRSU, ERRU);
			logLine.append(buff);
		}
		else{
			_snprintf(buff, sizeof(buff), "           U-SOLUTION F A I L E D AFTER %5d MATRIX SOLVER ITERATIONS; ESTIMATED ERROR %+14.5e\n", ITRSU, ERRU);
			logLine.append(buff);
		}
	}



FIVEHUNDRED:
	if (IT == 0 && ISSFLO == 2)
		goto SIXHUNDRED;
	if (ISSTRA == 1)
		goto EIGHTHUNDRED;

	_snprintf(buff, sizeof(buff), "\n\n\n           TIME INCREMENT : %+15.4e SECONDS\n\n           TIME AT END   %+15.4e SECONDS\n", DELT, TSEC);
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           OF STEP:      %+15.4e MINUTES\n                         %+15.4e HOURS\n                         %+15.4e DAYS\n                         %+15.4e WEEKS\n                         %+15.4e YEARS\n", TMIN, THOUR, TDAY, TWEEK, TMONTH, TYEAR);
	logLine.append(buff);
 
	if (ML == 2 && ISTOP >= 0)
		goto SEVENHUNDRED;
	if (ISSFLO > 0)
		goto SEVENHUNDRED;

	if (KPANDS == 1){
		logLine.append("\n\n\n" + string(11, ' ') + "P R E S S U R E \n\n");
		_snprintf(buff, sizeof(buff), "      NODE                       NODE                       NODE                       NODE                        NODE                \n\n");
		logLine.append(buff);
		for (int i = 1; i <= NN; i++){
			_snprintf(buff, sizeof(buff), " %9d %+15.8e", nodeContainer[i]->getNodeNumber(), nodeContainer[i]->getPVEC());
			logLine.append(buff);
			if (i% 5 == 0)
				logLine.append("\n");
		}

		logLine.append("\n\n\n" + string(11, ' ') + "S A T U R A T I O N \n\n");
		_snprintf(buff, sizeof(buff), "      NODE                       NODE                       NODE                       NODE                        NODE                \n\n");
		logLine.append(buff);
		for (int i = 1; i <= NN; i++){
			_snprintf(buff, sizeof(buff), " %9d %+15.8e", nodeContainer[i]->getNodeNumber(), nodeContainer[i]->getSWT());
			logLine.append(buff);
			if (i% 5 == 0)
				logLine.append("\n");
		}
	}

	if (KVEL == 1 && ITREL > 0){
		logLine.append("\n\n\n" + string(11, ' ') + "F L U I D   V E L O C I T Y\n\n");
		logLine.append(string(11, ' ') + " M A G N I T U D E   A T   C E N T R O I D   O F   E L E M E N T\n\n");
		_snprintf(buff, sizeof(buff), "   ELEMENT                   ELEMENT                   ELEMENT                   ELEMENT                   ELEMENT                \n\n");
		logLine.append(buff);
		for (int i = 1; i <= NE; i++){
			_snprintf(buff, sizeof(buff), " %9d %+15.8e", elementContainer[i]->getElementNumber(), elementContainer[i]->getVMAG());
			logLine.append(buff);
			if (i% 5 == 0)
				logLine.append("\n");
		}

		logLine.append("\n\n\n" + string(11, ' ') + "F L U I D   V E L O C I T Y\n\n");
		logLine.append(string(11, ' ') + "A N G L E 1   AT CENTROID OF ELEMENT, IN DEGREES FROM +X-AXIS TO PROJECTION OF FLOW DIRECTION IN XY-PLANE\n\n");
		_snprintf(buff, sizeof(buff), "   ELEMENT                   ELEMENT                   ELEMENT                   ELEMENT                   ELEMENT                \n\n");
		logLine.append(buff);
		for (int i = 1; i <= NE; i++){
			_snprintf(buff, sizeof(buff), " %9d %+15.8e", elementContainer[i]->getElementNumber(), elementContainer[i]->getVANG1());
			logLine.append(buff);
			if (i% 5 == 0)
				logLine.append("\n");
		}

		logLine.append("\n\n\n" + string(11, ' ') + "F L U I D   V E L O C I T Y\n\n");
		logLine.append(string(11, ' ') + "A N G L E 2   AT CENTROID OF ELEMENT, IN DEGREES FROM XY-PLANE TO FLOW DIRECTION\n\n");
		_snprintf(buff, sizeof(buff), "   ELEMENT                   ELEMENT                   ELEMENT                   ELEMENT                   ELEMENT                \n\n");
		logLine.append(buff);
		for (int i = 1; i <= NE; i++){
			_snprintf(buff, sizeof(buff), " %9d %+15.8e", elementContainer[i]->getElementNumber(), elementContainer[i]->getVANG2());
			logLine.append(buff);
			if (i% 5 == 0)
				logLine.append("\n");
		}
	}
	goto SEVENHUNDRED;

SIXHUNDRED:
	if (KPANDS == 1){
		logLine.append("\n\n\n" + string(11, ' ') + "S T E A D Y  -  S T A T E   P R E S S U R E\n\n");
		_snprintf(buff, sizeof(buff), "      NODE                       NODE                       NODE                       NODE                        NODE                 \n\n");
		logLine.append(buff);
		for (int i = 1; i <= NN; i++){
			_snprintf(buff, sizeof(buff), " %9d %+15.8e", nodeContainer[i]->getNodeNumber(), nodeContainer[i]->getPVEC());
			logLine.append(buff);
			if (i% 5 == 0)
				logLine.append("\n");
		}
	}
	goto THOUSAND;


SEVENHUNDRED:
	if (ML == 1 && ISTOP >= 0)
		goto THOUSAND;

	if (KCORT == 1){
		logLine.append("\n\n\n" + string(11, ' ') + "C O N C E N T R A T I O N \n\n");
		_snprintf(buff, sizeof(buff), "      NODE                       NODE                       NODE                       NODE                        NODE                 \n\n");
		logLine.append(buff);
		for (int i = 1; i <= NN; i++){
			_snprintf(buff, sizeof(buff), " %9d %+15.8e", nodeContainer[i]->getNodeNumber(), nodeContainer[i]->getUVEC());
			logLine.append(buff);
			if (i% 5 == 0)
				logLine.append("\n");
		}
		goto NINEHUNDRED;
	}


EIGHTHUNDRED :
	if (KCORT == 1){
		logLine.append("\n\n\n" + string(11, ' ') + "S T E A D Y  -  S T A T E   C O N C E N T R A T I O N \n\n");
		_snprintf(buff, sizeof(buff), "      NODE                       NODE                       NODE                       NODE                        NODE                 \n\n");
		logLine.append(buff);
		for (int i = 1; i <= NN; i++){
			_snprintf(buff, sizeof(buff), " %9d %+15.8e", nodeContainer[i]->getNodeNumber(), nodeContainer[i]->getUVEC());
			logLine.append(buff);
			if (i% 5 == 0)
				logLine.append("\n");
		}
	}
NINEHUNDRED:
	if (ISSFLO != 2 || IT != 1 || KVEL != 1)
		goto THOUSAND;

	logLine.append("\n\n\n" + string(11, ' ') + "S T E A D Y  -  S T A T E   F L U I D   V E L O C I T Y\n\n");
	logLine.append(string(11, ' ') + " M A G N I T U D E   A T   C E N T R O I D   O F   E L E M E N T\n\n");
	_snprintf(buff, sizeof(buff), "   ELEMENT                   ELEMENT                   ELEMENT                   ELEMENT                   ELEMENT                \n\n");
	logLine.append(buff);
	for (int i = 1; i <= NE; i++){
		_snprintf(buff, sizeof(buff), " %9d %+15.8e", elementContainer[i]->getElementNumber(), elementContainer[i]->getVMAG());
		logLine.append(buff);
		if (i% 5 == 0)
			logLine.append("\n");
	}

	logLine.append("\n\n\n" + string(11, ' ') + "S T E A D Y  -  S T A T E   F L U I D   V E L O C I T Y\n\n");
	logLine.append(string(11, ' ') + "A N G L E 1   AT CENTROID OF ELEMENT, IN DEGREES FROM +X-AXIS TO PROJECTION OF FLOW DIRECTION IN XY-PLANE\n\n");
	_snprintf(buff, sizeof(buff), "   ELEMENT                   ELEMENT                   ELEMENT                   ELEMENT                   ELEMENT                \n\n");
	logLine.append(buff);
	for (int i = 1; i <= NE; i++){
		_snprintf(buff, sizeof(buff), " %9d %+15.8e", elementContainer[i]->getElementNumber(), elementContainer[i]->getVANG1());
		logLine.append(buff);
		if (i% 5 == 0)
			logLine.append("\n");
	}

	logLine.append("\n\n\n" + string(11, ' ') + "S T E A D Y  -  S T A T E   F L U I D   V E L O C I T Y\n\n");
	logLine.append(string(11, ' ') + "A N G L E 2   AT CENTROID OF ELEMENT, IN DEGREES FROM XY-PLANE TO FLOW DIRECTION\n\n");
	_snprintf(buff, sizeof(buff), "   ELEMENT                   ELEMENT                   ELEMENT                   ELEMENT                   ELEMENT                \n\n");
	logLine.append(buff);
	for (int i = 1; i <= NE; i++){
		_snprintf(buff, sizeof(buff), " %9d %+15.8e", elementContainer[i]->getElementNumber(), elementContainer[i]->getVANG2());
		logLine.append(buff);
		if (i % 5 == 0)
			logLine.append("\n");
	}



THOUSAND:
	logWriter->writeContainer.push_back(logLine);
	logLine.clear();
}

void ControlParameters::OUTNOD(){

}

void ControlParameters::OUTOBS(){

}