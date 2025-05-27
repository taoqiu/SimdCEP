#pragma once
#include "global.h"
using namespace std;

class parseData
{
public:
		int eventNum;
		vector<string>  tag;
		vector<vector<string>> txtdata;
		vector<vector<float>>  numdata;
		unordered_map<int, int> txtcor;
		unordered_map<int, int> numcor;
public:
	bool AllisNum(string str);
	void parseStream(string file);
};