#include "fileio.h"
using namespace std;
global fg;
void fileio::loadConfig()
{
	char rule[256];
	getcwd(rule, 256);
	string realrule = rule;
	string configRule = realrule + "/config.txt";
	vector<string> configTxt;
	int configNum;
	fg.read1(configRule, configTxt, configNum);
	if (configNum != 5)
	{
		cout << "Not the expected input" << endl;
		exit(100);
	}
	else
	{
		inputQuery = rule + configTxt[0];
		inputData = rule + configTxt[1];
		forNum = stoi(configTxt[2]);
		timeSlice = stoi(configTxt[3]);
		outputResult = rule + configTxt[4];
	}

}

void fileio::output(global g,fileio fio)
{
		ofstream outfile;
		outfile.open(fio.outputResult);
		for (int i = 0; i < g.candis.size(); i++)
		{
			outfile << "timestamp: " << g.candis[i].timestamp << " event type "<< g.candis[i].type<< endl;
		}
		outfile.close();
}
