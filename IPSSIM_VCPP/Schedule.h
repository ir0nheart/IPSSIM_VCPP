#include <string>
#include <utility>
#include <vector>

#pragma once
using namespace std;
class ControlParameters;
class Schedule
{
public:
	Schedule();
	Schedule(string schName);
	~Schedule();
	void setScheduleName(string schName);
	void setScheduleType(string schType);
	string getScheduleName();
	string getScheduleType();
	virtual void createTimeList();
	void addToSList(double time, double step);
	vector<pair<double, double>> getSList();
	double TIMETS(int STEP);
	void setSBASED(bool val);
	bool getSBASED();
	void setELAPSED(bool val);
	bool getELAPSED();
	void reConstructSList(double tref);
	double FRCSTP(double TIME);
protected:
	vector<pair<double, double>> sList;
private:
	string scheduleName;
	string scheduleType;
	bool SBASED;
	bool ELAPSED;
		
};

