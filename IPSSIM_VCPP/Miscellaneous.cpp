#include "Miscellaneous.h"
#include <iostream>
#include <Windows.h>
#include <iomanip>
#include "Writer.h"
using namespace std;

Miscellaneous::Miscellaneous()
{
}

void Miscellaneous::spacer(){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 255);
	cout << setw(80) << setfill('-') << "" << endl;
}

void Miscellaneous::banner(){
	string logLine = "";
	Writer * logWriter = Writer::Instance();
	for (int i = 0; i < 133; i++){
		logLine.append("*");
	}
	logLine.append("\n\n\n");
	for (int i = 0; i < 2; i++){
		logLine.append(logLine);
	}

	logLine.append("\n\n\n\n\n");
	logLine.append(string(37, ' ') + "II   PPPPPPP     SSS            SSS     II   MMMM      MMMM\n");
	logLine.append(string(37, ' ') + "II   PP    PP  SS   SS        SS   SS   II   MM  MM  MM  MM\n");
	logLine.append(string(37, ' ') + "II   PP    PP  SS             SS        II   MM    MM    MM\n");
	logLine.append(string(37, ' ') + "II   PPPPPPP    SSSSS   ====   SSSSS    II   MM    MM    MM\n");
	logLine.append(string(37, ' ') + "II   PP             SS             SS   II   MM          MM\n");
	logLine.append(string(37, ' ') + "II   PP        SS   SS        SS   SS   II   MM          MM\n");
	logLine.append(string(37, ' ') + "II   PP          SSS            SSS     II   MM          MM\n");
	logLine.append("\n\n\n\n\n\n");
	logLine.append(setLSTLine("N O R T H E A S T E R N   U N I V E R S I T Y"));
	logLine.append("\n\n\n");
	logLine.append(setLSTLine("INDUCED PARTIAL SATURATION SIMULATION"));
	logLine.append("\n\n");
	logLine.append(setLSTLine("-IPSSIM VERSION v1.0-"));
	logLine.append("\n\n\n\n\n\n");
	logWriter->addToWriteContainer(logLine);
	logLine.clear();
	for (int j = 0; j < 3; j++){
		for (int i = 0; i < 133; i++){
		logLine.append("*");
		}
		logLine.append("\n\n\n");
	}
	for (int i = 0; i < 133; i++){
		logLine.append("*");
	}
	logLine.append("\n\n");
	
	logWriter->addToWriteContainer(logLine);
}
string Miscellaneous::setLSTLine(string s){
	int sLen = s.length();
	string tempStr1 = "";
	string tempStr2 = "";
	string tempStr = "";
	int p1, p2;
	p1 = p2 = 0;
	if ((133 - sLen) % 2 == 0){
		p1 = p2 = (133 - sLen) / 2;
	}
	else{
		p1 = (132 - sLen) / 2;
		p2 = p1 + 1;
	}
	tempStr1 = string(p1, ' ');
	tempStr2 = string(p2, ' ');
	tempStr.append(tempStr1 + s + tempStr2);
	return tempStr;

}
Miscellaneous::~Miscellaneous()
{
}
