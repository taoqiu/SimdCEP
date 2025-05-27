#include "fileio.h"
using namespace std;


void loadInfo(fileio& fio)
{
	fio.loadConfig();
}
void parseInput(vector<query>& a, parseData& procData, fileio fio)
{
	global pg;
	pg.parseFiletxt(a, fio.inputQuery);
	procData.parseStream(fio.inputData);
}
void construct(global& g, query a, parseData procData, vector<vector<size_t>>& bitvector, unordered_map<string, int>& eventmap, int timeSlice)
{
	sequence s;
	s.newbitVector(g, a, procData, bitvector, eventmap, timeSlice);
}
void matching(global& g,vector<vector<size_t>>& matchResult, query a, vector<vector<size_t>>& bitvector, unordered_map<string, int>& eventmap, int timeSlice)
{
	match m;
	m.newbitMatch(g,a, bitvector, eventmap, timeSlice);
}
void runBitparell(global& g, fileio fio)
{
	for (int i = 0; i < fio.forNum; i++)
	{
		double totalTime = 0;
		int totalResultNum = 0;

		vector<query> a;
		parseData procData;
		parseInput(a, procData, fio);
		for (int j = 0; j < a.size(); j++)
		{


			vector<vector<size_t>> bitvector;
			unordered_map<string, int> eventmap;
			vector<vector<size_t>> matchResult;

			auto start = std::chrono::high_resolution_clock::now();

			construct(g, a[j], procData, bitvector, eventmap, fio.timeSlice);

			matching(g, matchResult, a[j], bitvector, eventmap, fio.timeSlice);


			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
			double queryTime = (double(duration) / 1000000);

			totalTime += queryTime;
			totalResultNum += g.numresult.size();

			cout << "--------------------------------------------------------------------" << endl;
			cout << "The following is the matching result of Query " << j + 1 << " : " << endl << endl;
			cout << "The number of match: " << g.hmap.size() << endl;
			cout << "The number of match result: " << g.numresult.size() << endl;
			cout << "The total running time of Query " << j + 1 << " : " << queryTime << endl;
			cout << "--------------------------------------------------------------------" << endl << endl;

			vector <int>().swap(g.numresult);

		}
		cout << "*********************************" << endl;
		cout << "The total running time for all queries: " << totalTime << endl;
		cout << "The total matching result number for all queries: " << totalResultNum << endl;

	}
}
void printResult(global g, fileio fio)
{
	fio.output(g, fio);
	cin.get();
}
void start()
{
	fileio fio;
	global g;
	loadInfo(fio);
	runBitparell(g, fio);
	printResult(g, fio);
}

int main()
{
	start();
}


