
#define VIENNACL_HAVE_EIGEN
#include "ControlParameters.h"
#include <iostream>
#include <fstream>
#include <mutex>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>  
#include <iostream>
#include <viennacl/vector.hpp>
#include <viennacl/compressed_matrix.hpp>
#include <viennacl/linalg/ilu.hpp>
#include <viennacl/linalg/gmres.hpp>
#include <atomic>
#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/OrderingMethods>
#define SIZE(a) (sizeof(a)/sizeof(*(a)))
using namespace std;
using namespace Eigen;
typedef SparseMatrix<double> SpMat;
typedef Triplet<double> T;
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

/**
*  <h1>Defining Custom Monitors Functions for Iterative Solvers</h1>
*  Custom monitors for the iterative solvers require two ingredients:
*  First, a structure holding all the auxiliary data we want to reuse in the monitor.
*  Second, a callback function called by the solver in each iteration.
*
*  In this example we define a callback-routine for printing the current estimate for the residual and compare it with the true residual.
*  To do so, we need to pass the system matrix, the right hand side, and the initial guess to the monitor routine, which we achieve by packing pointers to these objects into a struct:
**/
template<typename MatrixT, typename VectorT>
struct monitor_user_data
{
	monitor_user_data(MatrixT const & A, VectorT const & b, VectorT const & guess) : A_ptr(&A), b_ptr(&b), guess_ptr(&guess) {}

	MatrixT const *A_ptr;
	VectorT const *b_ptr;
	VectorT const *guess_ptr;
};

/**
*  The actual callback-routine takes the current approximation to the result as the first parameter, and the current estimate for the relative residual norm as second argument.
*  The third argument is a pointer to our user-data, which in a first step cast to the correct type.
*  If the monitor returns true, the iterative solver stops. This is handy for defining custom termination criteria, e.g. one-norms for the result change.
*  Since we do not want to terminate the iterative solver with a custom criterion here, we always return 'false' at the end of the function.
*
*  Note to type-safety evangelists: This void*-interface is designed to be later exposed through a shared library ('libviennacl').
*  Thus, user types may not be known at the point of compilation, requiring a void*-approach.
**/
template<typename VectorT, typename NumericT, typename MatrixT>
bool my_custom_monitor(VectorT const & current_approx, NumericT residual_estimate, void *user_data)
{
	// Extract residual:
	monitor_user_data<MatrixT, VectorT> const *data = reinterpret_cast<monitor_user_data<MatrixT, VectorT> const*>(user_data);

	// Form residual r = b - A*x, taking an initial guess into account: r = b - A * (current_approx + x_initial)
	VectorT x = current_approx + *data->guess_ptr;
	VectorT residual = *data->b_ptr - viennacl::linalg::prod(*data->A_ptr, x);
	VectorT initial_residual = *data->b_ptr - viennacl::linalg::prod(*data->A_ptr, *data->guess_ptr);

	std::cout << "Residual estimate vs. true residual: " << residual_estimate << " vs. " << viennacl::linalg::norm_2(residual) / viennacl::linalg::norm_2(initial_residual) << std::endl;

	return false; // no termination of iteration
}

const string ControlParameters::Headers[] = { "Node", "X", "Y", "Z", "Pressure", "Concentration", "Saturation", "Eff. Stress", "Stress Rat." };

const string ControlParameters::SOLWRD[] = { "'DIRECT'", "'CG'", "'GMRES'", "'ORTHOMIN'" };
const string ControlParameters::SOLNAM[] = { "BANDED GAUSSIAN ELIMINATION (DIRECT)",
"IC-PRECONDITIONED CONJUGATE GRADIENT",
"ILU-PRECONDITIONED GMRES",
"ILU-PRECONDITIONED ORTHOMIN" };
 
ControlParameters* ControlParameters::m_cInstance = NULL;

unordered_map<string, DataSet *> ControlParameters::getDataSetMap(){
	return this->dataSetMap;
}
void ControlParameters::re_orient_matrix(int jmper_size,int vals_size,double vals[], int jmper[], int indices[], double new_vals[], int new_jmper[], int new_indices[])
{
	int * rr = new int[vals_size];
	int * nn = new int[jmper_size];
	for (int k = 0, i = 0; i < jmper_size; i++)
	for (int j = 0; j < jmper[i + 1] - jmper[i]; j++)
		rr[k++] = i;

	for (int i = 0; i < vals_size; i++)
		new_jmper[indices[i] + 1]++;

	for (int i = 1; i <= jmper_size-1; i++)
		new_jmper[i] += new_jmper[i - 1];

	memcpy(nn, new_jmper, sizeof(int)*jmper_size);

	for (int i = 0; i < vals_size; i++) {
		int x = nn[indices[i]]++;
		new_vals[x] = vals[i];
		new_indices[x] = rr[i];
	}
	delete[]rr;
	delete[]nn;
}
void ControlParameters::re_orient_matrix(int jmper_size,int vals_size,vector<double>& vals,vector<int>&jmper,vector<int>& indices,vector<double>&new_vals,vector<int>& new_jumper,vector<int>&new_indices)
{
	vector<int> rr(vals_size, 0);
	vector<int> nn(jmper_size, 0);
	vector<int> nnew_jmper(jmper_size, 0);
	vector<int> nnew_indices(vals_size, 0);
	vector<double> nnew_vals(vals_size, 0);

	for (int k = 0, i = 0; i < jmper_size-1; ++i)
	for (int j = 0; j < jmper[i + 1] - jmper[i]; ++j)
		rr[k++] = i;

	for (int i = 0; i < vals_size; i++)
		nnew_jmper[indices[i] + 1]++;

	for (int i = 1; i <= jmper_size - 1; i++)
		nnew_jmper[i] += nnew_jmper[i - 1];

	copy(nnew_jmper.begin(), nnew_jmper.end(), nn.begin());
	for (int i = 0; i < vals_size; i++) {
		int x = nn[indices[i]]++;
		nnew_vals[x] = vals[i];
		nnew_indices[x] = rr[i];
	}
	new_vals = nnew_vals;
	new_indices = nnew_indices;
	new_jumper = nnew_jmper;
}

void ControlParameters::createDataSets(){
	vector<string>dataSets{ "DataSet_1", "DataSet_2A", "DataSet_2B", "DataSet_3", "DataSet_4", "DataSet_5", "DataSet_6",
		"DataSet_7A", "DataSet_7B", "DataSet_7C", "DataSet_8ABC", "DataSet_8D", "DataSet_8E_9_10_11", "DataSet_12_13_14A",
		"DataSet_14B", "DataSet_15A", "DataSet_15B", "DataSet_17", "DataSet_18", "DataSet_19", "DataSet_20", "DataSet_22" };
	/*char * dataSets[] = { "DataSet_1","DataSet_2A","DataSet_2B","DataSet_3","DataSet_4","DataSet_5","DataSet_6",
		"DataSet_7A","DataSet_7B","DataSet_7C","DataSet_8ABC","DataSet_8D","DataSet_8E_9_10_11","DataSet_12_13_14A",
		"DataSet_14B","DataSet_15A","DataSet_15B","DataSet_17","DataSet_18","DataSet_19","DataSet_20","DataSet_22"};*/
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

void ControlParameters::setWaterTable(double val){ this->waterTable = val; }
double ControlParameters::getWaterTable(){ return this->waterTable; }


void ControlParameters::setTimeStepDivide(int TimeStepDivide){
	this->timeStepDivide = TimeStepDivide;
}

int ControlParameters::getTimeStepDivide(){
	return this->timeStepDivide;
}

void ControlParameters::setIGOI(int val)
{
	this->IGOI = val;
}
void ControlParameters::setIERR(double val)
{
	this->IERR = val;
}

void ControlParameters::setISTOP(int val)
{
	this->ISTOP = val;
}


double ControlParameters::getIERR()
{
	return this->IERR;
}

int ControlParameters::getIGOI()
{
	return this->IGOI;
}

int ControlParameters::getISTOP()
{
	return this->ISTOP;
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
	//auto node_number = aNode->getNodeNumber();
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
	isNewNod = true;
	isNewObs = true;

	nodeHeaders["'N'"] = ControlParameters::Headers[0];
	nodeHeaders["'X'"] = ControlParameters::Headers[1];
	nodeHeaders["'Y'"] = ControlParameters::Headers[2];
	nodeHeaders["'Z'"] = ControlParameters::Headers[3];
	nodeHeaders["'P'"] = ControlParameters::Headers[4];
	nodeHeaders["'U'"] = ControlParameters::Headers[5];
	nodeHeaders["'S'"] = ControlParameters::Headers[6];
	nodeHeaders["'ES'"] = ControlParameters::Headers[7];
	nodeHeaders["'RU'"] = ControlParameters::Headers[8];

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
	sort(layers.begin(), layers.end(), [](Layer* a, Layer* b){return (abs(a->getLayerBottom()) < abs(b->getLayerBottom())); });

}
//void ControlParameters::addDataSet(string dataSet,string dataSetName){
//	DataSet *dataSetx = new DataSet(dataSetName)
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
	Writer * logWriter = Writer::LSTInstance();
	string logLine = "";
	Miscellaneous::banner();
	/*for (DataSet* x : dataSetContainer){
		x->parseAllLines();
	}
	cout << " parsing all data finished in " << t << " seconds" << endl;*/
	// Parse Data Set 1
	cout << "Parsing Data Set 1 .." << endl;
	dataSetMap["DataSet_1"]->parseAllLines();
	cout << "Done.. Parsing Data Set 1 took " << t << " seconds" << endl;

	// Parse Data Set 2A
	cout << "Parsing Data Set 2A .." << endl;
	t.reset();
	dataSetMap["DataSet_2A"]->parseAllLines();
	cout << "Done.. Parsing Data Set 2A took " << t << " seconds" << endl;
	logLine.append(string(133, '*'));
	logLine.append("\n\n");
	logLine.append(Miscellaneous::setLSTLine("* * * * *   I N D U C E D   P A R T I A L   S A T U R A T I O N   S I M U L A T I O N   * * * * *"));
	logLine.append("\n\n"); 
	logLine.append(string(133, '*'));
	logLine.append("\n\n\n\n\n");
	logWriter->writeContainer.push_back(logLine);
	logLine.clear();

	// Parse Data Set 2B
	cout << "Parsing Data Set 2B .." << endl;
	t.reset();
	dataSetMap["DataSet_2B"]->parseAllLines();
	cout << "Done.. Parsing Data Set 2B took " << t << " seconds" << endl;
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
	cout << "Parsing PROPS.INP file.." << endl;
	t.reset();
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
	cout << "Done.. Parsing Props.INP took " << t << " seconds" << endl;
	
	// ParseData Set 3
	cout << "Parsing Data Set 3 .." << endl;
	t.reset();
	dataSetMap["DataSet_3"]->parseAllLines();
	cout << "Done .. Parsing Data Set 3 took " << t << " seconds" << endl;
	
	// ParseData Set 4
	cout << "Parsing Data Set 4 .." << endl;
	t.reset();
	dataSetMap["DataSet_4"]->parseAllLines();
	cout << "Done .. Parsing Data Set 4 took " << t << " seconds" << endl;
	
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
	cout << "Parsing Data Set 5 .." << endl;
	t.reset();
	dataSetMap["DataSet_5"]->parseAllLines();
	cout << "Done .. Parsing Data Set 5 took " << t << " seconds" << endl;
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
	cout << "Parsing Data Set 6 .." << endl;
	t.reset();
	dataSetMap["DataSet_6"]->parseAllLines();
	cout << "Done .. Parsing Data Set 6 took " << t << " seconds" << endl;
	
	//INPUT DATASET 7A : ITERATION CONTROLS FOR RESOLVING NONLINEARITIES
	cout << "Parsing Data Set 7A .." << endl;
	t.reset();
	dataSetMap["DataSet_7A"]->parseAllLines();
	cout << "Done .. Parsing Data Set 7A took " << t << " seconds" << endl;
	//INPUT DATASET 7b : MATRIX EQUATION SOLVER CONTROLS FOR PRESSURE
	cout << "Parsing Data Set 7B .." << endl;
	t.reset();
	dataSetMap["DataSet_7B"]->parseAllLines();
	cout << "Done .. Parsing Data Set 7B took " << t << " seconds" << endl;

	//INPUT DATASET 7c : MATRIX EQUATION SOLVER CONTROLS FOR TRANSPORT
	cout << "Parsing Data Set 7C .." << endl;
	t.reset();
	dataSetMap["DataSet_7C"]->parseAllLines();
	cout << "Done .. Parsing Data Set 7C took " << t << " seconds" << endl;
	// SOME ALLOCATION IT MAY NOT NECESSARY FOR THIS PROGRAM

	// INPUT DATASETS 8 to 15
	// INPUT DATASET 8A:  OUTPUT CONTROLS AND OPTIONS FOR LST FILE and SCREEN
	cout << "Parsing Data Set 8ABC .." << endl;
	t.reset();
	dataSetMap["DataSet_8ABC"]->parseAllLines();
	cout << "Done .. Parsing Data Set 8ABC took " << t << " seconds" << endl;
	// INPUT DATA  SET 8E: OUTPUT CONTROLS AND OPTIONS FOR BCOF,BCOP,BCOS and BCOU FILES
	cout << "Parsing Data Set 8D .." << endl;
	t.reset();
	dataSetMap["DataSet_8D"]->parseAllLines();
	cout << "Done .. Parsing Data Set 8D took " << t << " seconds" << endl;
	// INPUT DATASET 8 E:
	cout << "Parsing Data Sets 8E,9,10 and 11 .." << endl;
	t.reset();
	dataSetMap["DataSet_8E_9_10_11"]->parseAllLines();
	cout << "Done .. Parsing Data Sets 8E,,9,10 and 11 took " << t << " seconds" << endl;

	// INPUT DATA SET 12 : PRODUCTION OF ENERGY OR SOLUTE MASS
	cout << "Parsing Data Set 12,13 and 14A .." << endl;
	t.reset();
	dataSetMap["DataSet_12_13_14A"]->parseAllLines();
	cout << "Done .. Parsing Data Sets 12,13 and 14A took " << t << " seconds" << endl;
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
	IPBCT = 1;
	if (NPBC > 0){
		//dataSetMap["DataSet_19"]->parseAllLines();
		Bound("DataSet_19");
	}
	IUBCT = 1;
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
//nodePBC = new double[NN + 1]{};
//nodeUBC = new double[NN + 1]{};
nodeQIN = new double[NN + 1]{};
nodeUIN = new double[NN + 1]{};
nodeQUIN = new double[NN + 1]{};
//nodePVEC = new double[NN + 1]{};
//nodeUVEC = new double[NN + 1]{};
nodePVEC = vector<double>(NN + 1, 0);
nodeUVEC = vector<double>(NN + 1, 0);
//nodeVOL = new double[NN + 1]{};
//node_p_rhs = new double[NN]{};
nodeVOL = vector<double>(NN + 1, 0);
node_p_rhs = vector<double>(NN + 1, 0);
node_u_rhs = vector<double>(NN + 1, 0);
nodePBC = vector<double>(NN +1, 0);
nodeUBC = vector<double>(NN +1, 0);
p_solution = vector<double>(NN, 0);
u_solution = vector<double>(NN, 0);
p_rhs = vector<double>(NN, 0);
u_rhs = vector<double>(NN, 0);
F = vector<vector<double>>(N48, vector<double>(N48, 0));
W = vector<vector<double>>(N48, vector<double>(N48, 0));
DWDXG = vector<vector<double>>(N48, vector<double>(N48, 0));
DWDYG = vector<vector<double>>(N48, vector<double>(N48, 0));
DWDZG = vector<vector<double>>(N48, vector<double>(N48, 0));
DFDXG = vector<vector<double>>(N48, vector<double>(N48, 0));
DFDYG = vector<vector<double>>(N48, vector<double>(N48, 0));
DFDZG = vector<vector<double>>(N48, vector<double>(N48, 0));
init_guess = viennacl::scalar_vector<double>(NN,double(0.01));

//p_rhs.reserve(NN);
//u_rhs.reserve(NN);
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
void ControlParameters::setIT(int val){ this->IT = val; }
int ControlParameters::getIT(){ return this->IT; }
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
void ControlParameters::setITREL(double val){ this->ITREL = val; }
void ControlParameters::setITBCS(double val){ this->ITBCS = val; }
int ControlParameters::getITREL(){ return this->ITREL; }
int ControlParameters::getITBCS(){ return this->ITBCS; }
int ControlParameters::getITRST(){ return this->ITRST; }
void ControlParameters::setOnceP(bool val){ this->onceP = val; }
bool ControlParameters::getOnceP(){ return this->onceP; }
void ControlParameters::setDELTLC(double val){
	this->DELTLC = val;
}
void ControlParameters::setTELAPS(double val){ this->TELAPS = val; }
double ControlParameters::getTELAPS(){ return this->TELAPS; }
void ControlParameters::setRELCHG(double val){ this->RELCHG = val; }
double ControlParameters::getRELCHG(){ return this->RELCHG; }
double ControlParameters::getDELTLC(){ return this->DELTLC; }


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
	Writer * logWriter = Writer::LSTInstance();
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
						QINC = stod(strtok(NULL, del));
						if (QINC > 0){
							UINC = stod(strtok(NULL, del));
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
					nodeContainer[nn]->setQINDef(true);
					nodeContainer[nn]->setUINDef(true);
				
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
						QUINC = stod(strtok(NULL, del));
					}
					else{
						QUINC = 0.0;
						IQSOUT = -1;
					}
					IQSOU.push_back(IQCU);
					int nn = abs(IQCU);
					nodeQUIN[nn] = QUINC;
					nodeContainer[nn]->setQUINDef(true);
				

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
	Writer * logWriter = Writer::LSTInstance();
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
					nodeContainer[IDUMA]->setGNUP1(GNUP);
					if (IDUMA > NN){
						exitOnError("INP-19-1");

					}
					if(IPU == NPBC){
						break;
					}

					if (IDUM > 0){
						//aNode = nodeContainer[IDUM];
						IPBC.push_back(IDUMA);
						nodePBC[IDUMA] = stod(strtok(NULL, del));
						nodeUBC[IDUMA] = stod(strtok(NULL, del));
						nodeContainer[IDUMA]->setPBCDef(true);
						nodeContainer[IDUMA]->setUBCDef(true);
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
						IUBC.push_back(IDUMA);
						nodeUBC[IDUMA]= stod(strtok(NULL, del));
						nodeContainer[IDUMA]->setUBCDef(true);
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
	Writer * logWriter = Writer::LSTInstance();
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
void ControlParameters::writeToNODString(string str){
	string filename = InputFiles::Instance()->getInputDirectory() + "\\" + InputFiles::Instance()->filesToWrite["NOD"];
	ofstream outfile;
	if (!isNewNod){//is_file_exist(filename)){
		outfile.open(filename, std::ios::app);

		outfile << str << endl;

	}
	else{
		outfile.open(filename, std::ios_base::out);

		outfile << str << endl;
		isNewNod = false;
	}
}

void ControlParameters::writeToELEString(string str){
	string filename = InputFiles::Instance()->getInputDirectory() + "\\" + InputFiles::Instance()->filesToWrite["ELE"];
	ofstream outfile;
	if (!isNewNod){//is_file_exist(filename)){
		outfile.open(filename, std::ios::app);

		outfile << str << endl;

	}
	else{
		outfile.open(filename, std::ios_base::out);

		outfile << str << endl;
		isNewNod = false;
	}
}


void ControlParameters::writeToOBSString(string str){
	string filename = InputFiles::Instance()->getInputDirectory() + "\\" + InputFiles::Instance()->filesToWrite["OBS"];
	ofstream outfile;
	if (!isNewObs){//is_file_exist(filename)){
		outfile.open(filename, std::ios::app);

		outfile << str << endl;

	}
	else{
		outfile.open(filename, std::ios_base::out);

		outfile << str << endl;
		isNewObs = false;
	}
}

void ControlParameters::writeToIAString(string str){
	string filename = InputFiles::Instance()->getInputDirectory() + "\\" +"IA.txt";
	ofstream outfile;
	if (!isNewIA){//is_file_exist(filename)){
		outfile.open(filename, std::ios::app);

		outfile << str << endl;

	}
	else{
		outfile.open(filename, std::ios_base::out);

		outfile << str << endl;
		isNewIA = false;
	}
}


void ControlParameters::popWriteContainer(){
	writeContainer.pop_front();
}
void ControlParameters::parseDataSet_14B(){
	string logLine = "";
	Writer * logWriter = Writer::LSTInstance();
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
			for (int vi = 1; vi < sizeof(nodeX); vi++){
				_snprintf(buff, sizeof(buff), "         %9d   %+14.5e   %+14.5e   %14.5e      %9.5f\n", vi,nodeX[vi],nodeY[vi],nodeZ[vi],nodePorosity[vi]);
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
	Writer * logWriter = Writer::LSTInstance();
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
		auto size = dataSetPosition.second - dataSetPosition.first;
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
		//auto size = dataSetPosition.second - dataSetPosition.first;
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
		for (vector<ObservationPoints *>::iterator itx = observationPointsContainer.begin()+b; itx != observationPointsContainer.begin()+e+1; ++itx){
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
		nodeContainer[i]->setVOL(nodeVOL[i]);
		nodeContainer[i]->setSWT(1.0);
		nodeContainer[i]->setTOTSTR(0);
		nodeContainer[i]->setPOREP(0);
		nodeContainer[i]->setCNUB(0);

			if (KTYPE[0] == 3){
				if (abs(nodeContainer[i]->getZCoord()) == 0){
					nodeContainer[i]->setLayer(layers[0],0);
				}
				else{
					const auto size_layers = static_cast<int>(layers.size());
					for (int j = 0; j < size_layers;j++){
					if (abs(nodeContainer[i]->getZCoord()) <= abs(layers[j]->getLayerBottom()) && abs(nodeContainer[i]->getZCoord()) > abs(layers[j]->getLayerTop())){
						nodeContainer[i]->setLayer(layers[j],j);

					}
				}
			}
			}
			else{

			}

			for (int j = 0; j < nodeContainer[i]->getLayerN(); j++){
				nodeContainer[i]->setTOTSTR((abs(layers[j]->getLayerBottom()) - abs(layers[j]->getLayerTop()))*layers[j]->getLayerUnitWeight() + nodeContainer[i]->getTOTSTR());
				if (layers[j]->getSatCond() == 0)
					nodeContainer[i]->setPOREP((abs(layers[j]->getLayerBottom()) - abs(waterTable)) * 9810 + nodeContainer[i]->getPOREP());
				if (layers[j]->getSatCond() == 1)
					nodeContainer[i]->setPOREP((abs(layers[j]->getLayerBottom()) - abs(layers[j]->getLayerTop())) * 9810 + nodeContainer[i]->getPOREP());
			}

			nodeContainer[i]->setTOTSTR((abs(nodeContainer[i]->getZCoord()) - abs(layers[nodeContainer[i]->getLayerN()]->getLayerTop()))*layers[nodeContainer[i]->getLayerN()]->getLayerUnitWeight() + nodeContainer[i]->getTOTSTR());
			if (layers[nodeContainer[i]->getLayerN()]->getSatCond() == 0){
				if (abs(nodeContainer[i]->getZCoord()) > abs(waterTable)){
					nodeContainer[i]->setPOREP((abs(nodeContainer[i]->getZCoord()) - abs(waterTable)) * 9810 + nodeContainer[i]->getPOREP());
				}

			}
				
			if (layers[nodeContainer[i]->getLayerN()]->getSatCond() == 1)
				nodeContainer[i]->setPOREP((abs(nodeContainer[i]->getZCoord()) - abs(layers[nodeContainer[i]->getLayerN()]->getLayerTop())) * 9810 + nodeContainer[i]->getPOREP());

			nodeContainer[i]->setEFFSTR(nodeContainer[i]->getTOTSTR() - nodeContainer[i]->getPOREP());
			nodeContainer[i]->setEFFSTRI(nodeContainer[i]->getEFFSTR());
			if (abs(nodeContainer[i]->getEFFSTRI()) != 0){
				nodeContainer[i]->setSTRRAT(nodeContainer[i]->getEFFSTR() / nodeContainer[i]->getEFFSTRI());
			}
			else{
				nodeContainer[i]->setSTRRAT(1.0);
			}

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
			elementContainer[i]->setGXSI(new double[8]);
			elementContainer[i]->setGETA(new double[8]);
			elementContainer[i]->setGZET(new double[8]);
			elementContainer[i]->setDET(new double[8]);

		}
		VXG = vector<double>(8, 0);
		VYG = vector<double>(8, 0);
		VZG = vector<double>(8, 0);
		VGMAG = vector<double>(8, 0);
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
	Writer * logWriter = Writer::LSTInstance();
	char buff[512];

	logLine.append("\n\n\n\n           **** MESH ANALYSIS ****\n\n");
	int NDIF = 0;
	int ielo;
	int iehi;
	ielo = iehi = 0;
	for (int i = 1; i <= NE; i++){
		ielo = elementNodes[i][0];
		iehi = elementNodes[i][0];
		for (int j = 1; j < N48; j++){
			if (elementNodes[i][j] < ielo) ielo = elementNodes[i][j];
			if (elementNodes[i][j] > iehi) iehi = elementNodes[i][j];
		}
		if ((iehi - ielo) > NDIF){
			NDIF = iehi - ielo;
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
void ControlParameters::loadBCS(){
	BCSFL.reserve(ITMAX+1);
	BCSTR.reserve(ITMAX+1);
	for (int i = 0; i <= ITMAX; i++)
	{
		BCSFL.push_back(false);
		BCSTR.push_back(false);
	}
	char * map = InputFileParser::Instance()->mapViewOfBCSFile;
	char * start_str = map;
	char * end_str;
	int size = strlen(map);
	vector<char>lineBuffer;
	vector<string>lines;
	const char* del = " ";
	const char* diez = "#";
	// Lets put all lines into vector;
	for (int i = 0; i < size; i++){
		if (map[i] == '\n'){
			end_str = map + i;
			lineBuffer.assign(start_str, end_str);
			lineBuffer.push_back('\0');
			lines.push_back(string(start_str, end_str));
			start_str = map + i + 1;
			
		}
	}	
	// First Line definition Schedule Name, thus omit this line
	// Second Line actual schedule Name Parameters
	BCSSCH = lines[1].substr(0, lines[1].find_last_of('\'')+1);
	BCSSCH.erase(boost::remove_if(BCSSCH,boost::is_any_of("'") ), BCSSCH.end());
	

	for (Schedule * sch : listOfSchedules){
		if (sch->getScheduleName() == BCSSCH){
			BCSSchedule = sch;
			break;
		}
	}

	if (BCSSchedule == nullptr){
		exitOnError("BCS-1-1");
	}


	// Check File and Create BCS objects
	vector<int> timeStepPositions;
	for (int i = 0; i < lines.size(); i++){
		const string ch_str = lines[i].substr(0, 7);
		if (ch_str == "# BCSID"){
			timeStepPositions.push_back(i + 1);
		}	
	}

	// parse defined TS informations and create BCS and store in container
	vector<string> data;
	for (int j : timeStepPositions){
		boost::split(data, lines[j], boost::is_any_of(" "), boost::token_compress_on);
		BCS * bcs = new BCS();
		bcs->setScheduleName(BCSSCH);
		bcs->setBCSID(data[1]);
		size_t n = data[1].find_first_of("0123456789");
		string ssTs;
		if (n != string::npos){
			size_t m = data[1].find_first_not_of("0123456789", n);
			ssTs = data[1].substr(n, m != string::npos ? m - n : m);
		}
		bcs->setTimeStep(stoi(ssTs));
		bcs->setNumberOfQINC(stoi(data[2]));
		bcs->setNumberOfUINC(stoi(data[3]));
		bcs->setNumberOfPBC(stoi(data[4]));
		bcs->setNumberOfUBC(stoi(data[5]));

		bcsContainer.push_back(bcs);
	}
	
	// Read Node and Condition
	int i = 0;
	for (int k:timeStepPositions){
		int iNode;
		int j = 0;
		vector<string> bcsData;
		do{
			boost::split(bcsData, lines[k + 3 + j], boost::is_any_of(" \t\r\n"), boost::token_compress_off);
			iNode = stoi(bcsData[0]);
			if (iNode == 0)
				break;
			if (iNode > NN){
				exitOnError("BCS-3-1");
			}

			if (iNode > 0){
				bcsContainer[i]->addNode(iNode);
				if (bcsContainer[i]->getNumberOfQINC() > 0){
					double QINC = stod(bcsData[1]);
					if (QINC > 0){
						bcsContainer[i]->addQINC(QINC);
						bcsContainer[i]->addIsQINC(true);
						double UINC = stod(bcsData[2]);
						bcsContainer[i]->addIsUINC(true);
						bcsContainer[i]->addUINC(UINC);
					}
					else{
						bcsContainer[i]->addQINC(QINC);
						bcsContainer[i]->addIsQINC(true);
						bcsContainer[i]->addIsUINC(false);
						bcsContainer[i]->addUINC(0.0);
					}
				}

				if (bcsContainer[i]->getNumberOfQUINC() > 0){
					double QUINC = stod(bcsData[1]);
					
						bcsContainer[i]->addQINC(QUINC);
						bcsContainer[i]->addIsQUINC(true);
				}


				if (bcsContainer[i]->getNumberOfPBC() > 0){
					double PBC = stod(bcsData[1]);
					double UBC = stod(bcsData[2]);

					bcsContainer[i]->addPBC(PBC);
					bcsContainer[i]->addIsPBC(true);
					bcsContainer[i]->addUBC(UBC);
					bcsContainer[i]->addIsUBC(true);
				}

				if (bcsContainer[i]->getNumberOfUBC() > 0){
					double UBC = stod(bcsData[1]);
					bcsContainer[i]->addUBC(UBC);
					bcsContainer[i]->addIsUBC(true);
				}

				
			}
			else{
				bcsContainer[i]->addNode(abs(iNode));
				bcsContainer[i]->addIsPBC(false);
				bcsContainer[i]->addIsQINC(false);
				bcsContainer[i]->addIsQUINC(false);
				bcsContainer[i]->addIsUBC(false);
				bcsContainer[i]->addIsUINC(false);
				bcsContainer[i]->addPBC(0.0);
				bcsContainer[i]->addUBC(0.0);
				bcsContainer[i]->addQUINC(0.0);
				bcsContainer[i]->addQINC(0.0);
				bcsContainer[i]->addUINC(0.0);
			}
			j = j + 1;
		} while (iNode != 0);

	
		cout << "BCS extracted for Time Step : " << bcsContainer[i]->getTimeStep() << endl;
		i = i + 1;
	}


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

		
		DELTP = DELT*1.e16;
		DELTU = DELT*1.e16;

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

void ControlParameters::OUTLST3(int ML, int ISTOP,int IGOI, int IERRP,int ITRSP,double ERRP, int IERRU,int ITRSU,double ERRU ){
	//args ML, ISTOP, IGOI, IERRP, ITRSP, ERRP, IERRU, ITRSU, ERRU, PVEC, UVEC, VMAG, VANG1, VANG2, SW, SWT, SWB
	string logLine = "";
	Writer * logWriter = Writer::LSTInstance();
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
	_snprintf(buff, sizeof(buff),
	          "           OF STEP:      %+15.4e MINUTES\n                         %+15.4e HOURS\n                         %+15.4e DAYS\n                         %+15.4e WEEKS\n                         %+15.4e MONTHS\n                         %+15.4e YEARS\n",
	          TMIN, THOUR, TDAY, TWEEK, TMONTH, TYEAR);
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


void ControlParameters::setOnceNOD(bool val){ this->onceNOD = val; }
bool ControlParameters::getOnceNOD(){ return this->onceNOD; }
void ControlParameters::setOnceOBS(bool val){ this->onceOBS = val; }
bool ControlParameters::getOnceOBS(){ return this->onceOBS; }

void ControlParameters::OUTNOD(){

	string logLine = "";
	Writer * logWriter = Writer::NODInstance();
	char buff[512];
	int KT,KTMAX; //Number of printed time steps
	int JT; // Time Step Value
	int TS; // Time Step Information
	double DELTK; // time sstep increment
	vector<double> TT;
	vector<int> ITT;
	vector<int> ISHORP, ISTORC, ISSATU;
	int LCHORP, LCTORC;
	char CPSATU, CPTORC, CPHORP;
	//TT.push_back(0.0);
	//ITT.push_back(0);
	//ISHORP.push_back(0);
	//ISTORC.push_back(0);
	//ISSATU.push_back(0);


	string header = "\nNode              X              Y              Z       Pressure  Concentration     Saturation\n";

	if (onceNOD == false){

		if (ISSTRA != 1){
			KT = 1;
		}
		else{
			KT = 0;
		}
		for (int i = 1; i < ITMAX; i++){
			if (((i%NCOLPR) == 0) || (i == ITRST) || ((i == (ITRST + 1)) && ((ISSTRA != 0) || (NCOLPR >0)))){
				KT = KT + 1;
			}
		}
		if (ITMAX > 1 && (ITMAX%NCOLPR != 0))
			KT = KT + 1;

			KTMAX = KT;
			TS = TSTART;
			JT = 0;
			KT = 0;
			DELTK = DELT;

			

			// pressure conc sat print y or no
			// KPANDS pressure and sat
			if (KPANDS == 1){
				CPHORP = CPSATU = 'Y';
			}
			else{ CPHORP = CPSATU = 'N'; }
			if (KCORT == 1){
				CPTORC = 'Y';

			}
			else{ CPTORC = 'N'; }
			//KCORD concentration

			if (ISSTRA != 1){
				KT = KT + 1;
				TT.push_back(TS);
				ITT.push_back(JT);
				ISHORP.push_back(0);
				ISTORC.push_back(0);
				ISSATU.push_back(0);
			}

			for (int i = 0; i <= ITMAX; i++){
				TS = getTIMESTEPSSchedule()->getSList()[i].first;
				JT = getTIMESTEPSSchedule()->getSList()[i].second;

				if ((JT%NPCYC == 0) || (BCSFL[JT] || JT == 1))
					LCHORP = JT;
				if ((JT%NUCYC == 0) || (BCSTR[JT] || JT == 1))
					LCTORC = JT;
				if ((JT%NCOLPR == 0) || (JT == ITRST) || ((JT == (ITRST + 1)) && ((ISSTRA != 0) || NCOLPR >0))){
					KT = KT + 1;
				TT.push_back(TS);
				ITT.push_back(JT);
				ISHORP.push_back(LCHORP);
				ISTORC.push_back(LCTORC);
				ISSATU.push_back(LCHORP);
			}
			}

			if (ISSTRA == 1){
				TT.push_back(TSTART);
				ITT.push_back(0);
			}

			if (ITMAX > 1 && (ITMAX%NCOLPR != 0)){
				KT = KT + 1;
				TS = getTIMESTEPSSchedule()->getSList()[ITMAX].first;
				TT.push_back(TS);
				ITT.push_back(ITMAX);
				if ((ITMAX%NPCYC == 0) || (BCSFL[ITMAX]))
					LCHORP = ITMAX;
				if ((ITMAX%NPCYC == 0) || (BCSTR[ITMAX]))
					LCTORC = ITMAX;
				ISHORP.push_back(LCHORP);
				ISTORC.push_back(LCTORC);
				ISSATU.push_back(LCHORP);
			}

			if (ISSFLO != 0){
				for (int i = 0; i < KTMAX; i++){
					ISHORP.push_back(0);
					ISSATU.push_back(0);
				}
			}

			if (IREAD == 1){ KTPRN = KTMAX; }
			else{ KTPRN = 0;
			for (int i = 1; i <= KTMAX; i++){
				if (ITT[i] > ITRST)
					KTPRN = KTPRN + 1;
			}
			}

			logLine.append("## " + TITLE1 + "\n");
			logLine.append("## " + TITLE2 + "\n");
			logLine.append("## \n");

			string CTYPE2;
			if (KTYPE[1] > 1){

				if (KTYPE[1] == 3) { CTYPE2 = "BLOCKWISE MESH"; }
				else{ CTYPE2 = "REGULAR MESH"; }

				if (KTYPE[0] == 3){ // 3D
					_snprintf(buff, sizeof(buff), "## %1d-D,", KTYPE[0]);
					logLine.append(buff + CTYPE2);
					_snprintf(buff, sizeof(buff), "  (%9d)*(%9d)*(%9d) = %9d Nodes ( %9d Elems)\n", NN1, NN2, NN3, NN, NE);
					logLine.append(buff);
					logLine.append("## \n");
				}
				else{ // 2D
					_snprintf(buff, sizeof(buff), "## %1d-D,", KTYPE[0]);
					logLine.append(buff + CTYPE2);
					_snprintf(buff, sizeof(buff), "  (%9d)*(%9d) = %9d Nodes ( %9d Elems)\n", NN1, NN2, NN, NE);
					logLine.append(buff);
					logLine.append("## \n");

				}

			}
			else if (KTYPE[1] == 1){
				_snprintf(buff, sizeof(buff), "## %1d-D, LAYERED MESH [", KTYPE[0]);
				logLine.append(buff);
				logLine.append(LAYSTR + "]");
				_snprintf(buff, sizeof(buff), "       (%9d)*(%9d) = %9d Nodes ( %9d Elems)\n", NLAYS, NNLAY, NN, NE);
				logLine.append(buff);
				logLine.append("## \n");
			}
			else{
				_snprintf(buff, sizeof(buff), "## % 1d - D, IRREGULAR MESH", KTYPE[0]);
				logLine.append(buff);
				logLine.append(string(40, ' '));
				_snprintf(buff, sizeof(buff), "%9d Nodes ( %9d Elems)\n", NN, NE);
				logLine.append(buff);
				logLine.append("## \n");
			}


			logLine.append("## " + string(92, '=') +"\n## NODEWISE RESULTS" + string(48,' '));
			_snprintf(buff, sizeof(buff), "%9d Time steps printed\n",KTPRN);
			logLine.append(buff);
			logLine.append("## " + string(92, '=') + "\n## \n");
			logLine.append("##    Time steps" + string(23, ' ') + "[Printed? / Latest time step computed]\n");
			logLine.append("##    in this file      Time (sec)         Press          Conc           Sat\n");
			logLine.append("##   " + string(14, '-') + "   " + string(13, '-') + "    " + string(12, '-') + "   " + string(12, '-') + "   " + string(12, '-') + "\n");
			
			

			for (int i = 1; i <= KTMAX; i++){
				if (ITT[i] >= ITRST){
					_snprintf(buff, sizeof(buff), "##        %8d    %+13.6e      %c %8d     %c %8d     %c %8d\n",ITT[i],TT[i],CPHORP,ISHORP[i],CPTORC,ISTORC[i],CPSATU,ISSATU[i]);
					logLine.append(buff);
				}
			}



			onceNOD = true;
	}
	
	if ((IT == 0) || ((IT == 1) && (ISSTRA == 1))){
		DURN = 0.0;
		TOUT = TSTART;
		ITOUT = 0;
	}
	else{
		DURN = DELT;
		TOUT = TSEC;
		ITOUT = IT;
	}

	logLine.append("## \n");
	logLine.append("## " + string(98, '=')+"\n");
	_snprintf(buff, sizeof(buff), "## TIME STEP %8d",ITOUT);
	logLine.append(buff + string(26,' '));
	_snprintf(buff, sizeof(buff), "Duration: %+11.4e sec      Time: %+11.4e sec\n", DURN,TOUT);
	logLine.append(buff);
	logLine.append("## " + string(98, '=') + "\n");


	

		
	

	//if (NCOL[0] == "'N'"){ // printNodeNumber
		logLine.append("##");
		for (string a : NCOL){
			int fil = 15- nodeHeaders[a].length();
			logLine.append(string(fil, ' ') + nodeHeaders[a] + "  ");
		}

		logLine.append("\n");

		for (int i = 1; i <= NN; i++){
	
			_snprintf(buff, sizeof(buff), "  %+14.7e  %+14.7e  %+14.7e  %+14.7e  %+14.7e  %+14.7e  %+14.7e  %+14.7e\n",nodeContainer[i]->getXCoord(), nodeContainer[i]->getYCoord(), nodeContainer[i]->getZCoord(), nodeContainer[i]->getPVEC(), nodeContainer[i]->getUVEC(), nodeContainer[i]->getSWT(), nodeContainer[i]->getEFFSTR(), nodeContainer[i]->getSTRRAT());
			logLine.append(buff);
		}


		logWriter->writeContainer.push_back(logLine);
	

}

void ControlParameters::OUTOBS(){
	string logLine = "";
	Writer * logWriter = Writer::OBSInstance();
	char buff[512];
	int kt;
	int KTMAX;
	int LCNT;
	double TJT,SJT;
	bool IMPRTD = false;
	double LCHORP, LCTORC, TOB, SOB;
	vector<double> TT;
	vector<int> DITT;
	vector<int> ISHORP, ISTORC, ISSATU;


	Schedule * tmp = nullptr;
	if (onceOBS == false){
		// First Call for this File - Create File Header
		// If no observation points, write message and return
		if (observationPointsContainer.size() == 0){
			logLine.append("## " + TITLE1 + "\n");
			logLine.append("## " + TITLE2 + "\n");
			logLine.append("## \n");
			logLine.append("\n  *** NO OBSERVATION POINTS SPECIFIED (NOBS=0) ***");
			onceOBS = true;
			return;
		}

		for (Schedule * sch : listOfSchedules){
			if (sch->getScheduleName() == "Timed_Obs"){
				tmp = sch;
				break;
			}
		}

		if (ISSTRA != 0){

		}

		//if (ISSTRA != 0){
		//	KTMAX = 1;
		//	IMPRTD = true;
		//}
		//else{

		//	KTMAX = 2;
		//	IMPRTD = false;
		//	// Find obs Schedule
		//	
		//	for (Schedule * sch : listOfSchedules){
		//		if (sch->getScheduleName() == "Timed_Obs"){
		//			tmp = sch;
		//			break;
		//		}
		//	}

		//	int sz = tmp->getSList().size();
		//	if (sz != 0){		
		//			KTMAX = KTMAX + 1;
		//			SOB = tmp->getSList()[0].second;
		//			if (SOB == ITRST){
		//				KTMAX = KTMAX + 1;
		//			}
		//			for (int i = 1; i < sz; i++){
		//				KTMAX = KTMAX + 1;
		//			}
		//			SOB = tmp->getSList()[sz-1].second;
		//			if (SOB == ITMAX){
		//				KTMAX = KTMAX - 1;
		//				IMPRTD = true;
		//			}

		//	}

		//}

		//

		//if (ISSTRA != 0){
		//	KT = 1;
		//	TT.push_back(TSTART);
		//	DITT.push_back(1.0);
		//	ISHORP.push_back(0);
		//	ISTORC.push_back(1);
		//	ISSATU.push_back(0);
		//}
		//else{
		//	KT = 0;
		//	for (Schedule * sch : listOfSchedules){
		//		if (sch->getScheduleName() == "Timed_Obs"){
		//			tmp = sch;
		//			break;
		//		}
		//	}

		//	LCNT = 1;
		//	TOB = tmp->getSList()[0].first;
		//	SOB = tmp->getSList()[0].second;
		//	LCTORC = 0;
		//	LCHORP = 0;

		//	if (ITRST > 0){
		//		for (int i = 0; i < ITRST; i++){
		//			if (i%NPCYC == 0 || BCSFL[i] || i == 1)
		//				LCHORP = i; 
		//			if (i%NUCYC == 0 || BCSTR[i] || i == 1)
		//				LCTORC = i;
		//		}
		//	}
		//	TJT = tmp->getSList()[ITRST].first;
		//	SJT = tmp->getSList()[ITRST].second;
		//	int j = 1;
		//	while ((SOB < SJT) && (LCNT < tmp->getSList().size())){
		//		TOB = tmp->getSList()[j].first;
		//		SOB = tmp->getSList()[j].second;
		//		LCNT = LCNT + 1;
		//		j++;
		//	}

		//	KT = KT + 1;
		//	TT.push_back(TJT);
		//	DITT.push_back(SJT);
		//	ISHORP.push_back(LCHORP);
		//	ISTORC.push_back(LCTORC);
		//	ISSATU.push_back(LCHORP);

		//	if ((SOB == SJT) && (LCNT < tmp->getSList().size())){
		//		TOB = tmp->getSList()[0].first;
		//		SOB = tmp->getSList()[0].second;
		//		LCNT = LCNT + 1;
		//	}

		//	for (int i = ITRST+1; i < ITMAX; i++){
		//		TJT = tmp->getSList()[i].first;
		//		SJT = tmp->getSList()[i].second;
		//		if (i%NPCYC == 0 || BCSFL[i] || i == 1)
		//			LCHORP = i;
		//		if (i%NUCYC == 0 || BCSTR[i] || i == 1)
		//			LCTORC = i;

		//		int j = 0;
		//		while ((SOB <= i) && (LCNT < tmp->getSList().size())){
		//			KT = KT + 1;
		//			TT.push_back(TOB);
		//			DITT.push_back(SOB);
		//			ISHORP.push_back(LCHORP);
		//			ISTORC.push_back(LCTORC);
		//			ISSATU.push_back(LCHORP);
		//			if (LCNT < tmp->getSList().size()){
		//				TOB = tmp->getSList()[j].first;
		//				SOB = tmp->getSList()[j].second;
		//			}
		//			
		//			LCNT = LCNT + 1;
		//			j++;
		//		}

		//	}

		//	if (IMPRTD == false){
		//		KT = KT + 1;
		//		TT.push_back(TJT);
		//		DITT.push_back(SJT);
		//		ISHORP.push_back(ITMAX);
		//		ISSATU.push_back(ITMAX);
		//		ISTORC.push_back(ITMAX);
		//	}

		//	if (ISSFLO != 0){
		//		for (int KT = 0; KT < KTMAX; KT++){
		//			ISHORP[KT] = 0;
		//			ISSATU[KT] = 0;
		//		}
		//	}

		//	if (IREAD == 1){
		//	
		//		KTPRN = KTMAX;

		//	}
		//	else{
		//		KTPRN = 0;
		//		for (int i = 0; i < KTMAX; i++){
		//			if (DITT[i] >= ITRST)
		//				KTPRN = KTPRN + 1;
		//		}
		//	}

		//	logLine.append("## " + TITLE1 + "\n");
		//	logLine.append("## " + TITLE2 + "\n");
		//	logLine.append("## \n");

		//	string CTYPE2;
		//	if (KTYPE[1] > 1){

		//		if (KTYPE[1] == 3) { CTYPE2 = "BLOCKWISE MESH"; }
		//		else{ CTYPE2 = "REGULAR MESH"; }

		//		if (KTYPE[0] == 3){ // 3D
		//			_snprintf(buff, sizeof(buff), "## %1d-D,", KTYPE[0]);
		//			logLine.append(buff + CTYPE2);
		//			_snprintf(buff, sizeof(buff), "  (%9d)*(%9d)*(%9d) = %9d Nodes ( %9d Elems)\n", NN1, NN2, NN3, NN, NE);
		//			logLine.append(buff);
		//			logLine.append("## \n");
		//		}
		//		else{ // 2D
		//			_snprintf(buff, sizeof(buff), "## %1d-D,", KTYPE[0]);
		//			logLine.append(buff + CTYPE2);
		//			_snprintf(buff, sizeof(buff), "  (%9d)*(%9d) = %9d Nodes ( %9d Elems)\n", NN1, NN2, NN, NE);
		//			logLine.append(buff);
		//			logLine.append("## \n");

		//		}

		//	}
		//	else if (KTYPE[1] == 1){
		//		_snprintf(buff, sizeof(buff), "## %1d-D, LAYERED MESH [", KTYPE[0]);
		//		logLine.append(buff);
		//		logLine.append(LAYSTR + "]");
		//		_snprintf(buff, sizeof(buff), "       (%9d)*(%9d) = %9d Nodes ( %9d Elems)\n", NLAYS, NNLAY, NN, NE);
		//		logLine.append(buff);
		//		logLine.append("## \n");
		//	}
		//	else{
		//		_snprintf(buff, sizeof(buff), "## % 1d - D, IRREGULAR MESH", KTYPE[0]);
		//		logLine.append(buff);
		//		logLine.append(string(40, ' '));
		//		_snprintf(buff, sizeof(buff), "%9d Nodes ( %9d Elems)\n", NN, NE);
		//		logLine.append(buff);
		//		logLine.append("## \n");
		//	}


		//	logLine.append("## " + string(92, '=') + "\n## OBSERVATION POINT RESULTS" + string(48, ' '));
		//	_snprintf(buff, sizeof(buff), "%9d Time steps printed\n", KTPRN);
		//	logLine.append(buff);
		//	logLine.append("## " + string(92, '=') + "\n## \n");
		//	logLine.append("##    Time steps" + string(23, ' ') + "[Printed? / Latest time step computed]\n");
		//	logLine.append("##    in this file      Time (sec)         Press          Conc           Sat           Eff. Stress           Stress Rat.\n");
		//	logLine.append("##   " + string(14, '-') + "   " + string(13, '-') + "    " + string(12, '-') + "   " + string(12, '-') + "   " + string(12, '-') + "   " + string(12, '-') + "   " + string(12, '-') + "\n");










		//}

		//}

		logLine.append("## " + TITLE1 + "\n");
		logLine.append("## " + TITLE2 + "\n");
		logLine.append("## \n");

		string CTYPE2;
		if (KTYPE[1] > 1){

			if (KTYPE[1] == 3) { CTYPE2 = "BLOCKWISE MESH"; }
			else{ CTYPE2 = "REGULAR MESH"; }

			if (KTYPE[0] == 3){ // 3D
				_snprintf(buff, sizeof(buff), "## %1d-D,", KTYPE[0]);
				logLine.append(buff + CTYPE2);
				_snprintf(buff, sizeof(buff), "  (%9d)*(%9d)*(%9d) = %9d Nodes ( %9d Elems)\n", NN1, NN2, NN3, NN, NE);
				logLine.append(buff);
				logLine.append("## \n");
			}
			else{ // 2D
				_snprintf(buff, sizeof(buff), "## %1d-D,", KTYPE[0]);
				logLine.append(buff + CTYPE2);
				_snprintf(buff, sizeof(buff), "  (%9d)*(%9d) = %9d Nodes ( %9d Elems)\n", NN1, NN2, NN, NE);
				logLine.append(buff);
				logLine.append("## \n");

			}

		}
		else if (KTYPE[1] == 1){
			_snprintf(buff, sizeof(buff), "## %1d-D, LAYERED MESH [", KTYPE[0]);
			logLine.append(buff);
			logLine.append(LAYSTR + "]");
			_snprintf(buff, sizeof(buff), "       (%9d)*(%9d) = %9d Nodes ( %9d Elems)\n", NLAYS, NNLAY, NN, NE);
			logLine.append(buff);
			logLine.append("## \n");
		}
		else{
			_snprintf(buff, sizeof(buff), "## % 1d - D, IRREGULAR MESH", KTYPE[0]);
			logLine.append(buff);
			logLine.append(string(40, ' '));
			_snprintf(buff, sizeof(buff), "%9d Nodes ( %9d Elems)\n", NN, NE);
			logLine.append(buff);
			logLine.append("## \n");
		}


		logLine.append("## " + string(108, '=') + "\n## OBSERVATION POINT RESULTS" + string(55, ' '));
		_snprintf(buff, sizeof(buff), "%9d Time steps printed\n", tmp->getSList().size());
		logLine.append(buff);
		logLine.append("## " + string(108, '=') + "\n## \n");
		logLine.append("##     Time steps" + string(32, ' ') + "[Latest time step computed]\n");
		logLine.append("##    in this file      Time (sec)         Press          Conc           Sat        Eff. Stress    Stress Rat.\n");
		logLine.append("##   " + string(14, '-') + "   " + string(13, '-') + "    " + string(12, '-') + "   " + string(12, '-') + "   " + string(12, '-') + "   " + string(12, '-') + "   " + string(12, '-') + "\n");
		
		
		for (pair<double, double> p : tmp->getSList()){
			_snprintf(buff, sizeof(buff), "## %8d   %+13.6e       %8d       %8d       %8d       %8d       %8d\n", (int)p.second, p.first, (int)p.second, (int)p.second, (int)p.second, (int)p.second, (int)p.second);
			logLine.append(buff);
		}
		logLine.append("##\n## " + string(108, '=') + "\n## \n");
		logLine.append("##                                   ");
		
		for (ObservationPoints* p : observationPointsContainer){
			string oName = p->getOBSNAM();
			int lName = oName.size();
			int firstFill, secondFill;
			firstFill = (76 - lName) / 2;
			secondFill = 76 - firstFill - lName;
			logLine.append(string(firstFill,' ') + oName + string(secondFill, ' ') + "    ");
		}
		logLine.append("\n##                                   ");
		for (ObservationPoints* p : observationPointsContainer){
			logLine.append(string(76,'-') + "    ");
		}
		logLine.append("\n##                                   ");
		for (ObservationPoints* p : observationPointsContainer){
			_snprintf(buff, sizeof(buff), "(         %+14.7e,         %+14.7e,         %+14.7e)    ", p->getXOBS(), p->getYOBS(), p->getZOBS());
			logLine.append(buff);
		}
		logLine.append("\n##                                   ");
		for (ObservationPoints* p : observationPointsContainer){
			logLine.append(string(76, '-') + "    ");
		}
		logLine.append("\n##       Time Step     Time (sec)    ");
		for (ObservationPoints* p : observationPointsContainer){
			logLine.append("      Pressure  Concentration     Saturation     Eff.Stress    Stress Rat.      ");
		}
		//logLine.append("\n");



		logWriter->writeContainer.push_back(logLine);


		onceOBS = true;
	}
	logLine.clear();
	if ((NOBSN - 1) == 0){
		return;
	}

	double SFRAC = IT - ceil(IT) + 1.0;
	if ((IT == 0) || ((IT == 1.0) && (ISSTRA == 1))){
		TOUT = TSTART;
	}
	else{
		TOUT = TIME;
	}

	_snprintf(buff, sizeof(buff), "   %15.5f  %14.6e   ",(double)IT,(double)TOUT);
	logLine.append(buff);
	  for (ObservationPoints* p : observationPointsContainer){
		  logLine.append(PUSWF(p,SFRAC));

		/*  logLine.append(p->getOBSNAM() + " obs Element : ");
		  logLine.append(to_string(p->getObsElement()) + " Nodes : ");
		  Element * el = elementContainer[p->getObsElement()];
		  int * elNodes = el->getElementNodes();
		  for (int i = 0; i < sizeof(elNodes); i++){
			  logLine.append(to_string(elNodes[i]) + " ");
		  }
		  logWriter->writeContainer.push_back(logLine);
		  logLine.clear();*/

	  }
	logLine.append("\n");
	logWriter->writeContainer.push_back(logLine);

	}



void ControlParameters::setTIME(double val){ this->TIME = val; }
double ControlParameters::getTIME(){ return this->TIME; }


int ControlParameters::getML(){ return this->ML; }
double ControlParameters::getDLTPM1(){ return this->DLTPM1; }
double ControlParameters::getDLTUM1(){ return this->DLTUM1; }
double ControlParameters::getBDELP1(){ return this->BDELP1; }
double ControlParameters::getBDELP(){ return this->BDELP; }
double ControlParameters::getBDELU(){ return this->BDELU; }
double ControlParameters::getBDELU1(){ return this->BDELU1; }
void ControlParameters::setML(int val){ this->ML = val; }
void ControlParameters::setDLTPM1(double val){ this->DLTPM1 = val; }
void ControlParameters::setDLTUM1(double val){ this->DLTUM1 = val; }
void ControlParameters::setBDELP1(double val){ this->BDELP1 = val; }
void ControlParameters::setBDELP(double val){ this->BDELP = val; }
void ControlParameters::setBDELU(double val){ this->BDELU = val; }
void ControlParameters::setBDELU1(double val){ this->BDELU1 = val; }
void ControlParameters::setNOUMAT(int val){ this->NOUMAT = val; }
int ControlParameters::getNOUMAT(){ return this->NOUMAT; }
void ControlParameters::setITER(int val){ this->ITER = val; }
int ControlParameters::getITER(){ return this->ITER; }
void ControlParameters::setDELTU(double val){ this->DELTU = val; }
void ControlParameters::setDELTP(double val){ this->DELTP = val; }
double ControlParameters::getDELTU(){ return this->DELTU; }
double ControlParameters::getDELTP(){ return this->DELTP; }

string ControlParameters::PUSWF(ObservationPoints *p ,double SFRAC){
	string logLine = "";
	Writer * logWriter = Writer::OBSInstance();
	char buff[512];
	PUP(p);
	PU(p);
	double CSFRAC = 1.0 - SFRAC;
	double PINT = CSFRAC * p->getP1() + SFRAC* p->getP2();
	double UINT = CSFRAC * p->getU1() + SFRAC* p->getU2();
	double CNUBINT = CSFRAC * p->getC1() + SFRAC* p->getC2();
	double ESINT = CSFRAC * p->getE1() + SFRAC* p->getE2();
	double RINT = CSFRAC * p->getR1() + SFRAC* p->getR2();
	double SWTINT = CSFRAC * p->getS1() + SFRAC* p->getS2();

	_snprintf(buff, sizeof(buff), "%+14.6e %+14.6e %+14.6e %+14.6e %+14.6e      ", PINT, UINT, SWTINT, ESINT, RINT);
	return buff;
}

void ControlParameters::PU(ObservationPoints *p){
	if (KTYPE[0] == 2){}
	else{
		double XF1 = 1.0 - p->getXSI();
		double XF2 = 1.0 + p->getXSI();
		double YF1 = 1.0 - p->getETA();
		double YF2 = 1.0 + p->getETA();
		double ZF1 = 1.0 - p->getZET();
		double ZF2 = 1.0 + p->getZET();

		double FX[] = { XF1, XF2, XF2, XF1, XF1, XF2, XF2, XF1 };
		double FY[] = { YF1, YF1, YF2, YF2, YF1, YF1, YF2, YF2 };
		double FZ[] = { ZF1, ZF1, ZF1, ZF1, ZF2, ZF2, ZF2, ZF2 };
		double F[8];
		for (int i = 0; i < 8; i++)
			F[i] = 0.125*FX[i] * FY[i] * FZ[i];

		p->setP2(0.0);
		p->setU2(0.0);
		p->setC2(0.0);
		p->setE2(0.0);
		p->setR2(0.0);
		
		Element * obsEl = elementContainer[p->getObsElement()];
		for (int i = 0; i < 8; i++){
			int nn = obsEl->getElementNodes()[i];
			p->setP2(p->getP2() + nodeContainer[nn]->getPVEC() * F[i]);
			p->setU2(p->getU2() + nodeContainer[nn]->getUVEC() * F[i]);
			p->setC2(p->getC2() + nodeContainer[nn]->getCNUB() * F[i]);
			p->setS2(p->getS2() + nodeContainer[nn]->getSWT() * F[i]);
		}
	}
}

void ControlParameters::PUP(ObservationPoints *p){
	if (KTYPE[0] == 2){}
	else{
		double XF1 = 1.0 - p->getXSI();
		double XF2 = 1.0 + p->getXSI();
		double YF1 = 1.0 - p->getETA();
		double YF2 = 1.0 + p->getETA();
		double ZF1 = 1.0 - p->getZET();
		double ZF2 = 1.0 + p->getZET();

		double FX[] = { XF1, XF2, XF2, XF1, XF1, XF2, XF2, XF1 };
		double FY[] = { YF1, YF1, YF2, YF2, YF1, YF1, YF2, YF2 };
		double FZ[] = { ZF1, ZF1, ZF1, ZF1, ZF2, ZF2, ZF2, ZF2 };
		double F[8];
		for (int i = 0; i < 8; i++)
			F[i] = 0.125*FX[i] * FY[i] * FZ[i];

		p->setP1(0.0);
		p->setU1(0.0);
		p->setC1(0.0);
		p->setE1(0.0);
		p->setR1(0.0);

		Element * obsEl = elementContainer[p->getObsElement()];
		for (int i = 0; i < 8; i++){
			int nn = obsEl->getElementNodes()[i];
			p->setP1(p->getP1() + nodeContainer[nn]->getPM() * F[i]);
			p->setU1(p->getU1() + nodeContainer[nn]->getUM() * F[i]);
			p->setC1(p->getC1() + nodeContainer[nn]->getCNUB() * F[i]);
			p->setS1(p->getS1() + nodeContainer[nn]->getSWT() * F[i]);

		}
	}
}

void ControlParameters::setADSMOD(string val){ this->ADSMOD = val; }
string ControlParameters::getADSMOD(){ return this->ADSMOD; }






// PURPOSE
// USER-PROGRAMMED FUNCTION WHICH ALLOWS THE USER TO SPECIFY:
//        (1) TIME - DEPENDENT SPECIFIED PRESSURES AND TIME - DEPENDENT       
//            CONCENTRATIONS OR TEMPERATURES OF INFLOWS AT THESE POINTS   
//        (2) TIME - DEPENDENT SPECIFIED CONCENTRATIONS OR TEMPERATURES      
//        (3) TIME - DEPENDENT FLUID SOURCES AND CONCENTRATIONS              
//            OR TEMPERATURES OF INFLOWS AT THESE POINTS                   
//        (4) TIME - DEPENDENT ENERGY OR SOLUTE MASS SOURCES                 
void ControlParameters::BCTIME(){
	// REQUIRED PARAMETERES:
	//		IPBC,PBC,IUBC,UBC,QIN,UIN,QUIN,IQSOP,IQSOU,IPBCT, IUBCT, IQSOPT, IQSOUT, X, Y, Z, IBCPBC, IBCUBC, IBCSOP, IBCSOU
	// DEFINITION OF REQUIRED PARAMETERS
	// NN : EXACT NUMBER OF NODES IN MESH
	// NPBC = EXACT NUMBER OF SPECIFIED PRESSURE NODES
	// NUBC = EXACT NUMBER OF SPECIFIED CONCENTRATION
	// IT = NUMBER OF CURRENT TIME STEP 
	//  TSEC = TIME AT END OF CURRENT TIME STEP IN SECONDS  
	// TMIN = TIME AT END OF CURRENT TIME STEP IN MINUTES         
	// THOUR = TIME AT END OF CURRENT TIME STEP IN HOURS                
	// TDAY = TIME AT END OF CURRENT TIME STEP IN DAYS                   
	// TWEEK = TIME AT END OF CURRENT TIME STEP IN WEEKS             
	// TMONTH = TIME AT END OF CURRENT TIME STEP IN MONTHS             
	// TYEAR = TIME AT END OF CURRENT TIME STEP IN YEARS            

	//	PBC = SPECIFIED PRESSURE VALUE AT SPECIFIED PRESSURE NODE
	//	UBC= SPECIFIED CONCENTRATION VALUE OF ANY INFLOW OCCURRING AT SPECIFIED PRESSURE NODE
	//	IPBC = ACTUAL NODE NUMBER OF SPECIFIED PRESSURE NODE { WHEN NODE NUMBER I = IPBC(IP) IS NEGATIVE(I<0), VALUES MUST BE SPECIFIED FOR PBC AND UBC. }
	//	IBCPBC= INDICATOR OF WHERE THIS PRESSURE SPECIFICATION WAS MADE.MUST BE SET TO - 1 TO INDICATE THAT THIS SPECIFICATION WAS MADE IN SUBROUTINE BCTIME.

	// UBC(IUP) = SPECIFIED CONCENTRATION VALUE AT SPECIFIED CONCENTRATION NODE (WHERE IUP = IU + NPBC)
	// IUBC(IUP) = ACTUAL NODE NUMBER OF IU(TH) SPECIFIED CONCENTRATION NODE(WHERE IUP = IU + NPBC){ WHEN NODE NUMBER I = IUBC(IU) IS NEGATIVE(I<0),A VALUE MUST BE SPECIFIED FOR UBC. }
	// IBCUBC(IUP) = INDICATOR OF WHERE THIS CONCENTRATION SPECIFICATION WAS MADE.MUST BE SET TO - 1 TO INDICATE THAT THIS SPECIFICATION WAS MADE IN SUBROUTINE BCTIME.


	// IQSOP(IQP) = NODE NUMBER OF IQP(TH) FLUID SOURCE NODE. { WHEN NODE NUMBER I = IQSOP(IQP) IS NEGATIVE(I<0), VALUES MUST BE SPECIFIED FOR QIN AND UIN. }
	// QIN(-I) = SPECIFIED FLUID SOURCE VALUE AT NODE(-I)               
	// UIN(-I) = SPECIFIED CONCENTRATION VALUE OF ANY INFLOW OCCURRING AT FLUID SOURCE NODE(-I)              
	// IBCSOP(IQP) = INDICATOR OF WHERE THIS FLUID SOURCE SPECIFICATION WAS MADE.MUST BE SET TO - 1 TO INDICATE THAT THIS SPECIFICATION WAS MADE IN SUBROUTINE BCTIME.


	// IQSOU(IQU) = NODE NUMBER OF IQU(TH) SOLUTE MASS SOURCE NODE { WHEN NODE NUMBER I = IQSOU(IQU) IS NEGATIVE(I<0), A VALUE MUST BE SPECIFIED FOR QUIN. }  
	// QUIN(-I) = SPECIFIED ENERGY OR SOLUTE MASS SOURCE VALUE AT NODE(-I)                                          
	// IBCSOU(IQU) = INDICATOR OF WHERE THIS ENERGY OR SOLUTE MASS SOURCE SPECIFICATION WAS MADE.MUST BE SET TO - 1 TO INDICATE THAT THIS SPECIFICATION WAS MADE IN SUBROUTINE BCTIME.

	//NSOPI IS ACTUAL NUMBER OF FLUID SOURCE NODES
	NSOPI = NSOP - 1;
	//NSOUI IS ACTUAL NUMBER OF SOLUTE MASS SOURCE NODES 
	NSOUI = NSOU - 1;

	if (IPBCT < 0){
		// SET TIME-DEPENDENT SPECIFIED PRESSURES OR CONCENTRATIONS OF INFLOWS AT SPECIFIED PRESSURE NODES
		for (int nn : IPBC){
			if (nn < 0){
				nodeContainer[abs(nn)]->setIBCPBC(-1);
			}
			else{
				continue;
			}

		}
	}

	if (IPBCT >= 0){
		if (IUBCT < 0){
			// SET TIME - DEPENDENT SPECIFIED CONCENTRATIONS
			for (int nn : IUBC){
				if (nn < 0){
					nodeContainer[abs(nn)]->setIBCUBC(-1);
				}
				else{
					continue;
				}
			}
		}
		else{
			if (IQSOPT < 0){
				for (int nn : IQSOP){
					if (nn < 0){
						nodeContainer[abs(nn)]->setIBCSOP(-1);
					}
					else{
						continue;
					}
				}
			}
			else{
				if (IQSOUT < 0){
					for (int nn : IQSOU){
						if (nn < 0){
							nodeContainer[abs(nn)]->setIBCSOU(-1);
						}
						else{
							continue;
						}
					}
				}
			}


		}
	}
}

void ControlParameters::setOnceBCS(bool val){ this->onceBCS = val; }
bool ControlParameters::getOnceBCS(){ return this->onceBCS; }

void ControlParameters::BCSTEP(){
	// Set Boundary Condition

	BCS * bcs = nullptr;

	for (BCS* tbcs : bcsContainer){
		if (tbcs->getTimeStep() == ITBCS){
			bcs = tbcs;
			break;
		}
	}

	if (bcs != nullptr){

		USEFL = ((ISSFLO != 0) && (ITBCS == 0)) || ((ISSFLO == 0) && (ITBCS != 0));

		ANYFL = (bcs->getNumberOfQINC() + bcs->getNumberOfPBC()) > 0;
		ANYTR = (bcs->getNumberOfQUINC() + bcs->getNumberOfUBC()) > 0;
		BCSFL[ITBCS] = USEFL && ANYFL;
		BCSTR[ITBCS] = ANYTR;
		SETFL = setBCS && BCSFL[ITBCS];
		SETTR = setBCS && BCSTR[ITBCS];
	
		if ((bcs->getNumberOfQINC() + bcs->getNumberOfQUINC()) != 0){
			if (bcs->getNumberOfQINC() > 0){
				for (int nn = 0; nn < bcs->getNodes().size();nn++){
					int nodeNum = abs(bcs->getNodes()[nn]);
					bool inList = false;
					for (int inn : IQSOP){
						if (abs(bcs->getNodes()[nn]) == abs(inn)){
							inList = true;
							break;
						}
					}
					if (inList == false){
						exitOnError("BCS-3-2");
					}

					if (SETFL){
						if (bcs->getNodes()[nn] > 0){
							nodeContainer[nodeNum]->setQIN(bcs->getQINC()[nn]);
							if (bcs->getQINC()[nn] > 0){
								nodeContainer[nodeNum]->setUIN(bcs->getUINC()[nn]);
							}
						}
						else{
							nodeContainer[nodeNum]->setQIN(0.0);

						}
					}
				}

			}
			if (bcs->getNumberOfQUINC() > 0){
				for (int nn = 0; nn < bcs->getNodes().size(); nn++){
					int nodeNum = abs(bcs->getNodes()[nn]);
					bool inList = false;
					for (int inn : IQSOU){
						if (abs(bcs->getNodes()[nn]) == abs(inn)){
							inList = true;
							break;
						}
					}
					if (inList == false){
						exitOnError("BCS-4-2");
					}

					if (SETTR){
						if (bcs->getNodes()[nn] > 0){
							nodeContainer[nodeNum]->setQUIN(bcs->getQUINC()[nn]);
							
						}
						else{
							nodeContainer[nodeNum]->setQUIN(0.0);

						}
					}
				}
			}

		
		}
		if ((bcs->getNumberOfPBC() + bcs->getNumberOfUBC()) != 0){
			if (bcs->getNumberOfPBC() > 0){
				for (int nn = 0; nn < bcs->getNodes().size(); nn++){
					int nodeNum = abs(bcs->getNodes()[nn]);
					bool inList = false;
					for (int inn : IPBC){
						if (abs(bcs->getNodes()[nn]) == abs(inn)){
							inList = true;
							break;
						}
					}
					if (inList == false){
						exitOnError("BCS-5-2");
					}

					if (SETFL){
						if (bcs->getNodes()[nn] > 0){
							nodeContainer[nodeNum]->setPBC(bcs->getPBC()[nn]);
							nodeContainer[nodeNum]->setUBC(bcs->getUBC()[nn]);
							nodeContainer[nodeNum]->setGNUP1(GNUP);
							
						}
						else{
							nodeContainer[nodeNum]->setGNUP1(0.0);

						}
					}
				}
			}
			if (bcs->getNumberOfUBC() > 0){
				for (int nn = 0; nn < bcs->getNodes().size(); nn++){
					int nodeNum = abs(bcs->getNodes()[nn]);
					bool inList = false;
					for (int inn : IUBC){
						if (abs(bcs->getNodes()[nn]) == abs(inn)){
							inList = true;
							break;
						}
					}
					if (inList == false){
						exitOnError("BCS-6-2");
					}

					if (SETTR){
						if (bcs->getNodes()[nn] > 0){
							nodeContainer[nodeNum]->setUBC(bcs->getUBC()[nn]);
							nodeContainer[nodeNum]->setGNUU1(GNUU);
							
						}
						else{
							nodeContainer[nodeNum]->setGNUU1(0.0);

						}
					}
				}

			}

		}
	}

		


}

void ControlParameters::ELEMN3(){
 // Too Much to Do

	//for (int i = 0; i < 8; i++){
	//	vector<double> vec;
	//	for (int j = 0; j < 8; j++){
	//		vec.push_back(0);	
	//	}
	//	F.push_back(vec);
	//	W.push_back(vec);
	//	DWDXG.push_back(vec);
	//	DWDYG.push_back(vec);
	//	DWDZG.push_back(vec);
	//	DFDXG.push_back(vec);
	//	DFDYG.push_back(vec);
	//	DFDZG.push_back(vec);
	//}
	double GLOC = 0.577350269189626;
	// Decide Whether to Calculate Centroid velocities on this call
	double GXLOC[8] = { -1, 1, 1, -1, -1, 1, 1, -1 };
	double GYLOC[8] = { -1, -1, 1, 1, -1, -1, 1, 1 };
	double GZLOC[8] = { -1, -1, -1, -1, 1, 1, 1, 1 };
	int IVCALC = 0;
	int JVCALC = 0;
	int KVCALC = 0;
	if ((ML != 2) && (ITER == 1))
		IVCALC = 1;
	if (IT == 1)
		IVCALC = 1;
	if ((KVEL == 1) || InputFiles::Instance()->filesToWrite["ELE"] != "")
		JVCALC = 1;

		KVCALC = IVCALC + JVCALC;

	//	ON FIRST TIME STEP, PREPARE GRAVITY VECTOR COMPONENTS,GXSI, GETA, AND GZET FOR CONSISTENT VELOCITIES, AND CHECK ELEMENT SHAPES
		if (INTIM <= 0){
			INTIM = 1;
			for (int el = 1; el <= NE; el++){
				int * elNodes = elementContainer[el]->getElementNodes();
				for (int nn = 0; nn < 8; nn++){
					BASIS3(0,el,nn, elNodes[nn], GXLOC[nn], GYLOC[nn], GZLOC[nn],PORG[nn]);
					if (elementContainer[el]->getDET()[nn] <= 0){
						ISTOP = ISTOP + 1;
						string logLine = "";
						Writer * logWriter = Writer::LSTInstance();
						char buff[512];
						_snprintf(buff, sizeof(buff), "           THE DETERMINANT OF JACOBIAN AT NODE %9d IN ELEMENT %9d IS NEGATIVE OR ZERO, %+15.7e", elementContainer[el]->getElementNodes()[nn], el, elementContainer[el]->getDET()[nn]);
						logLine.append(buff);
						logWriter->writeContainer.push_back(logLine);
					}
				}
			}

			if (ISTOP != 0){
				exitOnError("INP-14B,22-1");
			}

			if (IUNSAT != 0)
				IUNSAT = 2;

		}


		//LOOP THROUGH ALL ELEMENTS TO CARRY OUT SPATIAL INTEGRATION OF FLUX TERMS IN P AND / OR U EQUATIONS

		for (int el = 1; el <= NE; el++){
			vector<double>VOLE(8, 0);
			vector<double>DFLOWE(8, 0);
			vector<vector<double>>BFLOWE(8, vector<double>(8, 0));
			vector<vector<double>>BTRANE(8, vector<double>(8, 0));
			vector<vector<double>>DTRANE(8, vector<double>(8, 0));
		/*	double VOLE[8] = {0};
			double DFLOWE[8] = {};
			double BFLOWE[8][8] = {};
			double BTRANE[8][8];
			double DTRANE[8][8];*/
			double RDDFJX, RDDFJY, RDDFJZ;
		/*	for (int i = 0; i < 8; i++){
				VOLE[i] = 0;
				DFLOWE[i] = 0;
				for (int j = 0; j < 8; j++){

					BFLOWE[i][j] = 0;
					BTRANE[i][j] = 0;
					DTRANE[i][j] = 0;
				}
			}*/


			double XIX, YIY, ZIZ;
			XIX = YIY = ZIZ = -1.0;
			double XLOC, YLOC, ZLOC;
			int KG = 0;
			for (int IZL = 0; IZL < 2; IZL++){
				for (int IYL = 0; IYL < 2; IYL++){
					for (int IZL = 0; IZL < 2; IZL++){
						XLOC = XIX*GLOC;
						YLOC = YIY*GLOC;
						ZLOC = ZIZ*GLOC;
						BASIS3(1, el, KG, elementContainer[el]->getElementNodes()[KG], XLOC, YLOC, ZLOC,PORG[KG]);
						XIX = -XIX;
						KG = KG++;
					}
					YIY = -YIY;
				}
				ZIZ = -ZIZ;
			}
			if ((KVCALC - 2) == 0){
				double AXSUM, AYSUM, AZSUM;
				AXSUM = AYSUM = AZSUM = 0;
				for (int nod = 0; nod < 8; nod++){
					AXSUM = AXSUM + VXG[nod];
					AYSUM = AYSUM + VYG[nod];
					AZSUM = AZSUM + VZG[nod];
				}
				elementContainer[el]->setVMAG(sqrt(AXSUM*AXSUM + AYSUM*AYSUM + AZSUM*AZSUM));
				if (elementContainer[el]->getVMAG() != 0){
					elementContainer[el]->setVANG2(asin(AZSUM / elementContainer[el]->getVMAG())*57.29577951308232);
					elementContainer[el]->setVMAG(elementContainer[el]->getVMAG()*0.125);
					elementContainer[el]->setVANG1(atan2(AYSUM, AXSUM)*57.29577951308232);
				}
				else{
					elementContainer[el]->setVANG2(0.0);
					elementContainer[el]->setVANG1(0.0);
				}
			}


			if ((ML - 1) <= 0){
				double SWTEST = 0.0;
				double ROMG = 0.0;
				double RXXG[8] = {}, RXYG[8] = {}, RXZG[8] = {};
				double RYXG[8] = {}, RYYG[8] = {}, RYZG[8] = {};
				double RZXG[8] = {}, RZYG[8] = {}, RZZG[8] = {};
			
				for (int i = 0; i < 8; i++){
					SWTEST = SWTEST + SWTG[i];
					ROMG = RHOG[i] * RELKTG[i] /VISCG[i];
					RXXG[i] = elementContainer[el]->getPERMXX()*ROMG;
					RXYG[i] = elementContainer[el]->getPERMXY()*ROMG;
					RXZG[i] = elementContainer[el]->getPERMXZ()*ROMG;
					RYXG[i] = elementContainer[el]->getPERMYX()*ROMG;
					RYYG[i] = elementContainer[el]->getPERMYY()*ROMG;
					RYZG[i] = elementContainer[el]->getPERMYZ()*ROMG;
					RZXG[i] = elementContainer[el]->getPERMZX()*ROMG;
					RZYG[i] = elementContainer[el]->getPERMZY()*ROMG;
					RZZG[i] = elementContainer[el]->getPERMZZ()*ROMG;
				}

				
				if ((UP <= 1e-6)){

					if (SWTEST - 3.999 < 0){
						for (int i = 0; i < 8; i++){
							double RXXGD, RXYGD, RXZGD, RYXGD, RYYGD, RYZGD, RZXGD, RZYGD, RZZGD;
							double DET = elementContainer[el]->getDET()[i];
							double RDRX, RDRY, RDRZ;
							RXXGD = RXXG[i] * DET;
							RXYGD = RXYG[i] * DET;
							RXZGD = RXZG[i] * DET;
							RYXGD = RYXG[i] * DET;
							RYYGD = RYYG[i] * DET;
							RYZGD = RYZG[i] * DET;
							RZXGD = RZXG[i] * DET;
							RZYGD = RZYG[i] * DET;
							RZZGD = RZZG[i] * DET;
							RDRX = RXXGD * RGXG[i] + RXYGD*RGYG[i] + RXZGD*RGZG[i];
							RDRY = RYXGD * RGXG[i] + RYYGD*RGYG[i] + RYZGD*RGZG[i];
							RDRZ = RZXGD * RGXG[i] + RZYGD*RGYG[i] + RZZGD*RGZG[i];

							for (int j = 0; j < 8; j++){
								VOLE[j] = VOLE[j] + F[i][j] * DET;
								DFLOWE[j] = DFLOWE[j] + RDRX*DWDXG[i][j] + RDRY*DWDYG[i][j] + RDRZ*DWDZG[i][j];

							}

							for (int j = 0; j < 8; j++){
								RDDFJX = RXXGD*DFDXG[i][j] + RXYGD*DFDYG[i][j] + RXZGD*DFDZG[i][j];
								RDDFJY = RYXGD*DFDXG[i][j] + RYYGD*DFDYG[i][j] + RYZGD*DFDZG[i][j];
								RDDFJZ = RZXGD*DFDXG[i][j] + RZYGD*DFDYG[i][j] + RZZGD*DFDZG[i][j];
								for (int k = 0; k < 8; k++){
									BFLOWE[i][k] = BFLOWE[i][k] + DWDXG[i][k] * RDDFJX + DWDYG[i][k] * RDDFJY + DWDZG[i][k] * RDDFJZ;
								}
							}
						}
					}else
					{
						for (int i = 0; i < 8; i++){
							double RXXGD, RXYGD, RXZGD, RYXGD, RYYGD, RYZGD, RZXGD, RZYGD, RZZGD;
							double DET = elementContainer[el]->getDET()[i];
							double RDRX, RDRY, RDRZ;
							RXXGD = RXXG[i] * DET;
							RXYGD = RXYG[i] * DET;
							RXZGD = RXZG[i] * DET;
							RYXGD = RYXG[i] * DET;
							RYYGD = RYYG[i] * DET;
							RYZGD = RYZG[i] * DET;
							RZXGD = RZXG[i] * DET;
							RZYGD = RZYG[i] * DET;
							RZZGD = RZZG[i] * DET;
							RDRX = RXXGD * RGXG[i] + RXYGD*RGYG[i] + RXZGD*RGZG[i];
							RDRY = RYXGD * RGXG[i] + RYYGD*RGYG[i] + RYZGD*RGZG[i];
							RDRZ = RZXGD * RGXG[i] + RZYGD*RGYG[i] + RZZGD*RGZG[i];

							for (int j = 0; j < 8; j++){
								VOLE[j] = VOLE[j] + F[i][j] * DET;
								DFLOWE[j] = DFLOWE[j] + RDRX*DFDXG[i][j] + RDRY*DFDYG[i][j] + RDRZ*DFDZG[i][j];

							}

							for (int j = 0; j < 8; j++){
								RDDFJX = RXXGD*DFDXG[i][j] + RXYGD*DFDYG[i][j] + RXZGD*DFDZG[i][j];
								RDDFJY = RYXGD*DFDXG[i][j] + RYYGD*DFDYG[i][j] + RYZGD*DFDZG[i][j];
								RDDFJZ = RZXGD*DFDXG[i][j] + RZYGD*DFDYG[i][j] + RZZGD*DFDZG[i][j];
								for (int k = 0; k < 8; k++){
									BFLOWE[j][k] = BFLOWE[j][k] + DFDXG[i][k] * RDDFJX + DFDYG[i][k] * RDDFJY + DFDZG[i][k] * RDDFJZ;
								}
							}
						}
					}
				}
				else{
					for (int i = 0; i < 8; i++){
						double RXXGD, RXYGD, RXZGD, RYXGD, RYYGD, RYZGD, RZXGD, RZYGD, RZZGD;
						double DET = elementContainer[el]->getDET()[i];
						double RDRX, RDRY, RDRZ;
						RXXGD = RXXG[i] * DET;
						RXYGD = RXYG[i] * DET;
						RXZGD = RXZG[i] * DET;
						RYXGD = RYXG[i] * DET;
						RYYGD = RYYG[i] * DET;
						RYZGD = RYZG[i] * DET;
						RZXGD = RZXG[i] * DET;
						RZYGD = RZYG[i] * DET;
						RZZGD = RZZG[i] * DET;
						RDRX = RXXGD * RGXG[i] + RXYGD*RGYG[i] + RXZGD*RGZG[i];
						RDRY = RYXGD * RGXG[i] + RYYGD*RGYG[i] + RYZGD*RGZG[i];
						RDRZ = RZXGD * RGXG[i] + RZYGD*RGYG[i] + RZZGD*RGZG[i];

						for (int j = 0; j < 8; j++){
							VOLE[j] = VOLE[j] + F[i][j] * DET;
							DFLOWE[j] = DFLOWE[j] + RDRX*DFDXG[i][j] + RDRY*DFDYG[i][j] + RDRZ*DFDZG[i][j];

						}
						
						for (int j = 0; j < 8; j++){
							RDDFJX = RXXGD*DFDXG[i][j] + RXYGD*DFDYG[i][j] + RXZGD*DFDZG[i][j];
							RDDFJY = RYXGD*DFDXG[i][j] + RYYGD*DFDYG[i][j] + RYZGD*DFDZG[i][j];
							RDDFJZ = RZXGD*DFDXG[i][j] + RZYGD*DFDYG[i][j] + RZZGD*DFDZG[i][j];
							for (int k = 0; k < 8; k++){
								BFLOWE[j][k] = BFLOWE[j][k] + DFDXG[i][k] * RDDFJX + DFDYG[i][k] * RDDFJY + DFDZG[i][k] * RDDFJZ;
							}
						}
					}
				}

				
			}
			if ((ML - 1) != 0){
				if (NOUMAT != 1){
					double BXXG[8] = {0}, BXYG[8] = {0}, BXZG[8] = {0}, BYXG[8] = {0}, BYYG[8] = {0}, BYZG[8] = {0}, BZXG[8] = {0}, BZYG[8] = {0}, BZZG[8] = {0};
					double EXG[8] = {0}, EYG[8] = {0}, EZG[8] = {0};
					for (int i = 0; i < 8; i++){
						double ESWG = PORG[i] * SWTG[i];
						double RHOCWG = RHOG[i] * CW;
						double ESRCG = ESWG * RHOCWG;
						
						double DXXG, DXYG, DXZG, DYXG, DYYG, DYZG, DZXG, DZYG, DZZG;
						
						double ESE;
						if (VGMAG[i] <= 0){
							EXG[i] = 0.0;
							EYG[i] = 0.0;
							EZG[i] = 0.0;
							DXXG = 0;
							DXYG = 0;
							DXZG = 0;
							DYXG = 0;
							DYYG = 0;
							DYZG = 0;
							DZXG = 0;
							DZYG = 0;
							DZZG = 0;
						}
						else{
							EXG[i] = ESRCG*VXG[i];
							EYG[i] = ESRCG*VYG[i];
							EZG[i] = ESRCG*VZG[i];
							DISPR3(i,el,VXG[i],VYG[i],VZG[i],VGMAG[i],DXXG,DXYG,DXZG,DYXG,DYYG,DYZG,DZXG,DZYG,DZZG);
						}

						ESE = ESRCG * SIGMAW + (1.0 - PORG[i])*RHOCWG*SIGMAS;
						BXXG[i] = ESRCG * DXXG + ESE;
						BXYG[i] = ESRCG * DXYG;
						BXZG[i] = ESRCG * DXZG;
						BYXG[i] = ESRCG * DYXG ;
						BYYG[i] = ESRCG * DYYG + ESE;
						BYZG[i] = ESRCG * DYZG ;
						BZXG[i] = ESRCG * DZXG ;
						BZYG[i] = ESRCG * DZYG ;
						BZZG[i] = ESRCG * DZZG + ESE;
					}

					
					for (int i = 0; i < 8; i++){
						for (int j = 0; j < 8; j++){
							BTRANE[i][j] = 0.0;
							DTRANE[i][j] = 0.0;
						}
					}

					for (int k = 0; k < 8; k++){
						double BXXGD, BXYGD, BXZGD, BYXGD, BYYGD, BYZGD, BZXGD, BZYGD, BZZGD;
						double EXGD, EYGD, EZGD;
						double BDDFJX, BDDFJY, BDDFJZ, EDDFJ;
						double DET = elementContainer[el]->getDET()[k];
						BXXGD = BXXG[k] * DET;
						BXYGD = BXYG[k] * DET;
						BXZGD = BXZG[k] * DET;
						BYXGD = BYXG[k] * DET;
						BYYGD = BYYG[k] * DET;
						BYZGD = BYZG[k] * DET;
						BZXGD = BZXG[k] * DET;
						BZYGD = BZYG[k] * DET;
						BZZGD = BZZG[k] * DET;
						EXGD = EXG[k] * DET;
						EYGD = EYG[k] * DET;
						EZGD = EZG[k] * DET;
						for (int j = 0; j < 8; j++){
							BDDFJX = BXXGD*DFDXG[k][j] + BXYGD*DFDYG[k][j] + BXZGD*DFDZG[k][j];
							BDDFJY = BYXGD*DFDXG[k][j] + BYYGD*DFDYG[k][j] + BYZGD*DFDZG[k][j];
							BDDFJZ = BZXGD*DFDXG[k][j] + BZYGD*DFDYG[k][j] + BZZGD*DFDZG[k][j];
							EDDFJ = EXGD*DFDXG[k][j] + EYGD*DFDXG[k][j] + EZGD*DFDZG[k][j];
							for (int i = 0; i < 8; i++){
								BTRANE[i][j] = BTRANE[i][j] + DFDXG[k][i] * BDDFJX + DFDYG[k][i] * BDDFJY + DFDZG[k][i] * BDDFJZ;
								DTRANE[i][j] = DTRANE[i][j] + EDDFJ * W[k][i];
							}
						}
					}



				}
			}

				
			if (KSOLVP == 0){
				GLOBAN();
			}
			else{
				GLOCOL(el, ML, VOLE, BFLOWE, DFLOWE, BTRANE, DTRANE);
			
			}
		}

		


}
void ControlParameters::DISPR3(int i, int el, double VX, double VY, double VZ, double VMAG, double& DXXG, double& DXYG, double& DXZG, double& DYXG, double& DYYG, double& DYZG, double& DZXG, double& DZYG, double& DZZG){
	double TOLISO = 0.0000001;
	double TOLVRT = TOLISO;
	double TOLCIR = 0.9999999;
	double VNX, VNY, VNZ;
	double ALMAX, ALMID, ALMIN, ATMAX, ATMID, ATMIN;
	double ALMXVL, ALMNVL, ATMXVL, ATMNVL;
	bool LISO, TISO;
	double DL,AT1,AT2;
	
	LISO = TISO = false;
	ALMAX = elementContainer[el]->getALMAX();
	ATMAX = elementContainer[el]->getATMAX();
	ATMID = elementContainer[el]->getATMID();
	ALMID = elementContainer[el]->getALMID();
	ATMIN = elementContainer[el]->getATMIN();
	ALMIN = elementContainer[el]->getALMIN();
	double AT []= { ATMAX, ATMID, ATMIN };
	double sortedAT[3];
	int maxIND[3] = { 0 };
	sort(begin(AT), end(AT));
	for (int i = 0; i < 3; i++){
		if (sortedAT[0] == AT[i]){
			maxIND[0] = i;
			break;
		}
	}
	for (int i = 0; i < 3; i++){
		if (sortedAT[1] == AT[i]){
			maxIND[1] = i;
			break;
		}
	}
	for (int i = 0; i < 3; i++){
		if (sortedAT[2] == AT[i]){
			maxIND[2] = i;
			break;
		}
	}
	// Normalize The Velocity Vector
	VNX = VX / VMAG;
	VNY = VY / VMAG;
	VNZ = VZ / VMAG;
	ALMNVL = min(min(elementContainer[el]->getALMAX(), elementContainer[el]->getALMID()), elementContainer[el]->getALMIN());
	ALMXVL = max(max(elementContainer[el]->getALMAX(), elementContainer[el]->getALMID()), elementContainer[el]->getALMIN());
	// Determine Long Dispersivity essentialy Isotropic
	if (ALMXVL == 0){
		LISO = true;
	}
	else{
		LISO = ((ALMXVL - ALMNVL) / ALMXVL) < TOLISO;
	}

	if (LISO){
		DL = elementContainer[el]->getALMAX()*VMAG;
	}
	else{
		vector<vector<double>> rotMat = elementContainer[el]->getRotationMatrix();
		double VNXX, VNYY, VNZZ;
		ROTATE(rotMat, VNX, VNY, VNZ, VNXX, VNYY, VNZZ);

		DL = VMAG / (VNXX*VNXX / elementContainer[el]->getALMAX() + VNYY*VNYY / elementContainer[el]->getALMAX()+VNZZ *VNZZ / elementContainer[el]->getALMIN());
		
	}

	ATMNVL = min(min(elementContainer[el]->getATMAX(), elementContainer[el]->getATMID()), elementContainer[el]->getATMIN());
	ATMXVL = max(max(elementContainer[el]->getATMAX(), elementContainer[el]->getATMID()), elementContainer[el]->getATMIN());

	if (ATMXVL == 0){
		TISO = true;
	}
	else{
		TISO = ((ATMXVL - ATMNVL) / ATMXVL) < TOLISO;
	}
	double UNX, UNY, UNZ;
	double WNY, WNX, WNZ;
	
	if (TISO){
		double TERM = 1 - VNZ*VNZ;
		if (TERM < TOLVRT){
			UNZ = WNX = UNY = WNZ = 0;
			UNX = WNY = 1;
		}
		else{
			double TERMH = sqrt(TERM);
			UNX = -VNY / TERMH;
			UNY = VNX / TERMH;
			UNZ = 0;
			WNX = -VNZ*UNY;
			WNY = VNZ*UNX;
			WNZ = TERMH;
		}
		AT1 = AT2 = elementContainer[el]->getATMAX();
	}
	else{
		double VNXX, VNYY, VNZZ;
		if (LISO){
			vector<vector<double>> rotMat = elementContainer[el]->getRotationMatrix();
			
			ROTATE(rotMat, VNX, VNY, VNZ, VNXX, VNYY, VNZZ);
		}

		vector<pair <double, double>> ATVN;
		ATVN.push_back(pair<double,double>(elementContainer[el]->getATMAX(), VNXX));
		ATVN.push_back(pair<double, double>(elementContainer[el]->getATMID(), VNYY));
		ATVN.push_back(pair<double, double>(elementContainer[el]->getATMIN(), VNZZ));
		sort(ATVN.begin(), ATVN.end(), [](pair<double,double> v1, pair<double,double> v2){return v1.first < v2.first; });
		//APPLY THE BIOT-FRESNEL CONSTRUCTION TO THE TRANSVERSE
		double A2B2, A2C2, B2C2;
		A2B2 = ATVN[0].first*ATVN[1].first;
		A2C2 = ATVN[0].first*ATVN[2].first;
		B2C2 = ATVN[1].first*ATVN[2].first;
		double COS2AV, SIN2AV, SINAV, TERM1, TERM2, OA1V, OA2V,COSAV;
		COS2AV = (A2C2 - B2C2) / (A2B2 - B2C2);
		SIN2AV = 1 - COS2AV;
		COSAV = sqrt(COS2AV);
		SINAV = sqrt(SIN2AV);
		TERM1 = COSAV * ATVN[0].second;
		TERM2 = SINAV * ATVN[2].second;
		OA1V = TERM1 + TERM2;
		OA2V = TERM1 - TERM2;
		double UNTXX, UNTYY, UNTZZ, WNTXX, WNTYY, WNTZZ;
		double RVJ1MG, RVJ2MG, RSUM, RDIF, OAUXX, OAUZZ, OAWXX, OAWZZ, OAUV, OAWV, OAUOAU, OAWOAW, UMTERM, WMTERM;
		if (max(abs(OA1V),abs(OA2V)) >= TOLCIR){
			UNTXX = -ATVN[2].second;
			UNTYY = 0;
			UNTZZ = ATVN[0].second;
			WNTXX = 0;
			WNTYY = 1;
			WNTZZ = 0;
			AT1 = AT2 = ATVN[1].first;
		}
		else{
			RVJ1MG = 1 / sqrt(1 - OA1V*OA1V);
			RVJ2MG = 1 / sqrt(1 - OA2V * OA2V);
			RSUM = RVJ1MG + RVJ2MG;
			RDIF = RVJ1MG - RVJ2MG;
			OAUXX = COSAV * RSUM;
			OAUZZ = SINAV * RDIF;
			OAWXX = COSAV * RDIF;
			OAWZZ = SINAV * RSUM;
			OAUV = OAUXX * ATVN[0].second + OAUZZ*ATVN[2].second;
			OAWV = OAWXX * ATVN[0].second + OAWZZ*ATVN[2].second;
			OAUOAU = OAUXX*OAUXX + OAUZZ * OAUZZ;
			OAWOAW = OAWXX*OAWXX + OAWZZ * OAWZZ;
			UMTERM = OAUOAU - OAUV *OAUV;
			WMTERM = OAWOAW - OAWV * OAWV;
			double RUMAGH, RWMAGH;
			if (UMTERM > WMTERM){
				RUMAGH = 1.0 / sqrt(UMTERM);
				UNTXX = (OAUXX - OAUV *ATVN[0].second)*RUMAGH;
				UNTYY = -OAUV*ATVN[1].second*RUMAGH;
				UNTZZ = (OAUZZ - OAUV*ATVN[2].second)*RUMAGH;
				WNTXX = UNTYY*ATVN[2].second - UNTZZ * ATVN[1].second;
				WNTYY = UNTZZ*ATVN[0].second - UNTXX * ATVN[2].second;
				WNTZZ = UNTXX*ATVN[1].second - UNTYY * ATVN[0].second;
			}
			else{
				RWMAGH = 1 / sqrt(WMTERM);
				WNTXX = (OAWXX - OAWV * ATVN[0].second)*RWMAGH;
				WNTYY = -OAWV * ATVN[1].second * RWMAGH;
				WNTZZ = (OAWZZ - OAWV*ATVN[2].second)*RWMAGH;
				UNTXX = WNTYY * ATVN[2].second - WNTZZ * ATVN[1].second;
				UNTYY = WNTZZ * ATVN[0].second - WNTXX*ATVN[2].second;
				UNTZZ = WNTXX*ATVN[1].second - WNTYY * ATVN[0].second;
			}
			double A2B2C2 = A2B2*ATVN[2].first;
			double DEN1, DEN2;
			DEN1 = B2C2*UNTXX*UNTXX + A2C2*UNTYY*UNTYY + A2B2*UNTZZ*UNTZZ;
			DEN2 = B2C2*WNTXX*WNTXX + A2C2*WNTYY*WNTYY + A2B2*WNTZZ*WNTZZ;
			AT1 = A2B2C2 / DEN1;
			AT2 = A2B2C2 / DEN2;
		}
		double UN[3],UNXX,UNYY,UNZZ,WN[3],WNXX,WNYY,WNZZ;
		UN[maxIND[0]] = UNTXX;
		UN[maxIND[1]] = UNTYY;
		UN[maxIND[2]] = UNTZZ;
		UNXX = UN[0];
		UNYY = UN[1];
		UNZZ = UN[2];
		WN[maxIND[0]] = WNTXX;
		WN[maxIND[1]] = WNTYY;
		WN[maxIND[2]] = WNTZZ;
		WNXX = WN[0];
		WNYY = WN[1];
		WNZZ = WN[2];
		vector<vector<double>> rotMat = elementContainer[el]->getRotationMatrix();
		ROTATE(rotMat, UNXX, UNYY, UNZZ, UNX, UNY, UNZ);
		ROTATE(rotMat, WNXX, WNYY, WNZZ, WNX, WNY, WNZ);
	}
	double DT1, DT2;
	DT1 = AT1 * VMAG;
	DT2 = AT2 * VMAG;

	TENSYM(DL, DT1, DT2, VNX, UNX, WNX,VNY, UNY, WNY, VNZ, UNZ, WNZ, DXXG, DXYG, DXZG, DYXG, DYYG, DYZG,DZXG, DZYG, DZZG);
}
void ControlParameters::ROTATE(vector<vector<double>> rotMat, double v1, double v2, double v3, double& vr1, double& vr2, double& vr3){
	vr1 = v1 * rotMat[0][0] + v2 * rotMat[0][1] + v3 * rotMat[0][2];
	vr2 = v1 * rotMat[0][0] + v2 * rotMat[0][1] + v3 * rotMat[0][2];
	vr3 = v1 * rotMat[0][0] + v2 * rotMat[0][1] + v3 * rotMat[0][2];
	
}
void ControlParameters::GLOCOL(int el, int ML, vector<double>& VOLE,vector<vector<double>>& BFLOWE, vector<double>& DFLOWE,vector<vector<double>>&BTRANE, vector<vector<double>>&DTRANE)//double VOLE[8], double BFLOWE[8][8], double DFLOWE[8], double BTRANE[8][8], double DTRANE[8][8])
{
	int ib, jb,M;
	ib = jb = M = -1;
	for (int i = 0; i < N48; i++){
		ib=elementContainer[el]->getElementNodes()[i] -1;
		nodeVOL[ib] = nodeVOL[ib] + VOLE[i];
		nodeContainer[ib+1]->setVOL(nodeVOL[ib]);
		//nodePVEC[ib] = nodePVEC[ib] + DFLOWE[i];
		node_p_rhs[ib] = node_p_rhs[ib] + DFLOWE[i];

		for (int j = 0; j < N48; j++)
		{
			jb = elementContainer[el]->getElementNodes()[j];
			int MBEG = JAVec[jb-1];
			int MEND = JAVec[jb] ;

			for (int k = MBEG; k < MEND; k++)
			{
				if (ib == IAVec[k])
				{
					M = k;
					break;
				}
			}
		//	double val = PMAT(M, 0) + BFLOWE[i][j];
		//	PMAT(M, 0) = val;
			PMATT[M] = PMATT[M] + BFLOWE[i][j];
			
		}
	}

	if ((ML - 1) != 0)
	{
		if (NOUMAT != 1)
		{
			ib = jb = M = -1;
			
			for (int i = 0; i < N48; i++){
				ib = elementContainer[el]->getElementNodes()[i]-1;
				for (int j = 0; j < N48; j++)
				{
					jb = elementContainer[el]->getElementNodes()[j];
					int MBEG = JAVec[jb - 1];
					int MEND = JAVec[jb];

					for (int k = MBEG; k < MEND; k++)
					{
						if (ib == IAVec[k])
						{
							M = k;
							break;
						}
					}
				//	double val = UMAT(M, 0) + DTRANE[i][j] + BTRANE[i][j];
				//	UMAT(M, 0) = val;
					UMATT[M] = UMATT[M] +DTRANE[i][j] + BTRANE[i][j];

				}
			}


		}
	}

	
}
void ControlParameters::GLOBAN(){}
void ControlParameters::ELEMN2(){}
void ControlParameters::ADSORB()
{
	if (ADSMOD == "NONE")
	{
		for (int i = 1; i <= NN; i++)
		{
			Node * n = nodeContainer[i];
			n->setCS1(0);
			n->setCS2(0);
			n->setCS3(0);
			n->setSL(0);
			n->setSR(0);
		}
	}

	if (ADSMOD == "LINEAR")
	{
		for (int i = 1; i <= NN; i++)
		{
			Node * n = nodeContainer[i];
			n->setCS1(CHI1*RHOW0);
			n->setCS2(0);
			n->setCS3(0);
			n->setSL(CHI1*RHOW0);
			n->setSR(0);
		}

	}

	if (ADSMOD == "FREUNDLICH")
	{
		double CHCH = CHI1 / CHI2;
		double DCHI2 = 1.0 / CHI2;
		double RH2 = pow(RHOW0,DCHI2);
		double CHI2F = ((1.0 - CHI2) / CHI2);
		for (int i = 1; i <= NN; i++)
		{
			Node * n = nodeContainer[i];
			double UCH = 0;
			if (n->getU() <= 0)
			{
				UCH = 1;
			}
			else
			{
				UCH = pow(n->getU(), CHI2F);
			}
			double RU = RH2 *UCH;
			n->setCS1(CHCH*RU);
			n->setCS2(0);
			n->setCS3(0);
			n->setSL(CHI1*RU);
			n->setSR(0);

		}
		
	}
	if (ADSMOD == "LANGMUIR")
	{
		for (int i = 1; i <= NN; i++)
		{
			Node * n = nodeContainer[i];
			double DD = 1.0 + CHI2*RHOW0*n->getU();

			n->setCS1(CHI1*RHOW0/(DD*DD));
			n->setCS2(0);
			n->setCS3(0);
			n->setSL(CHI1*RHOW0 / (DD*DD));
			n->setSR(CHI1*RHOW0 / (DD*DD) * CHI2 * RHOW0 * n->getU() * n->getU());
		}



	}
}
void ControlParameters::NODAL()
{
	if (IUNSAT != 0)
		IUNSAT = 1;

	if (KSOLVP == 0)
		JMID = NBHALF-1;
	else
		JMID = 0;

	// DO NOT UPDATE NODAL PARAMETERS ON A TIME STEP WHEN ONLY U IS SOLVED FOR BY BACK SUBSTITION (I.E. WHEN NOUMAT = 1)
	if (NOUMAT <= 0)
	{
		// SET UNSATURATED FLOW PARAMETERS AT NODES, SW AND DSWDP
		for (int i = 1; i <= NN; i++)
		{
			if ((IUNSAT - 1) == 0)
			{	
				if (nodeContainer[i]->getPITER() < 0)
				{
					UNSAT(nodeContainer[i]);
				}
				else
				{
					nodeContainer[i]->setSW(1.0);
					nodeContainer[i]->setDSWDP(0.0);
				}
			}
		}

		for (int i = 1; i <= NN; i++)
		{
			BUBSAT(nodeContainer[i]);
		}

		for (int i = 1; i <= NN; i++)
		{
			nodeContainer[i]->setRHO(RHOW0 + DRWDU*(nodeContainer[i]->getUITER() - URHOW0));
		}
	}

	for (int i = 1; i <= NN; i++)
	{
		if (KSOLVP == 0)
			IMID = i-1;
		else
			IMID = JAVec[i - 1];

		SWRHON = nodeContainer[i]->getSWT()*nodeContainer[i]->getRHO();

		if ((ML - 1) <= 0)
		{
			//AFLN = (1 - ISSFLO / 2)*(SWRHON * nodeContainer[i]->getSOP() + nodeContainer[i]->getPorosity()*nodeContainer[i]->getSWB*)
			double term1, term2, term3, term4, term5;
			double AFLN, CFLN, DUDT;
			term2 = PSTAR + nodeContainer[i]->getPITER();
			term1 = (nodeContainer[i]->getDSWDP() + nodeContainer[i]->getSW()*(1.0 - nodeContainer[i]->getSWB())) / term2;
			term3 = nodeVOL[i-1] / (TFINISH - TSTART);//DELTP;// nodeContainer[i]->getVOL() / DELTP;
			AFLN = (1 - ISSFLO / 2)*(SWRHON * nodeContainer[i]->getSOP() + nodeContainer[i]->getPorosity()*nodeContainer[i]->getSWB() *nodeContainer[i]->getRHO()*term1)*term3;
			CFLN = nodeContainer[i]->getPorosity() * nodeContainer[i]->getSWT()*DRWDU*nodeVOL[i-1];
			DUDT = (1 - ISSFLO / 2)*(nodeContainer[i]->getUM1() - nodeContainer[i]->getUM2()) / DLTUM1;
			CFLN = CFLN*DUDT - (nodeContainer[i]->getSW()*GCONST*TEMP*nodeContainer[i]->getPorosity()*nodeContainer[i]->getRHO()*(pow(nodeContainer[i]->getSWB(), 2) / term2)*(0.5*-1 * PRODF1*(nodeContainer[i]->getRHO()*nodeContainer[i]->getUITER() / SMWH)))*nodeContainer[i]->getVOL();
		//	PMAT(IMID, JMID) = PMAT(IMID, JMID) + AFLN;
			PMATT[IMID] = PMATT[IMID] + AFLN;
			term4 = node_p_rhs[i-1] - CFLN + AFLN * nodeContainer[i]->getPM1() + nodeContainer[i]->getQIN();
			p_rhs[i - 1] = term4;
			//p_rhs.push_back(term4);

			if ((ML - 1) == 0)
				continue;


			double EPRS, ATRN, GTRN, GSV, GSLTRN, GSRTRN, ETRN, QUR, QUL;

			EPRS = (1.0 - nodeContainer[i]->getPorosity())*RHOS;
			ATRN = (1 - ISSTRA)*(nodeContainer[i]->getPorosity()*SWRHON*CW + EPRS * nodeContainer[i]->getCS1())*nodeVOL[i - 1] / DELTU;
			GTRN = nodeContainer[i]->getPorosity()*SWRHON*PRODF1*nodeVOL[i - 1];
			GSV = EPRS*PRODS1*nodeVOL[i - 1];
			GSLTRN = GSV * nodeContainer[i]->getSL();
			GSRTRN = GSV * nodeContainer[i]->getSR();
			ETRN = (nodeContainer[i]->getPorosity()*SWRHON*PRODF0 + EPRS*PRODS0)*nodeVOL[i - 1];

			QUR = 0;
			QUL = 0;
			if (nodeContainer[i]->getQINITR() <= 0)
			{
				if (NOUMAT <= 0)
				{
				//	UMAT(IMID, JMID) = UMAT(IMID, JMID) + ATRN - GTRN - GSLTRN - QUL;
					UMATT[IMID] = UMATT[IMID] + ATRN - GTRN - GSLTRN - QUL;
					nodeUVEC[i] = nodeUVEC[i] + ATRN*  nodeContainer[i]->getUM1() + ETRN + GSRTRN + QUR + nodeContainer[i]->getQUIN();
				}
				else
				{
					nodeUVEC[i] = nodeUVEC[i] + ATRN*  nodeContainer[i]->getUM1() + ETRN + GSRTRN + QUR + nodeContainer[i]->getQUIN();
				}
			}
			else
			{
				QUL = -CW*nodeContainer[i]->getQINITR();
				QUR = -QUL * nodeContainer[i]->getUIN();
				if (NOUMAT <= 0)
				{
				//	UMAT(IMID, JMID) = UMAT(IMID, JMID) + ATRN - GTRN - GSLTRN - QUL;
					UMATT[IMID] = UMATT[IMID] + ATRN - GTRN - GSLTRN - QUL;
					nodeUVEC[i] = nodeUVEC[i] + ATRN*  nodeContainer[i]->getUM1() + ETRN + GSRTRN + QUR + nodeContainer[i]->getQUIN();
				}
				else
				{
					nodeUVEC[i] = nodeUVEC[i] + ATRN*  nodeContainer[i]->getUM1() + ETRN + GSRTRN + QUR + nodeContainer[i]->getQUIN();
				}
			}

			
		}
		else
		{
			double EPRS, ATRN, GTRN, GSV, GSLTRN, GSRTRN, ETRN, QUR, QUL;

			EPRS = (1.0 - nodeContainer[i]->getPorosity())*RHOS;
			ATRN = (1 - ISSTRA)*(nodeContainer[i]->getPorosity()*SWRHON*CW + EPRS * nodeContainer[i]->getCS1())*nodeContainer[i]->getVOL() / DELTU;
			GTRN = nodeContainer[i]->getPorosity()*SWRHON*PRODF1*nodeContainer[i]->getVOL();
			GSV = EPRS*PRODS1*nodeContainer[i]->getVOL();
			GSLTRN = GSV * nodeContainer[i]->getSL();
			GSRTRN = GSV * nodeContainer[i]->getSR();
			ETRN = (nodeContainer[i]->getPorosity()*SWRHON*PRODF0 + EPRS*PRODS0)*nodeContainer[i]->getVOL();

			QUR = 0;
			QUL = 0;
			if (nodeContainer[i]->getQINITR() <= 0)
			{
				if (NOUMAT <= 0)
				{
					//UMAT(IMID, JMID) = UMAT(IMID, JMID) + ATRN - GTRN - GSLTRN - QUL;
					UMATT[IMID] = UMATT[IMID] + ATRN - GTRN - GSLTRN - QUL;
					nodeUVEC[i] = nodeUVEC[i] + ATRN*  nodeContainer[i]->getUM1() + ETRN + GSRTRN + QUR + nodeContainer[i]->getQUIN();
				}
				else
				{
					nodeUVEC[i] = nodeUVEC[i] + ATRN*  nodeContainer[i]->getUM1() + ETRN + GSRTRN + QUR + nodeContainer[i]->getQUIN();
				}
			}
			else
			{
				QUL = -CW*nodeContainer[i]->getQINITR();
				QUR = -QUL * nodeContainer[i]->getUIN();
				if (NOUMAT <= 0)
				{
				//	UMAT(IMID, JMID) = UMAT(IMID, JMID) + ATRN - GTRN - GSLTRN - QUL;
					UMATT[IMID] = UMATT[IMID] +ATRN - GTRN - GSLTRN - QUL;
					nodeUVEC[i] = nodeUVEC[i] + ATRN*  nodeContainer[i]->getUM1() + ETRN + GSRTRN + QUR + nodeContainer[i]->getQUIN();
				}
				else
				{
					nodeUVEC[i] = nodeUVEC[i] + ATRN*  nodeContainer[i]->getUM1() + ETRN + GSRTRN + QUR + nodeContainer[i]->getQUIN();
				}
			}
		}
	}



}
void ControlParameters::BC()
{
	if (KSOLVP == 0)
	{
		JMID = NBHALF-1;
	}
	else
	{
		JMID = 0;
	}
	int ml_Case;

	if (NPBC != 0)
	{
		int ind = -1;
		if ((ML - 1) < 0)
		{
			ml_Case = 0;
		}
		else if ((ML - 1) == 0)
		{
			ml_Case = 1;
		}
		else
		{
			ml_Case = 2;
		}

		for (int i = 0; i < NPBC; i++)
		{
			ind = abs(IPBC[i]);
			if (KSOLVP == 0)
			{
				IMID = ind-1;
			}
			else
			{
				IMID = JAVec[ind-1];
			}


			switch (ml_Case)
			{
			case 0:
				{
					  GPINL  = - 1 * nodeContainer[ind]->getGNUP1();
					  GPINR = nodeContainer[ind]->getGNUP1() * nodePBC[ind];
			//		  PMAT(IMID, JMID) = PMAT(IMID, JMID) - GPINL;
					  PMATT[IMID] = PMATT[IMID] - GPINL;
					  p_rhs[ind-1] = p_rhs[ind-1] + GPINR;
					  GUR = 0.0;
					  GUL = 0.0;
					  if (nodeContainer[ind]->getQPLITR() > 0)
					  {
						  GUL = -CW*nodeContainer[ind]->getQPLITR();
						  GUR = -GUL*nodeUBC[ind];
						  if (NOUMAT > 0)
						  {
							  nodeUVEC[ind] = nodeUVEC[ind] + GUR;
						  }
						  else
						  {
							//  UMAT(IMID, JMID) = UMAT(IMID, JMID) - GUL;
							  UMATT[IMID] = UMATT[IMID] - GUL;
							  nodeUVEC[ind] = nodeUVEC[ind] + GUR;
						  }
					  }
					  else
					  {
						  if (NOUMAT > 0)
						  {
							  nodeUVEC[ind] = nodeUVEC[ind] + GUR;
						  }
						  else
						  {
							//  UMAT(IMID, JMID) = UMAT(IMID, JMID) - GUL;
							  UMATT[IMID] = UMATT[IMID] - GUL;
							  nodeUVEC[ind] = nodeUVEC[ind] + GUR;
						  }
					  }
			}break;
			case 1:
				{
					  GPINL = -1 * nodeContainer[ind]->getGNUP1();
					  GPINR = nodeContainer[ind]->getGNUP1() * nodePBC[ind];
					//  PMAT(IMID, JMID) = PMAT(IMID, JMID) - GPINL;
					  PMATT[IMID] = PMATT[IMID] - GPINL;
					  nodePVEC[ind] = nodePVEC[ind] + GPINR;
				}break;
			case 2:
				{
					  GUR = 0.0;
					  GUL = 0.0;
					  if (nodeContainer[ind]->getQPLITR() > 0)
					  {
						  GUL = -CW*nodeContainer[ind]->getQPLITR();
						  GUR = -GUL*nodeUBC[ind];
						  if (NOUMAT > 0)
						  {
							  nodeUVEC[ind] = nodeUVEC[ind] + GUR;
						  }
						  else
						  {
							//  UMAT(IMID, JMID) = UMAT(IMID, JMID) - GUL;
							  UMATT[IMID] = UMATT[IMID] - GUL;
							  nodeUVEC[ind] = nodeUVEC[ind] + GUR;
						  }
					  }
					  else
					  {
						  if (NOUMAT > 0)
						  {
							  nodeUVEC[ind] = nodeUVEC[ind] + GUR;
						  }
						  else
						  {
							//  UMAT(IMID, JMID) = UMAT(IMID, JMID) - GUL;
							  UMATT[IMID] = UMATT[IMID] - GUL;
							  nodeUVEC[ind] = nodeUVEC[ind] + GUR;
						  }
					  }
				}break;
			default:break;
			}





		}

		if (ml_Case != 1)
		{
			if (NUBC != 0)
			{
				int jnd = -1;
				for (int j = 0; j < NUBC; j++)
				{
					jnd = j + NPBC;
					jnd = abs(IUBC[jnd]);
					if (KSOLVP == 0)
					{
						IMID = jnd-1;
					}
					else
					{
						IMID = JA(jnd-1);
					}

					if (NOUMAT > 0)
					{
						GUINR = nodeContainer[jnd]->getGNUU1() * nodeUBC[jnd];
						nodeUVEC[jnd] = nodeUVEC[jnd] + GUINR;
					}
					else
					{
						GUINL = -1 * nodeContainer[jnd]->getGNUU1();
					//	UMAT(IMID, JMID) = UMAT(IMID, JMID) - GUINL;
						UMATT[IMID] = UMATT[IMID] - GUINL;
						GUINR = nodeContainer[jnd]->getGNUU1() * nodeUBC[jnd];
						nodeUVEC[jnd] = nodeUVEC[jnd] + GUINR;
						
					}
				}
			}
		}


		
	}
	for (int i = 1; i <= NN; i++)
		u_rhs[i-1]=nodeUVEC[i];
}
void ControlParameters::UNSAT(Node * node)
{ // Args  SW,DSWDP,RELK,PRES,KREG
//	E X A M P L E   C O D I N G   FOR                                 
//		    MESH WITH TWO REGIONS OF UNSATURATED PROPERTIES USING           
//		    THREE PARAMETER - UNSATURATED FLOW RELATIONSHIPS OF                 
//		    VAN GENUCHTEN(1980)                                              
//	        RESIDUAL SATURATION, SWRES, GIVEN IN UNITS{ L**0 }              
//		    PARAMETER, AA, GIVEN IN INVERSE PRESSURE UNITS{ m*(s**2) / kg }  
//		    PARAMETER, VN, GIVEN IN UNITS{ L**0 }                          
	float SWRES, AA, VN, SWRM1, AAPVN, VNF, AAPVNN, DNUM, DNOM, SWSTAR, SWRES1, SWRES2, AA1, AA2, VN1, VN2;
	SWRES1 = 0.30;
	AA1 = 5.0e-5;
	VN1 = 2.0;
	SWRES2 = SWRES1;
	AA2 = AA1;
	VN2 = VN1;

	//BECAUSE THIS ROUTINE IS CALLED OFTEN FOR UNSATURATED FLOW RUNS,
	//EXECUTION TIME MAY BE SAVED BY CAREFUL CODING OF DESIRED
	//RELATIONSHIPS USING ONLY INTEGER AND SINGLE PRECISION VARIABLES!
	//RESULTS OF THE CALCULATIONS MUST THEN BE PLACED INTO DOUBLE
	//PRECISION VARIABLES SW, DSWDP AND RELK BEFORE LEAVING
	//THIS SUBROUTINE.

	// Set Parameters for current region, KREG
	switch (node->getNREG())
	{
	case 1:
		{
			  SWRES = SWRES1;
			  AA = AA1;
			  VN = VN1;
		}break;
	case 2:
		{
			  SWRES = SWRES2;
			  AA = AA2;
			  VN = VN2;
		}break;
	default:break;
	}

	// Section (1):
	// SW VS. PRES (Value calculated on each call to UNSAT)
	// Coding MUST GIVE A VALUE TO SATURATION ,SW

	// Three parameter model of van Genuchten(1980)

	SWRM1 = 1.0 - SWRES;
	AAPVN = 1 + pow((AA*(-1 * node->getPVEC())),VN);
	VNF = (VN - 1.0) / VN;
	AAPVNN = pow(AAPVN, VNF);
	node->setSW(SWRES + SWRM1 / AAPVNN);

	int cond = IUNSAT - 2;

	if (cond == 0)
	{
		SWSTAR = (node->getSW() - SWRES) / SWRM1;
		node->setRELK(sqrt(SWSTAR)*pow(1.0 - pow(1.0 - pow(SWSTAR, (1.0 / VNF)), VNF), 2));
	}
	if (cond < 0)
	{
		DNUM = AA * (VN - 1.0)*SWRM1*pow((AA*(-1 * node->getPVEC())), VN - 1.0);
		DNOM = AAPVN * AAPVNN;
		node->setDSWDP(DNUM / DNOM);
	}

}
void ControlParameters::BUBSAT(Node * node)
{
	int N = 2;

	double NUM = (PSTAR + node->getPVEC());
	double NOM = (NUM + node->getCNUB()*GCONST*TEMP);
	node->setSWB(NUM / NOM);
	node->setRELKB(pow(node->getSWB(), N));

	node->setSWT(node->getSWB()*node->getSW());
	node->setRELKT(node->getRELKB()*node->getRELK());
}

void ControlParameters::SOURCE1(double& QIN1, double& UIN1, double IQSOP1, double& QUIN1, double IQSOU1, double& IQSOPT1, double& IQSOUT1, int NSOP1, int NSOU1, string BCSID){
	//TO READ AND ORGANIZE TIME-DEPENDENT FLUID MASS SOURCE DATA AND
	//SOLUTE MASS SOURCE DATA SPECIFIED IN THE OPTIONAL BCS INPUT FILE.
	//NSOPI1 IS ACTUAL NUMBER OF TIME - STEP - DEPENDENT FLUID SOURCE NODES.
	//NSOUI1 IS ACTUAL NUMBER OF TIME-STEP-DEPENDENT SOLUTE MASS SOURCE NODES.
	
	int NSOPI1 = NSOP1 - 1;
	int NSOUI1 = NSOU1 - 1;

	int NIQP = 0;
	int NIQU = 0;

	IQSOPT1 += 1;
	IQSOUT1 += 1;

	int IQP,IQU;


	if (NSOPI1 != 0)
	{
		IQSOPT1 -= 1;
		for (BCS* bcs : bcsContainer)
		{ // Checks all defined bcs in .BCS file
			int ind = -1;
			for (int IQCP : bcs->getNodes())
			{
				ind += 1;
				int IQP = abs(IQCP);
				NIQP = NIQP + 1;
				if (IQCP == 0)
				{
					NIQP = NIQP - 1;
					if (NIQP == NSOPI1)
						goto lblNSOU;

					exitOnError("BCS-2,3-1");
				}
				else if (IQP > NN)
				{
					exitOnError("BCS-3-1");
				}
				else if (NIQP > NSOPI1)
				{
					continue;
				}

				if (IQCP > 0)
				{
					if (bcs->getQINC()[ind] > 0)
					{
						nodeContainer[IQP]->setQIN1(bcs->getQINC()[ind]);
						nodeContainer[IQP]->setUIN1(bcs->getUINC()[ind]);
					}
					else
					{
						nodeContainer[IQP]->setQIN1(bcs->getQINC()[ind]);
					}

				}		
			}

		}

	}
	lblNSOU:
	if (NSOUI1 != 0)
	{
		IQSOUT1 = -1;
		for (BCS* bcs : bcsContainer)
		{ // Checks all defined bcs in .BCS file
			int ind = -1;
			for (int IQCU : bcs->getNodes())
			{	
				ind += 1;
				int IQU = abs(IQCU);
				NIQU = NIQU + 1;
				if (IQCU == 0)
				{
					NIQU = NIQU - 1;
					if (NIQU == NSOUI1)
						return;

					exitOnError("BCS-2,3-1");
				}
				else if (IQU > NN)
				{
					exitOnError("BCS-4-1");
				}
				else if (NIQU > NSOUI1)
				{
					continue;
				}
				
				if (IQCU > 0)
				{
					nodeContainer[IQU]->setQUIN1(bcs->getQUINC()[ind]);
				}
			}

		}

	}

	

}
void ControlParameters::BOUND1(int NPBC1,int NUBC1,int NBCN1,string BCSID)
{
	int ip;
	int iu = 0;
	int ipu = 0;
	ip = ipu = iu = 0;
	/*IPBCT1 = +1;
	IUBCT1 = +1;*/

	if (NPBC1 != 0)
	{
		for (BCS * bcs : bcsContainer)
		{
			int ind = -1;
			for (int IDUM : bcs->getNodes())
			{
				ind += 1;
				ipu += 1;
				int IDUMA = abs(IDUM);

				if (IDUM == 0)
				{
					ipu -= 1;
					ip = ipu;
					if (ip == NPBC1)
						goto lblNUBC1;
					exitOnError("BCS-2,5-1");
				}
				else if (IDUMA > NN)
				{
					exitOnError("BCS-5-1");
				}
				else if (ipu > NPBC1)
				{
					continue;
				}

				if (IDUM > 0)
				{
					
					nodeContainer[IDUMA]->setPBC1(bcs->getPBC()[ind]);
					nodeContainer[IDUMA]->setUBC1(bcs->getUBC()[ind]);
				}
				else if (IDUM < 0)
				{
					continue;
				}
				else
				{
					ipu -= 1;
					ip = ipu;
					if (ip == NPBC1)
						goto lblNUBC1;
					exitOnError("BCS-2,5-1");
				}
			}
		}
	}

	lblNUBC1:
	if (NUBC1 != 0)
	{
		for (BCS * bcs : bcsContainer)
		{
			int ind = -1;
			for (int IDUM : bcs->getNodes())
			{
				ind += 1;
				ipu += 1;
				int IDUMA = abs(IDUM);

				if (IDUM == 0)
				{
					ipu -= 1;
					iu = ipu;
					if (iu == NUBC1)
						return;
					exitOnError("BCS-2,6-1");
				}
				else if (IDUMA > NN)
				{
					exitOnError("BCS-6-1");
				}
				else if (ipu > (NUBC1+NPBC1))
				{
					continue;
				}

				if (IDUM > 0)
				{
					nodeContainer[IDUMA]->setUBC1(bcs->getUBC()[ind]);
				}
				else if (IDUM < 0)
				{
					continue;
				}
				else
				{
					ipu -= 1;
					iu = ipu;
					if (iu == NUBC1)
						return;
					exitOnError("BCS-2,6-1");;
				}
			}
		}
		
	}

}

void ControlParameters::setITERPARAMS(){

	if (ML == 0){
		for (int i = 1; i <= NN; i++){
			nodeContainer[i]->setPITER(BDELP1*nodeContainer[i]->getPVEC() - BDELP*nodeContainer[i]->getPM1());
			nodeContainer[i]->setUITER(BDELU1*nodeContainer[i]->getUVEC() - BDELU*nodeContainer[i]->getUM1());
			nodeContainer[i]->setRCITM1(nodeContainer[i]->getRCIT());
			nodeContainer[i]->setRCIT(RHOW0 + DRWDU*(nodeContainer[i]->getUITER() - URHOW0));
			nodeContainer[i]->setPM1(nodeContainer[i]->getPVEC());
		}
	}
}

void ControlParameters::BASIS3(int ICALL,int el,int node,int realNode, double XLOC, double YLOC, double ZLOC,double & PORGT){
	int nodeNum;
	double ODET;
	double XF1, XF2, YF1, YF2, ZF1, ZF2;
	double XIIX[8] = { -1,  1,  1, -1, -1,  1,  1, -1 };
	double YIIY[8] = { -1, -1,  1,  1, -1, -1,  1,  1 };
	double ZIIZ[8] = { -1, -1, -1, -1,  1,  1,  1,  1 };
	XF1 = 1 - XLOC;
	XF2 = 1 + XLOC;
	YF1 = 1 - YLOC;
	YF2 = 1 + YLOC;
	ZF1 = 1 - ZLOC;
	ZF2 = 1 + ZLOC;
	double FX[8] = { XF1, XF2, XF2, XF1, XF1, XF2, XF2, XF1 };
	double FY[8] = { YF1, YF1, YF2, YF2, YF1, YF1, YF2, YF2 };
	double FZ[8] = { ZF1, ZF1, ZF1, ZF1, ZF2, ZF2, ZF2, ZF2 };
	double DWDXL[8] = {0};
	double DWDYL[8] = {0};
	double DWDZL[8] = {0};
	double DFDXL[8] = {0};
	double DFDYL[8] = {0};
	double DFDZL[8] = {0};
	double AA, BB, CC;
	double XIXI, YIYI, ZIZI;
	double AFX[8] = {0};
	double AFY[8] = {0};
	double AFZ[8] = {0};
	double XDW[8] = {0};
	double YDW[8] = {0};
	double ZDW[8] = {0};
	double THAAX, THBBY, THCCZ;
	AA = BB = CC = 0;
	double CJ[3][3] = {};
	double CIJ[3][3] = {};
	double VV;
	double DET;
	double SWG, RELKG;
	double RGXL,RGYL,RGZL,RGXLM1,RGYLM1,RGZLM1;
	RGXL= RGYL= RGZL= RGXLM1= RGYLM1= RGZLM1 = 0;
	double RGXGM1, RGYGM1, RGZGM1;
	RGXGM1 = RGYGM1 = RGZGM1 = 0;
	double ADFDXL, ADFDYL, ADFDZL;
	double PITERG, UITERG, CNUBG, DPDXG, DPDYG, DPDZG;
	PITERG = UITERG = CNUBG = DPDXG = DPDYG = DPDZG = 0;
	//VXG[8] = VYG[8] = VZG[8] = { 0 };
	double DENOM, PGX, PGY, PGZ;
	double GXSI, GETA, GZET;
	double VLMAG, VXL, VYL, VZL; // LOCAL VELOCITIES
	double SWBG, RELKBG;
	double DSWDPG;

	for (int i = 0; i < 8; i++){
		F[node][i] = 0.125*FX[i] * FY[i] * FZ[i];
	}
	// CALCULATE Derivatives wrt local coords
	for (int i = 0; i < 8; i++){
		DFDXL[i] = XIIX[i] * 0.125*FY[i] * FZ[i];
		DFDYL[i] = YIIY[i] * 0.125*FX[i] * FZ[i];
		DFDZL[i] = ZIIZ[i] * 0.125*FX[i] * FY[i];
	}

	// CALCULATE ELEMENTS OF JACoBIAN MATRIX;
	
	for (int i = 0; i < 8; i++){
		nodeNum = elementNodes[el][i];
		CJ[0][0] = CJ[0][0] + DFDXL[i] * nodeContainer[nodeNum]->getXCoord();
		CJ[0][1] = CJ[0][1] + DFDXL[i] * nodeContainer[nodeNum]->getYCoord();
		CJ[0][2] = CJ[0][2] + DFDXL[i] * nodeContainer[nodeNum]->getZCoord();
		CJ[1][0] = CJ[1][0] + DFDYL[i] * nodeContainer[nodeNum]->getXCoord();
		CJ[1][1] = CJ[1][1] + DFDYL[i] * nodeContainer[nodeNum]->getYCoord();
		CJ[1][2] = CJ[1][2] + DFDYL[i] * nodeContainer[nodeNum]->getZCoord();
		CJ[2][0] = CJ[2][0] + DFDZL[i] * nodeContainer[nodeNum]->getXCoord();
		CJ[2][1] = CJ[2][1] + DFDZL[i] * nodeContainer[nodeNum]->getYCoord();
		CJ[2][2] = CJ[2][2] + DFDZL[i] * nodeContainer[nodeNum]->getZCoord();
	}
	
	DET = CJ[0][0] * (CJ[1][1] * CJ[2][2] - CJ[2][1] * CJ[1][2]) -
		CJ[1][0] * (CJ[0][1] * CJ[2][2] - CJ[2][1] * CJ[0][2]) +
		CJ[2][0] * (CJ[0][1] * CJ[1][2] - CJ[1][1] * CJ[0][2]);
	elementContainer[el]->putIntoDET(node,DET);
	ODET = 1.0 / DET;

	
	
	if (ICALL == 0){
		GXSI = CJ[0][0] * GRAVX + CJ[0][1] * GRAVY + CJ[0][2] * GRAVZ;
		GETA = CJ[1][0] * GRAVX + CJ[1][1] * GRAVY + CJ[1][2] * GRAVZ;
		GZET = CJ[2][0] * GRAVX + CJ[2][1] * GRAVY + CJ[2][2] * GRAVZ;
		elementContainer[el]->putIntoGXSI(node, GXSI);
		elementContainer[el]->putIntoGETA(node, GETA);
		elementContainer[el]->putIntoGZET(node, GZET);
	}

	if (ICALL == 0)
		return;

	CIJ[0][0] = +ODET*(CJ[1][1] * CJ[2][2] - CJ[2][1] * CJ[1][2]);
	CIJ[0][1] = -ODET*(CJ[0][1] * CJ[2][2] - CJ[2][1] * CJ[0][2]);
	CIJ[0][2] = +ODET*(CJ[0][1] * CJ[1][2] - CJ[1][1] * CJ[0][2]);
	CIJ[1][0] = -ODET*(CJ[1][0] * CJ[2][2] - CJ[2][0] * CJ[1][2]);
	CIJ[1][1] = +ODET*(CJ[0][0] * CJ[2][2] - CJ[2][0] * CJ[0][2]);
	CIJ[1][2] = -ODET*(CJ[0][0] * CJ[1][2] - CJ[1][0] * CJ[0][2]);
	CIJ[2][0] = +ODET*(CJ[1][0] * CJ[2][1] - CJ[2][0] * CJ[1][1]);
	CIJ[2][1] = -ODET*(CJ[0][0] * CJ[2][1] - CJ[2][0] * CJ[0][1]);
	CIJ[2][2] = +ODET*(CJ[0][0] * CJ[1][1] - CJ[1][0] * CJ[0][1]);
	//Calculate3 determinates in Global coordinates
	
	for (int i = 0; i < 8; i++){
		DFDXG[node][i] = CIJ[0][0] * DFDXL[i] + CIJ[0][1] * DFDYL[i] + CIJ[0][2] * DFDZL[i];
		DFDYG[node][i] = CIJ[1][0] * DFDXL[i] + CIJ[1][1] * DFDYL[i] + CIJ[1][2] * DFDZL[i];
		DFDZG[node][i] = CIJ[2][0] * DFDXL[i] + CIJ[2][1] * DFDYL[i] + CIJ[2][2] * DFDZL[i];
	}

	// CALCULATE CONSISTENT COMPONENTS OF (RHO*GRAV) TERM IN LOCAL COORDINATES
	for (int i = 0; i < 8; i++){
		int nodeNumber = elementNodes[el][i];
		ADFDXL = abs(DFDXL[i]);
		ADFDYL = abs(DFDYL[i]);
		ADFDZL = abs(DFDZL[i]);
		RGXL = RGXL + nodeContainer[nodeNumber]->getRCIT() * elementContainer[el]->getGXSI()[i] * ADFDXL;
		RGYL = RGYL + nodeContainer[nodeNumber]->getRCIT() * elementContainer[el]->getGETA()[i] * ADFDYL;
		RGZL = RGZL + nodeContainer[nodeNumber]->getRCIT() * elementContainer[el]->getGZET()[i] * ADFDZL;
		RGXLM1 = RGXLM1 + nodeContainer[nodeNumber]->getRCITM1() * elementContainer[el]->getGXSI()[i] * ADFDXL;
		RGYLM1 = RGYLM1 + nodeContainer[nodeNumber]->getRCITM1() * elementContainer[el]->getGETA()[i] * ADFDYL;
		RGZLM1 = RGZLM1 + nodeContainer[nodeNumber]->getRCITM1() * elementContainer[el]->getGZET()[i] * ADFDZL;
	}

	// TRANSFORM CONSISTENT COMPONENTS OF (RHO*GRAV) TERM TO GLOBAL Coordinates

	RGXG[node] = CIJ[0][0] * RGXL + CIJ[0][1] * RGYL + CIJ[0][2] * RGZL;
	RGYG[node] = CIJ[1][0] * RGXL + CIJ[1][1] * RGYL + CIJ[1][2] * RGZL;
	RGZG[node] = CIJ[2][0] * RGXL + CIJ[2][1] * RGYL + CIJ[2][2] * RGZL;
	RGXGM1 = CIJ[0][0] * RGXLM1 + CIJ[0][1] * RGYLM1 + CIJ[0][2] * RGZLM1;
	RGYGM1 = CIJ[1][0] * RGXLM1 + CIJ[1][1] * RGYLM1 + CIJ[1][2] * RGZLM1;
	RGZGM1 = CIJ[2][0] * RGXLM1 + CIJ[2][1] * RGYLM1 + CIJ[2][2] * RGZLM1;
	PORGT = 0;
	for (int i = 0; i < 8; i++){
		int nodeNum = elementNodes[el][i];
		DPDXG = DPDXG + nodeContainer[nodeNum]->getPVEL()*DFDXG[node][i];
		DPDYG = DPDYG + nodeContainer[nodeNum]->getPVEL()*DFDYG[node][i];
		DPDZG = DPDZG + nodeContainer[nodeNum]->getPVEL()*DFDZG[node][i];
		PORGT = PORGT + nodeContainer[nodeNum]->getPorosity()*F[node][i];
		PITERG = PITERG + nodeContainer[nodeNum]->getPITER()*F[node][i];
		UITERG = UITERG + nodeContainer[nodeNum]->getUITER()*F[node][i];
		CNUBG = CNUBG + nodeContainer[nodeNum]->getCNUB()*F[node][i];
	}

	RHOG[node] = RHOW0 + DRWDU*(UITERG - URHOW0);
	VISCG[node] = VISC0;

	if ((IUNSAT - 2) == 0){
		if (PITERG < 0){
			UNSAT(SWG, DSWDPG, RELKG, PITERG, elementContainer[el]->getLREG());
		}
		else{
			SWG = 1.0;
			RELKG = 1.0;
		}
	}
	else{
		SWG = 1.0;
		RELKG = 1.0;
	}
	RELKBG = SWBG = 0;
	BUBSAT(SWBG, RELKBG, PITERG, CNUBG, RELKTG[node], SWTG[node], SWG, RELKG);

	DENOM = 1.0 / (PORG[node]*SWTG[node] * VISCG[node]);
	PGX = DPDXG - RGXGM1;
	PGY = DPDYG - RGYGM1;
	PGZ = DPDZG - RGZGM1;

	if (DPDXG != 0){
		if ((abs(PGX / DPDXG) - (1E-10)) <= 0)
			PGX = 0.0;
	}
	if (DPDYG != 0){
		if ((abs(PGY / DPDYG) - (1E-10)) <= 0)
			PGY = 0.0;
	}
	if (DPDZG != 0){
		if ((abs(PGZ / DPDZG) - (1E-10)) <= 0)
			PGZ = 0.0;
	}

	VXG[node] = -DENOM*(elementContainer[el]->getPERMXX()*PGX + elementContainer[el]->getPERMXY()*PGY + elementContainer[el]->getPERMXZ()*PGZ)*RELKTG[node];
	VYG[node] = -DENOM*(elementContainer[el]->getPERMYX()*PGX + elementContainer[el]->getPERMYY()*PGY + elementContainer[el]->getPERMYZ()*PGZ)*RELKTG[node];
	VZG[node] = -DENOM*(elementContainer[el]->getPERMZX()*PGX + elementContainer[el]->getPERMZY()*PGY + elementContainer[el]->getPERMZZ()*PGZ)*RELKTG[node];
	VGMAG[node] = sqrt(VXG[node] * VXG[node] + VYG[node] * VYG[node] + VZG[node] * VZG[node]);
	

	if ((UP > (1E-6)) && (NOUMAT == 0))
		goto ASYM;

	for (int i = 0; i < 8; i++){
		W[i] = F[i];
		DWDXG[i] = DFDXG[i];
		DWDYG[i] = DFDYG[i];
		DWDZG[i] = DFDZG[i];
	}
	return;

ASYM:
	//CALCULATE FLUID VELOCITIES WITH RESPECT TO LOCAL COORDINATES,VXL, VYL, VZL, AND VLMAG, AT THIS LOCATION, (XLOC, YLOC, ZLOC).
	VXL = CIJ[0][0] * VXG[node] + CIJ[0][1] * VYG[node] + CIJ[0][2] * VZG[node];
	VYL = CIJ[1][0] * VXG[node] + CIJ[1][1] * VYG[node] + CIJ[1][2] * VZG[node];
	VZL = CIJ[2][0] * VXG[node] + CIJ[2][1] * VYG[node] + CIJ[2][2] * VZG[node];
	VLMAG = sqrt(VXL*VXL + VYL*VYL + VZL*VZL);


	if (VLMAG <= 0){
		XIXI = .7500*AA*XF1*XF2;
		YIYI = .7500*BB*YF1*YF2;
		ZIZI = .7500*CC*ZF1*ZF2;
	}
	else{
		AA = UP* VXL / VLMAG;
		BB = UP*VYL / VLMAG;
		CC = UP*VZL / VLMAG;
	}

	for (int i = 0; i < 8; i++){
		AFX[i] = 0.5*FX[i] + XIIX[i] * XIXI;
		AFY[i] = 0.5*FY[i] + YIIY[i] * YIYI;
		AFZ[i] = 0.5*FZ[i] + ZIIZ[i] * ZIZI;
	}
	//CALCULATE ASYMMETRIC WEIGHTING FUNCTION, W.
	for (int i = 0; i < 8; i++){
		W[i][node] = AFX[i] * AFY[i] * AFZ[i];
	}

	THAAX = 0.5 - 1.5*AA*XLOC;
	THBBY = 0.5 - 1.5*BB*YLOC;
	THCCZ = 0.5 - 1.5*CC*ZLOC;
	for (int i = 0; i < 8; i++){
		XDW[i] = XIIX[i] * THAAX;
		YDW[i] = YIIY[i] * THBBY;
		ZDW[i] = ZIIZ[i] * THCCZ;
	}
	//CALCULATE DERIVATIVES WITH RESPECT TO LOCAL COORDINATES
	for (int i = 0; i < 8; i++){
		DWDXL[i] = XDW[i] * AFY[i] * AFZ[i];
		DWDYL[i] = YDW[i] * AFX[i] * AFZ[i];
		DWDZL[i] = ZDW[i] * AFX[i] * AFY[i];
	}
	//CALCULATE DERIVATIVES WITH RESPECT TO GLOBAL COORDINATES
	for (int i = 0; i < 8; i++){
		DWDXG[node][i] = CIJ[0][0] * DWDXL[i] + CIJ[0][1] * DWDYL[i] + CIJ[0][2] * DWDZL[i];
		DWDYG[node][i] = CIJ[1][0] * DWDXL[i] + CIJ[1][1] * DWDYL[i] + CIJ[1][2] * DWDZL[i];
		DWDZG[node][i] = CIJ[2][0] * DWDXL[i] + CIJ[2][1] * DWDYL[i] + CIJ[2][2] * DWDZL[i];
	}
	
}

void ControlParameters::UNSAT(double& SW,double& DSWDP, double& RELK, double PRES, double KREG){

	// PURPOSE :            
	//	  USER - PROGRAMMED FUNCTION GIVING : 
	//	  (1)  SATURATION AS A FUNCTION OF PRESSURE(SW(PRES))  
	//	  (2)  DERIVATIVE OF SATURATION WITH RESPECT TO PRESSURE   
	//	       AS A FUNCTION OF EITHER PRESSURE OR SATURATION        
	//	       (DSWDP(PRES), OR DSWDP(SW))                      
	//	  (3)  RELATIVE PERMEABILITY AS A FUNCTION OF EITHER 
	//	       PRESSURE OR SATURATION(REL(PRES) OR RELK(SW)) 
	//	                                                     
	//	  CODE BETWEEN DASHED LINES MUST BE REPLACED TO GIVE THE
	//	  PARTICULAR UNSATURATED RELATIONSHIPS DESIRED
	//	  
	//	  DIFFERENT FUNCTIONS MAY BE GIVEN FOR EACH REGION OF THE MESH.
	//	  REGIONS ARE SPECIFIED BY BOTH NODE NUMBER AND ELEMENT NUMBER
	//	  IN INPUT DATA FILE FOR UNIT K1(INP).


	/*E X A M P L E   C O D I N G   FOR                                 
		     MESH WITH TWO REGIONS OF UNSATURATED PROPERTIES USING              
		     THREE PARAMETER - UNSATURATED FLOW RELATIONSHIPS OF                 
		     VAN GENUCHTEN(1980)                                            
		        RESIDUAL SATURATION, SWRES, GIVEN IN UNITS{ L**0 }           
		        PARAMETER, AA, GIVEN IN INVERSE PRESSURE UNITS{ m*(s**2) / kg }    
		        PARAMETER, VN, GIVEN IN UNITS{ L**0 }                           */

	double SWRES, AA, VN, SWRM1, AAPVN, VNF, AAPVNN, DNUM, DNOM, SWSTAR;
	double SWRES1, SWRES2, AA1, AA2, VN1, VN2;
	SWRES1 = SWRES2 = 0.3;
	AA1 = AA2 = 5e-5;
	VN1 = VN2 = 2.0;

	if (KREG == 1){
		SWRES = SWRES1;
		AA = AA1;
		VN = VN1;
	}
	else{
		SWRES = SWRES2;
		AA = AA2;
		VN = VN2;
	}

	//SECTION(1) :
	//	   SW VS.PRES(VALUE CALCULATED ON EACH CALL TO UNSAT)
	//	   CODING MUST GIVE A VALUE TO SATURATION, SW.

	SWRM1 = 1 - SWRES;
	AAPVN = 1 + pow(AA*(-PRES), VN);
	VNF = (VN - 1) / VN;
	AAPVNN = pow(AAPVN, VNF);
	SW = (SWRES + SWRM1) / AAPVNN;

	if ((IUNSAT - 2) < 0){
		DNUM = AA*(VN - 1)*SWRM1*pow(AA*(-PRES), (VN - 1));
		DNOM = AAPVN*AAPVNN;
		DSWDP = DNUM / DNOM;
	}
	else if ((IUNSAT - 2) == 0){
		SWSTAR = (SW - SWRES) / SWRM1;
		RELK = sqrt(SWSTAR*pow(1 - pow(1 - pow(SWSTAR, 1 / VNF), VNF), 2));
	}
}

void ControlParameters::BUBSAT(double& SWBG, double&  RELKBG, double  PITERG, double  CNUBG, double&  RELKTG, double& SWTG, double SWG, double  RELKG){
	int n = 2; // this should change
	SWBG = (PSTAR + PITERG) / ((PSTAR + PITERG) + (CNUBG*GCONST*TEMP));
	RELKBG = pow(SWBG, n);
	SWTG = SWBG * SWG;
	RELKTG = RELKBG*RELKG;
}

void ControlParameters::createSolverMatrix(){
	if (KSOLVP == 0){ // Direct Solver
		PMAT = MatrixXd::Zero(NELT, NBI);
		UMAT =MatrixXd::Zero(NELT, NBI);
		PPVEC = VectorXd::Zero(NNVEC);
		UUVEC = VectorXd::Zero(NNVEC);
		FWK = VectorXd::Zero(1);
		IWK = VectorXd::Zero(1);
	}
	else{

		int NWIP, NWIU, NWFP, NWFU;
		NWIP = NWIU = NWFP = NWFU = 0;
		DIMWRK(KSOLVP, NSAVEP, NWIP, NWFP);
		DIMWRK(KSOLVU, NSAVEU, NWIU, NWFU);
		NWI = max(NWIP, NWIU);
		NWF = max(NWFP, NWFU);

		//PMAT = MatrixXd::Zero(NELT, 1);
		PMATT = vector<double>(NELT, 0);
		//UMAT = MatrixXd::Zero(NELT, 1);
		UMATT = vector<double>(NELT, 0);
		stl_A = vector<map<unsigned int, double>>(NN, map<unsigned int, double>());
		vcl_rhs = viennacl::scalar_vector<double>(NN, 0);
		PPVEC = VectorXd::Zero(NNVEC);
		UUVEC = VectorXd::Zero(NNVEC);
		FWK = VectorXd::Zero(NWF);
		IWK = VectorXd::Zero(NWI);

	}
}

void ControlParameters::DIMWRK(int KSOLVR,int NSAVE,int& NWI,int &NWF){
	if (KSOLVR == 1){
		NL = (NELT + NN) / 2;
		NWI = 11 + 2 * NL;
		NWF = NL + 5 * NN + 1;
	}
	else if (KSOLVR == 2){
		NWI = NELT * 2 + 31;
		NWF = 2 + (NN * (NSAVE + 7)) + NSAVE *(NSAVE + 3) + (NELT - NN);
	}
	else if (KSOLVR == 3){
		NWI = NELT * 2 + 11;
		NWF = 1 + 3 * NN*(NSAVE + 1) + 7 * NN + NSAVE + (NELT - NN);
	}
}

void ControlParameters::PTRSET(){
	vector<int> LLIST;
	vector<vector<int>>HLIST;

	LLIST.reserve(NN);
	HLIST.reserve(NN);
	for (int i = 0; i < NN;i++){
		LLIST.push_back(0);
		HLIST.push_back(vector<int>(0));
	}
	
	int IC, JC;
	IC = JC = 0;

	for (int el = 1; el <= NE; el++){
		
		int * elNodes = elementNodes[el];
		for (int i = 0; i < N48; i++){
			IC = elNodes[i];
			//vector<int> neighbors;
			for (int j = 0; j < N48; j++){
				vector<int>defNeighbors;
				if (!HLIST.empty()){ defNeighbors= HLIST[IC - 1]; }
				JC = elNodes[j];
				if (JC == IC)
					continue;

				if (defNeighbors.empty()){ // Empty
					//neighbors.push_back(JC);
					HLIST[IC - 1].push_back(JC);
					LLIST[IC-1] = LLIST[IC-1] + 1;
				}
				else{
					if (!(find(defNeighbors.begin(), defNeighbors.end(), JC) != defNeighbors.end())){
						//neighbors.push_back(JC);
						HLIST[IC - 1].push_back(JC);
						LLIST[IC-1] = LLIST[IC-1] + 1;
					}
				}
			
			}
		}

	}

	// Sort Neighbors

	for (int i = 0; i < NN;i++){
		vector<int> f = HLIST[i];
		sort(f.begin(), f.end(), [](int& s, int& b){return s < b; });
		HLIST[i] = f;
	}

	for (int x : LLIST){
		NELT = NELT + x + 1;
	}

	IA = VectorXd::Zero(NELT);
	JA = VectorXd::Zero(NN+1);
	int * IAA = new int[NELT + 1];
	int * JAA = new int[NN + 2];

	int JASTART = 1;
	for (int i = 1; i <= NN; i++){
		JAA[i] = JASTART;
		IAA[JASTART] = i;
		for (int j = 1; j <= LLIST[i-1]; j++){
			IAA[JASTART + j] = HLIST[i-1][j-1];
		}
		JASTART = JASTART + LLIST[i-1]+1;
	}
	JAA[NN] = NELT + 1;

	for (int i = 1; i <= (NELT); i++){
		IA(i - 1) = IAA[i];
	}

	for (int i = 1; i <= (NN + 1);i++){
		JA(i - 1) = JAA[i];
	}

}

void ControlParameters::alpayPTR(){
	nodeNeighbors.reserve(NN+1);
	for (int i = 0; i <= NN; i++){
		nodeNeighbors.push_back(vector<int>());
	}
	for (int el = 1; el <= NE; el++){
		for (int node = 0; node < N48; node++){
			int nodeNum = elementNodes[el][node];
			for (int neighbor = 0; neighbor < N48; neighbor++){
				int nNum = elementNodes[el][neighbor];
				if (nodeNum == nNum )
					continue;
				nodeNeighbors[nodeNum].push_back(nNum);
			}
		}

	}

	int NELTX = 0;
	for (int node = 1; node <= NN; node++){
		sort(nodeNeighbors[node].begin(), nodeNeighbors[node].end());
		nodeNeighbors[node].erase(unique(nodeNeighbors[node].begin(), nodeNeighbors[node].end()), nodeNeighbors[node].end());
	 NELTX = NELTX + nodeNeighbors[node].size() + 1;
	}

	NELT = NELTX;

	int ctr = 0;
	for (int i = 0; i < NN; i++)
	{
		JAVec.push_back(ctr);
		IAVec.push_back(i);
		for (int nodN : nodeNeighbors[i + 1])
		{
			IAVec.push_back(nodN-1);
		}
		ctr = ctr + nodeNeighbors[i + 1].size()+1;
	}
	JAVec.push_back(NELT);
	//jJA.reserve(NELT);
	//int dif = 0;
	//ctr = 0;
	//for (int j = 1; j < JAVec.size(); j++)
	//{
	//	dif = JAVec[j] - JAVec[j - 1];

	//	for (int i = 0; i < dif; i++)
	//	{
	//		jJA.push_back(ctr);
	//	}
	//	ctr++;
	//}
}

void ControlParameters::TENSYM(double DL, double DT1, double DT2, double VNX, double UNX, double WNX,double VNY, double UNY, double WNY, double VNZ, double UNZ, double WNZ,
	double& DXXG, double& DXYG, double& DXZG, double& DYXG, double& DYYG, double& DYZG,double& DZXG, double& DZYG, double& DZZG){

	DXXG = DL*VNX*VNX + DT1 * UNX*UNX + DT2 * WNX*WNX;
	DXYG = DL*VNX*VNY + DT1*UNX*UNY + DT2*WNX*WNY;
	DXZG = DL*VNX*VNZ + DT1*UNX*UNZ + DT2*WNX*WNZ;
	DYYG = DL*VNY*VNZ + DT1*UNY*UNY + DT2*WNY*WNY;
	DYZG = DL*VNY*VNZ + DT1*UNY*UNZ + DT2*WNY*WNZ;
	DZZG = DL*VNZ*VNZ + DT1*UNZ*UNZ + DT2*WNZ*WNZ;
	DYXG = DXYG;
	DZXG = DXZG;
	DZYG = DYZG;


	
}
void ControlParameters::setITERPARAMS1(){
	Node * node;
	for (int i = 1; i <= NN; i++){
		node = nodeContainer[i];
		node->setDPDTITR((node->getPVEC() - node->getPM1()) / DELTP);
		node->setPITER(node->getPVEC());
		node->setPVEL(node->getPVEC());
		node->setUITER(node->getUVEC());
		node->setRCITM1(node->getRCIT());
		node->setRCIT(RHOW0 + DRWDU*(node->getUITER() - URHOW0));
	}
}

void ControlParameters::setITERPARAMS2(){
	Node * node;
	for (int i = 1; i <= NN; i++){
		node = nodeContainer[i];
		node->setQINITR(node->getQIN());
	}
}

void ControlParameters::setITERPARAMS3(){
	Node * node;
	for (int i = 1; i <= NN; i++){
		node = nodeContainer[i];
		node->setPITER(BDELP1*node->getPVEC() - BDELP*node->getPM1());
		node->setUITER(BDELU1*node->getUVEC() - BDELU*node->getUM1());
		node->setDPDTITR((node->getPVEC() - node->getPM1()) / DLTPM1);
		node->setPM1(node->getPVEC());
		node->setUM2(node->getUM1());
		node->setUM1(node->getUVEC());
	}
}

double ControlParameters::DNRM2(int N, vector<double>& X, int INCX)
{	
	double NORM, SSQ, SCALE;
	double ABSXI;
	NORM = 0.0;
	if (N < 1 || INCX < 1)
	{
		NORM = 0.0;
	}
	else if (N == 1)
	{
		NORM = abs(X[0]);
	}
	else
	{
		SCALE = 0.0;
		SSQ = 1.0;

		for (int it = 1; it < (1 + (N - 1)*INCX); it += INCX)
		{
			if (X[it] != 0.0)
			{
				ABSXI = abs(X[it]);
				if (SCALE < ABSXI)
				{
					SSQ = 1.0 + SSQ * pow(SCALE / ABSXI, 2);
					SCALE = ABSXI;
				}
				else
				{
					SSQ = SSQ + pow(ABSXI / SCALE, 2);
				}
			}
		}
		NORM = SCALE * sqrt(SSQ);
	}
	return NORM;
}

//void ControlParameters::solveEquation(int KPU, int KSOLVR, MatrixXd& MAT, vector<double>&rhs, vector<double>& solution,int& ITRMXS,double& IERR, int& ITRS, double& ERR){
void ControlParameters::solveEquation(int KPU, int KSOLVR, vector<double>& MAT, vector<double>&rhs, vector<double>& solution,int& ITRMXS,double& IERR, int& ITRS, double& ERR){
	char* KPUTEXT[2] = { "P", "U" };
	double RHSNRM = DNRM2(NNVEC, rhs, 1);
	if (RHSNRM == 0.0)
	{
		IERR = 0;
		ITRS = 0;
		ERR = 0;
		for (int i = 0; i < NN; i++)
		{
			solution[i] = 0.0;
		}

		cout << "       " << KPUTEXT[KPU] << "-solution (" << KPUTEXT[KPU] << "= 0) inferred from matrix equation A*" << KPUTEXT[KPU] << "=0; solver not called." << endl;
		return;
	}
	cout << "       Starting " << KPUTEXT[KPU] << "-solution using " << SOLWRD[KSOLVR] << " solver.." << endl;

	if (KSOLVR == 0)
	{
		SOLVEB(KMT, MAT, rhs,solution);
		IERR = 0;
		ITRS = 0;
		ERR = 0;
	}
	else
	{
		SOLWRP(KPU, KSOLVR, MAT, rhs,solution,ITRMXS,ITRS,ERR);
	}

}


void ControlParameters::SOLVEB(int KMT, vector<double>& MAT, vector<double>&rhs, vector<double>& solution)
{
	
}


//void ControlParameters::SOLWRP(int KPU, int KSOLVR, MatrixXd& MAT, vector<double>&rhs, vector<double>& solution, int& ITRMXS, int& ITRS, double& ERR)
void ControlParameters::SOLWRP(int KPU, int KSOLVR, vector<double>& MAT,vector<double>&rhs , vector<double>& solution,int& ITRMXS,int& ITRS, double& ERR)
{
	//SOLWRP(KPU, KSOLVR, A, R, XITER, B, NNP,IWK, FWK, IA, JA, IERR, ITRS, ERR)
	//SOLWRP(KPU, KSOLVR, C, R, XITER, B, NNP, IWK, FWK, IA, JA, IERR, ITRS, ERR)


		//COPY THE RHS VECTOR R INTO VECTOR B, THEN USE R AS THE
		//SOLUTION VECTOR.INITIALIZE IT FROM THE LATEST SUTRA
		//SOLUTION.XITER IS NOT USED AS THE SOLUTION VECTOR BECAUSE
		//DOING SO MIGHT INTERFERE WITH SUBSEQUENT CALCULATIONS.
	
	//double * B = new double[NN + 1]{};
	char* KPUTEXT[2] = { "P", "U" };

	if (KPU == 0)
	{
		ISYM = 0;
		ITRMX = max(ITRMXP, 1);
		ITOL = ITOLP;
		TOL = TOLP;
		NSAVE = NSAVEP;
	}
	else
	{
		ISYM = 0;
		ITRMX = max(ITRMXU, 1);
		ITOL = ITOLU;
		TOL = TOLU;
		NSAVE = NSAVEU;
	}

	if (KSOLVR == 1)
	{
		BiCGSTAB(MAT, rhs, solution, ITRS, ERR);
	}
	else if (KSOLVR == 2)
	{
		/*if (KPU == 0)
			copy(nodePVEC.begin() + 1, nodePVEC.end(), init_guess.begin());
		else
			copy(nodeUVEC.begin() + 1, nodeUVEC.end(), init_guess.begin());*/
		GMRES(MAT,rhs,solution,ITRMXS,ITRS,ERR);
	}
	else
	{
		ORTHOMIN(MAT, rhs, solution, ITRS, ERR);
	}

	if (IERR == 0)
	{
		cout << "       " << KPUTEXT[KPU] << "-solution converged in " << ITRS << " solver iterations (Error ~ " << ERR << ")" << endl;
	}
	else
	{
		cout << "       " << KPUTEXT[KPU] << "-solution FAILED after " << ITRS << " solver iterations (Error ~ " << ERR << ")" << endl;
	}

}
void ControlParameters::solveTimeStep()
{

	//string filename = InputFiles::Instance()->getInputDirectory() + "\\" + "rhs.txt";
	//ofstream rhsStream;

	//rhsStream.open(filename, std::ios_base::out);
	//rhsStream << p_rhs.size() << endl;
	//for (int i = 0; i < p_rhs.size(); i++)
	//{
	//	rhsStream << p_rhs[i] << endl;
	//}
	IHALFB = NBHALF - 1;
	IERRP = 0;
	IERRU = 0;

	if ((ML - 1) <= 0)
	{
		KMT = 0;
		KPU = 0;
		KSOLVR = KSOLVP;
		//  SOLVER(KMT,KPU,KSOLVR,PMAT,PVEC,PITER,B,NN,IHALFB,NELT,NCBI,IWK, FWK, IA, JA, IERRP, ITRSP, ERRP)
		solveEquation(KPU,KSOLVR,PMATT,p_rhs,p_solution,ITRMXP,IERRP,ITRSP,ERRP);
		// P solution is now in PVEC
		for (int i = 1; i <= NN; i++)
			nodeContainer[i]->setPVEC(p_solution[i - 1]);
		onceP = true;

		if (ISSFLO != 0)
		{
			for (int i = 1; i <= NN; i++)
			{
				nodeContainer[i]->setPM1(p_solution[i-1]);
			}
		}
	}

	if ((ML - 1) != 0)
	{
		KMT = 0;
		KPU = 1;
		KSOLVR = KSOLVU;
		if (IT == 2)
		{
			printUMATToFile("UMAT.txt");
			printUVECToFile("UVEC.txt");
			cout << "Check File" << endl;
		}
		solveEquation(KPU,KSOLVR, UMATT, u_rhs,u_solution, ITRMXU,IERRU, ITRSU, ERRU);
		for (int i = 1; i <= NN; i++){
			nodeContainer[i]->setUVEC(u_solution[i - 1]);
			nodeUVEC[i-1] = u_solution[i - 1];
		}
		// U solution is now in UVEC
	}

	IERR = abs(IERRP) + abs(IERRU);
	CNUBM1 = CNUB;

	for (int i = 1; i <= NN; i++)
	{
		double UEFF = max(nodeUVEC[i], 1e-10);
		nodeContainer[i]->setCNUB(nodeContainer[i]->getCNUBM1() + (0.5*-PRODF1*(nodeContainer[i]->getRHO()*UEFF / SMWH))*DELTU);
		if ((IUNSAT - 2) != 0)
		{
			nodeContainer[i]->setSW(1.0);
			nodeContainer[i]->setRELK(1.0);
			BUBSAT(nodeContainer[i]);
		}
		else
		{
			if (nodePVEC[i] >= 0)
			{
				nodeContainer[i]->setSW(1.0);
				nodeContainer[i]->setRELK(1.0);
				BUBSAT(nodeContainer[i]);
			}
			else
			{
				UNSAT(nodeContainer[i]);
				BUBSAT(nodeContainer[i]);
			}
		}

	}

	//CHECK PROGRESS AND CONVERGENCE OF NON - LINEARITY ITERATIONS   
	//AND SET STOP AND GO FLAGS : SUTRA........51800
	//	ISTOP = -1   NOT CONVERGED - STOP SIMULATION                
	//	ISTOP = 0   ITERATIONS LEFT OR CONVERGED - KEEP SIMULATING 
	//	ISTOP = 1   LAST TIME STEP REACHED - STOP SIMULATION   
	//	IGOI = 0   P AND U CONVERGED, OR NO ITERATIONS DONE
	//	IGOI = 1   ONLY P HAS NOT YET CONVERGED TO CRITERION
	//	IGOI = 2   ONLY U HAS NOT YET CONVERGED TO CRITERION
	//	IGOI = 3   BOTH P AND U HAVE NOT YET CONVERGED TO CRITERIA  

	ISTOP = 0;
	IGOI = 0;
	if (ITRMAX - 1 > 0)
	{
		RPM = 0;
		RUM = 0;
		IPWORS = 0;
		IUWORS = 0;
		if ((ML - 1) <= 0)
		{
			for (int i = 1; i <= NN; i++)
			{
				RP = abs(nodePVEC[i] - nodeContainer[i]->getPITER());
				if ((RP - RPM) > 0)
				{
					RPM = RP;
					IPWORS = i;
				}
			}
			if (RPM > RPMAX)
				IGOI = IGOI + 1;

		}

		if ((ML - 1) != 0)
		{
			for (int i = 1; i <= NN; i++)
			{
				RU = abs(nodeUVEC[i] - nodeContainer[i]->getUITER());
				if ((RU - RUM) > 0)
				{
					RUM = RU;
					IUWORS = i;
				}
			}
			if (RUM > RUMAX)
				IGOI = IGOI + 2;
		}
		char buff[1024];
		_snprintf(buff, sizeof(buff), "       Maximum changes in P, U : %1.8e , %1.8e", RPM, RUM);
		cout << buff << endl;
		// write to K00;
		}

	if (ISTOP != -1 && IT == ITMAX)
		ISTOP = 1;

	PRNALL = ((ISTOP != 0) || IERR != 0);
	PRN0 = ((ITREL == 0) && (ISSFLO != 0) && (ISSTRA != 1));
	PRNDEF = (PRNALL || PRN0);
	PRNK3 = (PRNDEF || (IT % NPRINT == 0) || (ITREL == 1 && NPRINT > 0));

	if (PRNK3)
	{
		if (KTYPE[0] == 3)
		{
			OUTLST3(ML, ISTOP, IGOI, IERRP, ITRSP, ERRP, IERRU, ITRSU, ERRU);
		}
		else
		{
			
		}
	}

	if (KBUDG == 1)
	{
		BUDGET();
	}

	PRNK5 = (PRNDEF || ((IT != 0) && (IT%NCOLPR == 0)) || ((ITREL == 1 && NCOLPR > 0)));

}



void ControlParameters::freeDataSets()
{
	for (unordered_map<string, DataSet*>::iterator it = dataSetMap.begin(); it != dataSetMap.end(); ++it)
	{
		it->second->free();
	}
}

bool ControlParameters::getBCSFL(int index)
{
	return this->BCSFL[index];
}

bool ControlParameters::getBCSTR(int index)
{
	return this->BCSTR[index];
}

void ControlParameters::printPVECToFile(string fname)
{
	string filename = InputFiles::Instance()->getInputDirectory() + "\\" + fname;
	ofstream rhsStream;

	rhsStream.open(filename, std::ios_base::out);
	rhsStream << p_rhs.size() << endl;
	for (int i = 0; i < p_rhs.size(); i++)
	{
		rhsStream << p_rhs[i] << endl;
	}
}
void ControlParameters::printUVECToFile(string fname)
{
	string filename = InputFiles::Instance()->getInputDirectory() + "\\" + fname;
	ofstream rhsStream;

	rhsStream.open(filename, std::ios_base::out);
	rhsStream << u_rhs.size() << endl;
	for (int i = 0; i < u_rhs.size(); i++)
	{
		rhsStream << u_rhs[i] << endl;
	}
}
void ControlParameters::printUMATToFile(string fname)
{
	string filename = InputFiles::Instance()->getInputDirectory() + "\\" + fname;
	ofstream rhsStream;

	rhsStream.open(filename, std::ios_base::out);
	rhsStream << UMATT.size() << endl;
	for (int i = 0; i < UMATT.size(); i++)
	{
		rhsStream << UMATT[i] << endl;
	}
}
//void ControlParameters::GMRES(MatrixXd& MAT, vector<double>& rhs, vector<double>& solution, int& ITRMXS, int& ITRS, double& ERR)
void ControlParameters::GMRES(vector<double>& MAT, vector<double>& rhs, vector<double>& solution, int& ITRMXS, int& ITRS, double& ERR)
{
	//if (stl_A[0].size()){
	//	for (int jk = 0; jk < IAVec.size(); jk++)
	//	{
	//		stl_A[IAVec[jk]][jJA[jk]] = MAT[jk];
	//	}
	//}
	//else
	//{
	//	for (int jk = 0; jk < IAVec.size(); jk++)
	//	{
	//		stl_A[IAVec[jk]].emplace(jJA[jk], MAT[jk]);
	//	}
	//}
	re_orient_matrix(NN + 1, NELT, MAT, JAVec, IAVec, r_MAT, row_jumper, column_indices);
	

	copy(rhs.begin(), rhs.end(), vcl_rhs.begin());

	viennacl::compressed_matrix<double> A;
	
	A.set(&row_jumper[0], &column_indices[0], &r_MAT[0],NN,NN,NELT);
	//viennacl::copy(stl_A, A);
	viennacl::linalg::ilu0_precond<viennacl::compressed_matrix<double>> ilu0(A, viennacl::linalg::ilu0_tag());
	viennacl::linalg::gmres_tag my_gmres_tag(1e-13, ITRMXS, ITRMXS/10);
	viennacl::linalg::gmres_solver<viennacl::vector<double> > my_gmres_solver(my_gmres_tag);
	init_guess[0] = 0;
	monitor_user_data<viennacl::compressed_matrix<double>, viennacl::vector<double> > my_monitor_data(A, vcl_rhs, init_guess);
	my_gmres_solver.set_monitor(my_custom_monitor<viennacl::vector<double>, double, viennacl::compressed_matrix<double> >, &my_monitor_data);
	my_gmres_solver.set_initial_guess(init_guess);
	vcl_results = my_gmres_solver(A, vcl_rhs, ilu0);
	copy(vcl_results, solution);
	ITRS = my_gmres_solver.tag().iters();
	ERR = my_gmres_solver.tag().error();
	
}

void ControlParameters::BiCGSTAB(vector<double>& MAT, vector<double>& rhs, vector<double>& solution, int& ITRS, double& ERR)
{
	
}

void ControlParameters::ORTHOMIN(vector<double>& MAT, vector<double>& rhs, vector<double>& solution, int& ITRS, double& ERR)
{
	
}

void ControlParameters::JACOB(int el, int node, int realNode, double XLOC, double YLOC, double ZLOC, double & PORGT){
	int nodeNum;
	double ODET;
	double XF1, XF2, YF1, YF2, ZF1, ZF2;
	double XIIX[8] = { -1, 1, 1, -1, -1, 1, 1, -1 };
	double YIIY[8] = { -1, -1, 1, 1, -1, -1, 1, 1 };
	double ZIIZ[8] = { -1, -1, -1, -1, 1, 1, 1, 1 };
	XF1 = 1 - XLOC;
	XF2 = 1 + XLOC;
	YF1 = 1 - YLOC;
	YF2 = 1 + YLOC;
	ZF1 = 1 - ZLOC;
	ZF2 = 1 + ZLOC;
	double FX[8] = { XF1, XF2, XF2, XF1, XF1, XF2, XF2, XF1 };
	double FY[8] = { YF1, YF1, YF2, YF2, YF1, YF1, YF2, YF2 };
	double FZ[8] = { ZF1, ZF1, ZF1, ZF1, ZF2, ZF2, ZF2, ZF2 };
	double DWDXL[8] = { 0 };
	double DWDYL[8] = { 0 };
	double DWDZL[8] = { 0 };
	double DFDXL[8] = { 0 };
	double DFDYL[8] = { 0 };
	double DFDZL[8] = { 0 };
	double GXSI, GETA, GZET;
	//double CJ[3][3] = {};
	//double CIJ[3][3] = {};
	vector<vector<double>> CJ(3, vector<double>(3));
	vector<vector<double>> CIJ(3, vector<double>(3));
	double DET;

	for (int i = 0; i < 8; i++){
		F[node][i] = 0.125*FX[i] * FY[i] * FZ[i];
	}
	// CALCULATE Derivatives wrt local coords
	for (int i = 0; i < 8; i++){
		DFDXL[i] = XIIX[i] * 0.125*FY[i] * FZ[i];
		DFDYL[i] = YIIY[i] * 0.125*FX[i] * FZ[i];
		DFDZL[i] = ZIIZ[i] * 0.125*FX[i] * FY[i];
	}

	// CALCULATE ELEMENTS OF JACoBIAN MATRIX;

	for (int i = 0; i < 8; i++){
		nodeNum = elementNodes[el][i];
		CJ[0][0] = CJ[0][0] + DFDXL[i] * nodeContainer[nodeNum]->getXCoord();
		CJ[0][1] = CJ[0][1] + DFDXL[i] * nodeContainer[nodeNum]->getYCoord();
		CJ[0][2] = CJ[0][2] + DFDXL[i] * nodeContainer[nodeNum]->getZCoord();
		CJ[1][0] = CJ[1][0] + DFDYL[i] * nodeContainer[nodeNum]->getXCoord();
		CJ[1][1] = CJ[1][1] + DFDYL[i] * nodeContainer[nodeNum]->getYCoord();
		CJ[1][2] = CJ[1][2] + DFDYL[i] * nodeContainer[nodeNum]->getZCoord();
		CJ[2][0] = CJ[2][0] + DFDZL[i] * nodeContainer[nodeNum]->getXCoord();
		CJ[2][1] = CJ[2][1] + DFDZL[i] * nodeContainer[nodeNum]->getYCoord();
		CJ[2][2] = CJ[2][2] + DFDZL[i] * nodeContainer[nodeNum]->getZCoord();
	}
	// Calculate Determinant of Jacobian Matrix
	DET = CJ[0][0] * (CJ[1][1] * CJ[2][2] - CJ[2][1] * CJ[1][2]) -
		CJ[1][0] * (CJ[0][1] * CJ[2][2] - CJ[2][1] * CJ[0][2]) +
		CJ[2][0] * (CJ[0][1] * CJ[1][2] - CJ[1][1] * CJ[0][2]);
	elementContainer[el]->putIntoDET(node, DET);

	GXSI = CJ[0][0] * GRAVX + CJ[0][1] * GRAVY + CJ[0][2] * GRAVZ;
	GETA = CJ[1][0] * GRAVX + CJ[1][1] * GRAVY + CJ[1][2] * GRAVZ;
	GZET = CJ[2][0] * GRAVX + CJ[2][1] * GRAVY + CJ[2][2] * GRAVZ;
	elementContainer[el]->putIntoGXSI(node, GXSI);
	elementContainer[el]->putIntoGETA(node, GETA);
	elementContainer[el]->putIntoGZET(node, GZET);
	ODET = 1.0 / DET;
	CIJ[0][0] = +ODET*(CJ[1][1] * CJ[2][2] - CJ[2][1] * CJ[1][2]);
	CIJ[0][1] = -ODET*(CJ[0][1] * CJ[2][2] - CJ[2][1] * CJ[0][2]);
	CIJ[0][2] = +ODET*(CJ[0][1] * CJ[1][2] - CJ[1][1] * CJ[0][2]);
	CIJ[1][0] = -ODET*(CJ[1][0] * CJ[2][2] - CJ[2][0] * CJ[1][2]);
	CIJ[1][1] = +ODET*(CJ[0][0] * CJ[2][2] - CJ[2][0] * CJ[0][2]);
	CIJ[1][2] = -ODET*(CJ[0][0] * CJ[1][2] - CJ[1][0] * CJ[0][2]);
	CIJ[2][0] = +ODET*(CJ[1][0] * CJ[2][1] - CJ[2][0] * CJ[1][1]);
	CIJ[2][1] = -ODET*(CJ[0][0] * CJ[2][1] - CJ[2][0] * CJ[0][1]);
	CIJ[2][2] = +ODET*(CJ[0][0] * CJ[1][1] - CJ[1][0] * CJ[0][1]);
}

void ControlParameters::BUDGET()
{
	int MN = 2;
	int nodeNum;
	double STPPOS, STPNEG, STUPOS, STUNEG, QINPOS, QINNEG;
	STPPOS = STPNEG = STUPOS = STUNEG = QINPOS = QINNEG = 0.0;
	double TERM;
	double STPTOT, STUTOT, STFPOS, STFNEG, STFTOT, QINTOT;
	double QPLPOS, QPLNEG,QPLTOT,QFFPOS,QFFNEG,QFFTOT;
	double ACTFMB, ERFMBA,ERFMBR;
	if (IUNSAT != 0)
		IUNSAT = 1;

	if (ME == -1)
		MN = 1;

	if (IUNSAT - 1 == 0)
	{
		for (int i = 1; i <= NN; i++)
		{
			if (nodeContainer[i]->getPVEC()<0)
			{
				UNSAT(nodeContainer[i]);
			} else
			{
				nodeContainer[i]->setSW(1.0);
				nodeContainer[i]->setDSWDP(0.0);
			}
		}
		for (int i = 1; i <= NN; i++)
			BUBSAT(nodeContainer[i]);
	}
		if (ML - 1 <= 0)
		{
			for (int i = 1; i <= NN; i++)
			{
				TERM = (1 - ISSFLO / 2)*nodeContainer[i]->getRHO()*nodeContainer[i]->getVOL()*
					(nodeContainer[i]->getSWT()*nodeContainer[i]->getSOP() + nodeContainer[i]->getPorosity()*nodeContainer[i]->getDSWDP())*
					(nodeContainer[i]->getPVEC() - nodeContainer[i]->getPM1()) / DELTP;
				STPPOS = STPPOS + max(0.0, TERM);
				STPNEG = STPNEG + min(0.0, TERM);
				TERM = (1 - ISSFLO / 2)*nodeContainer[i]->getPorosity()*nodeContainer[i]->getSWT()*DRWDU*nodeContainer[i]->getVOL()*
					(nodeContainer[i]->getUM1() - nodeContainer[i]->getUM2()) / DLTUM1;
				STUPOS = STUPOS + max(0.0, TERM);
				STUNEG = STUNEG + min(0.0, TERM);
				TERM = nodeContainer[i]->getQIN();
				QINPOS = QINPOS + max(0.0, TERM);
				QINNEG = QINNEG + min(0.0, TERM);
			}
			STPTOT = STPPOS + STPNEG;
			STUTOT = STUPOS + STUNEG;
			STFPOS = STPPOS + STUPOS;
			STFNEG = STPNEG + STUNEG;
			STFTOT = STPTOT + STUTOT;
			QINTOT = QINPOS + QINNEG;

			QPLPOS = 0.0;
			QPLNEG = 0.0;

			for (int i = 0; i < NPBC; i++)
			{
				nodeNum = abs(IPBC[i]);
				TERM = nodeContainer[nodeNum]->getGNUP1()*(nodeContainer[nodeNum]->getPBC() - nodeContainer[nodeNum]->getPVEC());
				QPLPOS = QPLPOS + max(0.0, TERM);
				QPLNEG = QPLNEG + min(0.0, TERM);
			}
			QPLTOT = QPLPOS + QPLNEG;
			QFFPOS = QINPOS + QPLPOS;
			QFFNEG = QINNEG + QPLNEG;
			QFFTOT = QINTOT + QPLTOT;

			ACTFMB = 0.5*(STFPOS - STFNEG + QFFPOS - QFFNEG);
			ERFMBA = STFTOT - QFFTOT;
			// Write To File;

			if (ACTFMB != 0)
			{
				ERFMBR = 100*ERFMBA / ACTFMB;
			}

			if (IBCT != 4)
			{
				
			}

			if (NPBC != 0)
			{
				
			}

			
		} 

		if (ML - 1 != 0)
		{
			
		}
	

}

void ControlParameters::ZeroVectorsAndMatrix()
{
	//PMAT = MatrixXd::Zero(NELT, 1);
	PMATT = vector<double>(NELT, 0);
	//UMAT = MatrixXd::Zero(NELT, 1);
	UMATT = vector<double>(NELT, 0);
	for (int i = 0; i < NN; i++)
	{
		nodeVOL[i] = 0;
		node_p_rhs[i] = 0;
		node_u_rhs[i] = 0;

	}
}
