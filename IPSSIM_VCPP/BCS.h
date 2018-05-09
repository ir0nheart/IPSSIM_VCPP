#ifndef BCS_
#define BCS_
#include <string>
#include <vector>

using namespace std;
#pragma once
class BCS
{
public:
	BCS();
	virtual ~BCS();
	void setTimeStep(int ts);
	void setScheduleName(string scheduleName);
	void setBCSID(string BCSID);
	void setNumberOfQINC(int val);
	void setNumberOfUINC(int val);
	void setNumberOfPBC(int val);
	void setNumberOfUBC(int val);
	void addNode(int val);
	void addQINC(double val);
	void addQUINC(double val);
	void addUINC(double val);
	void addPBC(double val);
	void addUBC(double val);
	void addIsQINC(bool val);
	void addIsQUINC(bool val);
	void addIsUINC(bool val);
	void addIsPBC(bool val);
	void addIsUBC(bool val);

	int getTimeStep();
	string getScheduleName();
	string getBCSID();
	int getNumberOfQINC();
	int getNumberOfQUINC();
	int getNumberOfPBC();
	int getNumberOfUBC();
	vector<int>getNodes();
	vector<double>getQINC();
	vector<double>getQUINC();
	vector<double>getUINC();
	vector<double>getPBC();
	vector<double>getUBC();
	vector<bool>getIsQINC();
	vector<bool>getIsQUINC();
	vector<bool>getIsUINC();
	vector<bool>getIsPBC();
	vector<bool>getIsUBC();
private:
	int timeStep;
	string scheduleName;
	string BCSID;
	int numberOfQINC;
	int numberOfQUINC;
	int numberOfPBC;
	int numberOfUBC;

	vector<int> node;
	vector<double> QINC;
	vector<double> QUINC;
	vector<double> UINC;
	vector<double> PBC;
	vector<double> UBC;
	vector<bool> isQINC;
	vector<bool> isQUINC;
	vector<bool> isUINC;
	vector<bool> isPBC;
	vector<bool> isUBC;
};
#endif 

