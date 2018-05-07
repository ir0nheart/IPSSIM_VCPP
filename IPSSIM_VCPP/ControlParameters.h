#ifndef _CONTROL_PARAMETERS
#define _CONTROL_PARAMETERS
#include <vector>
#include <list>
#include <string>
#include <Windows.h>
#include <algorithm>
#include <iterator>
#include <functional>
#include <time.h>
#include <chrono>
#include <thread>
#include "InputFiles.h"
#include "Layer.h"
#include "StepCycleSch.h"
#include "StepListSch.h"
#include "TimeCycleSch.h"
#include "TimeListSch.h"
#include "Miscellaneous.h"
#include "ObservationPoints.h"
#include "Node.h"
#include "Element.h"
#include <math.h>
#include <unordered_map>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <deque>
#include <process.h>
#include "Timer.h"
#include "DataSet.h"
#include "Writer.h"
#include <map>
using namespace std;
#pragma once
class InputFiles;
class DataSet;
class ControlParameters
{
public:
	// try for parsing
	
	double * nodeX;
	double * nodeY;
	double * nodeZ;
	double * nodePorosity;
	int * nodeRegion;
	double * nodeSOP;
	double * nodeUBC;
	double * nodePBC;
	double * nodeQIN;
	double * nodeUIN;
	double * nodeQUIN;
	double * nodePVEC;
	double * nodeUVEC;


	void createNodes();
	void createElements();

	int * elementRegion;
	int ** elementNodes;
	double * elALMAX;
	double * elALMID;
	double * elALMIN;
	double * elATMAX;
	double * elATMID;
	double * elATMIN;
	double * elPMAX;
	double * elPMID;
	double * elPMIN;
	double * elANGLE1;
	double * elANGLE2;
	double * elANGLE3;
	double * elANGLEX;


	void BANWID();
	bool FINDL3ver(int ElNo, int* elNodes, ObservationPoints * obs);//
	static ControlParameters* Instance();
	void createDataSets();
	const static string SOLNAM[];
	const static string SOLWRD[];
	const static string types[];
	const static string Headers[];
	map<std::string, std::string>nodeHeaders;
	void setKSOLVP(int val);
	void setKSOLVU(int val);
	int getKSOLVP();
	int getKSOLVU();
	void setElementParameters(int b, int e, int tid);
	vector<ObservationPoints *> temp;
	unordered_map<string, ObservationPoints *> observationPointMap;
	
	void findObservationElement(int b, int e,int tid);
	void setElementParametersThread();
	void SOURCE1(string BCSID);
	void BOUND1(string BCSID);
	void loadInitialConditions();

	void findObservationElementThread();
	void Source(string key); // READ SOURCE
	//void addDataSet(string dataSet, string dataSetName);
	vector<DataSet *> getDataSets();
	void parseDataSet_15B();
	void parseDataSet_8D();
	void setParameters();

	const string VERN = "2.2";
	void exitOnError(string ERRCOD, vector<string>CHERR = vector<string>());
	void writeToLSTString(string str);
	void writeToNODString(string str);
	void writeToELEString(string str);
	void writeToOBSString(string str);
	void popWriteContainer();
	void setTITLE1(string str);
	void setTITLE2(string str);
	void readPropsINP();
	void addLayer(Layer* lay);
	void setPSTAR(double PSTAR);
	void setGCONST(double GCONST);
	void setTEMP(double TEMP);
	void setSMWH(double SMWH);
	void setTimeStepDivide(int tsd);
	void setWaterTable(double val);
	void setTSTART(double val);
	void setITMAX(int val);
	void setIT(int val);
	int getIT();
	int getITMAX();
	double getTSTART();
	double getTFINISH();
	void setTFINISH(double val);

	double getPSTAR();
	double getGCONST();
	double getTEMP();
	double getSMWH();
	void Connec();
	void Bound(string key);
	int getTimeStepDivide();
	int getNumberOfLayers();
	double getWaterTable();
	void setNumberOfLayers(int nnLayers);
	void addToListOfSchedules(Schedule * sch);
	vector<Schedule *> getListOfSchedules();
	void addToListOfOBS(ObservationPoints * obs);
	void addToNodeContainer(Node * aNode);
	void addToElementContainer(Element * anElement);
	vector<Node *> getNodeContainer();
	vector<Element *> getElementContainer();
	unordered_map<string, DataSet *> getDataSetMap();
	void parseDataSet_14B();
	void setsSutra(string str);
	void setvVersion(string str);
	void setvVersionNum(string str);
	void setsimulationType(string str);
	void setsTransport(string str);
	void setME(int ME);
	void setKTYPE(int i, int val);
	void setMSHSTR(string str);
	void setNN1(int val);
	void setNN2(int val);
	void setNN3(int val);
	void setNN(int val);
	int getNN1();
	int getNN2();
	int getNN3();
	int getNN();
	bool FINDL3(int ElNo, vector<double>X, vector<double>Y, vector<double>Z,ObservationPoints * obs);
	bool FINDL2(int ElNo, vector<double>X, vector<double>Y, ObservationPoints * obs);
	void setCUNSAT(string str);
	void setCSSFLO(string str);
	void setCSSTRA(string str);
	void setCREAD(string str);
	void setIUNSAT(int val);
	void setISSFLO(int val);
	void setISSTRA(int val);
	void setIREAD(int val);
	void setISTORE(int val);
	void setNELT(int val);
	int getNELT();
	string getCUNSAT();
	string getCSSFLO();
	string getCSSTRA();
	string getCREAD();
	int getIUNSAT();
	int getISSFLO();
	int getISSTRA();
	int getIREAD();
	int getISTORE();
	void setUP(double val);
	void setGNUP(double val);
	void setGNUU(double val);
	double getUP();
	double getGNUP();
	double getGNUU();
	void setNSCH(int val);
	void setNSCHAU(int val);
	void setNPCYC(int val);
	void setNUCYC(int val);
	int getNSCH();
	int getNSCHAU();
	int getNPCYC();
	int getNUCYC();
	void setNE(int val);
	void setNPBC(int val);
	void setNUBC(int val);
	void setNSOP(int val);
	void setNSOPI(int val);
	void setNSOU(int val);
	void setNSOUI(int val);
	void setNOBS(int val);
	void setITRMAX(int val);
	void setRPMAX(double val);
	void setRUMAX(double val);
	int getITRMAX();
	double getRPMAX();
	double getRUMAX();
	void setCSOLVP(string str);
	void setCSOLVU(string str);
	void setITRMXP(int val);
	void setTOLP(double val);
	void setITRMXU(int val);
	void setTOLU(double val);
	string getCSOLVP();
	string getCSOLVU();
	int getITRMXP();
	int getITRMXU();
	double getTOLP();
	double getTOLU();
	void setITOLU(int val);
	void setITOLP(int val);
	int getITOLU();
	int getITOLP();
	int getNE();
	int getNPBC();
	int getNUBC();
	int getNSOP();
	int getNSOPI();
	int getNSOU();
	int getNSOUI();
	int getNOBS();
	string getMSHSTR();
	int getKTYPE(int i);
	string getsSutra();
	string getvVersion();
	string getvVersionNum();
	string getsimulationType();
	string getsTransport();
	int getME();
	void setNSAVEP(int val);
	void setNSAVEU(int val);
	int getNSAVEP();
	int getNSAVEU();
	void setNNNX(int val);
	void setNDIMJA(int val);
	void setNNVEC(int val);
	int getNNVEC();
	int getNDIMJA();
	int getNNNX();
	void setNOBSN(int val);
	void setNBCN(int val);
	int getNOBSN();
	int getNBCN();
	void setN48(int val);
	void setNIN(int val);
	void setNEX(int val);
	int getN48();
	int getNIN();
	int getNEX();

	void setNPRINT(int val);
	void setCNODAL(string str);
	void setCELMNT(string str);
	void setCINCID(string str);
	void setCPANDS(string str);
	void setCVEL(string str);
	void setCCORT(string str);
	void setCBUDG(string str);
	void setCSCRN(string str);
	void setCPAUSE(string str);
	int getNPRINT();
	string getCNODAL();
	string getCELMNT();
	string getCINCID();
	string getCPANDS();
	string getCVEL();
	string getCCORT();
	string getCBUDG();
	string getCSCRN();
	string getCPAUSE();
	void setKNODAL(int val);
	void setKELMNT(int val);
	void setKINCID(int val);
	void setKPANDS(int val);
	void setKVEL(int val);
	void setKCORT(int val);
	void setKBUDG(int val);
	void setKSCRN(int val);
	void setKPAUSE(int val);
	int getKNODAL();
	int getKELMNT();
	int getKINCID();
	int getKPANDS();
	int getKVEL();
	int getKCORT();
	int getKBUDG();
	int getKSCRN();
	int getKPAUSE();
	void setNCOLPR(int val);
	void setLCOLPR(int val);
	int getNCOLPR();
	int getLCOLPR();
	void setNCOLMX(int val);
	void setLCOLMX(int val);
	int getNCOLMX();
	int getLCOLMX();
	void addToNCOL(string str);
	vector<string> getNCOL();
	void addToLCOL(string str);
	vector<string> getLCOL();
	vector<ObservationPoints *> getObservationPointsContainer();
	string getVERN();
	string getADSMOD();
	void setADSMOD(string val);
	void setNBCFPR(int val);
	void setNBCSPR(int val);
	void setNBCPPR(int val);
	void setNBCUPR(int val);
	void setCINACT(string str);
	int getNBCFPR();
	int getNBCSPR();
	int getNBCPPR();
	int getNBCUPR();
	string getCINACT();
	void setKINACT(int val);
	int getKINACT();
	void BCTIME();
	void BCSTEP();
	void setCOMPFL(double val);
	void setCW(double val);
	void setSIGMAW(double val);
	void setRHOW0(double val);
	void setURHOW0(double val);
	void setDRWDU(double val);
	void setVISC0(double val);
	double getCOMPFL();
	double getCW();
	double getSIGMAW();
	double getRHOW0();
	double getURHOW0();
	double getDRWDU();
	double getVISC0();

	void  setCOMPMA(double val);
	void  setCS(double val);
	void  setSIGMAS(double val);
	void  setRHOS(double val);
	double getCOMPMA();
	double getCS();
	double getSIGMAS();
	double getRHOS();
	
	void setPRODF0(double val);
	void setPRODS0(double val);
	void setPRODF1(double val);
	void setPRODS1(double val);
	double getPRODF0();
	double getPRODS0();
	double getPRODF1();
	double getPRODS1();

	void setGRAVX(double val);
	double getGRAVX();
	void setGRAVY(double val);
	double getGRAVY();
	void setGRAVZ(double val);
	double getGRAVZ();

	void setSCALX(double val);
	void setSCALY(double val);
	void setSCALZ(double val);
	void setPORFAC(double val);
	double getSCALX();
	double getSCALY();
	double getSCALZ();
	double getPORFAC();
	

	void setPMAXFA(double val);
	void setPMIDFA(double val);
	void setPMINFA(double val);
	void setANG1FA(double val);
	void setANG2FA(double val);
	void setANG3FA(double val);
	void setALMAXF(double val);
	void setALMIDF(double val);
	void setALMINF(double val);
	void setATMXF(double val);
	void setATMDF(double val);
	void setATMNF(double val);
	void setANGFAC(double val);
	double getPMAXFA();
	double getPMIDFA();
	double getPMINFA();
	double getANG1FA();
	double getANG2FA();
	double getANG3FA();
	double getALMAXF();
	double getALMIDF();
	double getALMINF();
	double getATMXF();
	double getATMDF();
	double getATMNF();
	double getANGFAC();
	deque<string> getWriteContainer();
	void addToWriteContainer(string s);
	TimeCycleSch * getTIMESTEPSSchedule();
	void setNRTEST(int val);
	int getNRTEST();
	void setLRTEST(int val);
	int getLRTEST();
	void setIQSOPT(int val);
	void setIQSOUT(int val);
	void setIPBCT(int val);
	void setIUBCT(int val);
	int getIQSOPT();
	int getIQSOUT();
	int getIPBCT();
	int getIUBCT();
	void setIBCT();
	int getIBCT();
	void setTMAX(double val);
	double getTMAX(); 
	void setTEMAX();
	double getTEMAX();
	void setTSEC(double val);
	void setTSECP0(double val);
	void setTSECU0(double val);
	void setTSECM1(double val);
	void setTMIN(double val);
	void setTHOUR(double val);
	void setTDAY(double val);
	void setTWEEK(double val);
	void setTMONTH(double val);
	void setTYEAR(double val);
	void setDELT(double val);
	void setDELTM1(double val);
	double getTSEC();
	double getTSECP0();
	double getTSECU0();
	double getTSECM1();
	double getTMIN();
	double getTHOUR();
	double getTDAY();
	double getTWEEK();
	double getTMONTH();
	double getTYEAR();
	double getDELT();
	double getDELTM1();
	void OUTLST3(int ML, int ISTOP, int IGOI, int IERRP, int ITRSP, double ERRP, int IERRU, int ITRSU, double ERRU);
	void OUTLST2(int ML, int ISTOP, int IGOI, int IERRP, int ITRSP, double ERRP, int IERRU, int ITRSU, double ERRU);
	void OUTNOD();
	void OUTOBS();
	bool getOnceNOD();
	void setOnceNOD(bool val);
	bool getOnceOBS();
	void setOnceOBS(bool val);
	void setTIME(double val);
	double getTIME();

	void setML(int val);
	int getML();
	double getDLTPM1();
	double getDLTUM1();
	double getBDELP1();
	double getBDELP();
	double getBDELU();
	double getBDELU1();
	void setDLTPM1(double val);
	void setDLTUM1(double val);
	void setBDELP1(double val);
	void setBDELP(double val);
	void setBDELU(double val);
	void setBDELU1(double val);
	void setNOUMAT(int val);
	int getNOUMAT();
	void setITER(int val);
	int getITER();
	void setDELTU(double val);
	void setDELTP(double val);
	double getDELTU();
	double getDELTP();

	void PU(ObservationPoints *p );
	void PUP(ObservationPoints *p);
	string PUSWF(ObservationPoints * p,double SFRAC);
	void setITREL(double val);
	void setITBCS(double val);
	int getITRST();
	int getITREL();
	int getITBCS();
	void setOnceP(bool val);
	bool getOnceP();
	void setRELCHG(double val);
	void setDELTLC(double val);
	double getRELCHG();
	double getDELTLC();
	double getTELAPS();
	void setTELAPS(double val);
	void setOnceBCS(bool val);
	bool getOnceBCS();
	void loadBCS();
	void ADSORB();
	void ELEMN3();
	void ELEMN2();
	void NODAL();
	void BC();
	
private:
	double DELTLC, RELCHG,TELAPS;
	int NOUMAT;
	int ITOUT;
	double DURN, TOUT;
	bool onceNOD = false;
	bool onceOBS = false;
	bool onceP = false;
	bool onceBCS = false;
	int  ITBCS;
	int IQSOPT, IQSOUT, IPBCT, IUBCT,IBCT;
	int NBCFPR, NBCSPR, NBCPPR, NBCUPR;
	int KINACT;
	double TMAX,TEMAX,TSEC,TSECP0,TSECU0,TSECM1,TMIN,THOUR,TMONTH,TDAY,TWEEK,TYEAR,DELT,DELTM1,DELTU,DELTP;
	double SCALX, SCALY, SCALZ, PORFAC;
	double PMAXFA, PMIDFA, PMINFA, ANG1FA, ANG2FA, ANG3FA, ALMAXF, ALMIDF, ALMINF, ATMXF, ATMDF, ATMNF,ANGFAC;
	int NBCN, NOBSN, N48, NEX, NIN, NNNX, NDIMJA, NNVEC;
	
	ControlParameters();
	ControlParameters(ControlParameters const&){};
	ControlParameters& operator = (ControlParameters const&){};
	
	virtual ~ControlParameters();
	vector <ObservationPoints *> observationPointsContainer;
	static ControlParameters* m_cInstance;
	vector<Node *> nodeContainer; 
	Node **nodeArray;
	vector<Element *> elementContainer;
	vector<Schedule *> listOfSchedules;
	int numberOfLayers;
	double TEMP;
	double PSTAR;
	double GCONST;
	double SMWH;
	int timeStepDivide;
	double waterTable;

	vector<Layer *> layers;
	unordered_map <string, DataSet *> dataSetMap;
	vector<DataSet *> dataSetContainer;
	// Data Set 1 - > 5 Data words
		// 1- SUTRA	
		// 2- VERSION
		// 3- VERSION NUMBER
		// 4- TYPE SOLUTE/ENERGY
		// 5- TRANSPORT
	string sSutra = "";
	string vVersion = "";
	string vVersionNum = "";
	string simulationType = "";
	string sTransport = "";
	string TITLE1, TITLE2;
	int ME; // set ME = -1 for solute transport set ME = +1 for energy Transport
	string BCSSCH;
	int KSOLVP;
	int KSOLVU;
	int ITOLU;
	int ITOLP;
	int NSAVEP;
	int NSAVEU;

	/* Mesh and Coordinate Data*/
	float GRAVX; // x-component of gravity vector
	float GRAVY; // y-component of gravity vector
	float GRAVZ; // z-component of gravity vector
	// x,y,z coordinates of all noted in the mesh is in node object
	string MSHSTR = ""; // Mesh Structure 
	// "2D IRREGULAR"
	// "2D REGULAR"
	// "2D BLOCKWISE"
	// "3D IRREGULAR"
	// "3D LAYERED"
	// "3D REGULAR"
	// "3D IRREGULAR"
	int NN; // total number of nodes in mesh
	int NN1; // number of nodes in the first numbering direction (REGULAR or BLOCKWISE mesh only)
	int NN2; // number of nodes in the second numbering direction (REGULAR or BLOCKWISE mesh only)
	int NN3; // number of nodes in the third numbering direction (REGULAR or BLOCKWISE mesh only)

	// Nodal incidence list stored in Node object

	int NE; // number of elements in mesh
	int NBLK1; //number of blocks in the first numbering direction (BLOCKWISE MESH only)
	int NBLK2; //number of blocks in the second numbering direction (BLOCKWISE MESH only)
	int NBLK3; //number of blocks in the third numbering direction (BLOCKWISE MESH only)

	// Check this List part !!!!!!!
	//LDIV1;
	//LDIV2;
	//LDIV3;

	int NLAYS; // number of layers of nodes (LAYERED mesh only)
	int NNLAY; // number of nodes in a layer (LAYERED mesh only)
	int NELAY; // number of elements in a layer (LAYERED mesh only)

	string LAYSTR = ""; // first numbering direction (LAYERED mesh only) - ACROSS or WITHIN

	/* FLOW Parameters */

	double COMPFL = 0; //FLUID compressibility
	double COMPMA = 0; // Solid matrix compressibility
	double VISC0 = 0; // for energy transport : scale factor for fluid viscosity
	// for solute transport : fluid viscosity
	// POR -- volumetric porosity of solid matrix at each node in node object
	// PMAX, PMID, PMIN in each element in element object
	// ANGLE1,2,3 in element object
	double RHOW0 = 0; // fluid base density
	double DRWDU = 0; // for energy transport : coefficient of fluid density change with temperature
	// for solute transport : coefficient of fluid density change with concentration
	double URHOW0 = 0; // for energy transport : base temperature for density calculation
	// for solute transport : base concentration for density calculation


	/* TRANSPORT Parameters */
	//ALMAX, ALMID,ALMIN,ATMAX,ATMID,ATMIN are in element object
	double SIGMAW = 0; // for energy transport : fluid thermal conductivity
	// for solute transport : molecular diffusivity of solute in fluid
	double SIGMAS = 0; // for energy transport : solid grain thermal conductivity (equals zero for solute transport)
	double CW = 0;     // for energy transport : fluuid specific heat capacity ( equals one for solute transport)
	double CS = 0;     // for energy transport : solid grain specific heat capacity ( not specified in input data for solute transport)
	double RHOS = 0;   // density of a solid grain in the solid matrix;

	/* Reaction and production parameters */
	double CHI1 = 0;
	double CHI2 = 0;

	double PRODF1 = 0; // for solute transport : rate of first-order production of adsorbate mass in the fluid mass ( equals zero for energy transport)
	double PRODS1 = 0; // for solute transport : rate of first-order production of solute mass in the immobile phase ( equals zero for energy transport)
	double PRODF0 = 0; // for energy transport - zero-order rate of energy production in fluid
	// for solute transport - zero-order rate of solute mass production in fluid
	string ADSMOD;

	double PRODS0 = 0; // for energy transport : zero-order rate of energy production in the immobile phase 
	// for solute transport : zero-order rate of adsorbate mass production in the immobile phase 

	/* BOUNDARY CONDITIONS AND FLOW DATA */

	//---- FLOW Data -- SPECIFIED PRESSURES ---- //
	int NPBC; // number of nodes at which pressure is a specified constant or function of time
	vector<int> IPBC; // node number at which pressure is specified (for all NPBC nodes)
	// PBC and UBC are in node object

	//---- FLOW Data -- SPECIFIED FLOWS and FLUID SOURCES ---- //
	int NSOP,NSOPI; // number of nodes at which a source of fluid mass is specified
	vector<int> IQSOP; // node number at which a fluid source is specified (for all NPBC nodes)
	// QUINC, UINC are in node object

	//---- ENERGY OR SOLUTE Data -- SPECIFIED TEMPERATURES or CONCENTRATIONS ---- //
	int NUBC; // number of nodes at which temperature or concentration is a specified constant or function of time
	vector <int> IUBC; // node number at which temperature or concentration is specified (for all NUBC nodes)
	// UBC is in node object

	//---- ENERGY OR SOLUTE Data -- DIFFUSIVE FLUXES OF ENERGY or SOLUTE MASS AT BOUNDARIES ---- //
	int NSOU,NSOUI; // number of nodes at which a diffusive energy or solute mass flux(source) is specified
	vector<int> IQSOU; // node number at which a flux(source) is specified
	// QUINC is in node object

	//----INITIAL CONDITIONS ---- //
	double TICS = 0; // time to which the initial conditions correspond (not necessarily not equal to the starting time of the simulation t0)
	string CPUNI = "";// "UNIFORM" or "NONUNIFORM" (uniform initial P, nonuniform initial P)
	string CUUNI = "";// "UNIFORM" or "NONUNIFORM" (uniform initial U, nonuniform initial U)

	list<double> PVEC; // initial pressure at all nodes in mesh ( for uniform, a single value; for nonuniform, a list of values)
	list<double> UVEC; // for energy transport: initial temperature at all nodes in mesh ( for uniform, a single value; for nonuniform, a list of values)
	// for energy transport: initial concentration at all nodes in mesh ( for uniform, a single value; for nonuniform, a list of values)

	//---- Numerical and TEMPORAL CONTROL DATA ---- //
	double GNUP = 0; // specified pressure boundary condition "conductance" factor
	double GNUU = 0; // specified concentration boundary condition "conductance" factor
	double UP = 0; // fractional upstream weight for asymmetric weighting functions
	int	NSCH,NSCHAU; // number of schedules
	string SCHNAM; // schedule name
	string SCHTYP; // "TIME LIST"  "TIME CYCLE" "STEP LIST" "STEP CYCLE"
	string CREFT; // "ABSOLUTE" "ELAPSED"
	double SCALT = 0; // scaling factor for times
	int NTLIST; // number of times listed
	list<double> TLIST; // list of times
	int NTMAX; // maximum number of time cycles allowed
	double TIMEI = 0; // initial time for a time cycle
	double TIMEL = 0; // limiting time for a time cycle
	double TIMEC = 0; // initial time increment for a time cycle
	double TIME = 0;
	int NTCYC; // number of cycle after which the time increment is updated
	double TCMULT; // multiplier for time increment
	double TCMIN; // minimum time increment allowed
	double TCMAX; // maximum time increment allowed
	int NSLIST; // number of time steps listed
	list<int> ISLIST; // list of time steps
	int NSMAX; // maximum number of time step cycles
	double ISTEPI = 0; // initial step for a time step cycle
	double ISTEPL = 0; // limiting time step for a time step cycle
	double ISTEPC = 0; // time step increment for a time step cycle
	int NPCYC; // time steps in pressure solution cycle
	int NUCYC; // time steps in temperature or concentration solution cycle
	int ITRMAX; // maximum number of iterations for nonlinearities per time step
	double RPMAX = 0; // pressure convergence criterion for iterations
	double RUMAX = 0; // temperature or solution convergence criterion for resolving nonlinearities

	//---- Matrix Equation Solver Data ---- //
	string CSOLVP = ""; // "DIRECT" "CG" "GMRES" "ORTHOMIN"
	string CSOLVU = ""; // "DIRECT" "GMRES" "ORTHOMIN"
	int ITRMXP; // maximum number of solver iterations during P solution
	int ITRMXU;// maximum number of solver iterations during U solution
	double TOLP = 0; // convergence tolerance for solver iterations during P solution
	double TOLU = 0; // convergence tolerance for solver iterations during U solution

	//---- Data for Options ---- //
	string CREAD = ""; // "COLD" or "WARM"
	int ISTORE = 0;
	string SIMULA = ""; // "SUTRA ENERGY" "SUTRA SOLUTE" 
	string CUNSAT = "";// "UNSATURATED" "SATURATED"

	string CSSFLO = ""; // "STEADY" "TRANSIENT"
	string CSSTRA = ""; // "STEADY" "TRANSIENT"

	string CVEL = ""; // "Y" = output fluid velocity at element centroids 
	// "N" = no velocity outpuy
	int IUNSAT;
	int ISSFLO;
	int ISSTRA;
	int IREAD;

	int NOBS; // number of observation points
	int NOBLIN; // maximum number of observations output to a single line in a .obs file
	string OBSNAM; // observation point name
	double XOBS = 0;
	double YOBS = 0;
	double ZOBS = 0;
	string CDUM80 = "";
	string OBSSCH = ""; // name of schedule that controls output for an observation point
	string OBSFMT = ""; // "OBS" or "OBC"
	string CBUDG = ""; // "Y" or "N" - print budgets in .lst file
	string CNODAL = ""; // "Y" or "N" - print nodewise input data in .lst file
	string CELMNT = ""; // "Y" or "N" - print elementwise input data in .lst file
	string CINCID = "";  // "Y" or "N" - print incidence list in .lst file
	string CPANDS = "";  // "Y" or "N" - print pressure and saturation data in .lst file
	string CCORT = ""; // "Y" or "N" - print concentration or temperature data in .lst file
	string CINACT = "";  // "Y" or "N" - print inactive sources and boundary conduitoons input data in .bcof .bcos .bcop .bcou file
	string CPAUSE = "";
	string CSCRN = "";
	int NPRINT; // results are output to .lst file every NPRONT time steps
	int NCOLPR; // results are output to .nod  file every NCOLPR timesteps
	int LCOLPR; // results are output to .ele  file every NCOLPR timesteps
	vector <string> NCOL; // list of variables to output in columns in the .nod file
	vector <string> LCOL; // list of variables to output in columns in the .ele file
	int KNODAL, KELMNT, KINCID, KCORT, KPAUSE, KSCRN, KVEL, KPANDS,KBUDG;
	int KTYPE[2]; // DEPENDING ON MESH TYPE

	int NCOLMX,LCOLMX;
	int NELT;

	// BANDWITH 
	int NB;
	int NBI;
	int NBHALF;
	int LEM;
	deque<string>writeContainer;
	bool isNewRun;
	bool isNewNod;
	bool isNewObs;
	double TSTART;
	double TFINISH;
	list<pair<double,double>>timeSteps;
	list<pair<double, double>>step_0;
	list<pair<double, double>>step_1;
	list<pair<double, double>>step_1andUP;
	int ITMAX;
	int NRTEST,LRTEST;
	int IT, ITRST, ITREL,ITER,IPWORS,IUWORS;
	double RPM, RUM;
	int KTPRN;
	bool * BCSFL;
	bool * BCSTR;
	int ML;
	double DLTPM1, DLTUM1, BDELP1, BDELP, BDELU, BDELU1;
	Schedule * BCSSchedule = nullptr;
	bool setBCS = false;
	int NCID;

	int NBCN1, NSOPI1, NSOUI1;
	bool USEFL, ANYFL, ANYTR, SETFL, SETTR;
	vector<string>CIDBCS;

};
#endif
