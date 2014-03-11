/* dbg.h
* dbg.c header file. Contains macro declarations for assert, throw and warn.
* Written by: David Wotherspoon
*/
#include "tty_mbed.h"

void dbg_assert(char * msg, char * file, int line);
void dbg_warn(char * msg, char * file, int line);
void dbg_throw(char * msg, char * file, int line);

#define ASSERT(X, Y)  { if (X) {} else { dbg_assert(Y, __FILE__, __LINE__); } } 
#define WARN(X, Y)    { if (X) {} else { dbg_warn(Y, __FILE__, __LINE__);   } }
#define THROW(X)	  dbg_throw(X, __FILE__, __LINE__);