#ifndef _SCRIPTING_
#define _SCRIPTING_

#include <angelscript.h>
#include <string>
#include "typedefs.h"

class ScriptManager {
public:

	bool setupAS();

	asIScriptModule* getModule(const char* name) { return mEngine->GetModule(0); }
	
	asIScriptContext* createASContext(uint32* timeout);
	asIScriptModule* compileFile(const char* path);
private:
	void registerGlobalFunctions();
	void registerEntityInterface();
	void registerEntityType(std::string name);
	asIScriptEngine* mEngine;
};

#endif //_SCRIPTING_
