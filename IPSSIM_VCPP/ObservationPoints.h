#include <string>
#include "Element.h"

#pragma once
using namespace std;
class ObservationPoints
{
public:
	ObservationPoints(string OBSNAM,string schedule,string OBSFMT,double XOBS,double YOBS,double ZOBS = 0.0);
	~ObservationPoints();
	string getOBSNAM();
	string getOBSFMT();
	string getSchedule();
	double getXOBS();
	double getYOBS();
	double getZOBS();
	void setP1(double val);
	void setU1(double val);
	void setC1(double val);
	void setE1(double val);
	void setR1(double val);
	void setS1(double val);
	double getP1();
	double getU1();
	double getC1();
	double getE1();
	double getR1();
	double getS1();
	void setXSI(double XSI); 
	void setETA(double ETA);
	void setZET(double ZET);
	void setObsElement(int ElNo);
	
	double getXSI(); 
	double getETA(); 
	double getZET(); 
	int getObsElement();
	void print_Info();

	void setP2(double val);
	void setU2(double val);
	void setC2(double val);
	void setE2(double val);
	void setR2(double val);
	void setS2(double val);
	double getP2();
	double getU2();
	double getC2();
	double getE2();
	double getR2();
	double getS2();
private:
	string OBSNAM;
	double XOBS;
	double YOBS;
	double ZOBS;
	string schedule;
	string OBSFMT;
	double XSI =0;
	double ZET=0;
	double ETA=0;
	int obsElement;
	double P1, U1, C1, E1, R1,S1;
	double P2, U2, C2, E2, R2,S2;
};

