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
// Getters
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