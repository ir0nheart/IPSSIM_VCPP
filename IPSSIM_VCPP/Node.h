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
	void setVOL(double val);
	double getVOL();
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

	void setPBCDef(bool val);
	void setUBCDef(bool val);
	void setQINDef(bool val);
	void setUINDef(bool val);
	void setQUINDef(bool val);
	bool getPBCDef();
	bool getUBCDef();
	bool getQINDef();
	bool getUINDef();
	bool getQUINDef();

	void setCS1(double val);
	void setCS2(double val);
	void setCS3(double val);
	void setSR(double val);
	void setSL(double val);
	void setU(double val);
	void setDSWDP(double val);
	void setSW(double val);
	void setRHO(double val);
	void setRELK(double val);
	void setSWB(double val);
	void setRELKB(double val);
	void setRELKT(double val);
	void setQUIN1(double val);
	void setQIN1(double val);
	void setUIN1(double val);
	void setPBC1(double val);
	void setUBC1(double val);

	double getCS1();
	double getCS2();
	double getCS3();
	double getSR();
	double getSL();
	double getU();
	double getSW();
	double getDSWDP();
	double getRHO();
	double getRELK();
	double getSWB();
	double getRELKB();
	double getRELKT();
	double getQUIN1();
	double getQIN1();
	double getUIN1();
	double getUBC1();
	double getPBC1();
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
	double VOL;
	bool PBCDef, UBCDef, QINDef, UINDef, QUINDef;
	double CS1, CS2, CS3, SL, SR, U;
	double SW, DSWDP;
	double RHO;
	double RELK;
	double SWB;
	double RELKB;
	double RELKT;
	double QUIN1;
	double QIN1;
	double UIN1;
	double PBC1;
	double UBC1;
};

#endif