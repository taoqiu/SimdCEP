#include "bitSequence.h"
long GetTickCount64(){
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (ts.tv_sec*1000 + ts.tv_nsec/(1000*1000));
}

using namespace std;

void sequence::newbitVector(global& g,query a, parseData procData, vector<vector<size_t>>& bitvector, unordered_map<string, int>& eventmap, int timeSlice)
{


	struct attcons
	{
		vector<string> eventName;
		vector<string> attName;
		vector<string> opNum;
		vector<float>  value;
	};
	vector<attcons> constxt;
	for (int i = 0; i < a.addPredicate.size(); i++)//以下部分用于分析约束条件
	{
		vector<string> curcons;
		g.Tokenize(a.addPredicate[i][0], curcons, " ");

		vector<string> temp;
		g.Tokenize(curcons[0], temp, ".");


		if (constxt.size() == 0)
		{
			attcons curc;
			curc.eventName.push_back(temp[0]);
			curc.attName.push_back(temp[1]);
			curc.opNum.push_back(curcons[1]);
			curc.value.push_back(stoi(curcons[2]));
			constxt.push_back(curc);
		}
		else
		{
			for (int j = 0; j < constxt.size(); j++)
			{
				if (a.addPredicate[i][0][0] == constxt[j].eventName[0][0])
				{
					constxt[j].eventName.push_back(temp[0]);
					constxt[j].attName.push_back(temp[1]);
					constxt[j].opNum.push_back(curcons[1]);
					constxt[j].value.push_back(stoi(curcons[2]));
					break;
				}
				if (j == constxt.size() - 1)
				{
					attcons curc;
					curc.eventName.push_back(temp[0]);
					curc.attName.push_back(temp[1]);
					curc.opNum.push_back(curcons[1]);
					curc.value.push_back(stoi(curcons[2]));
					constxt.push_back(curc);
					break;
				}
			}

		}
	}
	unordered_map<string, string> namemap;
	for (int i = 0; i < a.pt.streal.size(); i++)
	{
		namemap.insert(pair<string, string>(a.pt.evtype[i], a.pt.streal[i]));
	}
	unordered_map<string, int> constxtmap;
	for (int i = 0; i < constxt.size(); i++)
	{
		constxtmap.insert(pair<string, int>(constxt[i].eventName[0], i + 1));
	}
	unordered_map<string, int> strealmap;
	for (int i = 0; i < a.pt.streal.size(); i++)
	{
		strealmap.insert(pair<string, int>(a.pt.streal[i], i + 1));
	}
	for (int i = 0; i < a.pt.streal.size(); i++)
	{
		eventmap.insert(pair<string, int>(a.pt.evtype[i], i + 1));
	}
	for (int i = 0; i < a.pt.evtype.size(); i++)
	{
		size_t ttemp = 0;
		vector<size_t> temp;
		temp.push_back(ttemp);
		bitvector.push_back(temp);
	}
	unordered_map<string, int> tagmap;
	for (int i = 0; i < procData.tag.size(); i++)
	{
		tagmap.insert(pair<string, int>(procData.tag[i], i));
	}
	int tcflag = 0;
	int unitflag = 0;
	size_t changeBit = 0x8000000000000000;
	int tsnum;
	for (int i = 0; i < procData.tag.size(); i++)
	{
		if (procData.tag[i] == "timestamp")
		{
			tsnum = procData.numcor[i];
			break;
		}
	}
	int jisu4 = 0, jisuun = 0;
	int typeNum = procData.txtcor[procData.eventNum];
	int positionnum = 0;
	for (int i = 0; i < procData.numdata[0].size(); i++)
	{

		if ((procData.numdata[tsnum][i] - tcflag) >= timeSlice)
		{
			if (changeBit == 1)
			{
				unitflag++;
				positionnum = 0;
				changeBit = 0x8000000000000000;
				for (int j = 0; j < bitvector.size(); j++)
				{
					size_t ttemp = 0;
					bitvector[j].push_back(ttemp);
				}
				tcflag = procData.numdata[tsnum][i];
			}
			else
			{
				changeBit = changeBit >> 1;
				positionnum++;
				tcflag = tcflag + timeSlice;
			}

		}
		string cor = procData.txtdata[typeNum][i];
		if (eventmap[cor] != 0)
		{
			int consnum = constxtmap[namemap[cor]];
			if (consnum != 0)
			{
				for (int j = 0; j < constxt[consnum - 1].attName.size(); j++)
				{
					int tagnum = tagmap[constxt[consnum - 1].attName[j]];
					if (constxt[consnum - 1].opNum[j] == ">")
					{
						if (procData.numdata[procData.numcor[tagnum]][i] <= constxt[consnum - 1].value[j])
						{
							break;
						}
					}
					else if (constxt[consnum - 1].opNum[j] == "<")
					{
						if (procData.numdata[procData.numcor[tagnum]][i] >= constxt[consnum - 1].value[j])
						{
							break;
						}
					}
					else
					{
						if (procData.numdata[procData.numcor[tagnum]][i] != constxt[consnum - 1].value[j])
						{
							break;
						}
					}
					if (j == constxt[consnum - 1].attName.size() - 1)
					{
						bitvector[eventmap[cor] - 1][unitflag] |= changeBit;
						event ea;
						ea.timestamp = procData.numdata[tsnum][i];
						ea.type = cor;
						if (cor == a.pt.evtype[0])
						{
							g.hmap.insert({ (unitflag * 64) + positionnum,ea });
						}


					}
				}
			}
			else
			{

					bitvector[eventmap[cor] - 1][unitflag] |= changeBit;
					event ea;
					ea.timestamp = procData.numdata[tsnum][i];
					ea.type = cor;
					if (cor == a.pt.evtype[0])
					{
						g.hmap.insert({ (unitflag * 64) + positionnum,ea });
					}
				
			}
		}
	}
	long t6 = GetTickCount64();
}
