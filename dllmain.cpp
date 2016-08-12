#include "dll.h"

#include "share.h"
#include "direct.h"
#include "Runner.h"
#include "FuncArgs.h"
#include "ElementData.h"

PCodeGenTools cgt;

DLLIMPORT int buildPrepareProc(TBuildPrepareRec *params) {
	CG_LOG_BEGIN

	_log_clear_();

	CG_LOG_RETURN(CG_SUCCESS)
}

void eraseData(id_sdk sdk) {
	CG_LOG_BEGIN

	for(int i = 0; i < cgt->sdkGetCount(sdk); i++) {
		id_element e = cgt->sdkGetElement(sdk, i);
		ElementData *data = (ElementData *)cgt->elGetData(e);
		if(data) {
			delete data;
			cgt->elSetData(e, NULL);
		}
		
		if(cgt->elGetClassIndex(e) == CI_MultiElement) {
			eraseData(cgt->elGetSDK(e));
		}
	}

	CG_LOG_END
}

DLLIMPORT int buildProcessProc(TBuildProcessRec *params) {
	CG_LOG_BEGIN

	initObjects();

	id_element e;
	cgt = params->cgt;
	erun = new TElementRunner();
	TArgs *args = new TArgs();

	ElementData::init();
	for (int i = 0; i < params->cgt->sdkGetCount(params->sdk); i++) {
		e = cgt->sdkGetElement(params->sdk, i);
		if(cgt->elGetFlag(e) & ELEMENT_FLG_IS_PARENT) {
			erun->createTools(e);
			TValue::free( erun->run(e, "dostart", args));
			erun->destroyTools(e);
			break;
		}
	}

	delete args;
	delete erun;
	
	// erase elements data
	eraseData(params->sdk);

	std::string str;
	TBlockObject *block = (TBlockObject*)searchObject("block");
	TBlockItemObject *item = block->searchBlock("result");
	if(item) {
		str = item->block->asCode();
	}
	else {
		str.append("block 'result' not found");
	}
	char *ch = new char[str.length()+1];
	strcpy(ch, str.c_str());
	params->result = ch;

	clearObjects();
	delete gvars;
	TValue::_dump();

	CG_LOG_RETURN(CG_SUCCESS)
}

DLLIMPORT int CheckVersionProc(THiAsmVersion *params) {
	CG_LOG_BEGIN
//	if ((params->major >= 3) && (params->minor >= 63) && (params->build >= 162))
//		return CG_SUCCESS;
	CG_LOG_RETURN(CG_SUCCESS)
}

/*
BOOL APIENTRY DllMain (HINSTANCE hInst, DWORD reason, LPVOID reserved) {
	switch (reason) {
	  case DLL_PROCESS_ATTACH:
		break;

	  case DLL_PROCESS_DETACH:
		break;

	  case DLL_THREAD_ATTACH:
		break;

	  case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}
 */

int main() {
	printf("LOAD LIB\n");
	return 0;
}
