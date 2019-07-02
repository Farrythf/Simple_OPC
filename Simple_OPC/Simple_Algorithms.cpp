//Description: This file is used for Calculating PCS and updating database

//author Hongfei Tian @ GEIRINA

//Last modify 2019-06-28

#include "M_depend.h"

#define SOC_UP_BOUND 90
#define SOC_LOW_BOUND 20
#define PCS_MIN_VALUE 2
#define PCS_MAX_VALUE 28
#define MAX 0.7
#define MIN 0.5
#define PCS_RATE 30


int  Call_EMS()
{
	//file init for test
	std::ofstream f1("PCS.csv", std::ios::app);
	if (!f1)
	{
		std::cout << "Fill open failed!\n";
	}
	std::ofstream f2("NetLoad.csv", std::ios::app);
	if (!f2)
	{
		std::cout << "Fill open failed!\n";
	}
	std::ofstream f3("SOC.csv", std::ios::app);
	if (!f3)
	{
		std::cout << "Fill open failed!\n";
	}
	//////////////////////

	char* s;				//capture the response from db
	float SOC;				//soc from db
	int Temp_PCS = 0;		//pcs for control
	char time[100];			//timestamp format
	char SQL_com[1000];		//quary format
	SYSTEMTIME st = { 0 };
	static int i = 1, j = 0;

	PGresult* result = PQexec(conn, "SELECT * FROM estimate WHERE p in (select max(p) from estimate)");
	std::cout << PQresultErrorMessage(result);
	s = PQgetvalue(result, 0, 2);		//get postgre response
	SOC = atof(s);						//trans to float

	//if netload larger than max, es will discharge
	if (net_load[i][j] > MAX)	
	{
		Temp_PCS = (net_load[i][j] - MAX) * PCS_RATE * 10 * 14;		//单位是百瓦所以乘10
		if (Temp_PCS > PCS_MAX_VALUE * 10)
			Temp_PCS = PCS_MAX_VALUE * 10;
		else if (Temp_PCS < PCS_MIN_VALUE * 10)
			Temp_PCS = PCS_MIN_VALUE * 10;
		if (SOC < SOC_LOW_BOUND * 10)		
			Temp_PCS = 0;
	}
	//if netload lower than min, es will charge
	else if (net_load[i][j] < MIN)
	{
		Temp_PCS = -(MIN - net_load[i][j]) * PCS_RATE * 10 * 14;		//单位是百瓦所以乘10
		if (Temp_PCS < -PCS_MAX_VALUE * 10)
			Temp_PCS = -PCS_MAX_VALUE * 10;
		else if (Temp_PCS > -PCS_MIN_VALUE * 10)
			Temp_PCS = -PCS_MIN_VALUE * 10;
		if (SOC > SOC_UP_BOUND * 10)
			Temp_PCS = 0;
	}
	//if netload in normal field, es will hold
	else
	{
		Temp_PCS = 0;
	}

	/*
	// Process for test
	int inter;
	inter = round((float)Temp_PCS / 36);
	SOC = SOC - inter;
	GetLocalTime(&st);
	sprintf_s(time, "'%d-%d-%d %d:%d:%d.%d'", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	sprintf_s(SQL_com, "INSERT INTO estimate VALUES( %s, %d, %d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)", time, p, (int)SOC);
	result = PQexec(conn, SQL_com);
	std::cout << PQresultErrorMessage(result);
	p++;
	////////////////////////////////
	*/

	GetLocalTime(&st);		//get time
	sprintf_s(time, "'%d-%d-%d %d:%d:%d.%d'", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	sprintf_s(SQL_com, "INSERT INTO pcs_control VALUES( %s, %d)", time, Temp_PCS);
	result = PQexec(conn, SQL_com);
	std::cout << PQresultErrorMessage(result);
	PQclear(result);
	Write_PCS(Temp_PCS);

	//file write for test
	f1 << Temp_PCS << ", ";
	f2 << net_load[i][j] << ", ";
	f3 << SOC << ", ";
	/////////////////////

	i++;
	if (i == 96)
	{
		//file write for test
		f1 << "\n";
		f2 << "\n";
		f3 << "\n";
		/////////////////////

		i = 1;
		j++;
		if (j == 12)
		{
			//file close for test
			f1.close();
			f2.close();
			f3.close();
			/////////////////////
			j = 0;
			i = 1;
			return 0;
		}
	}
	//file close for test
	f1.close();
	f2.close();
	f3.close();
	/////////////////////
}

int Check_SOC()
{
	//get SOC from es
	char* s;
	char time[100];
	char SQL_com[100];
	float cur_SOC,cur_PCS;
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	PGresult* result = PQexec(conn, "SELECT * FROM estimate WHERE times in (select max(times) from estimate)");
	std::cout << PQresultErrorMessage(result);
	s = PQgetvalue(result, 0, 2);
	cur_SOC = atof(s);

	//get PCS from es
	result = PQexec(conn, "SELECT * FROM pcs_control WHERE times in (select max(times) from pcs_control)");
	std::cout << PQresultErrorMessage(result);
	s = PQgetvalue(result, 0, 1);
	cur_PCS = atof(s);

	//if SOC is too high & es is charging or too low & es is discharging, stop control
	if ( (cur_SOC > 90 && cur_PCS < 0) || (cur_SOC < 10 && cur_PCS > 0) )
	{
		sprintf_s(time, "'%d-%d-%d %d:%d:%d.%d'", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		sprintf_s(SQL_com, "INSERT INTO pcs_control VALUES( %s, 0)", time);
		result = PQexec(conn, SQL_com);
		std::cout << PQresultErrorMessage(result);
	}
	PQclear(result);
	return 0;
}


int  Call_EMS_t()				//for future test
{
	std::ofstream f1("PSC.out", std::ios::app);
	if (!f1)
	{
		std::cout << "Fill open failed!\n";
	}
	char* s;
	float SOC,PV,LOAD,net_l;
	int Temp_PCS = 0;
	char time[100];
	char SQL_com[100];
	SYSTEMTIME st = { 0 };
	static int i = 1, j = 0;
	PGresult* result = PQexec(conn, "SELECT * FROM estimate WHERE times in (select max(times) from estimate)");
	std::cout << PQresultErrorMessage(result);
	s = PQgetvalue(result, 0, 2);
	SOC = atof(s);
	result = PQexec(conn, "SELECT * FROM pv_data WHERE date in (select max(date) from pv_data)");
	std::cout << PQresultErrorMessage(result);
	s = PQgetvalue(result, 0, 1);
	PV = atof(s);
	result = PQexec(conn, "SELECT * FROM load WHERE date in (select max(date) from load)");
	std::cout << PQresultErrorMessage(result);
	s = PQgetvalue(result, 0, 1);
	LOAD = atof(s);

	net_l = LOAD - PV;
	if (net_l > MAX)
	{
		Temp_PCS = (net_l - MAX) * PCS_RATE;//////////////////////////////////////////////////////
		if (Temp_PCS > PCS_MAX_VALUE)
			Temp_PCS = PCS_MAX_VALUE;
		else if (Temp_PCS < PCS_MIN_VALUE)
			Temp_PCS = PCS_MIN_VALUE;
		if (SOC < SOC_LOW_BOUND)
			Temp_PCS = 0;
	}
	else if (net_l < MIN)
	{
		Temp_PCS = (MIN - net_l) * PCS_RATE;//////////////////////////////////////////////////////
		if (Temp_PCS > PCS_MAX_VALUE)
			Temp_PCS = -PCS_MAX_VALUE;
		else if (Temp_PCS < PCS_MIN_VALUE)
			Temp_PCS = -PCS_MIN_VALUE;
		if (SOC < SOC_LOW_BOUND)
			Temp_PCS = 0;
	}
	else
	{
		Temp_PCS = 0;
	}
	GetLocalTime(&st);
	/*sprintf_s(time, "'%d-%d-%d %d:%d:%d.%d'", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	sprintf_s(SQL_com, "INSERT INTO pcs_control VALUES( %s, %d)", time, Temp_PCS);
	result = PQexec(conn, SQL_com);
	std::cout << PQresultErrorMessage(result);*/
	PQclear(result);
	i++;
	f1 << Temp_PCS << ", ";
	if (i == 96)
	{
		f1 << "\n";
		i = 1;
		j++;
		if (j == 12)
		{
			f1.close();
			return 0;
		}
	}
	f1.close();
}