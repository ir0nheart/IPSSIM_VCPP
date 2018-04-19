#pragma once
#include "Schedule.h"
#include <list>
class StepListSch :
	public Schedule
{
public:
	StepListSch();
	~StepListSch();
	void setNSLIST(int NSLIST);
	void addToISLIST(double step);
	int getNSLIST();
	list<double>getISLIST();
	virtual void createTimeList();
private:
	int NSLIST;
	list<double> ISLIST;
//	vector<pair<double, double>> sList;
};

