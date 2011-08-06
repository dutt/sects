#include <iostream>
using namespace std;

#include "windowmgr.h"
#include "inputmgr.h"

#include "scripting.h"
#include "messagequeue.h"
#include "messageclient.h"
using namespace trassel;

namespace CreatureMsgType {
	enum CreatureMsgType_t {
		Message,
		Proximity,
		Timer,
		Quit
	};
}

struct CreatureMessage {
	Creature* creature;
	CreatureMsgType::CreatureMsgType_t message;
};

struct ScriptFuncIDs {
	ScriptFuncIDs() : onMessage(-1), onProximity(-1), onTimer(-1), onQuit(-1) {}
	ScriptFuncIDs(int32 onMessageID, int32 onProximityID, int32 onTimerID, int32 onQuitID) 
		: onMessage(onMessageID), onProximity(onProximityID), onTimer(onTimerID), onQuit(onQuitID) {}
	int32 onMessage, onProximity, onTimer, onQuit;
};
void printExecuteResult(int32 result) {
	switch(result) {
	case asEXECUTION_FINISHED:
		cout <<"Done" <<endl;
		break;
	case asEXECUTION_ABORTED:
		cout <<"Aborted" <<endl;
		break;
	case asEXECUTION_SUSPENDED:
		cout <<"Suspended" <<endl;
		break;
	default:
		cout <<"Unknown result: " <<result <<endl;
	}
}
class ContextRunner : public Task {
	asIScriptContext* mScriptContext;
	ScriptFuncIDs mFuncIDs;
	uint32 mTimeout;
	uint32 mScriptTimeout;
	void prepareFuncIDs() {
		mFuncIDs.onMessage = engine->GetModule(0)->GetFunctionIdByDecl("void onMessage(uint32 msg)"); assert(mFuncIDs.onMessage > 0);
		mFuncIDs.onQuit = engine->GetModule(0)->GetFunctionIdByDecl("void onQuit()"); assert(mFuncIDs.onQuit > 0);
	}
public:
	ContextRunner(ConcreteDirectedChannel* channel) : Task(channel) {
		mScriptTimeout = 0;
		mTimeout = Timer::tick() + 1000;
		mTimeout = 0;
		mScriptContext = createASContext(mScriptTimeout);
		prepareFuncIDs();
	}
	void handleMessage(Message msg) {
		if(msg->type == MsgType::DataMsgType) {
			CreatureMessage* cm = (CreatureMessage*)msg->dataMsg.value;
			int r = -1;
			switch(cm->message) {
			case CreatureMsgType::Message:
				r = mScriptContext->Prepare(mFuncIDs.onMessage);
				break;
			case CreatureMsgType::Quit:
				r = mScriptContext->Prepare(mFuncIDs.onQuit);
				break;
			default:
				cout <<"Unknown creature message" <<endl;
				r = 0;
				break;
			}
			assert(r >= 0);
			//mScriptTimeout = Timer::tick() + mTimeout;
			printExecuteResult(mScriptContext->Execute());
		}
		else {
			throw "Invalid message type";
		}
		msg->done();
	}
};

class MTWorld : public MessageClient {
	ConcreteDirectedChannel* mChannel;
	ContextRunner* mCR;
public:
	MTWorld(ConcreteDirectedChannel* channel, ContextRunner* cr) : MessageClient(channel), mCR(cr) {}
	void operator()() {
		DataMsg msg;
		CreatureMessage* cm = new CreatureMessage();
		cm->creature = 0;
		cm->message = CreatureMsgType::Message;
		//msg.len = sizeof(CreatureMessage*);
		msg.value = (void*)cm;
		//cout <<"addr: " <<ios_base::hex <<int(&cm) <<endl;
		//while(ticks +1000 > Timer::tick()) {
		sendMessage(msg, mCR);

		///	Timer::sleep(700);
		//}
		CreatureMessage* cm2 = new CreatureMessage();
		cm2->message = CreatureMsgType::Quit;
		msg.value = (void*)cm2;
		sendMessage(msg, mCR);
	}
};

int as_main();
int main(int argc, char** argv) {
	//as_main();
	//return 0;
	WindowManager::setup();
	World world(50, 50);
	WorldWindow wrldwnd(world);
	InputManager::setup();
	setupAS();
	compileFile("../../scripts/test.as");
	bool keys[256];
	(void)keys;
	ConcreteDirectedChannel channel;
	ContextRunner cr(&channel);
	MTWorld mtworld(&channel, &cr);
	new boost::thread(cr);
	new boost::thread(mtworld);
	while(true) {}
	return 0;
}

int as_main() {
	setupAS();
	uint32 timeout = Timer::tick() + 1500;
	asIScriptContext* ctxt = createASContext(timeout);
	asIScriptModule* module = compileFile("../../scripts/test.as");
	int funcid = engine->GetModule(0)->GetFunctionIdByDecl("void main()");
	if(funcid < 0) {
		cout <<"Failed to retrieve function id of main()" <<endl;
		return -1;
	}
	int r = ctxt->Prepare(funcid);
	r = ctxt->Execute();
	if(r == asEXECUTION_FINISHED)
		cout <<"done" <<endl;
	else if(r == asEXECUTION_ABORTED)
		cout <<"aborted" <<endl;
	else if(r == asEXECUTION_SUSPENDED)
		cout <<"suspended" <<endl;
	else
		cout <<"r = " <<r <<endl;
	Timer::sleep(3000);
	ctxt->Release();
	engine->Release();
	return 0;
}
