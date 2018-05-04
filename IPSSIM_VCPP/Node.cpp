#include "Node.h"

using namespace std;

Node::Node(){}
Node::~Node(){}
Node::Node(int NodeNumber){this->NodeNumber = NodeNumber;}

Node::Node(Node const&rhs){
	NodeNumber = rhs.NodeNumber;
}
Node & Node::operator = (Node const&rhs){ 
	NodeNumber = rhs.NodeNumber;
	return *this; }
// Move Constructor
Node::Node(Node &&rhs){
	NodeNumber = rhs.NodeNumber;
}
Node & Node::operator = (Node &&rhs){
	NodeNumber = rhs.NodeNumber;
	return *this;
}
void Node::print_Node_Info(){
	cout << setfill('#')<< setw(80) << ""<< endl;
	cout << setfill('*') << setw(50);
	cout << "  Node Information  ";
	cout << setfill('*') << setw(30) << ""<< endl;
	cout << "\t NodeNumber : " << NodeNumber << endl;
	cout << "\t NodeRegion : " << NREG << endl;
	cout << "\t Node X : " << XCoord << endl;
	cout << "\t Node Y : " << YCoord << endl;
	cout << "\t Node Z : " << ZCoord << endl;
	cout << "\t Node Porosity : " << Porosity << endl;
	cout << "\t Node SOP : " << SOP << endl;
	cout << "\t Node PBC : " << PBC << endl;
	cout << "\t Node UBC : " << UBC << endl;
	cout << "\t Node QIN : " << QIN << endl;
	cout << "\t Node UIN : " << UIN << endl;
	cout << "\t Node QUIN : " << QUIN << endl;
	cout << "\t Node SWT : " << SWT << endl;
	cout << setfill('#') << setw(80) << ""<<endl;

}
// Setters
void Node::setNodeNumber(int NodeNumber){ this->NodeNumber = NodeNumber; }
void Node::setNREG(int NREG){ this->NREG = NREG; }
void Node::setXCoord(double XCoord){ this->XCoord = XCoord; }
void Node::setYCoord(double YCoord){ this->YCoord = YCoord; }
void Node::setZCoord(double ZCoord){ this->ZCoord = ZCoord; }
void Node::setPorosity(double Porosity){ this->Porosity = Porosity; }
void Node::setSOP(double SOP){ this->SOP = SOP; }
void Node::setPBC(double PBC){ this->PBC = PBC; }
void Node::setUBC(double UBC){ this->UBC = UBC; }
void Node::setQIN(double QIN){ this->QIN = QIN; }
void Node::setUIN(double UIN){ this->UIN = UIN; }
void Node::setQUIN(double QUIN){ this->QUIN = QUIN; }
void Node::setPVEC(double PVEC){ this->PVEC = PVEC; }
void Node::setUVEC(double UVEC){ this->UVEC = UVEC; }
void Node::setSWT(double SWT){ this->SWT = SWT; }
void Node::setSTRRAT(double STRRAT){ this->STRRAT = STRRAT; }
void Node::setEFFSTR(double EFFSTR){ this->EFFSTR = EFFSTR; }
void Node::setEFFSTRI(double EFFSTR){ this->EFFSTRI = EFFSTR; }
void Node::setTOTSTR(double val){ this->TOTSTR = val; }
void Node::setPOREP(double val){ this->POREP = val; }
void Node::setLayer(Layer * lay, int layN){
	this->layer = lay;
	this->layerN = layN;
}
void Node::setIBCPBC(int val){ this->IBCPBC = val; }
void Node::setIBCUBC(int val){ this->IBCUBC = val; }
void Node::setIBCSOP(int val){ this->IBCSOP = val; }
void Node::setIBCSOU(int val){ this->IBCSOU = val; }
void Node::setCNUB(double val){ this->CNUB = val; }
void Node::setCNUBM1(double val){ this->CNUBM1 = val; }
void Node::setPM(double val){ this->PM = val; }
void Node::setUM(double val){ this->UM = val; }
void Node::setCNUB(double val){ this->CNUB = val; }
void Node::setPM1(double val){ this->PM1 = val; }
void Node::setUM1(double val){ this->UM1 = val; }
void Node::setUM2(double val){ this->UM2 = val; }
void Node::setPITER(double val){ this->PITER = val; }
void Node::setUITER(double val){ this->UITER = val; }
void Node::setRCIT(double val){ this->RCIT = val; }
void Node::setRCITM1(double val){ this->RCITM1 = val; }
void Node::setDPDTITR(double val){ this->DPDTITR = val; }
void Node::setPVEL(double val){ this->PVEL = val; }
void Node::setGNUU1(double val){ this->GNUU1 = val; }
void Node::setGNUP1(double val){ this->GNUP1 = val; }
void Node::setQPLITR(double val){ this->QPLITR = val; }
void Node::setQINITR(double val) { this->QINITR = val; }

// Getters
int Node::getIBCPBC(){ return this->IBCPBC; }
int Node::getIBCUBC(){ return this->IBCUBC; }
int Node::getIBCSOP(){ return this->IBCSOP; }
int Node::getIBCSOU(){ return this->IBCSOU; }
int Node::getNodeNumber(){ return this->NodeNumber; }
int Node::getNREG(){ return this->NREG;}
double Node::getXCoord(){ return this->XCoord; }
double Node::getYCoord(){ return this->YCoord; }
double Node::getZCoord(){ return this->ZCoord; }
double Node::getPorosity(){ return this->Porosity; }
double Node::getSOP(){ return this->SOP; }
double Node::getPBC(){ return this->PBC; }
double Node::getUBC(){ return this->UBC; }
double Node::getQIN(){ return this->QIN; }
double Node::getUIN(){ return this->UIN; }
double Node::getQUIN(){ return this->QUIN; }
double Node::getPVEC(){ return this->PVEC; }
double Node::getUVEC(){ return this->UVEC; }
double Node::getSWT(){ return this->SWT; }
double Node::getSTRRAT(){ return this->STRRAT; }
double Node::getEFFSTR(){ return this->EFFSTR; }
double Node::getEFFSTRI(){ return this->EFFSTRI; }
Layer * Node::getLayer(){ return this->layer; }
int Node::getLayerN(){ return this->layerN; }
double Node::getPOREP(){ return this->POREP; }
double Node::getTOTSTR(){ return this->TOTSTR; }
double Node::getCNUB(){ return this->CNUB; }
double Node::getPM(){ return this->PM; }
double Node::getUM(){ return this->UM; }
double Node::getCNUB(){ return this->CNUB; }
double Node::getCNUBM1(){ return this->CNUBM1; }
double Node::getPM1(){ return this->PM1; }
double Node::getUM1(){ return this->UM1; }
double Node::getUM2(){ return this->UM2; }
double Node::getPITER(){ return this->PITER; }
double Node::getUITER(){ return this->UITER; }
double Node::getRCIT(){ return this->RCIT; }
double Node::getRCITM1(){ return this->RCITM1; }
double Node::getDPDTITR(){ return this->DPDTITR; }
double Node::getPVEL(){ return this->PVEL; }
double Node::getGNUU1(){ return this->GNUU1; }
double Node::getGNUP1(){ return this->GNUP1; }
double Node::getQPLITR(){ return this->QPLITR; }
double Node::getQINITR(){ return this->QINITR; }