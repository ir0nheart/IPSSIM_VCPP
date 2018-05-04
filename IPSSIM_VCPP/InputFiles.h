#include <string>
#include <vector>
#include <unordered_map>

#pragma once
using namespace std;


class InputFiles
{
public:
	static InputFiles* Instance();
	void getPath();
	void getFileList();
	string getInputDirectory();
	void exitOnError();
	unordered_map<string, string> filesToRead;
	unordered_map<string, string> filesToWrite;
	void checkInputFiles();
	void readPropsINP();
	void printInputFilesToLST();
	bool getBcsDef();
	void setBcsDef(bool val);

private:
	InputFiles();
	InputFiles(InputFiles const&){};
	InputFiles& operator = (InputFiles const&){};
	virtual ~InputFiles();
	static InputFiles* m_pInstance;
	string inputDir;
	unordered_map<string, string> inputFileList;
	bool bcsDef = false;
};

