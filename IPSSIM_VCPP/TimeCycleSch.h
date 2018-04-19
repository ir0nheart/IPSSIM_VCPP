#pragma once
#include "Schedule.h"
#include <string>
#include <float.h>
class TimeCycleSch :
	public Schedule
{
public:
	TimeCycleSch();
	~TimeCycleSch();
	void setCREFT(string CREFT);
	void setSCALT(double SCALT);
	void setNTMAX(int NTMAX);
	void setTIMEI(double TIMEI);
	void setTIMEL(double TIMEL);
	void setTIMEC(double TIMEC);
	void setNTCYC(int NTCYC);
	void setTCMULT(double TCMULT);
	void setTCMIN(double TCMIN);
	void setTCMAX(double TCMAX);
	virtual void createTimeList();
	string getCREFT();
	double getSCALT();
	int getNTMAX();
	double getTIMEI();
	double getTIMEL();
	double getTIMEC();
	int getNTCYC();
	double getTCMULT();
	double getTCMIN();
	double getTCMAX();
	void setELAPSD(bool val);
	bool getELAPSD();
	double FRCSTP(double TIME);
	void changeListElement(int index, double time, double step);
private:
	string CREFT;
	double SCALT;
	int NTMAX;
	double TIMEI;
	double TIMEL;
	double TIMEC;
	int NTCYC;
	double TCMULT;
	double TCMIN;
	double TCMAX;
	bool ELAPSD;
//	vector<pair<double, double>> sList;
};

