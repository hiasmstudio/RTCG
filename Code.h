#ifndef _CODE_
#define _CODE_

#include "FuncArgs.h"
#include "CGTShare.h"
#include "CodeTree.h"

class TUnitNode;

class TCode {
  private:
  public:
    char *name;
	char codePath[512];
	TUnitNode *root;
  
    TCode();
    ~TCode();
	
    TValue* run(id_element e, const char *entry, TArgs *args);
    int parseUnit(id_element e, const char *unit);
	
	void dump();
};

#endif
