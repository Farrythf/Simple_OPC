#pragma once

//author Hongfei Tian @ GEIRINA

//Last modify 2019-07-01

#ifndef _BMS_READ_H
#define _BMS_READ_H

#include "M_depend.h"

class ReadESS 
{
public:

	int init();
	int Send_Recv();
	int Write_PCS(int User_num);
private:

	int iRet = 0;															//a global identifier for error detect
	SOCKET clientSocket = NULL;
	int filecount = 0;
	void welcome_info();

};




#endif
