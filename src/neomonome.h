#ifndef __NEOMONOME_H__
#define __NEOMONOME_H__

#define NEOMONOME_VERSION "0.1"

#ifndef NEOMONOME_API
#define NEOMONOME_API __attribute__ ((visibility ("default")))
#endif

// extern "C" {

#include "lua.h"
#include "lauxlib.h"

NEOMONOME_API int luaopen_neomonome(lua_State *L);

// } // extern "C"

#endif
