#include <iostream>
#include <mutex>
#include <thread>
#include <fstream>
#include <condition_variable>
#include <unistd.h>

using namespace std;

string FILENAME = "entrada.txt";
string DEFAULT_OUTPUT_NAME = "salida.txt";
mutex writerMutex;
mutex priorityMutex;
mutex fileMutex;
mutex counterMutex;
mutex firstReaderMutex;
condition_variable writersVariable;
condition_variable counterVariable;
condition_variable firstReaderVariable;
thread* threads;
int size;
int* priority;
int readerCounter = 0;
bool noReaders = false;
ofstream oFile;

void useSharedResource(char type, int id, int time){
	this_thread::sleep_for(chrono::milliseconds(time));
	fileMutex.lock();
	oFile << string(1,type) + " " + to_string(id) + '\n';
	fileMutex.unlock();
}

int getMax(){
	int max = -2;
	for(int i = 0; i < size; ++i) if(max < priority[i]) max = priority[i];
	return max;
}

bool waitForTurn(int id){
	priorityMutex.lock();
	int max = getMax();
	if(max >= 3 && max != priority[id-1]){
		priorityMutex.unlock();
		return true;
	}
	priorityMutex.unlock();
	return false;
}

void addOneWaiting(){
	for(int i = 0; i < size; ++i) if(priority[i] != -1) ++priority[i];
}

void writer(int id, int time){
	priorityMutex.lock();
	priority[id-1] = 0;
	priorityMutex.unlock();

	unique_lock<mutex> writerLock(writerMutex);
	bool firstTime = true;
	while(waitForTurn(id)){
		if(!firstTime) writersVariable.notify_one();
		writersVariable.wait(writerLock);
		firstTime = false;
	}
	priorityMutex.lock();
	priority[id-1] = -1;
	addOneWaiting();
	priorityMutex.unlock();

	useSharedResource('E',id,time);

	counterVariable.notify_one();
	writersVariable.notify_one();
	writerLock.unlock();
}


void reader(int id, int time){

	priorityMutex.lock();
	priority[id-1] = 0;
	priorityMutex.unlock();


	unique_lock<mutex> counterLock(counterMutex);
	bool firstTime = true;
	while(waitForTurn(id)){
		if(!firstTime) writersVariable.notify_one();
		counterVariable.wait(counterLock);
		firstTime = false;
	}

	++readerCounter;
	unique_lock<mutex> writerLock(writerMutex, defer_lock);
	bool firstReader = false;
	if(readerCounter == 1) {
		writerLock.lock();
		bool firstTime = true;
		firstReader = true;
		while(waitForTurn(id)){
			if(!firstTime) writersVariable.notify_one();
			writersVariable.wait(writerLock);
			firstTime = false;
		}
	}

	priorityMutex.lock();
	priority[id-1] = -1;
	addOneWaiting();
	priorityMutex.unlock();
	counterVariable.notify_one();
	counterLock.unlock();
	useSharedResource('L',id,time);
	counterLock.lock();
	--readerCounter;


	if(readerCounter == 0) {
		if(firstReader) {
			counterVariable.notify_one();
			counterLock.unlock();
			writersVariable.notify_one();
			writerLock.unlock();
		}
		else{
			firstReaderMutex.lock();
			noReaders = true;
			firstReaderVariable.notify_one();
			firstReaderMutex.unlock();
			counterVariable.notify_one();
			counterLock.unlock();
		}
	}
	else {
		if(firstReader) {
			unique_lock<mutex> firstReaderLock(firstReaderMutex);
			noReaders = false;
			counterVariable.notify_one();
			counterLock.unlock();
			while(!noReaders) {
				firstReaderVariable.wait(firstReaderLock);
			}
			writersVariable.notify_one();
			writerLock.unlock();
		}
		else{
			counterVariable.notify_one();
			counterLock.unlock();
		}
	}
}

void createOutput(){
	cout << "Desea cambiar el nombre del archivo de salida?(Y/N)\n";
	string response;
	cin >> response;
	if(response == "y" || response == "Y") {
		string nombre;
		cout << "Ingrese el nombre del archivo de salida (sin la extension).";
		cin >> nombre;
		oFile.open(nombre + ".txt");
	}
	else oFile.open(DEFAULT_OUTPUT_NAME);
}

void readFile(){
	ifstream iFile(FILENAME, ios::in);
	if(iFile.fail()) {
		cout << "No se pudo abrir el archivo!"".\n";
		getchar();
		exit(1);
	}

	createOutput();
	string sSize;
	getline(iFile, sSize);
	size = stoi(sSize);
	priority = new int [size];
	for(int i = 0; i < size; ++i) priority[i] = -1;
	threads = new thread[size];
	string threadInfo;
	char type;
	string sId;
	string sTime;

	for(int i = 0; i < size; ++i){
		getline(iFile, threadInfo);
		type = threadInfo[0];
		int j;
		for(j = 2; threadInfo[j] != ' '; ++j) sId += threadInfo[j];
		++j;
		for(j; j < threadInfo.size(); ++j) sTime += threadInfo[j];
		if(type == 'L') threads[i] = thread(reader,stoi(sId),stoi(sTime));
		else threads[i] = thread(writer,stoi(sId),stoi(sTime));
		threadInfo.clear();
		sId.clear();
		sTime.clear();
	}
	iFile.close();
}

int main() {
	readFile();

	for(int i = 0; i < size; ++i){
		threads[i].join();
	}
	oFile.close();
	delete[] priority;
	delete[] threads;
	return 0;
}