#include "scriptmgr.h"
#include <../../add_on/scriptstdstring/scriptstdstring.h>
#include "timer.h"
#include <assert.h> 
#include <iostream>
using namespace std;

//
// Global functions for scripts
void as_print(string& text) {
	cout <<"script talking: \"" <<text <<"\"" <<endl;
}

void as_sleep(uint32 ms) {
	Timer::sleep(ms);
}

void ScriptManager::registerEntityType(string name) {
	cout <<"Registering type \"" <<name <<"\"" <<endl;
}

//
// Script mEngine callbacks
void asMsgCallback(const asSMessageInfo* msg, void* param) {
	const char* type = "ERR ";
	if(msg->type == asMSGTYPE_WARNING)
		type = "WARN ";
	else if(msg->type == asMSGTYPE_INFORMATION)
		type = "INFO ";
	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

void asLineCallback(asIScriptContext* context, uint32* timeout) {
	if(*timeout != 0 && *timeout < Timer::tick()) {
		cout <<"Script timed out" <<endl;
		context->Abort();
	}
}

void ScriptManager::registerGlobalFunctions() {
	int r = mEngine->RegisterGlobalFunction("void print(string& in)", asFUNCTION(as_print), asCALL_CDECL); assert(r >= 0);
	r = mEngine->RegisterGlobalFunction("void sleep(uint32)", asFUNCTION(as_sleep), asCALL_CDECL); assert(r >= 0);
}

void ScriptManager::registerEntityInterface() {
	int32 r = mEngine->RegisterInterface("Entity"); assert(r >= 0);
	r = mEngine->RegisterInterfaceMethod("Entity", "void create(uint32 x, uint32 y)"); assert(r >= 0);
}

bool ScriptManager::setupAS() {
	if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY")) {
		cout <<"AngelScript option AS_MAX_PORTABILITY compiled, this isn't supported in sects" <<endl;
	}
	mEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	if(!mEngine) {
		cout <<"Failed to create script mEngine" <<endl;
		return false;
	}
	int32 r = mEngine->SetMessageCallback(asFUNCTION(asMsgCallback), 0, asCALL_CDECL);
	if(r < 0) {
		cout <<"Failed to set line callback function" <<endl;
		mEngine->Release();
		return false;
	}
	RegisterStdString(mEngine);
	registerGlobalFunctions();
	registerEntityInterface();
	return true;
}

asIScriptContext* ScriptManager::createASContext(uint32* timeout) {
	asIScriptContext* context = mEngine->CreateContext();
	int32 r = context->SetLineCallback(asFUNCTION(asLineCallback), timeout, asCALL_CDECL);
	if(r < 0) {
		cout <<"Failed to set line callback function" <<endl;
		context->Release();
		return 0;
	}
	return context;
}

asIScriptModule* ScriptManager::compileFile(const char* path) {
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
	asIScriptModule* module = mEngine->GetModule(0, asGM_ALWAYS_CREATE);
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
