#pragma once
#include "Schedule.h"
class StepCycleSch :
	public Schedule
{
public:
	StepCycleSch();
	~StepCycleSch();
	void setISTEPI(int ISTEPI);
	void setISTEPL(int ISTEPL);
	void setISTEPC(int ISTEPC);
	void setNSMAX(int NSMAX);
	virtual void createTimeList();
	int getISTEPI();
	int getISTEPL();
	int getISTEPC();
	int getNSMAX();
private:
	int NSMAX;
	int ISTEPI;
	int ISTEPL;
	int ISTEPC;
//	vector<pair<double, double>> sList;
};

