#include "Writer.h"
using namespace std;


Writer * Writer::m_pInstance = NULL;
mutex mmtx;


Writer* Writer::Instance(){
	if (!m_pInstance)
		m_pInstance = new Writer;
	return m_pInstance;
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
