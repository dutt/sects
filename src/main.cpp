#include <iostream>
using namespace std;

#include "windowmgr.h"
#include "inputmgr.h"

#include "scriptmgr.h"
#include "group.h"
#include "timer.h"
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
	ScriptManager& mScriptMgr;
	ScriptFuncIDs mFuncIDs;
	uint32 mTimeout;
	uint32* mScriptTimeout;
	void prepareFuncIDs() {
		mFuncIDs.onMessage = mScriptMgr.getModule("behaviour")->GetFunctionIdByDecl("void onMessage(uint32 msg)");
		assert(mFuncIDs.onMessage > 0);
		mFuncIDs.onQuit = mScriptMgr.getModule("behaviour")->GetFunctionIdByDecl("void onQuit()");
		assert(mFuncIDs.onQuit > 0);
	}
public:
	ContextRunner(ConcreteDirectedChannel* channel, ScriptManager& scriptManager) : Task(channel), mScriptMgr(scriptManager) {
		mScriptTimeout = new uint32;
		*mScriptTimeout = 0;
		mTimeout = 1000*4;
		mScriptContext = mScriptMgr.createASContext(mScriptTimeout);
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
			delete cm;
			*mScriptTimeout = Timer::tick() + mTimeout;
			assert(r >= 0);
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
	Group* mRunners;
	void sendCreatureMessage(CreatureMsgType::CreatureMsgType_t msg) {
		DataMsg dmsg;
		CreatureMessage* cm = new CreatureMessage();
		cm->creature = 0;
		cm->message = msg;
		dmsg.value = (void*)cm;
		sendMessage(dmsg, mRunners);
	}
public:
	MTWorld(ConcreteDirectedChannel* channel, Group* runners) : MessageClient(channel), mRunners(runners) {}
	void operator()() {

		/*DataMsg msg;
		CreatureMessage* cm = new CreatureMessage();
		cm->creature = 0;
		cm->message = CreatureMsgType::Message;
		msg.value = (void*)cm;
		sendMessage(msg, mCR);

		CreatureMessage* cm2 = new CreatureMessage();
		cm2->message = CreatureMsgType::Quit;
		msg.value = (void*)cm2;
		sendMessage(msg, mCR);*/
		mRunners->setMode(GroupMode::FIFO);
		sendCreatureMessage(CreatureMsgType::Message);
		sendCreatureMessage(CreatureMsgType::Message);
		mRunners->setMode(GroupMode::Broadcast);
		sendCreatureMessage(CreatureMsgType::Quit);
	}
};

void runTypeCreation(ScriptManager& mgr) {
	asIScriptContext* context = mgr.createASContext(0);
	int32 funcID = mgr.getModule("init")->GetFunctionIdByDecl("void createTypes()"); assert(funcID > 0);
	context->Prepare(funcID);
	context->Execute();
}
int as_main();
int main(int argc, char** argv) {
	//as_main();
	//return 0;

	//world and viewing
	WindowManager::setup();
	World world(50, 50);
	WorldWindow wrldwnd(world);
	InputManager::setup();
	//Player* player = new Player(world, 
	//	WindowManager::getScreenWidth() / 2, WindowManager::getScreenHeight() / 2);
	bool keys[256]; (void)keys;

	//scripting
	ScriptManager scriptmgr;
	if(!scriptmgr.setupAS())
		return -1;
	scriptmgr.compileFile("../../scripts/init.as", "init");
	scriptmgr.compileFile("../../scripts/behaviour.as", "behaviour");
	runTypeCreation(scriptmgr);

	//run unit creation

	//threading
	ConcreteDirectedChannel channel;
	Group contextRunners(&channel, GroupMode::FIFO);
	ContextRunner cr1(&contextRunners, scriptmgr);
	ContextRunner cr2(&contextRunners, scriptmgr);
	MTWorld mtworld(&channel, &contextRunners);
	/*new boost::thread(cr1);
	new boost::thread(cr2);
	new boost::thread(contextRunners);
	new boost::thread(mtworld);*/
	cr1.start();
	cr2.start();
	contextRunners.start();
	mtworld.start();

	//old main loop
	/*
	uint64 ticks = Timer::tick();
	uint64 lastUpdate = 0;
	while(true) {
		if(InputManager::keyDown()) {
			InputManager::updateKeys(keys);
			if (keys[InputManager::K_ESCAPE])
				break;
			player->handleInput(keys);
			window.setWorldX(player->getX() -
				(WindowManager::getScreenWidth() / 2));
			window.setWorldY(player->getY() -
				(WindowManager::getScreenHeight() / 2));
			WindowManager::refresh();
		} else {
			if(ticks > lastUpdate + 1000) {
				world.update();
				WindowManager::refresh();
				lastUpdate = ticks;
			}
			else
				ticks = Timer::tick();
			Timer::sleep(1);
			continue;
		}
	}/**/
	return 0;
}

int as_main() {
	/*setupAS();
	uint32 timeout = Timer::tick() + 1500;
	asIScriptContext* ctxt = createASContext(&timeout);
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
	engine->Release();*/
	return 0;
}
