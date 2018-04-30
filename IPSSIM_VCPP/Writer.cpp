#include "Writer.h"
using namespace std;


Writer * Writer::mLST_pInstance = NULL;
Writer * Writer::mNOD_pInstance = NULL;
Writer * Writer::mELE_pInstance = NULL;
Writer * Writer::mOBS_pInstance = NULL;
mutex mmtx;


Writer* Writer::LSTInstance(){
	if (!mLST_pInstance)
		mLST_pInstance = new Writer;
	return mLST_pInstance;
}
Writer* Writer::NODInstance(){
	if (!mNOD_pInstance)
		mNOD_pInstance = new Writer;
	return mNOD_pInstance;
}
Writer* Writer::ELEInstance(){
	if (!mELE_pInstance)
		mELE_pInstance = new Writer;
	return mELE_pInstance;
}
Writer* Writer::OBSInstance(){
	if (!mOBS_pInstance)
		mOBS_pInstance = new Writer;
	return mOBS_pInstance;
}
void Writer::addToWriteContainer(string str){ 
	lock_guard<mutex>guard(mmtx);
	writeContainer.push_back(str); 
}

deque<string> Writer::getWriteContainer(){
	deque<string> temp;
	mmtx.lock();
	temp = writeContainer;
	mmtx.unlock();

	return this->writeContainer; 

}

void Writer::popFrontWriteContainer(){ writeContainer.pop_front(); }

Writer::Writer()
{
}


Writer::~Writer()
{
}
