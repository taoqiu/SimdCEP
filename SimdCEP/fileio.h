#pragma once
#include "parseResult.h"
using namespace std;
class fileio
{
public:
	int forNum;
	int timeSlice;
	string inputQuery;
	string inputData;
	string outputResult;
public:
	void loadConfig();
	void output(global g,fileio fio);
};