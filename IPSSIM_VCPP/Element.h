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
	double VMAG, VANG1, VANG2;
};

