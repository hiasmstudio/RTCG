#include "IntFunc.h"
#include "Runner.h"
#include <regex>

#define RET_EMPTY return new TValue("empty");

TUserTypes userTypes;

TValue *map_replace(void *node, TArgs *args, Context &context) {
	std::string str = (const char*) args->value(0)->toStr();
	std::string from = (const char*) args->value(1)->toStr();
	std::string to = (const char*) args->value(2)->toStr();
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	char *buf = new char[str.size() + 1];
	std::copy(str.begin(), str.end(), buf);
	buf[str.size()] = '\0';
	TValue *val = new TValue(buf, false);
	delete[] buf;
	CG_LOG_RETURN(val)
}

TValue *map_lower(void *node, TArgs *args, Context &context) {
	const char *src = args->value(0)->toStr();
	char *buf = new char[strlen(src)+1];
	lowerCase(buf, src);
	TValue *val = new TValue(buf, args->value(0)->flags & FLG_CODE);
	delete buf;
	CG_LOG_RETURN(val)
}

TValue *map_upper(void *node, TArgs *args, Context &context) {
	const char *src = args->value(0)->toStr();
	char *buf = new char[strlen(src)+1];
	upperCase(buf, src);
	TValue *val = new TValue(buf, args->value(0)->flags & FLG_CODE);
	delete buf;
	CG_LOG_RETURN(val)
}

TValue *map_copy(void *node, TArgs *args, Context &context) {
	std::string s = args->value(0)->toStr();
	int pos = args->value(1)->toInt();
	int len = args->value(2)->toInt();
	char buf[len+1];
	std::size_t length = s.copy(buf, len, pos);
	buf[length] = '\0';
	CG_LOG_RETURN(new TValue(buf, false))
}

TValue *map_pos(void *node, TArgs *args, Context &context) {
	std::string s = args->value(0)->toStr();
	int pos = args->size() == 3 ? args->value(2)->toInt() : 0;
	CG_LOG_RETURN(new TValue((int)s.find(args->value(1)->toStr(), pos)))
}

TValue *map_delete(void *node, TArgs *args, Context &context) {

	RET_EMPTY
}

TValue *map_split(void *node, TArgs *args, Context &context) {
	std::string str = (const char*) args->value(0)->toStr();
	std::string delimiter = (const char*) args->value(1)->toStr();
	std::string token;
	size_t pos = 0;
	TArrayObject *aobj = new TArrayObject();
	while ((pos = str.find(delimiter)) != std::string::npos) {
		token = str.substr(0, pos);
		aobj->add(new TValue((char *) token.c_str()));
		str.erase(0, pos + delimiter.length());
	}
	if (str.length() > 0)
		aobj->add(new TValue((char *) str.c_str()));

	CG_LOG_RETURN(new TValue(aobj))
}

TValue *map_len(void *node, TArgs *args, Context &context) {
	std::string str = (const char*) args->value(0)->toStr();
	CG_LOG_RETURN(new TValue((int)str.length()))
}

TValue *map_trace(void *node, TArgs *args, Context &context) {
	CG_LOG_BEGIN

	CALL_TRACE("*trace")
	std::string buf;
	for (TArgs::iterator a = args->begin(); a != args->end(); a++) {
		buf += (*a)->value->toStr();
		buf += " ";
	}
	cgt->trace(buf.c_str());

	CG_LOG_RETURN(args->value(0)->duplicate())
}

TValue *map_error(void *node, TArgs *args, Context &context) {
	TTreeNode *n = (TTreeNode*)node;
	while(n->parent && n->nodeType != ND_UNIT) {
		n = n->parent;
	}
	char buf[64];
	sprintf(buf, "%d", ((TCallNode*)node)->line+1);
	std::string s = ((TUnitNode*)n)->codeTree->name;
	s += "[";
	s += buf;
	s += "]: ";
	for (TArgs::iterator a = args->begin(); a != args->end(); a++) {
		s += (*a)->value->toStr();
		s += " ";
	}
	cgt->error(s.c_str());
	RET_EMPTY
}

TValue *map_code(void *node, TArgs *args, Context &context) {
	TValue *val = args->value(0)->duplicate();
	if(val->getType() == DATA_STR) {
		val->flags |= FLG_CODE;
	}
	else {
		TValue *v = new TValue(val->toCode(), true);
		TValue:free(val);
		val = v;
	}
	CG_LOG_RETURN(val)
}

TValue *map_string(void *node, TArgs *args, Context &context) {
	TValue *val = args->value(0)->duplicate();
	if(val->getType() == DATA_STR) {
		if(val->flags & FLG_CODE)
			val->flags ^= FLG_CODE;
	}
	else {
		TValue *v = new TValue(val->toCode());
		TValue:free(val);
		val = v;
	}
	
	CG_LOG_RETURN(val)
}

TValue *map_number(void *node, TArgs *args, Context &context) {
	CG_LOG_RETURN(new TValue(args->at(0)->value->toReal()))
}

int loopCounter = 0;

TValue *map_event(void *node, TArgs *args, Context &context) {
	CG_LOG_BEGIN

	CALL_TRACE_BEGIN("*event")
	const char *pname = args->at(0)->value->toStr();
	id_point p = cgt->elGetPtName(context.element, pname);
	if(p) {
		id_point rp = cgt->ptGetRLinkPoint(p);
		if (rp) {
			id_element pe = cgt->ptGetParent(rp);
			int type = cgt->ptGetDataType(p);
			TArgs *_args = new TArgs;
			// dynamic point index
			const char *event_name = cgt->pt_dpeGetName(rp);
			if(strlen(event_name) == 0)
				event_name = cgt->ptGetName(rp);
			else {
				if(cgt->elGetClassIndex(cgt->ptGetParent(rp)) == CI_DPLElement) {
					_args->add(new TValue(cgt->ptGetName(rp)));
				}
				else {
					_args->add(new TValue(cgt->ptGetIndex(rp)));
				}
			}
			// event args
			for (int i = 1; i < args->size(); i++) {
				TValue *v = new TValue(args->value(i));
				if(type)
					v->setCodeType(type);
				_args->add(v);
			}
			
#ifdef BUILDING_DLL
			char __buf[128];
			strcpy(__buf, event_name);
			event_name =__buf;
#endif
			TValue *ret;
			if(loopCounter < 256) {
				loopCounter++;
				ret = erun->run(pe, event_name, _args);
				loopCounter--;
			}
			else {
				char buf[256];
				sprintf(buf, "Loop detected: %s.%s -> %s, %d", cgt->elGetClassName(context.element), pname, event_name, loopCounter);
				cgt->error(buf);
				ret = new TValue();
				loopCounter = 0;
			}
			delete _args;
#ifdef BUILDING_DLL
			delete event_name;
#endif
			
			CALL_TRACE_END("*event")
			CG_LOG_RETURN(ret)
		}
	}
	else {
		CG_LOG_INFO("Point not found ")
	}

	CG_LOG_RETURN(new TValue())
}

TValue *map_typeof(void *node, TArgs *args, Context &context) {
	return new TValue(args->at(0)->value->flags & FLG_CODE ? 0 : args->at(0)->value->getType());
}

TValue *map_expof(void *node, TArgs *args, Context &context) {
	return new TValue(args->at(0)->value->getCodeType());
}

TValue *convert(TValue *val, int type, Context &context) {
	if(type == 0)
		return val;
	
	TArgs *args = new TArgs();
	args->add(val);
	args->add(new TValue(type));
	val = erun->tools->run(context.element, "to_type", args);
	delete args;
	
	return val;
}

TValue *map_d(void *node, TArgs *args, Context &context) {
	CG_LOG_BEGIN

	CALL_TRACE("*d")
	const char *name = args->value(0)->toStr();
	id_point p = cgt->elGetPtName(context.element, name);
	int type = 0;
	if(p && cgt->ptGetType(p) == pt_Data) {
		type = cgt->ptGetDataType(p);
		id_point link = cgt->ptGetRLinkPoint(p);
		if(link) {
			TArgs *args = new TArgs();
			const char *event_name = cgt->pt_dpeGetName(link);
			if(strlen(event_name) == 0)
				event_name = cgt->ptGetName(link);
			else {
				if(cgt->elGetClassIndex(cgt->ptGetParent(link)) == CI_DPLElement) {
					args->add(new TValue(cgt->ptGetName(link)));
				}
				else {
					args->add(new TValue(cgt->ptGetIndex(link)));
				}
			}
#ifdef BUILDING_DLL
			char __buf[128];
			strcpy(__buf, event_name);
			event_name =__buf;
#endif
			TValue *ret = erun->run(cgt->ptGetParent(link), event_name, args);
			delete args;
#ifdef BUILDING_DLL
			delete event_name;
#endif
			int t = cgt->ptGetDataType(link);
			if(t)
				ret->setCodeType(t);
			
			CG_LOG_RETURN(convert(ret, type, context))
		}
	}

	id_prop prop = cgt->getPropByName(context.element, name);
	if(prop && !cgt->isDefProp(context.element, prop))
		CG_LOG_RETURN(TValue::fromProperty(context.element, prop))

	CG_LOG_RETURN(convert(context.data(), type, context))
}

TValue *map_linked(void *node, TArgs *args, Context &context) {
	id_point p = cgt->elGetPtName(context.element, args->value(0)->toStr());
	if(p)
		return new TValue(cgt->ptGetRLinkPoint(p) > 0);
	return new TValue(false);
}

TValue *map_isdef(void *node, TArgs *args, Context &context) {
	id_prop prop = cgt->getPropByName(context.element, args->value(0)->toStr());
	if(prop)
		CG_LOG_RETURN((new TValue(cgt->isDefProp(context.element, prop))))
	return new TValue(false);
}

TValue *map_sub(void *node, TArgs *args, Context &context) {
	if(args->size() == 0)
		return new TValue(0);
	
	TValue *var = args->value(0);
	if(args->size() == 1)
		return new TValue(var->getCodeType());
	
	var->setCodeType(args->value(1)->toInt());
	
	return var->duplicate();
}

TValue *map_regex_replace(void *node, TArgs *args, Context &context) {
	std::regex r(args->value(1)->toStr());
	std::string result = std::regex_replace(args->value(0)->toStr(), r, args->value(2)->toStr());
	return new TValue((char*)result.c_str());
}

TValue *map_fopen(void *node, TArgs *args, Context &context) {

	RET_EMPTY
}

TValue *map_fputs(void *node, TArgs *args, Context &context) {

	RET_EMPTY
}

TValue *map_fgets(void *node, TArgs *args, Context &context) {

	RET_EMPTY
}

TValue *map_fclose(void *node, TArgs *args, Context &context) {

	RET_EMPTY
}

TValue *map_project_dir(void *node, TArgs *args, Context &context) {
	char *s = cgt->ReadStrParam(PARAM_PROJECT_NAME, context.element);
	std::string p(s);
	int pos = p.find_last_of('/');
	TValue *val = new TValue(p.substr(0, pos+1).c_str(), true);
	delete[] s;
	return val;
}

TValue *map_project_name(void *node, TArgs *args, Context &context) {
	char *s = cgt->ReadStrParam(PARAM_PROJECT_NAME, context.element);
	std::string p(s);
	int pos = p.find_last_of('/');
	TValue *val = new TValue(p.substr(pos + 1, p.length() - pos - 5).c_str(), true);
	delete[] s;
	return val;
}

const TFuncMap func_map[] = {
	// ------------ STRINGS ----------------------------
	{ "replace", 3, map_replace, "str, dst, src"},
	{ "lower", 1, map_lower, "str"},
	{ "upper", 1, map_upper, "str"},
	{ "copy", 3, map_copy, "str, position, length"},
	{ "pos", 3, map_pos, "substr, str, start"},
	{ "delete", 3, map_delete, "str, position, length"},
	{ "split", 2, map_split, "str, delimiter"},
	{ "len", 1, map_len, "str"},

	// ------------ SYSTEM -----------------------------
	{ "trace", -1, map_trace, "text"},
	{ "error", 1, map_error, "text"},
	{ "code", 1, map_code, "string"},
	{ "string", 1, map_string, "string"},
	{ "number", 1, map_number, "string"},
	{ "event", -1, map_event, "ename[, args, ...]"},
	{ "typeof", 1, map_typeof, "var"},
	{ "expof", 1, map_expof, "var"},
	{ "d", 1, map_d, "name"},
	{ "linked", 1, map_linked, "name"},
	{ "isdef", 1, map_isdef, "name"},
	{ "sub", -1, map_sub, "name[, type]"},
	
	{ "regex_replace", 3, map_regex_replace, "source, expression, format"},

	// ------------ FILES ------------------------------
	{ "fopen", 2, map_fopen, "filename, mode"},
	{ "fputs", 2, map_fputs, "id, str"},
	{ "fgets", 1, map_fgets, "id"},
	{ "fclose", 1, map_fclose, "id"},

	// ------------ ENVEROUMENT ------------------------
	{ "project_dir", 0, map_project_dir, ""},
	{ "project_name", 0, map_project_name, ""}
};
