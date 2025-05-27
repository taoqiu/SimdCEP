#include "parseResult.h"
using namespace std;
void result::FindPosition(global& g,const unsigned& k, size_t num)
{
	size_t r1 = 0;
	size_t temp = 0;
	while (num)
	{
		static const int MultiplyDeBruijinBitPosition1[64] =
		{
			0, // change to 1 if you want bitSize(0) = 1
			1,  2, 53,  3,  7, 54, 27, 4, 38, 41,  8, 34, 55, 48, 28,
			62,  5, 39, 46, 44, 42, 22,  9, 24, 35, 59, 56, 49, 18, 29, 11,
			63, 52,  6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,
			51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12
		};
		temp = (num & (-num));
		r1 = MultiplyDeBruijinBitPosition1[((temp) * 0x022FDD63CC95386DU) >> 58];
		r1 = k * 64 + (63 - r1);
		g.numresult.push_back(r1);
		num = num - temp;
	}
}
void result::finalResult(global& g, vector<vector<size_t>> result)
{
	for (int i = 0; i < result[result.size() - 1].size(); i++)
	{
		FindPosition(g,i, result[result.size() - 1][i]);
	}
	for (int i = 0; i < g.numresult.size(); i++)
	{
		g.candis.push_back(g.hmap[g.numresult[i]]);
	}
}