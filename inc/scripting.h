#ifndef _SCRIPTING_
#define _SCRIPTING_

#include <iostream>

#include <angelscript.h>
#include <../../add_on/scriptstdstring/scriptstdstring.h>

#include "timer.h"
#include "typedefs.h"
using namespace std;

asIScriptEngine* engine;

int as_print(string& text) {
	cout <<"script talking: \"" <<text <<"\"" <<endl;
	return 5;
}

void as_sleep(uint32 ms) {
	Timer::sleep(ms);
}

void asMsgCallback(const asSMessageInfo* msg, void* param) {
	const char* type = "ERR ";
	if(msg->type == asMSGTYPE_WARNING)
		type = "WARN ";
	else if(msg->type == asMSGTYPE_INFORMATION)
		type = "INFO ";
	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}
bool setupAS() {
	if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY")) {
		cout <<"AngelScript option AS_MAX_PORTABILITY compiled, this isn't supported in sects" <<endl;
	}
	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	if(!engine) {
		cout <<"Failed to create script engine" <<endl;
		return false;
	}
	engine->SetMessageCallback(asFUNCTION(asMsgCallback), 0, asCALL_CDECL);
	RegisterStdString(engine);
	int r = engine->RegisterGlobalFunction("int print(string& in)", asFUNCTION(as_print), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("void sleep(uint32 ms)", asFUNCTION(as_sleep), asCALL_CDECL); assert(r >= 0);
	return true;
}

void asLineCallback(asIScriptContext* context, uint32* timeout) {
	if(*timeout != 0 && *timeout < Timer::tick()) {
		cout <<"Script timed out" <<endl;
		context->Abort();
	}
}

asIScriptContext* createASContext(uint32& timeout) {
	asIScriptContext* context = engine->CreateContext();
	int32 r = context->SetLineCallback(asFUNCTION(asLineCallback), &timeout, asCALL_CDECL);
	if(r < 0) {
		cout <<"Failed to set line callback function" <<endl;
		context->Release();
		return 0;
	}
	return context;
}
asIScriptModule* compileFile(const char* path) {
	//open
	FILE* f = fopen(path, "rb");
	if(f == 0) {
		cout <<"Failed to open file \"" <<path <<"\"" <<endl;
		return 0;
	}
	//get length
	fseek(f, 0, SEEK_END);
	uint32 len = ftell(f);
	fseek(f, 0, SEEK_SET);
	//read
	string script;
	script.resize(len);
	int c = fread(&script[0], len, 1, f);
	fclose(f);
	if(!c) {
		cout <<"Failed to load script file." <<endl;
		return 0;
	}
	//create module
	asIScriptModule* module = engine->GetModule(0, asGM_ALWAYS_CREATE);
	int r = module->AddScriptSection("script", &script[0], len);
	if(r < 0) {
		cout <<"Failed to add script module" <<endl;
		return 0;
	}
	r = module->Build();
	if(r < 0) {
		cout <<"Build() failed" <<endl; //errors will be written to asMsgCallback
		return 0;
	}
	return module;
}
#endif //_SCRIPTING_
