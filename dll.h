#ifndef _DLL_H_
#define _DLL_H_

#ifdef BUILDING_DLL
# define DLLIMPORT extern "C" __declspec(dllexport)
#else
#define DLLIMPORT extern "C"
#endif

#endif
