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
#include <unordered_set>
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
#include "BCS.h"
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
#include <iostream>
#include <Eigen\Dense>
#include <viennacl/vector.hpp>
using namespace std;
using namespace Eigen;
#pragma once
class InputFiles;
class DataSet;
class ControlParameters
{
public:

	void GMRES(MatrixXd& MAT, vector<double>& rhs, vector<double>& solution,int& ITRS,double& ERR);
	void BiCGSTAB(MatrixXd& MAT, vector<double>& rhs, vector<double>& solution, int& ITRS, double& ERR);
	void ORTHOMIN(MatrixXd& MAT, vector<double>& rhs, vector<double>& solution, int& ITRS, double& ERR);
	void BUDGET();
	// try for parsing
	void printToFile(string fname);
	void set_bcs(bool val){ setBCS = val; }
	bool get_bcs() const{ return setBCS; }
	double * nodeX;
	double * nodeY;
	double * nodeZ;
	double * nodePorosity;
	int * nodeRegion;
	double * nodeSOP;
	//double * nodeUBC;
	//double * nodePBC;
	vector<double> nodePBC;
	vector<double> nodeUBC;
	double * nodeQIN;
	double * nodeUIN;
	double * nodeQUIN;
	//double * nodePVEC;
	//double * nodeUVEC;
	/*double * nodeVOL;
	double * node_p_rhs;*/
	int IMID, JMID;
	double GUL, GUR, GPINL, GPINR, GUINR, GUINL;
	double SWRHON;


	void PTRSET();
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
	void SOURCE1(double& QIN1, double& UIN1, double IQSOP1, double& QUIN1, double IQSOU1, double& IQSOPT1, double& IQSOUT1, int NSOP1, int NSOU1, string BCSID);
	void BOUND1(int NPBC1, int NUBC1, int NBCN1, string BCSID);
	void loadInitialConditions();
	void solveTimeStep();
	void findObservationElementThread();
	void freeDataSets();
	void Source(string key); // READ SOURCE
	//void addDataSet(string dataSet, string dataSetName);
	vector<DataSet *> getDataSets();
	void parseDataSet_15B();
	void parseDataSet_8D();
	void setParameters();
	double DNRM2(int N,vector<double>& X, int INCX);
	const string VERN = "2.2";
	void exitOnError(string ERRCOD, vector<string>CHERR = vector<string>());
	void writeToLSTString(string str);
	void writeToNODString(string str);
	void writeToELEString(string str);
	void writeToOBSString(string str);
	void writeToIAString(string str);
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
	void setIGOI(int val);
	void setIERR(double val);
	void setISTOP(int val);

	double getPMAXFA();;
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
	void solveEquation(int KPU, int KSOLVR, MatrixXd& MAT, vector<double>&rhs, vector<double>& solution,double &IERR, int &ITRS, double & ERR);
	void SOLVEB(int KMT, MatrixXd& MAT, vector<double>&rhs, vector<double>& solution);
	void SOLWRP(int KPU, int KSOLVR,MatrixXd& MAT, vector<double>&rhs, vector<double>& solution, int& ITRS,double &ERR);

	void setML(int val);
	int getML();
	int getIGOI();
	int getISTOP();
	double getIERR();
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
	void setITERPARAMS();
	void ADSORB();
	void ELEMN3();
	void ELEMN2();
	void NODAL();
	void BC();
	void BASIS3(int ICALL,int el, int node,int realNode, double XLOC, double YLOC, double ZLOC,double &PORGT);
	void JACOB (int el, int node, int realNode, double XLOC, double YLOC, double ZLOC, double &PORGT);
	void UNSAT(double& SW, double& DSWDP, double& RELK, double PRES, double KREG);
	void UNSAT(Node * node);
	void BUBSAT(Node * node);
	void BUBSAT(double& SWBG, double&  RELKBG, double  PITERG, double  CNUBG, double&  RELKTG, double& SWTG, double SWG, double  RELKG);
	void GLOCOL(int el, int ML,double VOLE[8], double BFLOWE[8][8],double DFLOWE[8], double BTRANE[8][8],double DTRANE[8][8]);
	void GLOBAN();
	void createSolverMatrix();
	void DIMWRK(int KSOLVR,int NSAVE,int& NWI, int& NWF);
	void alpayPTR();
	MatrixXd PMAT;
	MatrixXd UMAT;
	VectorXd FWK;
	VectorXd IWK;
	VectorXd PPVEC;
	VectorXd UUVEC;
	VectorXd IA;
	VectorXd JA;
	
	void setITERPARAMS1();
	void setITERPARAMS2();
	void setITERPARAMS3();
	bool getBCSFL(int index);
	bool getBCSTR(int index);
	void DISPR3(int i, int el, double VX, double VY, double VZ, double VMAG, double& DXXG, double& DXYG, double& DXZG, double& DYXG, double& DYYG, double& DYZG, double& DZXG, double& DZYG, double& DZZG);
	void ROTATE(vector<vector<double>> rotMat, double v1, double v2, double v3, double& vr1, double& vr2, double& vr3);
	void TENSYM(double DL, double DT1, double DT2, double VNX, double UNX, double WNX, double VNY, double UNY, double WNY, double VNZ, double UNZ, double WNZ,
		double& DXXG, double& DXYG, double& DXZG, double& DYXG, double& DYYG, double& DYZG, double&DZXG, double& DZYG, double& DZZG);
	void ZeroVectorsAndMatrix();
private:

	static ControlParameters* m_cInstance;
	ControlParameters();
	ControlParameters(ControlParameters const&){};
	ControlParameters& operator = (ControlParameters const&){};
	virtual ~ControlParameters();

	bool isNewRun;
	bool isNewNod;
	bool isNewObs;
	bool isNewIA;
	bool PRNALL;
	bool PRN0;
	bool PRNDEF;
	bool PRNK3;
	bool PRNK5;
	bool onceNOD = false;
	bool onceOBS = false;
	bool onceP = false;
	bool onceBCS = false;
	double TOL;

	double DELTLC, RELCHG, TELAPS;
	double DURN, TOUT;
	double TMAX, TEMAX, TSEC, TSECP0, TSECU0, TSECM1, TMIN, THOUR, TMONTH, TDAY, TWEEK, TYEAR, DELT, DELTM1, DELTU, DELTP;
	double SCALX, SCALY, SCALZ, PORFAC;
	double PMAXFA, PMIDFA, PMINFA, ANG1FA, ANG2FA, ANG3FA, ALMAXF, ALMIDF, ALMINF, ATMXF, ATMDF, ATMNF, ANGFAC;
	double waterTable;
	double TEMP;
	double PSTAR;
	double GCONST;
	double SMWH;
	double GRAVX; 
	double GRAVY; 
	double GRAVZ; 
	double COMPFL = 0; 
	double COMPMA = 0; 
	double VISC0 = 0; 
	double RHOW0 = 0; 
	double DRWDU = 0; 
	double URHOW0 = 0; 
	double SIGMAW;
	double SIGMAS;
	double CW = 0;
	double CS = 0;
	double RHOS = 0;

	double PRODS0 = 0; // for energy transport : zero-order rate of energy production in the immobile phase 
	double CHI1 = 0;
	double CHI2 = 0;

	double PRODF1 = 0; // for solute transport : rate of first-order production of adsorbate mass in the fluid mass ( equals zero for energy transport)
	double PRODS1 = 0; // for solute transport : rate of first-order production of solute mass in the immobile phase ( equals zero for energy transport)
	double PRODF0 = 0; // for energy transport - zero-order rate of energy production in fluid
	
	double TICS = 0; // time to which the initial conditions correspond (not necessarily not equal to the starting time of the simulation t0)
	double GNUP = 0; // specified pressure boundary condition "conductance" factor
	double GNUU = 0; // specified concentration boundary condition "conductance" factor
	double UP = 0; // fractional upstream weight for asymmetric weighting functions

	double SCALT = 0; // scaling factor for times
	
	double TIMEI = 0; // initial time for a time cycle
	double TIMEL = 0; // limiting time for a time cycle
	double TIMEC = 0; // initial time increment for a time cycle
	double TIME = 0;

	double TCMULT; // multiplier for time increment
	double TCMIN; // minimum time increment allowed
	double TCMAX; // maximum time increment allowed
	
	double ISTEPI = 0; // initial step for a time step cycle
	double ISTEPL = 0; // limiting time step for a time step cycle
	double ISTEPC = 0; // time step increment for a time step cycle
	
	double RPMAX = 0; // pressure convergence criterion for iterations
	double RUMAX = 0; // temperature or solution convergence criterion for resolving nonlinearities
	
	double TSTART;
	double TFINISH;
	
	double RPM, RUM;
	
	double DLTPM1, DLTUM1, BDELP1, BDELP, BDELU, BDELU1;
	bool setBCS = false;
	
	double IERRU, IERRP, IERR;
	double CNUB, CNUBM1;
	double RP, RU;
	
	double ERRP, ERRU;
	
	bool USEFL, ANYFL, ANYTR, SETFL, SETTR;
	double XOBS = 0;
	double YOBS = 0;
	double ZOBS = 0;

	double TOLP = 0; // convergence tolerance for solver iterations during P solution
	double TOLU = 0; // convergence tolerance for solver iterations during U solution


	double VXG[8] ;
	double VYG[8] ;
	double VZG[8] ;
	double VGMAG[8] ;
	double SWTG[8] ;
	double RHOG[8] ;
	double RELKTG[8] ;
	double VISCG[8] ;
	double RGXG[8] ;
	double RGYG[8] ;
	double RGZG[8] ;
	double PORG[8];


	int NSOU; 
	int NSOUI; // number of nodes at which a diffusive energy or solute mass flux(source) is specified
	int	NSCH; 
	int NSCHAU; // number of schedules
	int NTLIST; // number of times listed
	int NSLIST; // number of time steps listed
	int NTMAX; // maximum number of time cycles allowed
	int NTCYC; // number of cycle after which the time increment is updated
	int ISTORE = 0;
	int NSMAX; // maximum number of time step cycles
	int NPCYC; // time steps in pressure solution cycle
	int NUCYC; // time steps in temperature or concentration solution cycle
	int ITRMAX; // maximum number of iterations for nonlinearities per time step
	int IUNSAT;
	int ISSFLO;
	int ISSTRA;
	int IREAD;
	int NPRINT; // results are output to .lst file every NPRONT time steps
	int NCOLPR; // results are output to .nod  file every NCOLPR timesteps
	int LCOLPR; // results are output to .ele  file every NCOLPR timesteps
	int KNODAL;
	int KELMNT;
	int KINCID;
	int KCORT;
	int KPAUSE;
	int KSCRN; 
	int KVEL; 
	int KPANDS;
	int KBUDG;
	int NCOLMX;
	int LCOLMX;
	int NELT;
	int NB;
	int NBI;
	int NBHALF;
	int LEM;
	int ITMAX;
	int NRTEST;
	int LRTEST;
	int IT;
	int ITRST; 
	int ITREL; 
	int ITER; 
	int IPWORS; 
	int IUWORS;
	int KTPRN;
	int ML;
	int NCID;
	int INTIM = 0;
	int ISTOP = 0;
	int IHALFB;
	int KSOLVR; 
	int KMT; 
	int KPU;
	int IGOI;
	int ITRSP; 
	int ITRSU;
	int NBCN1; 
	int NSOPI1; 
	int NSOUI1;
	int ITRMXP; // maximum number of solver iterations during P solution
	int ITRMXU;// maximum number of solver iterations during U solution
	int NOBS;
	int NOBLIN;
	int NPBC;
	int NSOP;
	int NSOPI;
	int NUBC;
	int NN; 
	int NN1;
	int NN2;
	int NN3;
	int NE; 
	int NBLK1; 
	int NBLK2;
	int NBLK3;
	int NL;
	int NWI;
	int NWF;
	int NOUMAT;
	int ITOUT;
	int  ITBCS;
	int IQSOPT;
	int IQSOUT;
	int IPBCT;
	int IUBCT;
	int IBCT;
	int NBCFPR;
	int NBCSPR;
	int NBCPPR; 
	int NBCUPR;
	int KINACT;
	int NBCN; 
	int NOBSN;
	int N48; 
	int NEX; 
	int NIN; 
	int NNNX;
	int NDIMJA;
	int NNVEC;
	int ISYM; 
	int ITOL; 
	int NSAVE;
	int ITRMX;
	int KSOLVP;
	int KSOLVU;
	int ITOLU;
	int ITOLP;
	int NSAVEP;
	int NSAVEU;
	int ME; 
	int numberOfLayers;
	int timeStepDivide;
	int NLAYS;
	int NNLAY;
	int NELAY; 
	int KTYPE[2];

	Node **nodeArray;
	vector <ObservationPoints *> observationPointsContainer;
	vector<Node *> nodeContainer; 
	vector<Element *> elementContainer;
	vector<Schedule *> listOfSchedules;
	vector<Layer *> layers;
	vector<DataSet *> dataSetContainer;
	unordered_map <string, DataSet *> dataSetMap;

	string sSutra = "";
	string vVersion = "";
	string vVersionNum = "";
	string simulationType = "";
	string sTransport = "";
	string TITLE1, TITLE2;
	string BCSSCH;

	
	string MSHSTR = ""; // Mesh Structure 
	// "2D IRREGULAR"
	// "2D REGULAR"
	// "2D BLOCKWISE"
	// "3D IRREGULAR"
	// "3D LAYERED"
	// "3D REGULAR"
	// "3D IRREGULAR"


	// Check this List part !!!!!!!
	//LDIV1;
	//LDIV2;
	//LDIV3;



	string LAYSTR = ""; // first numbering direction (LAYERED mesh only) - ACROSS or WITHIN

	/* FLOW Parameters */

	
	/* Reaction and production parameters */
	
	// for solute transport - zero-order rate of solute mass production in fluid
	string ADSMOD;

	
	// for solute transport : zero-order rate of adsorbate mass production in the immobile phase 

	/* BOUNDARY CONDITIONS AND FLOW DATA */

	//---- FLOW Data -- SPECIFIED PRESSURES ---- //
	
	vector<int> IPBC; // node number at which pressure is specified (for all NPBC nodes)
	// PBC and UBC are in node object

	//---- FLOW Data -- SPECIFIED FLOWS and FLUID SOURCES ---- //
	
	vector<int> IQSOP; // node number at which a fluid source is specified (for all NPBC nodes)
	// QUINC, UINC are in node object

	//---- ENERGY OR SOLUTE Data -- SPECIFIED TEMPERATURES or CONCENTRATIONS ---- //
	
	vector <int> IUBC; // node number at which temperature or concentration is specified (for all NUBC nodes)
	// UBC is in node object

	//---- ENERGY OR SOLUTE Data -- DIFFUSIVE FLUXES OF ENERGY or SOLUTE MASS AT BOUNDARIES ---- //
	
	vector<int> IQSOU; // node number at which a flux(source) is specified
	// QUINC is in node object

	//----INITIAL CONDITIONS ---- //
	
	string CPUNI = "";// "UNIFORM" or "NONUNIFORM" (uniform initial P, nonuniform initial P)
	string CUUNI = "";// "UNIFORM" or "NONUNIFORM" (uniform initial U, nonuniform initial U)

	list<double> PVEC; // initial pressure at all nodes in mesh ( for uniform, a single value; for nonuniform, a list of values)
	list<double> UVEC; // for energy transport: initial temperature at all nodes in mesh ( for uniform, a single value; for nonuniform, a list of values)
	// for energy transport: initial concentration at all nodes in mesh ( for uniform, a single value; for nonuniform, a list of values)

	//---- Numerical and TEMPORAL CONTROL DATA ---- //

	string SCHNAM; // schedule name
	string SCHTYP; // "TIME LIST"  "TIME CYCLE" "STEP LIST" "STEP CYCLE"
	string CREFT; // "ABSOLUTE" "ELAPSED"
	
	list<double> TLIST; // list of times
	list<int> ISLIST; // list of time steps
	
	string OBSNAM; // observation point name

	//---- Matrix Equation Solver Data ---- //
	string CSOLVP = ""; // "DIRECT" "CG" "GMRES" "ORTHOMIN"
	string CSOLVU = ""; // "DIRECT" "GMRES" "ORTHOMIN"


	string CREAD = ""; // "COLD" or "WARM"
	string SIMULA = ""; // "SUTRA ENERGY" "SUTRA SOLUTE" 
	string CUNSAT = "";// "UNSATURATED" "SATURATED"
	string CSSFLO = ""; // "STEADY" "TRANSIENT"
	string CSSTRA = ""; // "STEADY" "TRANSIENT"
	string CVEL = ""; // "Y" = output fluid velocity at element centroids 	
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
	
	vector <string> NCOL; // list of variables to output in columns in the .nod file
	vector <string> LCOL; // list of variables to output in columns in the .ele file
	
	deque<string>writeContainer;
	
	list<pair<double,double>>timeSteps;
	list<pair<double, double>>step_0;
	list<pair<double, double>>step_1;
	list<pair<double, double>>step_1andUP;
	
	
	
	Schedule * BCSSchedule = nullptr;
	

	

	
	vector<bool> BCSFL;
	vector<bool> BCSTR;

	vector<string>CIDBCS;
	vector<BCS *> bcsContainer;
	vector<double> nodeVOL;
	vector<double> node_p_rhs;
	vector<double> node_u_rhs;
	vector<double> p_solution;
	vector<double> u_solution;
	vector<double> p_rhs;
	vector<double> u_rhs;
	vector<vector<double>> DWDXG;
	vector<vector<double>> DWDYG;
	vector<vector<double>> DWDZG;
	vector<vector<double>> DFDXG;
	vector<vector<double>> DFDYG;
	vector<vector<double>> DFDZG;
	vector<vector<double>> F;
	vector<vector<double>> W;
	vector<vector<int>> nodeNeighbors;
	vector<map<unsigned int, double>> stl_A;
	vector<int> jJA;
	viennacl::vector<double> vcl_rhs;
	viennacl::vector<double> vcl_results;
	
	viennacl::vector<double> init_guess;
	vector<double>nodePVEC;
	vector<double>nodeUVEC;
	vector<int>IAVec;
	vector<int>JAVec;

};
#endif
