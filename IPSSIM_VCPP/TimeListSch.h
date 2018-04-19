#pragma once
#include "Schedule.h"
#include <string>
#include <vector>

class TimeListSch :
	public Schedule
{
public:
	TimeListSch();
	~TimeListSch();
	void setCREFT(string CREFT);
	void setSCALT(double SCALT);
	void setNTLIST(int NTLIST);
	void addToTLIST(double step);
	string getCREFT();
	double getSCALT();
	int getNTLIST();
	vector<double>getTLIST();
	virtual void createTimeList();
	void setELAPSD(bool val);
	bool getELAPSD();
private:
	string CREFT;
	double SCALT;
	int NTLIST;
	vector<double>TLIST; 
//	vector<pair<double, double>> sList;
	bool ELAPSD;
};

