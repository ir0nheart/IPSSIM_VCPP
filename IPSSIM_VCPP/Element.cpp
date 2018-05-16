#include "Element.h"

#define _USE_MATH_DEFINES
#include <math.h>

const double multiplier = M_PI / 180;

Element::Element(){}  // Constructor
Element::~Element(){} // Destructor

double * Element::getGXSI(){
	return this->GXSI;
}
double * Element::getGETA(){
	return this->GETA;
}
double * Element::getGZET(){
	return this->GZET;
}
double * Element::getDET(){
	return this->DET;
}
void Element::setANGLE1(double ANGLE1){ 
	this->ANGLE1 = ANGLE1;
	this->PANGL1 = ANGLE1*multiplier;
}
void Element::setANGLE2(double ANGLE2){ 
	this->ANGLE2 = ANGLE2;
	this->PANGL2 = ANGLE2*multiplier;
}
void Element::setANGLE3(double ANGLE3){ 
	this->ANGLE3 = ANGLE3; 
	this->PANGL3 = ANGLE3*multiplier;
}
void Element::setANGLEX(double ANGLEX){
	this->ANGLEX = ANGLEX; 
	this->PANGLX = ANGLEX*multiplier;
}

void Element::setPMAX(double PMAX){ this->PMAX = PMAX; }
void Element::setPMID(double PMID){ this->PMID = PMID; }
void Element::setPMIN(double PMIN){ this->PMIN = PMIN; }
void Element::setALMAX(double ALMAX){ this->ALMAX = ALMAX; }
void Element::setALMID(double ALMID){ this->ALMID = ALMID; }
void Element::setALMIN(double ALMIN){ this->ALMIN = ALMIN; }
void Element::setATMAX(double ATMAX){ this->ATMAX = ATMAX; }
void Element::setATMID(double ATMID){ this->ATMID = ATMID; }
void Element::setATMIN(double ATMIN){ this->ATMIN = ATMIN; }
void Element::setLREG(int LREG){ this->LREG = LREG; }
void Element::setVANG1(double val){ this->VANG1 = val; }
void Element::setVANG2(double val){ this->VANG2 = val; }
void Element::setVMAG(double val){ this->VMAG = val; }
//Getters
vector<vector<double>> Element::getRotationMatrix(){
	vector<vector<double>> temp;
	vector<double> fr(rotationMatrix[0], rotationMatrix[0] + 3);
	vector<double> sr(rotationMatrix[1], rotationMatrix[1] + 3);
	vector<double> tr(rotationMatrix[2], rotationMatrix[2] + 3);
	temp.push_back(fr);
	temp.push_back(sr);
	temp.push_back(tr);
	return temp;
}

double Element::getPANGL1(){
	return this->PANGL1;
}
double Element::getPANGL2(){
	return this->PANGL2;
}
double Element::getPANGL3(){
	return this->PANGL3;
}
double Element::getPANGLX(){
	return this->PANGLX;
}

int Element::getElementNumber(){ return this->ElementNumber; }
int Element::getLREG(){ return this->LREG; }
double Element::getPMAX(){ return this->PMAX; }
double Element::getPMID(){ return this->PMID; }
double Element::getPMIN(){ return this->PMIN; }
double Element::getALMAX(){ return this->ALMAX; }
double Element::getALMID(){ return this->ALMID; }
double Element::getALMIN(){ return this->ALMIN; }
double Element::getATMAX(){ return this->ATMAX; }
double Element::getATMID(){ return this->ATMID; }
double Element::getATMIN(){ return this->ATMIN; }
double Element::getPERMXX(){ return this->PERMXX; }
double Element::getPERMXY(){ return this->PERMXY; }
double Element::getPERMXZ(){ return this->PERMXZ; }
double Element::getPERMYX(){ return this->PERMYX; }
double Element::getPERMYY(){ return this->PERMYY; }
double Element::getPERMYZ(){ return this->PERMYZ; }
double Element::getPERMZX(){ return this->PERMZX; }
double Element::getPERMZY(){ return this->PERMZY; }
double Element::getPERMZZ(){ return this->PERMZZ; }
double Element::getVMAG(){ return this->VMAG; }
double Element::getVANG1(){ return this->VANG1; }
double Element::getVANG2(){ return this->VANG2; }


void Element::ROTMATTENSYM(){
	double s1, c1, s2, c2, s3, c3;
	s1 = sin(PANGL1);
	c1 = cos(PANGL1);
	s2 = sin(PANGL2);
	c2 = cos(PANGL2);
	s3 = sin(PANGL3);
	c3 = cos(PANGL3);
	
	rotationMatrix[0][0]=(c1*c2);
	rotationMatrix[0][1] = (-c1*s2*s3 - s1*c3);
	rotationMatrix[0][2] = (-c1*s2*c3 + s1*s3);
	

	rotationMatrix[1][0] = (s1*c2);
	rotationMatrix[1][1] = (-s1*s2*s3 + c1*c3);
	rotationMatrix[1][2] = (-s1*s2*c3 - c1*s3);
	
	rotationMatrix[2][0] = (s2);
	rotationMatrix[2][1] = (c2*s3);
	rotationMatrix[2][2] = (c2*c3);
	

	PERMXX = PMAX* rotationMatrix[0][0] * rotationMatrix[0][0] + PMID * rotationMatrix[0][1] * rotationMatrix[0][1] + PMIN * rotationMatrix[0][2] * rotationMatrix[0][2];
	PERMXY = PMAX* rotationMatrix[0][0] * rotationMatrix[1][0] + PMID * rotationMatrix[0][1] * rotationMatrix[1][1] + PMIN * rotationMatrix[0][2] * rotationMatrix[1][2];
	PERMXZ = PMAX* rotationMatrix[0][0] * rotationMatrix[2][0] + PMID * rotationMatrix[0][1] * rotationMatrix[2][1] + PMIN * rotationMatrix[0][2] * rotationMatrix[2][2];
	PERMYY = PMAX* rotationMatrix[1][0] * rotationMatrix[1][0] + PMID * rotationMatrix[1][1] * rotationMatrix[1][1] + PMIN * rotationMatrix[1][2] * rotationMatrix[1][2];
	PERMYZ = PMAX* rotationMatrix[1][0] * rotationMatrix[2][0] + PMID * rotationMatrix[1][1] * rotationMatrix[2][1] + PMIN * rotationMatrix[1][2] * rotationMatrix[2][2];
	PERMZZ = PMAX* rotationMatrix[2][0] * rotationMatrix[2][0] + PMID * rotationMatrix[2][1] * rotationMatrix[2][1] + PMIN * rotationMatrix[2][2] * rotationMatrix[2][2];
	PERMYX = PERMXY;
	PERMZX = PERMXZ;
	PERMZY = PERMYZ;
}

void Element::setPERMS2D(){ // Set PERM VALUES FOR 2D Element
		double sinA, cosA, sinA2, cosA2;
		sinA = sin(PANGLX);
		cosA = cos(PANGLX);
		sinA2 = sinA*sinA;
		cosA2 = cosA*cosA;
	
		PERMXX = PMAX*cosA2 + PMIN *sinA2;
		PERMYY = PMAX*sinA2 + PMIN*cosA2;
		PERMXY = (PMAX - PMIN)*sinA*cosA;
		PERMYX = PERMXY;
}
void Element::setElementNodes(int * Nodes){
	int nSize = sizeof(Nodes);
	this->elementNodes = Nodes;
}
Element::Element(int elementNumber):ElementNumber(elementNumber){}
Element::Element(Element const&other){
	ElementNumber = other.ElementNumber;
}
Element& Element::operator = (Element const&other){ 
	ElementNumber = other.ElementNumber;
	return *this; }

Element::Element(Element && other){
	ElementNumber = other.ElementNumber;
}
Element & Element::operator = (Element && other){ 
	ElementNumber = other.ElementNumber; 
	return *this;
}

int * Element::getElementNodes(){ 
	return this->elementNodes; 
}
void Element::print_Info(){
	cout << setfill('#') << setw(80) <<""<< endl;
	cout << setfill('*') << setw(60);
	cout << "Element Information"<<endl;
	cout << setfill('*') << setw(20) << "" << endl;
	cout << "\t ElementNumber : " << ElementNumber << endl;
	cout << "\t ElementRegion: " << LREG << endl;
	cout << "\t PMAX : " << PMAX << endl;
	cout << "\t PMID : " << PMID << endl;
	cout << "\t PMIN : " << PMIN << endl;
	cout << "\t ATMAX : " << ATMAX << endl;
	cout << "\t ATMID : " << ATMID << endl;
	cout << "\t ATMIN : " << ATMIN << endl;
	cout << "\t ALMAX : " << ALMAX << endl;
	cout << "\t ALMID : " << ALMID << endl;
	cout << "\t ALMIN : " << ALMIN << endl;
	cout << "\t ANGLE 1 : " << ANGLE1 << endl;
	cout << "\t ANGLE 2 : " << ANGLE2 << endl;
	cout << "\t ANGLE 3 : " << ANGLE3 << endl;

	cout << "\n\t\t Calculated Element Properties" << endl;
	cout << "\t PANGLE 1 :" << PANGL1 << endl;
	cout << "\t PANGLE 2 :" << PANGL2 << endl;
	cout << "\t PANGLE 3 :" << PANGL3 << endl;

	cout << "\t\t Element Nodes" << endl;
	cout << "\t";
	for (int i = 0; i < sizeof(elementNodes); i++){
		cout << elementNodes[i] << "  ";		
	}
	cout << endl;

	cout << "\t Rotation Matrix: " << endl;
	for (int i = 0; i < 3; i++){
		cout << "\t\t";
		for (int j = 0; j < 3; j++){
			cout << rotationMatrix[i][j] << "\t";
		}
		cout << "\t" << endl;
	}
	cout << endl;

	cout << "\t PERMEABILITY TENSOR : " << endl;
	cout << "\t\t" <<PERMXX << "\t" << PERMXY << "\t" << PERMXZ << endl;
	cout << "\t\t" << PERMYX << "\t" << PERMYY << "\t" << PERMYZ << endl;
	cout << "\t\t" << PERMZX << "\t" << PERMZY << "\t" << PERMZZ << endl;
	cout << setfill('#') << setw(80) << ""<< endl;

}

void Element::setGXSI(double * GXSI){

	this->GXSI = GXSI;
}

void Element::setGETA(double * GETA){

	this->GETA = GETA;
}
void Element::setGZET(double * GZET){
	this->GZET = GZET;
}
void Element::setDET(double * DET){
	this->DET = DET;
}
void Element::putIntoGXSI(int ind,double GXSI){

	this->GXSI[ind] = GXSI;
}
void Element::putIntoGETA(int ind, double GETA){

	this->GETA[ind] = GETA;
}
void Element::putIntoGZET(int ind, double GZET){

	this->GZET[ind] = GZET;
}
void Element::putIntoDET(int ind, double DET){

	this->DET[ind] = DET;
}

//void Element::setF(double ** F){ this->F = F; for (int i = 0; i < 8; i++){ this->F[i] = new double[8]; } }
//void Element::setW(double ** W){ this->W = W; for (int i = 0; i < 8; i++){ this->W[i] = new double[8]; } }
//void Element::setDWDXG(double ** DWDXG){ this->DWDXG = DWDXG; for (int i = 0; i < 8; i++){ this->DWDXG[i] = new double[8]; } }
//void Element::setDWDYG(double ** DWDYG){ this->DWDYG = DWDYG; for (int i = 0; i < 8; i++){ this->DWDYG[i] = new double[8]; } }
//void Element::setDWDZG(double ** DWDZG){ this->DWDZG = DWDZG; for (int i = 0; i < 8; i++){ this->DWDZG[i] = new double[8]; } }
//void Element::setDFDXG(double ** DFDXG){ this->DFDXG = DFDXG; for (int i = 0; i < 8; i++){ this->DFDXG[i] = new double[8]; } }
//void Element::setDFDYG(double ** DFDYG){ this->DFDYG = DFDYG; for (int i = 0; i < 8; i++){ this->DFDYG[i] = new double[8]; } }
//void Element::setDFDZG(double ** DFDZG){ this->DFDZG = DFDZG; for (int i = 0; i < 8; i++){ this->DFDZG[i] = new double[8]; } }
//void Element::setVXG(double *VXG){this->VXG = VXG;}
//void Element::setVYG(double *VYG){this->VYG = VYG;}
//void Element::setVZG(double *VZG){this->VZG = VZG;}
//void Element::setSWG(double *SWG){this->SWG = SWG;}
//void Element::setSWTG(double *SWTG){this->SWTG = SWTG;}
//void Element::setSWBG(double *SWBG){this->SWBG = SWBG;}
//void Element::setRHOG(double *RHOG){this->RHOG = RHOG;}
//void Element::setVISCG(double *VISCG){this->VISCG = VISCG;}
//void Element::setPORG(double *PORG){this->PORG = PORG;}
//void Element::setVGMAG(double *VGMAG){this->VGMAG =VGMAG ;}
//void Element::setCNUBG(double *CNUBG){this->CNUBG =CNUBG ;}
//void Element::setRGXG(double *RGXG){this->RGXG =RGXG ;}
//void Element::setRGYG(double *RGYG){this->RGYG =RGYG ;}
//void Element::setRGZG(double *RGZG){this->RGZG =RGZG ;}
//void Element::setRELKG(double *RELKG){this->RELKG = RELKG;}
//void Element::setRELKTG(double *RELKTG){this->RELKTG =RELKTG ;}
//void Element::setRELKBG(double *RELKBG){this->RELKBG =RELKBG ;}
//
//void Element::putIntoF(int ind, int jnd, double  F){ this->F[ind][jnd] = F; }
//void Element::putIntoW(int ind, int jnd, double  W){ this->W[ind][jnd] = W; }
//void Element::putIntoDWDXG(int ind,int jnd, double  DWDXG){ this->DWDXG[ind][jnd] = DWDXG; }
//void Element::putIntoDWDYG(int ind, int jnd, double  DWDYG){ this->DWDYG[ind][jnd] = DWDYG; }
//void Element::putIntoDWDZG(int ind, int jnd, double  DWDZG){ this->DWDZG[ind][jnd] = DWDZG; }
//void Element::putIntoDFDXG(int ind,int jnd, double  DFDXG){this->DFDXG[ind][jnd] = DFDXG;}
//void Element::putIntoDFDYG(int ind, int jnd, double  DFDYG){ this->DFDYG[ind][jnd] = DFDYG; }
//void Element::putIntoDFDZG(int ind, int jnd, double DFDZG){ this->DFDZG[ind][jnd] = DFDZG; }
//void Element::putIntoVXG(int ind, double VXG){this->VXG[ind] = VXG;}
//void Element::putIntoVYG(int ind, double VYG){this->VYG[ind] = VYG;}
//void Element::putIntoVZG(int ind, double VZG){this->VZG[ind] = VZG;}
//void Element::putIntoSWG(int ind, double SWG){this->SWG[ind] = SWG;}
//void Element::putIntoSWTG(int ind, double SWTG){this->SWTG[ind] = SWTG;}
//void Element::putIntoSWBG(int ind, double SWBG){this->SWBG[ind] = SWBG;}
//void Element::putIntoRHOG(int ind, double RHOG){this->RHOG[ind] = RHOG;}
//void Element::putIntoVISCG(int ind, double VISCG){this->VISCG[ind] = VISCG;}
//void Element::putIntoPORG(int ind, double PORG){this->PORG[ind] = PORG;}
//void Element::putIntoVGMAG(int ind, double VGMAG){this->VGMAG[ind] = VGMAG;}
//void Element::putIntoCNUBG(int ind, double CNUBG){ this->CNUBG[ind] = CNUBG; }
//void Element::putIntoRGXG(int ind, double RGXG){this->RGXG[ind] = RGXG;}
//void Element::putIntoRGYG(int ind, double RGYG){this->RGYG[ind] = RGYG;}
//void Element::putIntoRGZG(int ind, double RGZG){this->RGZG[ind] = RGZG;}
//void Element::putIntoRELKG(int ind, double RELKG){this->RELKG[ind] = RELKG;}
//void Element::putIntoRELKTG(int ind, double RELKTG){this->RELKTG[ind] = RELKTG;}
//void Element::putIntoRELKBG(int ind, double RELKBG){this->RELKBG[ind] = RELKBG;}
//
//double ** Element::getF(){return this->F;}
//double ** Element::getW(){return this->W;}
//double ** Element::getDWDXG(){return this->DWDXG;}
//double ** Element::getDWDYG(){return this->DWDYG;}
//double ** Element::getDWDZG(){return this->DWDZG;}
//double ** Element::getDFDXG(){return this->DFDXG;}
//double ** Element::getDFDYG(){return this->DFDYG;}
//double ** Element::getDFDZG(){return this->DFDZG;}
//double * Element::getVXG(){return this->VXG;}
//double * Element::getVYG(){return this->VYG;}
//double * Element::getVZG(){return this->VZG;}
//double * Element::getSWG(){return this->SWG;}
//double * Element::getSWTG(){return this->SWTG;}
//double * Element::getSWBG(){return this->SWBG;}
//double * Element::getRHOG(){return this->RHOG;}
//double * Element::getVISCG(){return this->VISCG;}
//double * Element::getPORG(){return this->PORG;}
//double * Element::getVGMAG(){return this->VGMAG;}
//double * Element::getCNUBG(){return this->CNUBG;}
//double * Element::getRGXG(){return this->RGXG;}
//double * Element::getRGYG(){return this->RGYG;}
//double * Element::getRGZG(){return this->RGZG;}
//double * Element::getRELKG(){return this->RELKG;}
//double * Element::getRELKTG(){return this->RELKTG;}
//double * Element::getRELKBG(){return this->RELKBG;}