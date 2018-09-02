#include "DataSet.h"
#include <omp.h>

DataSet::DataSet(string dSName, size_t size) :numberOfLines(size),dataSetName(dSName), Data(new vector<string>[numberOfLines])
{
	// cout << "Data Set " << dataSetName << " is created" << endl;
}

DataSet::~DataSet()
{
	free();
}

void DataSet::free(){
	delete[] Data;
	Data = nullptr;
	return;
}

DataSet::DataSet(DataSet const&other) :numberOfLines(other.numberOfLines), dataSetName(other.dataSetName), Data(new vector<string>[other.numberOfLines]){
	dataSetName = other.dataSetName;
	memcpy(Data, other.Data, numberOfLines);
}

DataSet & DataSet::operator = (DataSet const&other){
	vector<string> *p = new vector<string>[other.numberOfLines];
	free();

	Data = p;
	numberOfLines = other.numberOfLines;
	dataSetName = other.dataSetName;
	memcpy(Data, other.Data, numberOfLines);
	return *this;
}

DataSet::DataSet(DataSet&& other):numberOfLines(other.numberOfLines),dataSetName(other.dataSetName),Data(other.Data){
	other.numberOfLines = 0;
	other.dataSetName = "";
	other.Data = nullptr;
}

DataSet & DataSet::operator=(DataSet&& other){
	delete[] Data;
	numberOfLines = other.numberOfLines;
	dataSetName = other.dataSetName;
	Data = other.Data;
	other.numberOfLines = 0;
	other.dataSetName = "";
	other.Data = nullptr;
	return *this;
}

string DataSet::getDataSetName(){
	return this->dataSetName;
}


void DataSet::addData(string Data){
	this->Data->push_back(Data);
	numberOfLines = this->Data->size();
}



vector<string> * DataSet::getData(){
	return this->Data;
}

void DataSet::parseAllLines(){
	string key = this->dataSetName;
	if (key == "DataSet_1"){
		parseDataSet_1();
	}
	else if (key == "DataSet_2A"){
		parseDataSet_2A();
	}
	else if (key == "DataSet_2B"){
		parseDataSet_2B();
	}
	else if (key == "DataSet_3"){
		parseDataSet_3();
	}
	else if (key == "DataSet_4"){
		parseDataSet_4();
	}
	else if (key == "DataSet_5"){
		parseDataSet_5();
	}
	else if (key == "DataSet_6"){
		parseDataSet_6();
	}
	else if (key == "DataSet_7A"){
		parseDataSet_7A();
	}
	else if (key == "DataSet_7B"){
		parseDataSet_7B();
	}
	else if (key == "DataSet_7C"){
		parseDataSet_7C();
	}
	else if (key == "DataSet_8ABC"){
		parseDataSet_8ABC();
	}
	else if (key == "DataSet_8D"){
		parseDataSet_8D();
	}
	else if (key == "DataSet_8E_9_10_11"){
		parseDataSet_8E_9_10_11();
	}
	else if (key == "DataSet_12_13_14A"){
		parseDataSet_12_13_14A();
	}
	else if (key == "DataSet_14B"){
		parseDataSet_14B();
	}
	else if (key == "DataSet_15A"){
		parseDataSet_15A();
	}
	else if (key == "DataSet_15B"){
		parseDataSet_15B();
	}
	else if (key == "DataSet_17" || key == "DataSet_18"){
		parseDataSet_17_18(key);
	}
	else if (key == "DataSet_19" || key == "DataSet_20"){
		parseDataSet_19_20(key);
	}
	else if (key == "DataSet_22"){
		parseDataSet_22();
	}
	

}

void DataSet::parseDataSet_1(){
	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[0];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map;
	char * str_end;
	for (int i = 0; i < size; i ++){
			if (map[i] == '\r'){
				str_end = map + i- 1;
				line.assign(str_start, str_end);
				lines.push_back(line);
				str_start = map + i + 2;
			}
		}
	ControlParameters::Instance()->setTITLE1(lines[0]);
	ControlParameters::Instance()->setTITLE2(lines[1]);

}
void DataSet::parseDataSet_2A(){
	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[1];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i - 1;
			line.assign(str_start, str_end);
			lines.push_back(line);
			str_start = map + i + 2;
		}
	}

	
	string delimiter = "'";
	size_t pos = 0;
	string token;
	ControlParameters * CP = ControlParameters::Instance();
	while ((pos = line.find(delimiter)) != std::string::npos) {
		token = line.substr(0, pos);
		line.erase(pos, delimiter.length());
	}

	vector<string> dataPos;
	boost::split(dataPos, line, boost::is_any_of("  ,\r"), boost::token_compress_on);
	
	CP->setsSutra(dataPos.at(0));
	CP->setvVersion(dataPos.at(1));
	CP->setvVersionNum(dataPos.at(2));
	CP->setsimulationType(dataPos.at(3));
	CP->setsTransport(dataPos.at(4));

	if (CP->getsSutra() != "SUTRA")
		CP->exitOnError("INP-2A-1");

	if (CP->getvVersionNum() != "2.2"){
		vector<string> CHERR;
		CHERR.push_back(CP->getvVersionNum());
		CHERR.push_back("2.2");
		CP->exitOnError("INP-2A-4", CHERR);
	}

	if (CP->getsimulationType() == "SOLUTE"){
		CP->setME(-1);
	}
	else{
		CP->exitOnError("INP-2A-3");
	}
}

void DataSet::parseDataSet_2B(){
	ControlParameters * CP = ControlParameters::Instance();
	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[2];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i - 1;
			line.assign(str_start, str_end);
			lines.push_back(line);
			str_start = map + i + 2;
		}
	}

	string delimiter = "'";
	size_t pos = 0;
	string token;
	vector<string> dataPos;

	while ((pos = line.find(delimiter)) != std::string::npos) {
		token = line.substr(0, pos);
		line.erase(pos, delimiter.length());
	}

	boost::split(dataPos, line, boost::is_any_of("  ,\r"), boost::token_compress_on);

	CP -> setMSHSTR(dataPos.at(0) + " " + dataPos.at(1));
	if (dataPos.at(0) == "2D"){
		CP->setKTYPE(0, 2);
		CP->setN48(4);
	}
	else if (dataPos.at(0) == "3D"){
		CP->setKTYPE(0,3);
		CP->setN48(8);
	}
	else{
		CP->exitOnError("INP-2B-1");
	}

	if (dataPos.at(1) == "REGULAR"){ // Regular 
		if (CP->getKTYPE(0) == 2){
			CP->setNN1(stoi(dataPos.at(3)));
			CP->setNN2(stoi(dataPos.at(4)));
			CP->setNN3(1);
		}
		else{
			CP->setNN1(stoi(dataPos.at(3)));
			CP->setNN2(stoi(dataPos.at(4)));
			CP->setNN3(stoi(dataPos.at(5)));
		}
		CP->setKTYPE(1, 2);

		if (CP->getNN1() < 2 || CP->getNN2() < 2 || (CP->getKTYPE(0) == 3 && CP->getNN3() < 2)){
			CP->exitOnError("INP-2B-3");
			
		}

	}
	else if (dataPos.at(1) == "IRREGULAR"){
		CP->setKTYPE(1,0);
	}
	else {
		CP->exitOnError("INP-2B-4");
	}
}

void DataSet::parseDataSet_3(){

	
	vector<string> dataPos;
	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[3];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i - 1;
			line.assign(str_start, str_end);
			lines.push_back(line);
			str_start = map + i + 2;
		}
	}

	ControlParameters * CP = ControlParameters::Instance();

	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setNN(stoi(dataPos.at(0)));
	CP->setNE(stoi(dataPos.at(1)));
	CP->setNPBC(stoi(dataPos.at(2)));
	CP->setNUBC(stoi(dataPos.at(3)));
	CP->setNSOP(stoi(dataPos.at(4)));
	CP->setNSOU(stoi(dataPos.at(5)));
	CP->setNOBS(stoi(dataPos.at(6)));

	

	
	//!Add Checks Here!//


}
void DataSet::parseDataSet_4(){
	ControlParameters * CP = ControlParameters::Instance();
	int pos;
	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[4];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i;
			line.assign(str_start, str_end);
			line.push_back('\0');
			lines.push_back(line);
			str_start = map + i +1;
		}
	}
	string delimiter = "'";
	vector<string>dataPos;
	string token;
	pos = 0;

	while ((pos = line.find(delimiter)) != std::string::npos) {
		token = line.substr(0, pos);
		line.erase(pos, delimiter.length());
	}
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	if (dataPos.at(0) == "SATURATED" || dataPos.at(0) == "UNSATURATED"){
		if (dataPos.at(0) == "SATURATED"){
			CP->setCUNSAT("SATURATED");
			CP->setIUNSAT(0);
		}
		else{
			CP->setCUNSAT("UNSATURATED");
			CP->setIUNSAT(1);
		}
	}
	else{
		CP->exitOnError("INP-4-1");
	}

	if (dataPos.at(1) == "TRANSIENT" || dataPos.at(1) == "STEADY"){
		if (dataPos.at(1) == "TRANSIENT"){
			CP->setCSSFLO("TRANSIENT");
			CP->setISSFLO (0);
		}
		else{
			CP->setCSSFLO("STEADY");
			CP->setISSFLO(1);
		}
	}
	else{
		CP->exitOnError("INP-4-2");
	}

	if (dataPos.at(3) == "TRANSIENT" || dataPos.at(3) == "STEADY"){
		if (dataPos.at(3) == "TRANSIENT"){
			CP->setCSSTRA("TRANSIENT");
			CP->setISSTRA(0);
		}
		else{
			CP->setCSSTRA("STEADY");
			CP->setISSTRA(1);
		}
	}
	else{
		CP->exitOnError("INP-4-3");
	}

	if (dataPos.at(5) == "COLD" || dataPos.at(5) == "WARM"){
		if (dataPos.at(5) == "COLD"){
			CP->setCREAD("COLD");
			CP->setIREAD(1);
		}
		else{
			CP->setCREAD("WARM");
			CP->setIREAD(-1);
		}
	}
	else{
		CP->exitOnError("INP-4-4");
	}

	CP->setISTORE(stoi(dataPos.at(6)));


}
void DataSet::parseDataSet_5(){
	ControlParameters * CP = ControlParameters::Instance();

	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[5];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i - 1;
			line.assign(str_start, str_end);
			line.push_back('\0');
			lines.push_back(line);
			str_start = map + i + 2;
		}
	}

	vector<string>dataPos;
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setUP(stod(dataPos.at(0)));
	CP->setGNUP(stod(dataPos.at(1)));
	CP->setGNUU(stod(dataPos.at(2)));

}
void DataSet::parseDataSet_6(){
	// ! Here Reads ICS file if Cold Start or WARM
	Writer * logWriter = Writer::LSTInstance();
	string logLine = "";
	logLine.append(string(11, ' ') + "T E M P O R A L   C O N T R O L   A N D   S O L U T I O N   C Y C L I N G   D A T A\n\n");

	char * map = InputFileParser::Instance()->mapViewOfICSFile;
	char * start_str = map;
	char * end_str;
	int size = strlen(map);
	vector<char>lineBuffer;
	const char* del = " ";
	double TICS,TICS0;
	double DELTP, DELTU;
	int ITRST;
	if (ControlParameters::Instance()->getIREAD() == 1){
		for (int j = 0; j < size; j++){
			if (map[j] == '\r'){
				end_str = map + j - 1;
				lineBuffer.assign(start_str, end_str);
				lineBuffer.push_back('\0');
				start_str = map + j + 2;
				TICS = stod(strtok(lineBuffer.data(), del));
				break;
			}
		}
	}
	else{			
		for (int j = 0; j < size; j++){
			if (map[j] == '\r'){
				end_str = map + j - 1;
				lineBuffer.assign(start_str, end_str);
				lineBuffer.push_back('\0');
				start_str = map + j + 2;
				TICS = stod(strtok(lineBuffer.data(), del));
				DELTP = stod(strtok(NULL, del));
				DELTU = stod(strtok(NULL, del));
				ITRST = stoi(strtok(NULL, del));
				TICS0 = stod(strtok(NULL, del));
				break;
			}
		}
		
	}
	// Add Code Here
	int ISCHTS = 0;
	ControlParameters * CP = ControlParameters::Instance();
	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[6];
	size = dataSetPosition.second - dataSetPosition.first;
	map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	CP->setNSCHAU(3);
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\n'){
			str_end = map + i;
			line.assign(str_start, str_end);
			lines.push_back(line);
			str_start = map + i + 1;
		}
	}
	line = lines[0];
	vector<string> dataPos;
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setNSCH(stoi(dataPos.at(0)));
	CP->setNPCYC(stoi(dataPos.at(1)));
	CP->setNUCYC(stoi(dataPos.at(2)));

	if (CP->getNSCH() < 0){
		CP->exitOnError("INP-6-8");
	}
	else if (CP->getNSCH() == 0){
		if (CP->getISSTRA() == 0){
			CP->exitOnError("INP-6-13");
		}
		CP->setNPCYC(1);
		CP->setNUCYC(1);
	}

	if (CP->getNPCYC() < 1 || CP->getNUCYC() < 1){
		CP->exitOnError("INP-6-1");
	}
	else if (CP->getNPCYC() != 1 && CP->getNUCYC() != 1){
		CP->exitOnError("INP-6-2");
	}

	if (CP->getISSTRA() == 1){
		CP->setTSTART(TICS);
		double DELT = max((1 - abs(TICS)), 1.0);
		CP->setDELT(DELT);
		CP->setNSCH(1 + CP->getNSCHAU());
		double TIME = TICS;
		double STEP = 0.0;
		Schedule * timeStep = new Schedule("TIME_STEP");
		Schedule * step_0 = new Schedule("STEP_0");
		Schedule * step_1 = new Schedule("STEP_1");
		Schedule * step_1andUP = new Schedule("STEP_1&UP");
		timeStep->addToSList(TIME, STEP);
		step_0->addToSList(TIME, STEP);
		TIME = TIME + DELT;
		STEP = 1.0;
		
		timeStep->addToSList(TIME, STEP);
		step_1->addToSList(TIME, STEP);
		step_1andUP->addToSList(TIME, STEP);
		CP->setITMAX(1);
		logLine.append(string(13, ' ') + "NOTE: BECAUSE FLOW AND TRANSPORT ARE STEADY-STATE, USER-DEFINED SCHEDULES ARE NOT IN EFFECT.\n");
		logLine.append(string(13, ' ') + "STEADY-STATE RESULTS WILL BE WRITTEN TO THE APPROPRIATE OUTPUT FILES.\n");
		char buff[512];
		_snprintf(buff, sizeof(buff), "             THE FOLLOWING %1d SCHEDULES CAN BE USED TO CONTROL SPECIFICATION OF STEADY-STATE BOUNDARY\n",CP->getNSCH());
		logLine.append(buff);
		logLine.append(string(13, ' ') + "CONDITIONS IN (OPTIONAL) .BCS FILES:\n");
		logLine.append(string(16, ' ') + "SCHEDULE \"TIME_STEPS\"   CONSISTS OF TIME STEPS 0 (STEADY FLOW) AND 1 (STEADY TRANSPORT);\n");
		logLine.append(string(41, ' ') + "THIS SCHEDULE IS DEFINED AUTOMATICALLY BY IPSSIM\n\n");
		logLine.append(string(16, ' ') + "SCHEDULE \"STEPS_1&UP\"   IDENTICAL TO SCHEDULE \"TIME_STEPS\", EXCEPT THAT IT OMITS TIME STEP 0;\n");
		logLine.append(string(41, ' ') + "THIS SCHEDULE IS DEFINED AUTOMATICALLY BY IPSSIM\n");
		logLine.append(string(16, ' ') + "SCHEDULE \"STEP_0\"    CONSISTS ONLY TIME STEP 0;\n\n");
		logLine.append(string(41, ' ') + "THIS SCHEDULE IS DEFINED AUTOMATICALLY BY IPSSIM\n\n");
		logLine.append(string(16, ' ') + "SCHEDULE \"STEP_1\"    CONSISTS ONLY TIME STEP 1;\n");
		logLine.append(string(41, ' ') + "THIS SCHEDULE IS DEFINED AUTOMATICALLY BY IPSSIM\n\n");

		logLine.append(string(13, ' ') + "SOLUTION CYCLING DATA:\n\n");
		_snprintf(buff, sizeof(buff), "           %15d     FLOW SOLUTION CYCLE (IN TIME STEPS)\n", CP->getNPCYC());
		logLine.append(buff);
		_snprintf(buff, sizeof(buff), "           %15d     TRANSPORT SOLUTION CYCLE (IN TIME STEPS)\n\n", CP->getNUCYC());
		logLine.append(buff);
		logLine.append(string(13, ' ') + "FLOW AND TRANSPORT SOLUTIONS ARE ALSO COMPUTED AUTOMATICALLY ON TIME STEPS ON WHICH FLOW-RELATED\n");
		logLine.append(string(13, ' ') + "AND TRANSPORT-RELATED BOUNDARY CONDITIONS, RESPECTIVELY, ARE SET IN (OPTIONAL) BCS FILES.\n");
		if (CP->getISSFLO() == 1){
			CP->setNPCYC(CP->getITMAX() + 1);
			CP->setNUCYC(1);
		}
		logWriter->writeContainer.push_back(logLine);
		logLine.clear();
	}
	else{
		CP->setNSCH(CP->getNSCH() + CP->getNSCHAU());
		char buff[512];
		_snprintf(buff, sizeof(buff), "             THE %5d SCHEDULES ARE LISTED BELOW. SCHEDULE \"TIME_STEPS\" CONTROLS TIME STEPPING.\n", CP->getNSCH());
		logLine.append(buff);
		logWriter->writeContainer.push_back(logLine);
		logLine.clear();

		for (int i = 1; i <= CP->getNSCH()-3; i++){
			line = lines[i];
			dataPos.clear();
			int pos = 0;
			string delimiter = "'";
			string token;
			while ((pos = line.find(delimiter)) != std::string::npos) {
				token = line.substr(0, pos);
				line.erase(pos, delimiter.length());
			}
			boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
			string schName = dataPos.at(0);
			string schType = dataPos.at(1) + " " + dataPos.at(2);
			if (schName == "STEP_0" || schName == "STEP_1" || schName == "STEP_1&UP"){

				vector<string> CHERR;
				CHERR.push_back(schName);
				CP->exitOnError("INP-6-11", CHERR);
			}
			// check for same schedule name;
			if (schType == "TIME CYCLE"){
				TimeCycleSch* sch = new TimeCycleSch();
				sch->setScheduleName(schName);
				sch->setScheduleType("TIME CYCLE");
				sch->setSBASED(false);
				sch->setCREFT(dataPos.at(3));
				sch->setSCALT(stod(dataPos.at(4)));
				sch->setNTMAX(stoi(dataPos.at(5)));
				sch->setTIMEI(stod(dataPos.at(6)));
				sch->setTIMEL(stod(dataPos.at(7)));
				sch->setTIMEC(stod(dataPos.at(8)));
				sch->setNTCYC(stoi(dataPos.at(9)));
				sch->setTCMULT(stod(dataPos.at(10)));
				sch->setTCMIN(stod(dataPos.at(11)));
				sch->setTCMAX(stod(dataPos.at(12)));

				if (sch->getCREFT() == "ELAPSED"){
					if (sch->getScheduleName() == "TIME_STEPS" && sch->getTIMEI() != 0){
						CP->exitOnError("INP-6-7");
					}
					sch->setELAPSED(true);
				}
				else if (sch->getCREFT() == "ABSOLUTE"){
					sch->setELAPSED(false);
				}
				else{
					vector<string>CHERR;
					CHERR.push_back("CREFT");
					CP->exitOnError("INP-6-6", CHERR);
				}
				CP->addToListOfSchedules(sch);
				sch->createTimeList();

				
				logLine.append(string(16, ' '));
				logLine.append("SCHEDULE " + sch->getScheduleName() + "   TIME CYCLE WITH THE FOLLOWING SPECIFICATIONS IN TERMS OF" + sch->getCREFT() + " TIMES:\n");
				logLine.append(string(46, ' '));
				_snprintf(buff, sizeof(buff), "%8d     MAXIMUM NUMBER OF TIMES AFTER INITIAL TIME\n", sch->getNTMAX());
				logLine.append(buff);
				logLine.append(string(39, ' '));
				_snprintf(buff, sizeof(buff), "%15.7e     INITIAL TIME\n", sch->getTIMEI());
				logLine.append(buff);
				logLine.append(string(39, ' '));
				_snprintf(buff, sizeof(buff), "%15.7e     LIMITING TIME\n", sch->getTIMEL());
				logLine.append(buff);
				logLine.append(string(39, ' '));
				_snprintf(buff, sizeof(buff), "%15.7e     INITIAL TIME INCREMENT\n", sch->getTIMEC());
				logLine.append(buff);
				logLine.append(string(39, ' '));
				_snprintf(buff, sizeof(buff), "%15d     TIME INCREMENT CHANGE CYCLE\n", sch->getNTCYC());
				logLine.append(buff);
				logLine.append(string(39, ' '));
				_snprintf(buff, sizeof(buff), "%15.7e     TIME INCREMENT MULTIPLIER\n", sch->getTCMULT());
				logLine.append(buff);
				logLine.append(string(39, ' '));
				_snprintf(buff, sizeof(buff), "%15.7e     MINIMUM TIME INCREMENT\n", sch->getTCMIN());
				logLine.append(buff);
				logLine.append(string(39, ' '));
				_snprintf(buff, sizeof(buff), "%15.7e     MAXIMUM TIME INCREMENT\n", sch->getTCMAX());
				logLine.append(buff);
				logWriter->writeContainer.push_back(logLine);
				logLine.clear();
				/*

				  WRITE(K3,732) SCHDLS(I)%NAME, TRIM(CREFT), NTMAX, TIMEI,     INDAT0.......38900
				  1          TIMEL, TIMEC, NTCYC, TCMULT, TCMIN, TCMAX                INDAT0.......39000
				  732       FORMAT(/16X,'SCHEDULE ',A, 3X,'TIME CYCLE WITH THE ',        INDAT0.......39100
				  1         'FOLLOWING SPECIFICATIONS IN TERMS OF ', A, ' TIMES:'     INDAT0.......39200
				  2         /46X, I8, 5X, 'MAXIMUM NUMBER OF TIMES AFTER ',           INDAT0.......39300
				  3            'INITIAL TIME'                                         INDAT0.......39400
				  4         /39X, 1PE15.7, 5X, 'INITIAL TIME'                         INDAT0.......39500
				  5         /39X, 1PE15.7, 5X, 'LIMITING TIME'                        INDAT0.......39600
				  6         /39X, 1PE15.7, 5X, 'INITIAL TIME INCREMENT'               INDAT0.......39700
				  7         /46X, I8, 5X, 'TIME INCREMENT CHANGE CYCLE '              INDAT0.......39800
				  8         /39X, 1PE15.7, 5X, 'TIME INCREMENT MULTIPLIER'            INDAT0.......39900
				  9         /39X, 1PE15.7, 5X, 'MINIMUM TIME INCREMENT'               INDAT0.......40000
				  1         /39X, 1PE15.7, 5X, 'MAXIMUM TIME INCREMENT')   */

			}
			else if (schType == "TIME LIST"){
				TimeListSch* sch = new TimeListSch();
				sch->setScheduleName(schName);
				sch->setScheduleType("TIME LIST");
				sch->setSBASED(false);
				sch->setCREFT(dataPos.at(3));
				sch->setSCALT(stod(dataPos.at(4)));
				sch->setNTLIST(stod(dataPos.at(5)));
				for (int i = 0; i < sch->getNTLIST(); i++){
					sch->addToTLIST(stod(dataPos.at(6 + i)));
				}

				if (sch->getCREFT() == "ELAPSED"){
					if (sch->getScheduleName() == "TIME_STEPS" && sch->getTLIST().at(0) != 0){
						CP->exitOnError("INP-6-7");
					}
					sch->setELAPSED(true);
				}
				else if (sch->getCREFT() == "ABSOLUTE"){
					sch->setELAPSED(false);
				}
				else{
					vector<string>CHERR;
					CHERR.push_back("CREFT");
					CP->exitOnError("INP-6-6", CHERR);
				}


				sch->createTimeList();
				CP->addToListOfSchedules(sch);
				logLine.append(string(16, ' '));
				logLine.append("SCHEDULE " + sch->getScheduleName() + "   TIME LIST THAT INCLUDES THE FOLLOWING " + sch->getCREFT() + " TIMES (SEC):\n");
				
				int s = sch->getSList().size();
				int i = 0;
				while (i < s){
					logLine.append(string(38, ' '));
					if (s - i >= 4){
						for (int j = i; j < i + 4; j++){
							_snprintf(buff, sizeof(buff), " %15.7e", sch->getSList()[j].first);
							logLine.append(buff);
						}
						i = i + 4;
					}
					else{
						for (int j = i; j < s - i; j++){
							_snprintf(buff, sizeof(buff), " %15.7e", sch->getSList()[j].first);
							logLine.append(buff);
						}
						i = s;
					}
					logLine.append("\n");
				}
				logWriter->writeContainer.push_back(logLine);
				logLine.clear();
			
		

				


			}
			else if (schType == "STEP LIST"){
				StepListSch* sch = new StepListSch();
				sch->setScheduleName(schName);
				sch->setScheduleType("STEP LIST");
				sch->setSBASED(true);
				sch->setELAPSED(false);
				sch->setNSLIST(stoi(dataPos.at(3)));
				for (int i = 0; i < sch->getNSLIST(); i++){
					sch->addToISLIST(stoi(dataPos.at(4 + i)));
				}

				sch->createTimeList();

				CP->addToListOfSchedules(sch);
				logLine.append(string(16, ' '));
				logLine.append("SCHEDULE "+sch->getScheduleName()+"   STEP LIST THAT INCLUDES THE FOLLOWING TIME STEPS:\n");
				
				
				int s = sch->getSList().size();
				int ix = 0;
				while (ix < s){
					logLine.append(string(38, ' '));
					if (s - ix > 8){
						for (int j = ix; j < (ix + 8); j++){
							pair<double, double> p = sch->getSList()[j];
							_snprintf(buff, sizeof(buff), "  %8d", (int)p.first);
							logLine.append(buff);
						}
						ix = ix + 8;
					}
					else{
						for (int j = ix; j < s; j++){
							_snprintf(buff, sizeof(buff), "  %8d", (int)(sch->getSList()[j]).first);
							logLine.append(buff);
						}
						ix = s;
					}
					logLine.append("\n");
				}
				logWriter->writeContainer.push_back(logLine);
				logLine.clear();
				/*WRITE(K3,742) SCHDLS(I)%NAME, (ISLIST(NS),NS=1,NSLIST)       INDAT0.......42800
  742       FORMAT(/16X,'SCHEDULE ',A, 3X,'STEP LIST THAT INCLUDES ',    INDAT0.......42900
     1         'THE FOLLOWING TIME STEPS:'/:(38X,8(2X,I8)))   */
			

			}
			else if (schType == "STEP CYCLE"){
				StepCycleSch* sch = new StepCycleSch();
				sch->setScheduleName(schName);
				sch->setScheduleType("STEP CYCLE");
				sch->setSBASED(true);
				sch->setELAPSED(false);
				sch->setNSMAX(stoi(dataPos.at(3)));
				sch->setISTEPI(stod(dataPos.at(4)));
				sch->setISTEPL(stod(dataPos.at(5)));
				sch->setISTEPC(stod(dataPos.at(6)));
				sch->createTimeList();

				CP->addToListOfSchedules(sch);
				logLine.append(string(16, ' '));
				logLine.append("SCHEDULE "+sch->getScheduleName()+"   STEP CYCLE WITH THE FOLLOWING SPECIFICATIONS.\n");
				
				logLine.append(string(40, ' '));
				_snprintf(buff, sizeof(buff), "%8d     MAXIMUM NUMBER OF TIME STEPS AFTER INITIAL TIME STEP NUMBER\n", sch->getNSMAX());
				logLine.append(buff);
				logLine.append(string(40, ' '));
				_snprintf(buff, sizeof(buff), "%8d     INITIAL TIME STEP NUMBER\n", sch->getISTEPI());
				logLine.append(buff);
				logLine.append(string(40, ' '));
				_snprintf(buff, sizeof(buff), "%8d     LIMITING STEP NUMBER\n", sch->getISTEPL());
				logLine.append(buff);
				logLine.append(string(40, ' '));
				_snprintf(buff, sizeof(buff), "%8d     TIME STEP INCREMENT\n", sch->getISTEPC());
				logLine.append(buff);
				logWriter->writeContainer.push_back(logLine);
				logLine.clear();
			}
			else{
				CP->exitOnError("INP-");
			}


		}
	

	vector<Schedule *> vS = CP->getListOfSchedules();
	int ind = -1;
	for (int i = 0; i < vS.size(); i++){
		if (vS[i]->getScheduleName() == "TIME_STEPS"){
			ind = i;
			break;
		}
	}

	TimeCycleSch * TIME_STEPS = (TimeCycleSch*)(vS[ind]);
	double TREF = 0.0;
	if (TIME_STEPS->getCREFT() == "ELAPSED"){
		if (CP->getIREAD() == 1){
			TREF = TICS;
		}
		else{
			TREF = TICS0;
		}
	}



	// Define STEPS_1&UP
	TimeCycleSch * step_1andUP = new TimeCycleSch();
	//TimeCycleSch * TIME_STEPS = CP->getTIMESTEPSSchedule();
	step_1andUP->setScheduleName("STEP_1&UP");
	step_1andUP->setScheduleType("TIME CYCLE");
	for (int i = 1; i < TIME_STEPS->getSList().size(); i++){
		double TIME = TREF + TIME_STEPS->getSList()[i].first;
		double NSTEP = i;
		TIME_STEPS->changeListElement(i, TIME, NSTEP);

		step_1andUP->addToSList(TIME_STEPS->getSList()[i].first, TIME_STEPS->getSList()[i].second);
	
		
	}
		CP->addToListOfSchedules(step_1andUP);
		int lastind = TIME_STEPS->getSList().size() - 1;
		CP->setTSTART(TREF + TIME_STEPS->getSList()[0].first);
		CP->setTFINISH(TREF + TIME_STEPS->getSList()[lastind].first);
		CP->setITMAX(TIME_STEPS->getSList().size() - 1);
		CP->setDELT(TIME_STEPS->getSList()[1].first - TIME_STEPS->getSList()[0].first);
	double TIME;
	//define STEP_0
		TimeCycleSch * step_0 = new TimeCycleSch();
		step_0->setScheduleName("STEP_0");
		step_0->setScheduleType("TIME CYCLE");
		step_0->addToSList(0, 0);
		CP->addToListOfSchedules(step_0);

	//define STEP_1
		TimeCycleSch * step_1 = new TimeCycleSch();
		step_1->setScheduleName("STEP_1");
		step_1->setScheduleType("TIME CYCLE");
		TIME = step_1->TIMETS(1);
		step_1->addToSList(TIME, 1);
		CP->addToListOfSchedules(step_1);


	// FILL IN TIMES OR STEPS
		for (int i = 0; i < CP->getListOfSchedules().size()- 3; i++){
			if (CP->getListOfSchedules()[i]->getScheduleName() == "TIME_STEPS")
				continue;

			if (CP->getListOfSchedules()[i]->getELAPSED()){
				TREF = CP->getTIMESTEPSSchedule()->getSList()[0].first;
			}
			else{
				TREF = 0.0;
			}

			
			CP->getListOfSchedules()[i]->reConstructSList(TREF);
		

		}

		logLine.append(string(16, ' ') + "SCHEDULE \"STEPS_1&UP\"   IDENTICAL TO SCHEDULE \"TIME_STEPS\", EXCEPT THAT IT OMITS TIME STEP 0;\n");
		logLine.append(string(41, ' ') + "THIS SCHEDULE IS DEFINED AUTOMATICALLY BY IPSSIM\n\n");
		logLine.append(string(16, ' ') + "SCHEDULE \"STEP_0\"       CONSISTS ONLY TIME STEP 0;\n");
		logLine.append(string(41, ' ') + "THIS SCHEDULE IS DEFINED AUTOMATICALLY BY IPSSIM\n\n");
		logLine.append(string(16, ' ') + "SCHEDULE \"STEP_1\"       CONSISTS ONLY TIME STEP 1;\n");
		logLine.append(string(41, ' ') + "THIS SCHEDULE IS DEFINED AUTOMATICALLY BY IPSSIM\n\n");

		logLine.append(string(13, ' ') + "SOLUTION CYCLING DATA:\n\n");
		_snprintf(buff, sizeof(buff), "           %15d     FLOW SOLUTION CYCLE (IN TIME STEPS)\n", CP->getNPCYC());
		logLine.append(buff);
		_snprintf(buff, sizeof(buff), "           %15d     TRANSPORT SOLUTION CYCLE (IN TIME STEPS)\n\n", CP->getNUCYC());
		logLine.append(buff);
		logLine.append(string(13, ' ') + "FLOW AND TRANSPORT SOLUTIONS ARE ALSO COMPUTED AUTOMATICALLY ON TIME STEPS ON WHICH FLOW-RELATED\n");
		logLine.append(string(13, ' ') + "AND TRANSPORT-RELATED BOUNDARY CONDITIONS, RESPECTIVELY, ARE SET IN (OPTIONAL) BCS FILES.\n");
		if (CP->getISSFLO() == 1){
			CP->setNPCYC(CP->getITMAX() + 1);
			CP->setNUCYC(1);
		}
		logWriter->writeContainer.push_back(logLine);
		logLine.clear();

}
}
void DataSet::parseDataSet_7A(){
	ControlParameters * CP = ControlParameters::Instance();
	Writer * logWriter = Writer::LSTInstance();
	string logLine = "";
	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[7];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i - 1;
			line.assign(str_start, str_end);
			lines.push_back(line);
			str_start = map + i + 2;
		}
	}
	line = lines[0];
	vector<string> dataPos;

	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setITRMAX(stoi(dataPos.at(0)));
	line.clear();
	if (CP->getITRMAX() > 1){
		CP->setRPMAX(stoi(dataPos.at(1)));
		CP->setRUMAX(stoi(dataPos.at(2)));
	}
	dataPos.clear();

	if ((CP->getITRMAX() - 1) <= 0){
		logLine.append("\n\n\n\n           I T E R A T I O N   C O N T R O L   D A T A\n\n");
		logLine.append("              NON-ITEREATIOVE SOLUTION\n"); 

	}
	else{
		logLine.append("\n\n\n\n           I T E R A T I O N   C O N T R O L   D A T A\n\n");
		logLine.append("           " + to_string(CP->getITRMAX()) +  "     MAXIMUM NUMBER OF ITERATIONS PER TIME STEP\n");
		logLine.append("           " + to_string(CP->getRPMAX()) + "     ABSOLUTE CONVERGENCE CRITERION FOR FLOW SOLUTION\n");
		logLine.append("           " + to_string(CP->getRUMAX()) + "     ABSOLUTE CONVERGENCE CRITERION FOR TRANSPORT SOLUTION");
	}

	logWriter->writeContainer.push_back(logLine);
}
void DataSet::parseDataSet_7B(){
	ControlParameters * CP = ControlParameters::Instance();
	vector<string> dataPos;
	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[8];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i - 1;
			line.assign(str_start, str_end);
			lines.push_back(line);
			str_start = map + i + 2;
		}
	}
	line = lines[0];
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setCSOLVP(dataPos.at(0));
	if (CP->getCSOLVP() != CP->SOLWRD[0]){
		CP->setITRMXP(stoi(dataPos.at(1)));
		CP->setTOLP(stod(dataPos.at(2)));
	}
	dataPos.clear();

}
void DataSet::parseDataSet_7C(){
	Writer * logWriter = Writer::LSTInstance();
	string logLine = "";
	ControlParameters * CP = ControlParameters::Instance();
	vector<string> dataPos;
	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[9];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char buff[512];
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i - 1;
			line.assign(str_start, str_end);
			lines.push_back(line);
			str_start = map + i + 2;
		}
	}
	line = lines[0];
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setCSOLVU(dataPos.at(0));
	if (CP->getCSOLVU() != CP->SOLWRD[0]){
		CP->setITRMXU (stoi(dataPos.at(1)));
		CP->setTOLU(stod(dataPos.at(2)));
	}
	dataPos.clear();
	CP->setKSOLVP(-1);
	CP->setKSOLVU(-1);
	for (int i = 0; i < 4; i++){
		if (CP->getCSOLVP() == CP->SOLWRD[i])
			CP->setKSOLVP(i);
		if (CP->getCSOLVU() == CP->SOLWRD[i])
			CP->setKSOLVU(i);
	}

	if (CP->getKSOLVP() < 0 || CP->getKSOLVU() < 0){
		CP->exitOnError("INP-7B&C-1");
	}
	else if (((CP->getKSOLVP()*CP->getKSOLVU()) == 0) && ((CP->getKSOLVP() + CP->getKSOLVU()) != 0)){
		CP->exitOnError("INP-7B&C-2");
	}
	else if (CP->getKSOLVU() == 1 || CP->getKSOLVP() == 1 && CP->getUP() != 0){
		CP->exitOnError("INP-7B&C-3");
	}

	if (CP->getKSOLVP() == 2)
		CP->setITOLP(0);
	else
		CP->setITOLP(1);
	if (CP->getKSOLVU() == 2)
		CP->setITOLU(0);
	else
		CP->setITOLU(1);

	CP->setNSAVEP(10);
	CP->setNSAVEU(10);

	logLine.append("\n\n\n\n           S O L V E R - R E L A T E D   P A R A M E T E R S");
	if (CP->getKSOLVP() != 0){
		logLine.append("\n             SOLVER FOR P : " +CP->SOLNAM[CP->getKSOLVP()]);
		
		_snprintf(buff, sizeof(buff), "\n                    %15d     MAXIMUM NUMBER OF MATRIX SOLVER ITERATIONS DURING P SOLUTION\n", CP->getITRMXP());
		logLine.append(buff);
		_snprintf(buff, sizeof(buff), "                    %15.4e     CONVERGENCE TOLERANCE FOR MATRIX SOLVER ITERATIONS DURING P SOLUTION\n", CP->getTOLP());
		logLine.append(buff);
		logLine.append("\n             SOLVER FOR U : " + CP->SOLNAM[CP->getKSOLVU()]);
		_snprintf(buff, sizeof(buff), "\n                    %15d     MAXIMUM NUMBER OF MATRIX SOLVER ITERATIONS DURING U SOLUTION\n", CP->getITRMXU());
		logLine.append(buff);
		_snprintf(buff, sizeof(buff), "                    %15.4e     CONVERGENCE TOLERANCE FOR MATRIX SOLVER ITERATIONS DURING U SOLUTION\n", CP->getTOLU());
		logLine.append(buff);

	}
	else{
		logLine.append("\n             SOLVER FOR P AND U : " + CP->SOLNAM[CP->getKSOLVP()]);

	}
	logWriter->writeContainer.push_back(logLine);

	// SET DIMENSIONS FOR DIRECT SOLVER ? IS IT REALLY REQUIRED HERE DON'T KNOW

	if (CP->getKSOLVP() == 0){
		CP->setNNNX(1);
		CP->setNDIMJA(1);
		CP->setNNVEC(CP->getNN());
	}
	else{
		CP->setNNNX(CP->getNN());
		CP->setNDIMJA(CP->getNN() + 1);
		CP->setNNVEC(CP->getNN());
	}


	CP->setNBCN(CP->getNPBC() + CP->getNUBC() + 1);
	CP->setNSOP(CP->getNSOP() + 1);
	CP->setNSOU(CP->getNSOU() + 1);
	CP->setNOBSN(CP->getNOBS() + 1);
	if (CP->getKTYPE(0) == 3){
		CP->setN48(8);
		CP->setNEX(CP->getNE());
	}
	else{
		CP->setN48(4);
		CP->setNEX(1);
	}
	CP->setNIN(CP->getNE()*CP->getN48());

}
void DataSet::parseDataSet_8ABC(){
	Writer * logWriter = Writer::LSTInstance();
	string logLine = "";
	ControlParameters * CP = ControlParameters::Instance();
	vector<string> dataPos;
	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[10];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i - 1;
			line.assign(str_start, str_end);
			lines.push_back(line);
			str_start = map + i + 2;
		}
	}
	line = lines[0];
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setNPRINT(stoi(dataPos.at(0)));
	CP->setCNODAL(dataPos.at(1));
	CP->setCELMNT(dataPos.at(2));
	CP->setCINCID(dataPos.at(3));
	CP->setCPANDS(dataPos.at(4));
	CP->setCVEL(dataPos.at(5));
	CP->setCCORT(dataPos.at(6));
	CP->setCBUDG(dataPos.at(7));
	CP->setCSCRN(dataPos.at(8));
	CP->setCPAUSE(dataPos.at(9));
	if (CP->getCNODAL() == "'Y'"){
		CP->setKNODAL(1);
	}
	else if (CP->getCNODAL() == "'N'"){
		CP->setKNODAL(0);
	}
	else{
		CP->exitOnError("INP-8A-1");
	}
	if (CP->getCELMNT() == "'Y'"){
		CP->setKELMNT(1);
	}
	else if (CP->getCELMNT() == "'N'"){
		CP->setKELMNT(0);
	}
	else{
		CP->exitOnError("INP-8A-2");
	}
	if (CP->getCINCID() == "'Y'"){
		CP->setKINCID(1);
	
	}
	else if (CP->getCINCID() == "'N'"){
		CP->setKINCID(0);
	}
	else{
		CP->exitOnError("INP-8A-3");
	}
	if (CP->getCPANDS() == "'Y'"){
		CP->setKPANDS(1);
	}
	else if (CP->getCPANDS() == "'N'"){
		CP->setKPANDS(0);
	}
	else{
		CP->exitOnError("INP-8A-8");
	}
	if (CP->getCVEL() == "'Y'"){
		CP->setKVEL(1);
	}
	else if (CP->getCVEL() == "'N'"){
		CP->setKVEL(0);
	}
	else{
		CP->exitOnError("INP-8A-4");
	}
	if (CP->getCCORT() == "'Y'"){
		CP->setKCORT(1);
	}
	else if (CP->getCCORT() == "'N'"){
		CP->setKCORT(0);
	}
	else{
		CP->exitOnError("INP-8A-9");
	}
	if (CP->getCBUDG() == "'Y'"){
		CP->setKBUDG(1);
	}
	else if (CP->getCBUDG() == "'N'"){
		CP->setKBUDG(0);
	}
	else{
		CP->exitOnError("INP-8A-5");
	}
	if (CP->getCSCRN() == "'Y'"){
		CP->setKSCRN(1);
	}
	else if (CP->getCSCRN() == "'N'"){
		CP->setKSCRN(0);
	}
	else{
		CP->exitOnError("INP-8A-6");
	}
	if (CP->getCPAUSE() == "'Y'"){
		CP->setKPAUSE(1);
	}
	else if (CP->getCPAUSE() == "'N'"){
		CP->setKPAUSE(0);
	}
	else{
		CP->exitOnError("INP-8A-7");
	}
	logLine.append("\n\n\n\n           O U T P U T   C O N T R O L S   A N D   O P T I O N S\n");
		logLine.append("\n\n             .LST FILE\n             ---------\n\n");
		logLine.append("            " + to_string(CP->getNPRINT()) + "   PRINTED OUTPUT CYCLE (IN TIME STEPS)\n\n");
	if (CP->getKNODAL() == 1){
		logLine.append("            - PRINT NODE COORDINATES, THICKNESSES AND POROSITIES\n");
	}
	else{
		logLine.append("            - CANCEL PRINT OF NODE COORDINATES, THICKNESSES AND POROSITIES\n");
	}
	if (CP->getKELMNT() == 1){
		logLine.append("            - PRINT ELEMENT PERMEABILITIES AND DISPERSIVITIES\n");
	}
	else{
		logLine.append("            - CANCEL PRINT OF ELEMENT PERMEABILITIES AND DISPERSIVITIES\n");
	}
	if (CP->getKINCID() == 1){
		logLine.append("            - PRINT NODE INCIDENCES IN EACH ELEMENT\n\n");
	}
	else{
		logLine.append("            - CANCEL PRINT OF NODE INCIDENCES IN EACH ELEMENT\n\n");
	}
	if (CP->getKPANDS() == 1){
		logLine.append("            - PRINT PRESSURES AND SATURATIONS AT NODES ON EACH TIME STEP WITH OUTPUT\n");
	}
	else{
		logLine.append("            - CANCEL PRINT OF PRESSURES AND SATURATIONS\n");
	}
	if (CP->getKVEL() == 1){
		logLine.append("            - CALCULATE AND PRINT VELOCITIES AT ELEMENT CENTROIDS ON EACH TIME STEP WITH OUTPUT\n");
	}
	else{
		logLine.append("            - CANCEL PRINT OF VELOCITIES\n");
	}
	if (CP->getKCORT() == 1){
		logLine.append("            - PRINT CONCENTRATIONS AT NODES ON EACH TIME STEP WITH OUTPUT\n\n");
	}
	else{
		logLine.append("            - CANCEL PRINT OF CONCENTRATIONS\n\n");
	}
	if (CP->getKBUDG() == 1){
		logLine.append("            - CALCULATE AND PRINT FLUID AND SOLUTE BUDGETS ON EACH TIME STEP WITH OUTPUT\n");
	}
	else{
		logLine.append("            - CANCEL PRINT OF BUDGETS\n");
	}

	logWriter->writeContainer.push_back(logLine);
	logLine.clear();
	// INPUT (*Data) SET 8B OUTPUT CONTROLS AND OPTIONS FOR NOD FILE
	line = lines[1];
	dataPos.clear();
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setNCOLPR (stoi(dataPos.at(0)));
	int i = 1;
	while (dataPos.at(i) != "'-'"){
		CP->addToNCOL(dataPos.at(i));
		i++;
	}
	CP->setNCOLMX(CP->getNCOL().size());
	logLine.append( "\n\n             .NOD FILE\n             ---------\n\n");
	char buff[512];
	_snprintf(buff, sizeof(buff), "             %8d   PRINTED OUTPUT CYCLE (IN TIME STEPS)\n\n", CP->getNCOLPR());
	logLine.append(buff);
	for (int i = 0; i < CP->getNCOLMX(); i++){
		logLine.append("             COLUMN " + to_string(i + 1) + ":  " + CP->getNCOL()[i] +"\n");
		

	}


	line = lines[2];
	dataPos.clear();
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setLCOLPR(stoi(dataPos.at(0)));
	i = 1;
	while (dataPos.at(i) != "'-'"){
		CP->addToLCOL(dataPos.at(i));
		i++;
	}
	CP->setLCOLMX(CP->getLCOL().size());
	logLine.append("\n\n             .ELE FILE\n             ---------\n\n");
	_snprintf(buff, sizeof(buff), "             %8d   PRINTED OUTPUT CYCLE (IN TIME STEPS)\n\n", CP->getLCOLPR());
	logLine.append(buff);
	for (int i = 0; i < CP->getLCOLMX(); i++){
		logLine.append("             COLUMN " + to_string(i + 1) + ":  " + CP->getLCOL()[i] + "\n");
	}

	logWriter->writeContainer.push_back(logLine);
}
void DataSet::parseDataSet_8D(){
	string logLine = "";
	Writer * logWriter = Writer::LSTInstance();
	char buff[512];

	ControlParameters * CP = ControlParameters::Instance();
	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[11];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i - 1;
			line.assign(str_start, str_end);
			lines.push_back(line);
			str_start = map + i + 2;
		}
	}
	vector <string> dataPos;
	int nobs = CP->getNOBS();
	for (int i = 1; i <= nobs; i++){
		line = lines[i];
		boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
		if (dataPos.at(0) == "'-'"){
			CP->exitOnError("INP-8D-4");
		}
		if (CP->getKTYPE(0) == 3){
			/*OBSNAM = dataPos.at(0);
			XOBS = stod(dataPos.at(1));
			YOBS = stod(dataPos.at(2));
			ZOBS = stod(dataPos.at(3));
			CDUM80 = dataPos.at(4);
			OBSFMT = dataPos.at(5);*/
			ObservationPoints * obs = new ObservationPoints(dataPos.at(0), dataPos.at(4), dataPos.at(5), stod(dataPos.at(1)), stod(dataPos.at(2)), stod(dataPos.at(3)));
			CP->addToListOfOBS(obs);
		}
		else{
			/*OBSNAM = dataPos.at(0);
			XOBS = stod(dataPos.at(1));
			YOBS = stod(dataPos.at(2));
			ZOBS = 0.0;
			CDUM80 = dataPos.at(3);
			OBSFMT = dataPos.at(4);*/
			ObservationPoints * obs = new ObservationPoints(dataPos.at(0), dataPos.at(3), dataPos.at(4), stod(dataPos.at(1)), stod(dataPos.at(2)));
			CP->addToListOfOBS(obs);
		}

	}
	// Fix This later boring
	if (CP->getISSTRA() == 1){
		logLine.append("\n\n             .OBS AND .OBC FILES\n             -------------------\n\n");
		logLine.append("             UNIT           ASSIGNED TO " + InputFiles::Instance()->filesToWrite["OBS"] + "\n");
	}
	else if (CP->getVERN() == "2.2"){
		logLine.append("\n\n             .OBS AND .OBC FILES\n             -------------------\n\n");
		logLine.append("               SCHEDULE \n");
	}
	else{
	}

	
	logLine.append("\n\n\n\n\n           O B S E R V A T I O N   P O I N T S\n\n\n");
	if (CP->getKTYPE(0) == 3){
		logLine.append("             NAME                                         COORDINATES                                            SCHEDULE       FORMAT\n");
		logLine.append("             ----                                         -----------                                            --------       ------\n");
		for (ObservationPoints* x : CP->getObservationPointsContainer()){
			_snprintf(buff, sizeof(buff), "             %10s                                   (%+14.7e , %+14.7e, %+14.7e)   %s    %s\n", (x->getOBSNAM()).c_str(), x->getXOBS(), x->getYOBS(), x->getZOBS(), (x->getSchedule()).c_str(), (x->getOBSFMT()).c_str());
			logLine.append(buff);
		}
	}
	else{
		logLine.append("             NAME                                         COORDINATES                     SCHEDULE       FORMAT\n");
		logLine.append("             ----                                         -----------                     --------       ------\n");
		for (ObservationPoints* x : CP->getObservationPointsContainer()){
			_snprintf(buff, sizeof(buff), "             %10s                                         (%+14.7e , %+14.7e)   %s    %s\n", x->getOBSNAM().c_str(), x->getXOBS(), x->getYOBS(), x->getSchedule().c_str(), x->getOBSFMT().c_str());
		logLine.append(buff);
		}
	}

	logWriter->writeContainer.push_back(logLine); 
}
void DataSet::parseDataSet_8E_9_10_11(){
	string logLine = "";
	Writer * logWriter = Writer::LSTInstance();
	char buff[512];
	ControlParameters * CP = ControlParameters::Instance();

	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[12];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i - 1;
			line.assign(str_start, str_end);
			lines.push_back(line);
			str_start = map + i + 2;
		}
	}
	line = lines[0];
	vector<string>dataPos;
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setNBCFPR(stoi(dataPos.at(0)));
	CP->setNBCSPR(stoi(dataPos.at(1)));
	CP->setNBCPPR(stoi(dataPos.at(2)));
	CP->setNBCUPR(stoi(dataPos.at(3)));
	CP->setCINACT(dataPos.at(4));
	if (CP->getCINACT() == "Y"){
		CP->setKINACT(1);
	}
	else if (CP->getCINACT() == "N"){ CP->setKINACT(0); }
	else{
		CP->exitOnError("INP-8E-1");
	}
	
	logLine.append("\n" + string(13, ' ') + ".BCOF, .BCOS, .BCOP, AND .BCOU FILES\n");
	logLine.append(string(13, ' ') + "------------------------------------\n\n");
	
	_snprintf(buff, sizeof(buff),"             %8d   PRINTED OUTPUT CYCLE FOR FLUID SOURCES/SINK NODES TO .BCOF FILE (IN TIME STEPS)\n",CP->getNBCFPR());
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "             %8d   PRINTED OUTPUT CYCLE FOR SOLUTE SOURCES/SINK NODES TO .BCOS FILE (IN TIME STEPS)\n", CP->getNBCSPR());
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "             %8d   PRINTED OUTPUT CYCLE FOR SPECIFIED PRESSURE NODES TO .BCOP FILE (IN TIME STEPS)\n", CP->getNBCPPR());
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "             %8d   PRINTED OUTPUT CYCLE FOR SPECIFIED CONCENTRATION NODES TO .BCOU FILE (IN TIME STEPS)\n", CP->getNBCUPR());
	logLine.append(buff);

	
	if (CP->getKINACT() == 1){
		logLine.append("\n" + string(13, ' ') + "- PRINT INACTIVE BOUNDARY CONDITIONS");
	}else{
		logLine.append("\n" + string(13, ' ') + "- CANCEL PRINT OF INACTIVE BOUNDARY CONDITIONS");
	}
	logWriter->writeContainer.push_back(logLine);
	logLine.clear();



	// INPUT (*Data) SET 9 FLUID PROPERTIES
	line = lines[1];
	dataPos.clear();
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setCOMPFL(stod(dataPos.at(0)));
	CP->setCW(stod(dataPos.at(1)));
	CP->setSIGMAW(stod(dataPos.at(2)));
	CP->setRHOW0(stod(dataPos.at(3)));
	CP->setURHOW0(stod(dataPos.at(4)));
	CP->setDRWDU(stod(dataPos.at(5)));
	CP->setVISC0(stod(dataPos.at(6)));

	// INPUT (*Data) SET 10
	line = lines[2];
	dataPos.clear();
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setCOMPMA(stod(dataPos.at(0)));
	CP->setCS(stod(dataPos.at(1)));
	CP->setSIGMAS(stod(dataPos.at(2)));
	CP->setRHOS(stod(dataPos.at(3)));

	logLine.append("\n\n\n\n");
	logLine.append(string(11,' ') + "C O N S T A N T   P R O P E R T I E S   O F   F L U I D   A N D   S O L I D   M A T R I X\n\n");
	_snprintf(buff, sizeof(buff), "           %+15.4e     COMPRESSIBILITY OF FLUID\n", CP-> getCOMPFL());
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %+15.4e     COMPRESSIBILITY OF POROUS MATRIX\n\n", CP-> getCOMPMA());
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %+15.4e     FLUID VISCOSITY\n\n", CP-> getVISC0());
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %+15.4e     DENSITY OF A SOLID GRAIN\n\n", CP->getRHOS());
	logLine.append(buff);
	logLine.append(string(13, ' ') + "FLUID DENSITY RHOW\n" + string(13, ' ') + "CALCULATED BY IPSSIM IN TERMS OF SOLUTE CONCENTRATION, U, AS :\n");
	logLine.append(string(13, ' ') + "RHOW = RHOW0 + DRWDU*(U-URHOW0)\n\n");
	_snprintf(buff, sizeof(buff), "           %+15.4e     FLUID BASE DENSITY,RHOW0\n", CP->getRHOW0());
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %+15.4e     COEFFICIENT OF DENSITY CHANGE WITH SOLUTE CONCENTRATION, DRWDU\n", CP->getDRWDU());
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %+15.4e     SOLUTE CONCENTRATION,URHOW0, AT WHICH FLUID DENSITY IS AT BASE VALUE, RHOW0\n\n", CP->getDRWDU());
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "           %+15.4e     MOLECULAR DIFFUSIVITY OF SOLUTE IN FLUID", CP->getSIGMAW());
	logLine.append(buff);
	
	logWriter->writeContainer.push_back(logLine);
	logLine.clear();

	// INPUT (*Data) SET 11 : ADSORPTION PARAMETERS
	line = lines[3];
	dataPos.clear();
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setADSMOD(dataPos.at(0));
	if (dataPos.at(0) == "'NONE'"){
		logLine.append("\n\n\n\n\n           A D S O R P T I O N   P A R A M E T E R S\n\n");
		logLine.append("                NON-SORBING SOLUTE");
		logWriter->writeContainer.push_back(logLine);
		logLine.clear();
	}
	else{
		logLine.append("\n\n\n\n\n           A D S O R P T I O N   P A R A M E T E R S\n\n");
		logLine.append(string(16,' ')+dataPos.at(0)+ "     EQUILIBRIUM SORPTION ISOTHERM'");
	
		if (dataPos.at(0) == "'LINEAR'"){
			_snprintf(buff, sizeof(buff), "           %+15.4e     LINEAR DISTRIBUTION COEFFICIENT\n", stod(dataPos[1]));
			logLine.append(buff);
		}
		else  if (dataPos.at(0) == "'FREUNDLICH'"){
			if (stod(dataPos[2]) <= 0){
				CP->exitOnError("INP-11-2");
			}
			_snprintf(buff, sizeof(buff), "           %+15.4e     FREUNDLICH DISTRIBUTION COEFFICIENT\n", stod(dataPos[1]));
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "           %+15.4e     SECOND FREUNDLICH COEFFICIENT\n", stod(dataPos[2]));
			logLine.append(buff);
		
		}
		else if (dataPos.at(0) == "'LANGMUIR'"){
			_snprintf(buff, sizeof(buff), "           %+15.4e     LANGMUIR DISTRIBUTION COEFFICIENT\n", stod(dataPos[1]));
			logLine.append(buff);
			_snprintf(buff, sizeof(buff), "           %+15.4e     SECOND LANGMUIR COEFFICIENT\n", stod(dataPos[2]));
			logLine.append(buff);
		}
		else{
			CP->exitOnError("INP-11-1");
		}
		logWriter->writeContainer.push_back(logLine);
		logLine.clear();
	}


}
void DataSet::parseDataSet_12_13_14A(){

	string logLine = "";
	Writer * logWriter = Writer::LSTInstance();
	char buff[512];


	ControlParameters * CP = ControlParameters::Instance();

	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[13];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i - 1;
			line.assign(str_start, str_end);
			lines.push_back(line);
			str_start = map + i + 2;
		}
	}
	line = lines[0];
	vector<string> dataPos;
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	CP->setPRODF0(stod(dataPos.at(0)));
	CP->setPRODS0(stod(dataPos.at(1)));
	CP->setPRODF1(stod(dataPos.at(2)));
	CP->setPRODS1(stod(dataPos.at(3)));

	logLine.append("\n\n\n\n           P R O D U C T I O N   A N D   D E C A Y   O F    S P E C I E S   M A S S\n\n");
	logLine.append("              PRODUCTION RATE (+)\n");
	logLine.append("              DECAY RATE (-)\n\n");
	_snprintf(buff, sizeof(buff), "             %+15.4e     ZERO-ORDER RATE OF SOLUTE MASS PRODUCTION/DECAY IN FLUID\n", CP->getPRODF0());
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "             %+15.4e     ZERO-ORDER RATE OF ABSORBATE MASS PRODUCTION/DECAY IN IMMOBILE PHASE\n", CP->getPRODS0());
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "             %+15.4e     FIRST-ORDER RATE OF SOLUTE MASS PRODUCTION/DECAY IN FLUID\n", CP->getPRODF1());
	logLine.append(buff);
	_snprintf(buff, sizeof(buff), "             %+15.4e     FIRST-ORDER RATE OF ABSORBATE MASS PRODUCTION/DECAY IN IMMOBILE PHASE\n", CP->getPRODS1());
	logLine.append(buff);

	CP->setCS(0.0);
	CP->setCW(1.0);
	CP->setSIGMAS(0.0);

	logWriter->writeContainer.push_back(logLine);
	logLine.clear();
	// INPUT (*Data) SET 13 : PRODUCTION OF ENERGY OR SOLUTE MASS
	if (CP->getKTYPE(0) == 3){
		line = lines[1];
		dataPos.clear();
		boost::split(dataPos, line, boost::is_any_of(" ,\n"), boost::token_compress_on);
		CP->setGRAVX(stod(dataPos.at(0)));
		CP->setGRAVY(stod(dataPos.at(1)));
		double x = (stod(dataPos.at(2))*1000);
		int perm = (int)x;
		CP->setGRAVZ(x/1000);
		logLine.append("\n\n\n\n           C O O R D I N A T E   O R I E N T A T I O N   T O   G R A V I T Y\n\n");
		logLine.append("             COMPONENT OF GRAVITY VECTOR\n");
		logLine.append("             IN +X DIRECTION, GRAVX\n");
		_snprintf(buff, sizeof(buff), "             %+15.4e     GRAVX = -GRAV*D(ELEVATION)/DX\n\n", CP->getGRAVX());
		logLine.append(buff);
		logLine.append("             COMPONENT OF GRAVITY VECTOR\n");
		logLine.append("             IN +Y DIRECTION, GRAVY\n");
		_snprintf(buff, sizeof(buff), "             %+15.4e     GRAVY = -GRAV*D(ELEVATION)/DY\n\n", CP->getGRAVY());
		logLine.append(buff);
		logLine.append("             COMPONENT OF GRAVITY VECTOR\n");
		logLine.append("             IN +Z DIRECTION, GRAVZ\n");
		_snprintf(buff, sizeof(buff), "             %+15.4e     GRAVZ = -GRAV*D(ELEVATION)/DZ\n\n", CP->getGRAVZ());
		logLine.append(buff);

		logWriter->writeContainer.push_back(logLine);
		logLine.clear();

		// INPUT (*Data) SET 14a : PRODUCTION OF ENERGY OR SOLUTE MASS
		line = lines[2];
		dataPos.clear();
		boost::split(dataPos, line, boost::is_any_of(" "), boost::token_compress_on);
		string CDUM10 = " ";
		CDUM10 = dataPos.at(0);
		CP->setSCALX(stod(dataPos.at(1)));
		CP->setSCALY(stod(dataPos.at(2)));
		CP->setSCALZ(stod(dataPos.at(3)));
		CP->setPORFAC(stod(dataPos.at(4)));
		if (CDUM10 != "'NODE'"){
			CP->exitOnError("INP-14A-1");
		}
	
		CP->setNRTEST(1);
	}
	else if (CP->getKTYPE(0) == 2){
		line = lines[1];
		dataPos.clear();
		boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
		CP->setGRAVX(stod(dataPos.at(0)));
		CP->setGRAVY(stod(dataPos.at(1)));
		CP->setGRAVZ(0.0);
		logLine.append("\n\n\n\n           C O O R D I N A T E   O R I E N T A T I O N   T O   G R A V I T Y\n\n");
		logLine.append("             COMPONENT OF GRAVITY VECTOR\n");
		logLine.append("             IN +X DIRECTION, GRAVX\n");
		_snprintf(buff, sizeof(buff), "             %+15.4e     GRAVX = -GRAV*D(ELEVATION)/DX\n\n", CP->getGRAVX());
		logLine.append(buff);
		logLine.append("             COMPONENT OF GRAVITY VECTOR\n");
		logLine.append("             IN +Y DIRECTION, GRAVY\n");
		_snprintf(buff, sizeof(buff), "             %+15.4e     GRAVY = -GRAV*D(ELEVATION)/DY\n\n", CP->getGRAVY());
		logLine.append(buff);


		// INPUT (*Data) SET 14a : PRODUCTION OF ENERGY OR SOLUTE MASS
		line = lines[2];
		dataPos.clear();
		boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
		string CDUM10 = " ";
		CDUM10 = dataPos.at(0);
		CP->setSCALX(stod(dataPos.at(1)));
		CP->setSCALY(stod(dataPos.at(2)));
		CP->setSCALZ(stod(dataPos.at(3)));
		CP->setPORFAC(stod(dataPos.at(4)));
		if (CDUM10 != "'NODE'"){
			CP->exitOnError("INP-14A-1");
		}
		CP->setNRTEST(1);

	}
}
void DataSet::parseDataSet_14B(){
	ControlParameters * CP = ControlParameters::Instance();
	string logLine = "";
	Writer * logWriter = Writer::LSTInstance();
	char buff[512];
	if (CP->getKTYPE(0) == 3){
		const char * del = " ";

		Timer gent,t;
		int i; // NodeNumber

		pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[14];
		int size = dataSetPosition.second - dataSetPosition.first;
		char * map = InputFileParser::Instance()->mapViewOfINPFile;
		vector<string> lines;
		vector<char> lineBuffer;
		char* str_start = map + dataSetPosition.first;
		char * str_end;
		t.reset();
		Node * nNode;
		for (int j = dataSetPosition.first; j < dataSetPosition.second; j++){
			if (map[j] == '\r'){
				str_end = map + j - 1;
				vector<char>lineBuffer(str_start, str_end);
				str_start = map + j + 2;
				lineBuffer.push_back('\0');
				//vector<char> lineBuffer(line.begin(), line.end());
				i = stoi(strtok(lineBuffer.data(), del));
				nNode = CP->getNodeContainer()[i];
				
				nNode->setNREG(stoi(strtok(NULL, del)));

				nNode->setXCoord(stod(strtok(NULL, del))*CP->getSCALX());

				nNode->setYCoord(stod(strtok(NULL, del))*CP->getSCALY());

				nNode->setZCoord(stod(strtok(NULL, del))*CP->getSCALZ());

				nNode->setPorosity(stod(strtok(NULL, del))*CP->getPORFAC());

				nNode->setSOP(1 - nNode->getPorosity()*CP->getCOMPMA() + nNode->getPorosity() * CP->getCOMPFL());
			}
			
		}
		
		
		if (CP->getKNODAL() == 0){
			logLine.append("\n\n\n\n           N O D E   I N F O R M A T I O N\n\n");
			cout << "\n                PRINTOUT OF NODE COORDINATES AND POROSITIES CANCELLED" << endl;
			cout << "\n                SCALE FACTORS:" << endl;
			cout << "\n                                 " << CP->getSCALX() << "     X-SCALE" << endl;
			cout << "\n                                 " << CP->getSCALY() << "     Y-SCALE" << endl;
			cout << "\n                                 " << CP->getSCALZ() << "     Z-SCALE" << endl;
			cout << "\n                                 " << CP->getPORFAC() << "     POROSITY FACTOR" << endl;
		}
		// Check unsaturated

		// INPUT DATASETS 15A & 15B

		
		cout << "Extracting Nodes took: " << gent << " seconds" << endl;
	}
	if (CP->getKTYPE(0) == 2){

		int NROLD = 0;
		int NRTEST = 1;

		const char * del = " ";
		vector<char>lineBuffer;
		clock_t t;
		t = clock();
		int NN = CP->getNN();
		int i;
		for (int j = 1; j < NN+1; j++){
			lineBuffer.assign((*Data)[j].begin(), (*Data)[j].end());
			lineBuffer.push_back('\0');


			i = stoi(strtok(lineBuffer.data(), del));
			CP->getNodeContainer()[i]->setNREG(stoi(strtok(NULL, del)));
			CP->getNodeContainer()[i]->setXCoord(stod(strtok(NULL, del))*CP->getSCALX());
			CP->getNodeContainer()[i]->setYCoord(stod(strtok(NULL, del))*CP->getSCALY());
			CP->getNodeContainer()[i]->setZCoord(0.0);
			CP->getNodeContainer()[i]->setPorosity(stod(strtok(NULL, del))*CP->getPORFAC());
			CP->getNodeContainer()[i]->setSOP(1 - CP->getNodeContainer()[i]->getPorosity()*CP->getCOMPMA() + CP->getNodeContainer()[i]->getPorosity() * CP->getCOMPFL());


			/*if (NREG != NROLD){
			NROLD = NREG;
			NRTEST++;
			}
			*/
			//IF(I.GT.1.AND.NREG(II).NE.NROLD) NRTEST = NRTEST + 1                   INDAT1.......82000
			//NROLD = NREG(II)                                                     INDAT1.......82100

		}

		if (CP->getKNODAL() == 0){
			cout << "\n\n\n\n           N O D E   I N F O R M A T I O N" << endl;
			cout << "\n                PRINTOUT OF NODE COORDINATES AND POROSITIES CANCELLED" << endl;
			cout << "\n                SCALE FACTORS:" << endl;
			cout << "\n                                 " << CP->getSCALX() << "     X-SCALE" << endl;
			cout << "\n                                 " << CP->getSCALY()<< "     Y-SCALE" << endl;
			cout << "\n                                 " << CP->getSCALZ() << "     Z-SCALE" << endl;
			cout << "\n                                 " << CP->getPORFAC() << "     POROSITY FACTOR" << endl;
		}

		// Check Unsaturated
		t = clock() - t;
		cout << "Extracting Nodes took: " << ((float)t) / CLOCKS_PER_SEC << " seconds" << endl;
	}

}
void DataSet::parseDataSet_15A(){
	string logLine = "";
	Writer * logWriter = Writer::LSTInstance();
	char buff[512];
	ControlParameters * CP = ControlParameters::Instance();

	pair<int, int> dataSetPosition = InputFileParser::Instance()->getDataSets()[15];
	int size = dataSetPosition.second - dataSetPosition.first;
	char * map = InputFileParser::Instance()->mapViewOfINPFile;
	string line;
	vector<string> lines;
	char* str_start = map + dataSetPosition.first;
	char * str_end;
	for (int i = dataSetPosition.first; i < dataSetPosition.second; i++){
		if (map[i] == '\r'){
			str_end = map + i - 1;
			line.assign(str_start, str_end);
			lines.push_back(line);
			str_start = map + i + 2;
		}
	}
	line = lines[0];
	vector<string> dataPos;
	string CDUM10;
	boost::split(dataPos, line, boost::is_any_of(" ,\r"), boost::token_compress_on);
	if (CP->getKTYPE(0) == 3){
		CDUM10 = dataPos.at(0);
		CP->setPMAXFA(stod(dataPos.at(1)));
		CP->setPMIDFA(stod(dataPos.at(2)));
		CP->setPMINFA(stod(dataPos.at(3)));
		CP->setANG1FA(stod(dataPos.at(4)));
		CP->setANG2FA(stod(dataPos.at(5)));
		CP->setANG3FA(stod(dataPos.at(6)));
		CP->setALMAXF(stod(dataPos.at(7)));
		CP->setALMIDF(stod(dataPos.at(8)));
		CP->setALMINF(stod(dataPos.at(9)));
		CP->setATMXF(stod(dataPos.at(10)));
		CP->setATMDF(stod(dataPos.at(11)));
		CP->setATMNF(stod(dataPos.at(12)));

		if (CDUM10 != "'ELEMENT'"){
			CP->exitOnError("INP-15A-1");
		}
		if (CP->getKELMNT() == 1){
			if (CP->getIUNSAT() == 1){
				logLine.append("\n\n           E L E M E N T   I N F O R M A T I O N\n\n");
				logLine.append(string(11, ' ') + "ELEMENT   REGION    MAXIMUM         MIDDLE          MINIMUM                  ANGLE1         ANGLE2         ANGLE3    LONGITUDINAL   LONGITUDINAL   LONGITUDINAL     TRANSVERSE     TRANSVERSE     TRANSVERSE");
				logLine.append(string(31, ' ') + "PERMEABILITY    PERMEABILITY    PERMEABILITY        (IN DEGREES)   (IN DEGREES)   (IN DEGREES)   DISPERSIVITY   DISPERSIVITY   DISPERSIVITY   DISPERSIVITIY   DISPERSIVITY   DISPERSIVITY\n");
				logLine.append(string(128, ' ') + " IN MAX-PERM   IN MID-PERM   IN MIN-PERM   IN MAX-PERM   IN-MID-PERM   IN MIN-PERM\n");
				logLine.append(string(128, ' ') + "   DIRECTION   DIRECTION   DIRECTION   DIRECTION   DIRECTION   DIRECTION\n");
				
			}
			else{
				logLine.append("\n\n           E L E M E N T   I N F O R M A T I O N\n\n");
				logLine.append(string(11, ' ') + "ELEMENT    MAXIMUM         MIDDLE          MINIMUM                  ANGLE1         ANGLE2         ANGLE3    LONGITUDINAL   LONGITUDINAL   LONGITUDINAL     TRANSVERSE     TRANSVERSE     TRANSVERSE");
				logLine.append(string(22, ' ') + "PERMEABILITY    PERMEABILITY    PERMEABILITY        (IN DEGREES)   (IN DEGREES)   (IN DEGREES)   DISPERSIVITY   DISPERSIVITY   DISPERSIVITY   DISPERSIVITIY   DISPERSIVITY   DISPERSIVITY\n");
				logLine.append(string(119, ' ') + " IN MAX-PERM   IN MID-PERM   IN MIN-PERM   IN MAX-PERM   IN-MID-PERM   IN MIN-PERM\n");
				logLine.append(string(119, ' ') + "   DIRECTION   DIRECTION   DIRECTION   DIRECTION   DIRECTION   DIRECTION\n");

			}

			logWriter->writeContainer.push_back(logLine);
			logLine.clear();
		}

	}
	if (CP->getKTYPE(0) == 2){
		
		CDUM10 = dataPos.at(0);
		CP->setPMAXFA(stod(dataPos.at(1)));
		CP->setPMINFA(stod(dataPos.at(2)));
		CP->setANGFAC(stod(dataPos.at(3)));
		CP->setALMAXF(stod(dataPos.at(4)));
		CP->setALMINF(stod(dataPos.at(5)));
		CP->setATMXF(stod(dataPos.at(6)));
		CP->setATMNF(stod(dataPos.at(7)));

		if (CDUM10 != "'ELEMENT'"){
			CP->exitOnError("INP-15A-1");
		}
		if (CP->getKELMNT() == 1){
			if (CP->getIUNSAT() == 1){
				logLine.append("\n\n           E L E M E N T   I N F O R M A T I O N\n\n");
				logLine.append(string(11, ' ') + "ELEMENT   REGION    MAXIMUM          MINIMUM            ANGLE BETWEEN   LONGITUDINAL   LONGITUDINAL     TRANSVERSE     TRANSVERSE");
				logLine.append(string(31, ' ') + "PERMEABILITY    PERMEABILITY    +X-DIRECTION AND   DISPERSIVITY   DISPERSIVITIY   DISPERSIVITY   DISPERSIVITY\n");
				logLine.append(string(59, ' ') + "MAXIMUM PERMEABILITY   IN MAX-PERM   IN MIN-PERM   IN MAX-PERM   IN MIN-PERM\n");
				logLine.append(string(67, ' ') + "(IN DEGREES)   DIRECTION   DIRECTION   DIRECTION   DIRECTION\n");

			}
			else{
				logLine.append("\n\n           E L E M E N T   I N F O R M A T I O N\n\n");
				logLine.append(string(11, ' ') + "ELEMENT    MAXIMUM         MINIMUM            ANGLE BETWEEN   LONGITUDINAL   LONGITUDINAL   TRANSVERSE     TRANSVERSE");
				logLine.append(string(22, ' ') + "PERMEABILITY    PERMEABILITY    +X-DIRECTION AND   DISPERSIVITY   DISPERSIVITY   DISPERSIVITY   DISPERSIVITY\n");
				logLine.append(string(50, ' ') + "MAXIMUM PERMEABILITY   IN MAX-PERM   IN MIN-PERM   IN MAX-PERM   IN MIN-PERM\n");
				logLine.append(string(58, ' ') + "(IN DEGREES)   DIRECTION   DIRECTION   DIRECTION   DIRECTION\n");
			}
		
			logWriter->writeContainer.push_back(logLine);
			logLine.clear();
		}

	}

}
void DataSet::parseDataSet_15B(){
	ControlParameters * CP = ControlParameters::Instance();
	if (CP->getKTYPE(0) == 3){
		
		vector<char> lineBuffer;
		const char * del = " ";
	
		int LRTEST = 1;

		clock_t t;
		t = clock();
		int NE = CP->getNE();
		for (int i = 0; i < NE; i++){
			lineBuffer.assign((*Data)[i].begin(), (*Data)[i].end());
			lineBuffer.push_back('\0');
			i = stoi(strtok(lineBuffer.data(), del));
			CP->getElementContainer()[i]->setLREG(stoi(strtok(NULL, del)));
			CP->getElementContainer()[i]->setPMAX(stod(strtok(NULL, del))*CP->getPMAXFA());
			CP->getElementContainer()[i]->setPMID(stod(strtok(NULL, del))*CP->getPMIDFA());
			CP->getElementContainer()[i]->setPMIN(stod(strtok(NULL, del))*CP->getPMINFA());
			CP->getElementContainer()[i]->setANGLE1(stod(strtok(NULL, del))*CP->getANG1FA());
			CP->getElementContainer()[i]->setANGLE2(stod(strtok(NULL, del))*CP->getANG2FA());
			CP->getElementContainer()[i]->setANGLE3(stod(strtok(NULL, del))*CP->getANG3FA());
			CP->getElementContainer()[i]->setALMAX(stod(strtok(NULL, del))*CP->getALMAXF());
			CP->getElementContainer()[i]->setALMID(stod(strtok(NULL, del))*CP->getALMIDF());
			CP->getElementContainer()[i]->setALMIN(stod(strtok(NULL, del))*CP->getALMINF());
			CP->getElementContainer()[i]->setATMAX(stod(strtok(NULL, del))*CP->getATMXF());
			CP->getElementContainer()[i]->setATMID(stod(strtok(NULL, del))*CP->getATMDF());
			CP->getElementContainer()[i]->setATMIN(stod(strtok(NULL, del))*CP->getATMNF());






			//IF(I.GT.1.AND.NREG(II).NE.NROLD) NRTEST = NRTEST + 1                   INDAT1.......82000
			//NROLD = NREG(II)                                                     INDAT1.......82100


		}

		if (CP->getKELMNT() == 0){
			cout << "\n\n\n\n E L E M E N T   I N F O R M A T I O N" << endl;
			cout << "                PRINTOUT OF ELEEMENT PERMEABILITIES AND DISPERSIVITIES CANCELLED." << endl;
			cout << "                SCALE FACTORS:" << endl;
			cout << "                                 " << CP->getPMAXFA() << "     MAXIMUM PERMEABILITY FACTOR" << endl;
			cout << "                                 " << CP->getPMIDFA() << "     MIDDLE PERMEABILITY FACTOR" << endl;
			cout << "                                 " << CP->getPMINFA() << "     MINIMUM PERMEABILITY FACTOR" << endl;
			cout << "                                 " << CP->getANG1FA() << "     ANGLE1 FACTOR" << endl;
			cout << "                                 " << CP->getANG2FA() << "     ANGLE2 FACTOR" << endl;
			cout << "                                 " << CP->getANG3FA() << "     ANGLE3 FACTOR" << endl;
			cout << "                                 " << CP->getALMAXF() << "     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MAX-PERM DIRECTION" << endl;
			cout << "                                 " << CP->getALMIDF() << "     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MID-PERM DIRECTION" << endl;
			cout << "                                 " << CP->getALMINF() << "     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MIN-PERM DIRECTION" << endl;
			cout << "                                 " << CP->getATMXF() << "     FACTOR FOR TRANSVERSE DISPERSIVITY IN MAX-PERM DIRECTION" << endl;
			cout << "                                 " << CP->getATMDF() << "     FACTOR FOR TRANSVERSE DISPERSIVITY IN MID-PERM DIRECTION" << endl;
			cout << "                                 " << CP->getATMNF() << "     FACTOR FOR TRANSVERSE DISPERSIVITY IN MIN-PERM DIRECTION" << endl;

		}
		if (CP->getIUNSAT() == 1 && CP->getKELMNT() == 0 && LRTEST != 1)
			cout << "MORE THAN ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE ELEMENTS." << endl;
		if (CP->getIUNSAT() == 1 && CP->getKELMNT() == 0 && LRTEST == 0)
			cout << "ONLY ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE ELEMENTS." << endl;

		t = clock() - t;
		cout << "Extracting Elements took: " << ((float)t) / CLOCKS_PER_SEC << " seconds" << endl;
	}
	if (CP->getKTYPE(0) == 2){
		
		vector<char> lineBuffer;
		const char * del = " ";
		
		int LRTEST = 1;
		clock_t t;
		t = clock();
		int LROLD = 0;
		int NE = CP->getNE();
		for (int i = 0; i<NE; i++){
			lineBuffer.assign((*Data)[i].begin(), (*Data)[i].end());
			lineBuffer.push_back('\0');
			i = stoi(strtok(lineBuffer.data(), del));
			CP->getElementContainer()[i]->setLREG(stoi(strtok(NULL, del)));
			CP->getElementContainer()[i]->setPMAX(stod(strtok(NULL, del))*CP->getPMAXFA());

			CP->getElementContainer()[i]->setPMIN(stod(strtok(NULL, del))*CP->getPMINFA());


			CP->getElementContainer()[i]->setANGLEX(stod(strtok(NULL, del))*CP->getANGFAC());
			CP->getElementContainer()[i]->setALMAX(stod(strtok(NULL, del))*CP->getALMAXF());

			CP->getElementContainer()[i]->setALMIN(stod(strtok(NULL, del))*CP->getALMINF());
			CP->getElementContainer()[i]->setATMAX(stod(strtok(NULL, del))*CP->getATMXF());

			CP->getElementContainer()[i]->setATMIN(stod(strtok(NULL, del))*CP->getATMNF());


			/*EleNum = stoi(dataPos.at(0));
			LREG = stoi(dataPos.at(1));
			PMAX = stod(dataPos.at(2));
			PMIN = stod(dataPos.at(4));
			ANGLEX = stod(dataPos.at(4));
			ALMAX = stod(dataPos.at(8));
			ALMIN = stod(dataPos.at(10));
			ATMAX = stod(dataPos.at(11));
			ATMIN = stod(dataPos.at(13));

			PMAX = PMAX*PMAXFA;
			PMIN = PMIN*PMINFA;
			ALMAX = ALMAX*ALMAXF;
			ALMIN = ALMIN*ALMINF;
			ATMAX = ATMAX*ATMXF;
			ATMIN = ATMIN*ATMNF;
			ANGLEX = ANGLEX*ANGFAC;*/

			/*	if (LREG != LROLD){
			LRTEST++;
			LROLD = LREG;
			}*/




			//IF(I.GT.1.AND.NREG(II).NE.NROLD) NRTEST = NRTEST + 1                   INDAT1.......82000
			//NROLD = NREG(II)                                                     INDAT1.......82100


		}

		if (CP->getKELMNT() == 0){
			cout << "\n\n\n\n E L E M E N T   I N F O R M A T I O N" << endl;
			cout << "                PRINTOUT OF ELEEMENT PERMEABILITIES AND DISPERSIVITIES CANCELLED." << endl;
			cout << "                SCALE FACTORS:" << endl;
			cout << "                                 " << CP->getPMAXFA() << "     MAXIMUM PERMEABILITY FACTOR" << endl;
			cout << "                                 " << CP->getPMINFA() << "     MINIMUM PERMEABILITY FACTOR" << endl;
			cout << "                                 " << CP->getANGFAC() << "     ANGLE FROM +X TO MAXIMUM DIRECTION FACTOR" << endl;
			cout << "                                 " << CP->getALMAXF() << "     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MAX-PERM DIRECTION" << endl;
			cout << "                                 " << CP->getALMINF() << "     FACTOR FOR LONGITUDINAL DISPERSIVITY IN MIN-PERM DIRECTION" << endl;
			cout << "                                 " << CP->getATMXF() << "     FACTOR FOR TRANSVERSE DISPERSIVITY IN MAX-PERM DIRECTION" << endl;
			cout << "                                 " << CP->getATMNF() << "     FACTOR FOR TRANSVERSE DISPERSIVITY IN MIN-PERM DIRECTION" << endl;

		}
		if (CP->getIUNSAT() == 1 && CP->getKELMNT() == 0 && LRTEST != 1)
			cout << "MORE THAN ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE ELEMENTS." << endl;
		if (CP->getIUNSAT() == 1 && CP->getKELMNT() == 0 && LRTEST == 0)
			cout << "ONLY ONE REGION OF UNSATURATED PROPERTIES HAS BEEN SPECIFIED AMONG THE ELEMENTS." << endl;

		t = clock() - t;
		cout << "Extracting Elements took: " << ((float)t) / CLOCKS_PER_SEC << " seconds" << endl;

	}
}
void DataSet::parseDataSet_17_18(string key){
	ControlParameters * CP = ControlParameters::Instance();
	CP->Source(key);
}

void DataSet::parseDataSet_19_20(string key){
	ControlParameters * CP = ControlParameters::Instance();
	CP->Bound(key);
}

void DataSet::parseDataSet_22(){
	ControlParameters::Instance()->Connec();

}