#include "BCS.h"


BCS::BCS()
{
}


BCS::~BCS()
{
}


void BCS::setTimeStep(int ts){ this->timeStep = ts; }
void BCS::setScheduleName(string scheduleName){ this->scheduleName = scheduleName; }
void BCS::setBCSID(string BCSID){ this->BCSID = BCSID; }
void BCS::setNumberOfQINC(int val){ this->numberOfQINC = val; }
void BCS::setNumberOfUINC(int val){ this->numberOfQUINC = val; }
void BCS::setNumberOfPBC(int val){ this->numberOfPBC = val; }
void BCS::setNumberOfUBC(int val){ this->numberOfUBC = val; }
void BCS::addNode(int val){ this->node.push_back(val); }
void BCS::addQINC(double val){ this->QINC.push_back(val); }
void BCS::addQUINC(double val){ this->QUINC.push_back(val); }
void BCS::addUINC(double val){ this->UINC.push_back(val); }
void BCS::addPBC(double val){ this->PBC.push_back(val); }
void BCS::addUBC(double val){ this->UBC.push_back(val); }
void BCS::addIsQINC(bool val){ this->isQINC.push_back(val); }
void BCS::addIsQUINC(bool val){ this->isQUINC.push_back(val); }
void BCS::addIsUINC(bool val){ this->isUINC.push_back(val); }
void BCS::addIsPBC(bool val){ this->isPBC.push_back(val); }
void BCS::addIsUBC(bool val){ this->isUBC.push_back(val); }

int BCS::getTimeStep(){ return this->timeStep; }
string BCS::getScheduleName(){ return this->scheduleName; }
string BCS::getBCSID(){ return this->BCSID; }
int BCS::getNumberOfQINC(){ return this->numberOfQINC; }
int BCS::getNumberOfQUINC(){ return this->numberOfQUINC; }
int BCS::getNumberOfPBC(){ return this->numberOfPBC; }
int BCS::getNumberOfUBC(){ return this->numberOfUBC; }
vector<int>BCS::getNodes(){ return this->node; }
vector<double>BCS::getQINC(){ return this->QINC; }
vector<double>BCS::getQUINC(){ return this->QUINC; }
vector<double>BCS::getUINC(){ return this->UINC; }
vector<double>BCS::getPBC(){ return this->PBC; }
vector<double>BCS::getUBC(){ return this->UBC; }

vector<bool>BCS::getIsQINC(){ return this->isQINC; }
vector<bool>BCS::getIsQUINC(){ return this->isQUINC; }
vector<bool>BCS::getIsUINC(){ return this->isUINC; }
vector<bool>BCS::getIsPBC(){ return this->isPBC; }
vector<bool>BCS::getIsUBC(){ return this->isUBC; }