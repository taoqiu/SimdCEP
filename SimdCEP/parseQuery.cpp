#include "parseQuery.h"
#include "global.h"
using namespace std;
global g;
int query::parseFastQueryLine(query& a, string line)
{

	if (startswith(line, "PATTERN")) {
		parseFastQueryLineStaWithPAattern(line, a);
		return 1;
	}
	else if (startswith(line, "WHERE")) {
		// parse the selection strategy
		vector<string>tokens;
		g.Tokenize(line, tokens, " ");
		a.selectionStrategy = tokens[1];
		return 1;
	}
	else if (startswith(line, "AND")) {
		parseFastQueryLineStartWithAND(line, a);
		return 1;
	}
	else if (startswith(line, "WITHIN")) {
		// parse the time window
		vector<string>tokens;
		g.Tokenize(line, tokens, " ");
		a.timeWindow = stoi(tokens[1]);
		return 1;
	}
	else if (line == "--------------------")
	{
		return 2;
	}
}

bool query::startswith(const std::string& str, const std::string& start)
{
	int srclen = str.size();
	int startlen = start.size();
	if (srclen >= startlen)
	{
		string temp = str.substr(0, startlen);
		if (temp == start)
			return true;
	}
	return false;
}

string query::trim(string s)
{
	if (s.empty())
		return s;
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

void query::parseFastQueryLineStartWithAND(string line, query& a)
{
	vector<string>tokens;
	g.Tokenize(line, tokens, " ");
	vector<string>::iterator it = tokens.begin() + 1;
	if (((*it).find_first_of("[")) == 0)
	{
		if (!a.hasPartitionAttribute)
		{
			a.hasPartitionAttribute = true;
			a.PartitionAttribute.push_back((*it).substr(1, (*it).length() - 2));
		}
		else
		{
			a.hasMorePartitionAttribute = true;
			a.morePartitionAttribute.push_back((*it).substr(1, (*it).length() - 2));
		}
	}
	else
	{
		char initial = tokens.data()->at(0);
		int stateNum = initial - 'a';
		vector<string> predicate;
		predicate.push_back(trim(line.substr(3)));
		a.addPredicate.push_back(predicate);
	}
}

void query::parseFastQueryLineStaWithPAattern(string line, query& a)
{
	string seq = line.substr(line.find_first_of('(') + 1, (line.size() - (line.find_first_of('(') + 2)));
	//cout << "this is seq**     " << seq << endl;
	vector<string> pat;
	g.Tokenize(seq, pat, ",");
	int size = pat.size();
	/*for (int i = 0; i < size; i++)
	{
		cout << "pat " << i << " is:" << pat[i] << endl;
	}*/
	vector<string>::iterator it = pat.begin();
	for (int i = 0; i < size; i++)
	{
		bool iskleeneClosure = false;
		bool isNegation = false;
		bool isOr = false;
		vector<string> onepat;
		g.Tokenize(*it, onepat, " ");
		//for (int i = 0; i < onepat.size(); i++)
		//{
		//	cout << "onepat " << i << " is:" << onepat[i] << endl;
		//}
		string eventType = onepat[0];
		string eventReal = onepat[1];
		//cout << eventType << endl << eventReal << endl;
		if (eventType.find("+") != -1)
		{
			iskleeneClosure = true;
			eventType = eventType.substr(0, eventType.length() - 1);
		}
		else if (eventType.find("!") != -1)
		{
			isNegation = true;
			eventType = eventType.substr(1, eventType.length() - 1);
		}
		else if (eventType.find("|") != -1)
		{
			isOr = true;
			eventType = eventType.substr(1, eventType.length() - 1);
		}
		if (iskleeneClosure)
		{
			a.pt.evtype.push_back(eventType);
			a.pt.streal.push_back(eventReal);
			a.pt.sttype.push_back("kleeneClosure");
		}
		else if (isNegation)
		{
			a.pt.evtype.push_back(eventType);
			a.pt.streal.push_back(eventReal);
			a.pt.sttype.push_back("negation");
		}
		else if (isOr)
		{
			a.pt.evtype.push_back(eventType);
			a.pt.streal.push_back(eventReal);
			a.pt.sttype.push_back("or");
		}
		else
		{
			eventType = eventType.substr(0, eventType.length());
			a.pt.evtype.push_back(eventType);
			a.pt.streal.push_back(eventReal);
			a.pt.sttype.push_back("normal");
		}
		it++;
	}
}