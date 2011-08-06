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
	ScriptFuncIDs(int32 onMessageId, int32 onProximityId, int32 onTimerId) 
		: onMessage(onMessageId), onProximity(onProximityId), onTimer(onTimerId) {}
	int32 onMessage, onProximity, onTimer;
};
class ContextRunner : public Task {
	asIScriptContext* mScriptContext;
	ScriptFuncIDs mFuncIDs;
public:
	ContextRunner(ConcreteDirectedChannel* channel) : Task(channel) {
		mScriptContext = createASContext(0);
	}
	void handleMessage(Message msg) {
		if(msg->type == MsgType::DataMsgType) {
			CreatureMessage* cm = (CreatureMessage*)msg->dataMsg.value;
			switch(cm->message) {
			case CreatureMsgType::Message:

				break;
			case CreatureMsgType::Quit:
				//call quit
				break;
			}
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
		/*DataMsg msg;
		uint64 ticks = Timer::tick();
		CreatureMessage* cm = new CreatureMessage();
		cm->creature = 0;
		cm->message = CreatureMsgType::GetAction;
		msg.len = sizeof(CreatureMessage*);
		msg.value = (void*)cm;
		cout <<"addr: " <<ios_base::hex <<int(&cm) <<endl;
		//while(ticks +1000 > Timer::tick()) {
		sendMessage(msg, mCR);
		*///	Timer::sleep(700);
		//}
	}
};

int main(int argc, char** argv) {
	WindowManager::setup();
	World world(50, 50);
	WorldWindow wrldwnd(world);
	InputManager::setup();
	setupAS();
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

void as_main() {
	setupAS();
	asIScriptContext* ctxt = createASContext(0);
	asIScriptModule* module = compileFile("../../scripts/test.as");
	int funcid = engine->GetModule(0)->GetFunctionIdByDecl("void main()");
	if(funcid < 0) {
		cout <<"Failed to retrieve function id" <<endl;
		return -1;
	}
	int r = ctxt->Prepare(funcid);
	r = ctxt->Execute();
	if(r == asEXECUTION_FINISHED)
		cout <<"done" <<endl;
	else
		cout <<"r = " <<r <<endl;
	Timer::sleep(3000);
	ctxt->Release();
	engine->Release();
}