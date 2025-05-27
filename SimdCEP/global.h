#include "includefile.h"
#include "parseQuery.h"
#include "event.h"
using namespace std; 
class global
{
public:
	vector<event> candis;
	vector<int> numresult;
	unordered_map<int, event> hmap;

public:
	bool read1(string a, vector<string>& b, int& c);
	void Tokenize(const string& str, vector<string>& tokens, const string& delimiters);
	void test(query& a, vector<vector<size_t>> b);
	void parseFiletxt(vector<query>& a, string b);
	void printAsBit(size_t val);
};

