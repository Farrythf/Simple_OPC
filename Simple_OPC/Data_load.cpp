//Description: This file is used for loading data

//author Hongfei Tian @ GEIRINA

//Last modify 2019-06-28

#include "Data_load.h"

float load[96][12], PV[96][12], net_load[96][12];
#define RATIO_PV 0.2
#define RATIO_ES 0.2
#define RATIO_LOAD 1
#define CAPACITY_LOAD 30
#define CAPACITY_PV 1500
using namespace std;

void Data_load()
{
	//read data from load file
	ifstream file("GEIRINA_load.csv", ios::in);
	string line;
	int a = 0, b = 0;
	while (getline(file, line))
	{
		stringstream ss(line);
		string str;
		while (getline(ss, str, ','))
		{
			load[a][b] = atof(str.c_str());
			b = b + 1;
		}
		a = a + 1;
		b = 0;
	}
	file.close();

	//read data from pv file
	a = 0;
	b = 0;
	ifstream file1("PV.csv", ios::in);
	while (getline(file1, line))
	{
		stringstream ss(line);
		string str;
		vector<string> lineArray;
		while (getline(ss, str, ','))
		{
			PV[a][b] = atof(str.c_str());
			b = b + 1;
		}
		a = a + 1;
		b = 0;
	}
	file1.close();

	//normalization
	float united_load[96][12], united_PV[96][12];
	for (int i = 0; i < 96; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			united_load[i][j] = RATIO_LOAD * load[i][j] / CAPACITY_LOAD;
		}
	}
	for (int i = 0; i < 96; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			united_PV[i][j] = RATIO_PV * PV[i][j] / CAPACITY_PV;
		}
	}
	
	//calculate netload
	for (int i = 0; i < 96; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			net_load[i][j] = united_load[i][j] - united_PV[i][j];
		}
	}
}