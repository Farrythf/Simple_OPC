#pragma once

//author Hongfei Tian @ GEIRINA

//Last modify 2019-06-28

#ifndef _M_DEPEND_H
#define _M_DEPEND_H

#include <iostream>
#include <time.h>
#include <fstream>
#include <math.h>
#include <string>
#include <unordered_map>
#include <set>
#include <stdlib.h>
#include <sstream>
#include <ctime>
#include <vector>  
#include <fstream>  
#include <sstream>  
#include <assert.h>
#include <comutil.h>
#include <excpt.h>
#include <tchar.h>
#include <windows.h>
#include <libpq-fe.h>
//#include "debug.h"
//#include <mutex>
//#include <list>
//#include <memory>


#include "Simple_Algorithms.h"
#include "Data_load.h"
#include "TimerEngine.h"
#include "BMS_read.h"


extern float load[96][12], PV[96][12], net_load[96][12];
extern PGconn* conn;

#endif