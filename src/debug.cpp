#include "debug.h"
#include <string.h>

#ifndef KIMERA_NO_DEBUG

uint  TraceFunc::_callcount;

const char*
TraceFunc::prefix()
{ 
  static char str[20];
  memset(str, '#', sizeof(str));
  str[qMin(_callcount, sizeof(str) - 1)] = 0;
  return str;
}

#endif
