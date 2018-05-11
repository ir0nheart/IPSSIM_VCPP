#include <vector>
#include "Node.h"
#pragma once
using namespace std;
class Element
{
public:
	Element();
	~Element();
	Element(int elementNumber);

	//Setters
	double ** getF();
	double ** getW();
	double ** getDWDXG();
	double ** getDWDYG();
	double ** getDWDZG();
	double ** getDFDXG();
	double ** getDFDYG();
	double ** getDFDZG();
	double * getVXG();
	double * getVYG();
	double * getVZG();
	double * getSWG();
	double * getSWTG();
	double * getSWBG();
	double * getRHOG();
	double * getVISCG();
	double * getPORG();
	double * getVGMAG();
	double * getCNUBG();
	double * getRGXG();
	double * getRGYG();
	double * getRGZG();
	double * getRELKG();
	double * getRELKTG();
	double * getRELKBG();
	double * getGXSI();
	double * getGETA();
	double * getGZET();
	double * getDET();
	void setElementNumber(int elementNumber);
	void setLREG(int elementRegion);
	void setANGLE1(double ANG);
	void setANGLE2(double ANG);
	void setANGLE3(double ANG);
	void setANGLEX(double ANG);
	void setPMAX(double PMAX);
	void setPMID(double PMID);
	void setPMIN(double PMIN);
	void setALMAX(double ALMAX);
	void setALMID(double ALMID);
	void setALMIN(double ALMIN);
	void setATMAX(double ATMAX);
	void setATMID(double ATMID);
	void setATMIN(double ATMIN);
	void setPANG1(double ANG);
	void setPANG2(double ANG);
	void setPANG3(double ANG);
	void setPANGX(double ANG);
	void setVMAG(double val);
	void setVANG1(double val);
	void setVANG2(double val);
	void setElementNodes(int * Nodes);
	int * getElementNodes();
	//Getters
	int getElementNumber();
	int getLREG();

	double getANGLE1();
	double getANGLE2();
	double getANGLE3();
	double getPMAX();
	double getPMID();
	double getPMIN();
	double getALMAX();
	double getALMID();
	double getALMIN();
	double getATMAX();
	double getATMID();
	double getATMIN();
	double getPANGL1();
	double getPANGL2();
	double getPANGL3();
	double getPANGLX();

	double getPERMXX();
	double getPERMXY();
	double getPERMXZ();
	double getPERMYX();
	double getPERMYY();
	double getPERMYZ();
	double getPERMZX();
	double getPERMZY();
	double getPERMZZ();
	double getVMAG();
	double getVANG1();
	double getVANG2();

	
	
	Element(Element const&other);
	Element & operator = (Element const&other);

	Element(Element && other);
	Element & operator = (Element && other);

	double getRotationMatrix();
	void ROTMATTENSYM();
	void setPERMS2D();
	void ROTMAT3D(); // Compute Rotation Matrix
	void TENSYM3D();
	void print_Info();
	void setGXSI(double * GXSI);
	void setGETA(double * GETA);
	void setGZET(double * GZET);
	void setDET(double * DET);
	void setF(double ** F);
	void setW(double ** W);
	void setDWDXG(double ** DWDXG);
	void setDWDYG(double ** DWDYG);
	void setDWDZG(double ** DWDZG);
	void setDFDXG(double ** DFDXG);
	void setDFDYG(double ** DFDYG);
	void setDFDZG(double ** DFDZG);
	void setVXG(double *VXG);
	void setVYG(double *VYG);
	void setVZG(double *VZG);
	void setSWG(double *SWG);
	void setSWTG(double *SWTG);
	void setSWBG(double *SWBG);
	void setRHOG(double *RHOG);
	void setVISCG(double *VISCG);
	void setPORG(double *PORG);
	void setVGMAG(double *VGMAG);
	void setCNUBG(double *CNUBG);
	void setRGXG(double *RGXG);
	void setRGYG(double *RGYG);
	void setRGZG(double *RGZG);
	void setRELKG(double *RELKG);
	void setRELKTG(double *RELKTG);
	void setRELKBG(double *RELKBG);

	void putIntoGETA(int ind, double GETA);
	void putIntoGXSI(int ind, double GXSI);
	void putIntoGZET(int ind,double GZET);
	void putIntoDET(int ind, double DET);
	void putIntoF(int ind, int jnd, double  F);
	void putIntoW(int ind, int jnd, double  W);
	void putIntoDWDXG(int ind, int jnd, double  DWDXG);
	void putIntoDWDYG(int ind, int jnd, double  DWDYG);
	void putIntoDWDZG(int ind, int jnd, double  DWDZG);
	void putIntoDFDXG(int ind, int jnd, double  DFDXG);
	void putIntoDFDYG(int ind, int jnd, double  DFDYG);
	void putIntoDFDZG(int ind, int jnd, double DFDZG);
	void putIntoVXG(int ind,double VXG);
	void putIntoVYG(int ind,double VYG);
	void putIntoVZG(int ind,double VZG);
	void putIntoSWG(int ind,double SWG);
	void putIntoSWTG(int ind,double SWTG);
	void putIntoSWBG(int ind,double SWBG);
	void putIntoRHOG(int ind,double RHOG);
	void putIntoVISCG(int ind,double VISCG);
	void putIntoPORG(int ind,double PORG);
	void putIntoVGMAG(int ind,double VGMAG);
	void putIntoCNUBG(int ind,double CNUBG);
	void putIntoRGXG(int ind,double RGXG);
	void putIntoRGYG(int ind,double RGYG);
	void putIntoRGZG(int ind,double RGZG);
	void putIntoRELKG(int ind,double RELKG);
	void putIntoRELKTG(int ind,double RELKTG);
	void putIntoRELKBG(int ind,double RELKBG);
private:
    // Element Number
	int ElementNumber;
	// Element Region
	int LREG;
	//Permeabilities
	double PERMXX;
	double PERMXY;
	double PERMXZ;
	double PERMYX;
	double PERMYY;
	double PERMYZ;
	double PERMZX;
	double PERMZY;
	double PERMZZ;
	
	double PMAX;
	double PMID;
	double PMIN;
	double ATMAX;
	double ATMID;
	double ATMIN;
	double ALMAX;
	double ALMID;
	double ALMIN;
	// Angles in Radians
	double ANGLE1;
	double ANGLE2;
	double ANGLE3;
	double ANGLEX;
	double PANGL1;
	double PANGL2;
	double PANGL3;
	double PANGLX;


	double rotationMatrix[3][3];
	int * elementNodes;
	double * GXSI;
	double * GETA;
	double * GZET;
	double * DET;
	double VMAG, VANG1, VANG2;

	double ** F;
	double ** W;
	double ** DWDXG;
	double ** DWDYG;
	double ** DWDZG;
	double ** DFDXG;
	double ** DFDYG;
	double ** DFDZG;
	double *VXG;
	double *VYG;
	double *VZG;
	double *SWG;
	double *SWTG;
	double *SWBG;
	double *RHOG;
	double *VISCG;
	double *PORG;
	double *VGMAG;
	double *CNUBG;
	double *RGXG;
	double *RGYG;
	double *RGZG;
	double *RELKG;
	double *RELKTG;
	double *RELKBG;

};
