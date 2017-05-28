#ifndef _INTFUNC_H_
#define _INTFUNC_H_

#include "CodeTree.h"
#include "CGTShare.h"

extern PCodeGenTools cgt;

typedef struct {
	const char *name;
	int count;
	internal_proc proc;
	const char *ainfo;
} TFuncMap;

#define func_map_size 30
extern const TFuncMap func_map[func_map_size];

typedef struct {
	std::string name;
	int type;
} TUserType;
typedef std::list<TUserType> TUserTypes;

extern TUserTypes userTypes;
extern TValue *convert(TValue *val, int type, Context &context);
extern TValue *map_get(void *node, TArgs *args, Context &context);

#endif
