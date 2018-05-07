#ifndef _NODE
#define _NODE
#include <vector>
#include <iomanip>
#include <iostream>
#include "Layer.h"
#pragma once

class Node
{
public:
	Node();
	Node(int NodeNumber);
	~Node();
	void print_Node_Info();
	
	void setNodeNumber(int NodeNumber); 
	void setNREG(int NREG);
	void setXCoord(double XCoord);
	void setYCoord(double YCoord);
	void setZCoord(double ZCoord);
	void setPorosity(double Porosity);
	void setSOP(double SOP);
	void setQIN(double QIN);
	void setUIN(double UIN);
	void setQUIN(double QUIN);
	void setPBC(double PBC);
	void setUBC(double UBC);
	void setPVEC(double PVEC);
	void setUVEC(double UVEC);
	void setSWT(double SWT);
	void setEFFSTR(double EFFSTR);
	void setEFFSTRI(double EFFSTR);
	void setSTRRAT(double STRRAT);
	
	int getNodeNumber();
	int getNREG();
	double getXCoord();
	double getYCoord();
	double getZCoord();
	double getPorosity();
	double getSOP();
	double getQIN();
	double getUIN();
	double getQUIN();
	double getPBC();
	double getUBC();
	double getPVEC();
	double getSWT();
	double getUVEC();
	double getEFFSTR();
	double getEFFSTRI();
	double getSTRRAT();


	//Copy Constructor
	Node(Node const&rhs);
	Node& operator = (Node const&rhs);
	// Move Constructor
	Node(Node &&rhs);
	Node & operator = (Node &&rhs);
	void setLayer(Layer * lay,int layN);
	Layer * getLayer();
	int getLayerN();
	
	void setTOTSTR(double val);
	void setPOREP(double val);
	double getTOTSTR();
	double getPOREP();
	void setCNUB(double val);
	double getCNUB();
	void setPM(double val);
	void setUM(double val);
	void setCNUBM1(double val);
	double getPM();
	double getUM();
	void setPM1(double val);
	void setUM1(double val);
	void setUM2(double val);
	void setPITER(double val);
	void setUITER(double val);
	void setRCIT(double val);
	void setRCITM1(double val);
	void setDPDTITR(double val);
	void setPVEL(double val);
	double getPM1();
	double getUM1();
	double getUM2();
	double getPITER();
	double getUITER();
	double getRCIT();
	double getRCITM1();
	double getDPDTITR();
	double getPVEL();
	void setGNUP1(double val);
	void setGNUU1(double val);
	void setQPLITR(double val);
	void setQINITR(double val);
	double getGNUP1();
	double getGNUU1();
	double getQPLITR();
	double getQINITR();
	double getCNUBM1();
	int getIBCPBC();
	int getIBCUBC();
	int getIBCSOP();
	int getIBCSOU();
	void setIBCPBC(int val);
	void setIBCUBC(int val);
	void setIBCSOP(int val);
	void setIBCSOU(int val);
private: 
	Layer * layer;
	int layerN;
	int NodeNumber;
	double XCoord;		// X-Coordinate of the Node
	double YCoord;		// Y-Coordinate of the Node
	double ZCoord;		// Z-Coordinate of the Node
	double Porosity;	// Porosity of the Node
	double SOP;			// Specific pressure storavity of the Node
	int NREG;			// Unsaturated flow property region number to which node belongs. Set to any integer value when flow simulation is saturated only
	double QIN;			//
	double UIN;			//
	double QUIN;		//
	double PBC;			//
	double UBC;			//
	double PVEC;
	double UVEC;
	double SWT;
	double EFFSTR;
	double EFFSTRI;
	double STRRAT;
	double TOTSTR;
	double POREP;
	double CNUB;
	double PM, UM;
	double DPDTITR;
	double PM1;
	double PITER;
	double PVEL;
	double UITER;
	double RCITM1;
	double RCIT;
	double UM1;
	double UM2;
	double GNUU1, GNUP1, QPLITR,QINITR,CNUBM1;
	int IBCPBC,IBCUBC,IBCSOP,IBCSOU;
};

#endif