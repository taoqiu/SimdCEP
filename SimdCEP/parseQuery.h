#pragma once
#include "includefile.h"
using namespace std; 
class query
{
public:
	struct patter
	{
		vector<string> evtype;
		vector<string> sttype;
		vector<string> streal;
	}pt;
	string selectionStrategy;
	bool hasMorePartitionAttribute;
	bool hasPartitionAttribute;
	vector<string> PartitionAttribute;
	vector<string> morePartitionAttribute;
	vector<vector<string>> addPredicate;
	int timeWindow;


public:
	string trim(string s);
	bool startswith(const std::string& str, const std::string& start);
	int parseFastQueryLine(query& a, string line);
	void parseFastQueryLineStaWithPAattern(string line, query& a);
	void parseFastQueryLineStartWithAND(string line, query& a);

}; 
