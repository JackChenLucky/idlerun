#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <stdio.h>
#include <stdlib.h>
#include <node.h>
#include <v8.h>
#include <uv.h>
#include <windows.h>
#include <direct.h>
#include "nan.h"

using namespace v8;
using namespace node;

struct Emitter: ObjectWrap {
	static Handle<Value> New(const Arguments& args);
	static Handle<Value> Idle(const Arguments& args);
};

int idleTime = 0;

uv_loop_t *loop;
uv_process_t child_req;
uv_process_options_t options;

static const unsigned int interval = 1000;
int timeout, timeoutSeconds;
int timeIdle = 0;
LASTINPUTINFO lif;
DWORD tickCount;
Local<Function> idleCB;

void track_idle_time(uv_timer_t *handle, int status) {

	GetLastInputInfo(&lif);
	tickCount = GetTickCount();
	timeIdle = (tickCount - lif.dwTime) / 1000;

	if(timeIdle >= timeoutSeconds) {
		//printf("Idle %d seconds > %d seconds\n", timeIdle, timeoutSeconds);

		/*
		Local<Object> global = Context::GetCurrent()->Global();
		Local<Object> emitter = global->Get(String::NewSymbol("Emitter::Tock"))->ToObject();
		MakeCallback(emitter, "emit", 1, argv);
		*/
		/*
		Handle<Value> argv[2] = {
			String::New("tock"),
			Number::New(timeIdle)
		};
		*/
		idleTime = timeIdle;
		//FunctionTemplate::New(Emitter::Idle)->GetFunction(argv);
		//const unsigned argc = 1;
		Handle<Value> argv[1] = { Local<Value>::New(Number::New(timeIdle)) };
		//Local<Value> argv[argc] = { Local<Value>::New(timeout) };
		idleCB->Call(Context::GetCurrent()->Global(), 1, argv);
	}
}

int idle_loop() {
	uv_loop_t *main_loop = uv_default_loop();
	uv_timer_t idler;

	uv_timer_init(main_loop, &idler);
	//uv_unref((uv_handle_t*) &idler);
	uv_timer_start(&idler, track_idle_time, timeoutSeconds, interval);
	//uv_unref((uv_handle_t*)&idler);
	return uv_run(main_loop, UV_RUN_DEFAULT);
	//return idleTime;
}

int idling(int usertimeout) {
	lif.cbSize = sizeof(LASTINPUTINFO);
	//timeout = usertimeout->NumberValue();
	timeoutSeconds = usertimeout / 1000;

	/*uv_thread_t idle_id;
	uv_thread_create(&idle_id, IdlingLoop, 0);
	uv_thread_join(&idle_id);*/

	return idle_loop();
}

Handle<Value> Emitter::New(const Arguments& args) {
	HandleScope scope;

	assert(args.IsConstructCall());
	Emitter* self = new Emitter();
	self->Wrap(args.This());

	return scope.Close(args.This());
}

Handle<Value> Emitter::Idle(const Arguments& args) {
	HandleScope scope;

	Handle<Value> argv[2] = {
		String::New("idle"), // event name
		args[0]->ToString()  // argument
	};

	MakeCallback(args.This(), "emit", 2, argv);

	return Undefined();
}

Handle<Value> activity(const Arguments& args) {
	HandleScope scope;

	Local<Object> obj = Object::New();
	obj->Set(String::NewSymbol("idle"), Number::New(idleTime));

	return scope.Close(obj);
}

Handle<Value> idle(const Arguments& args) {
	HandleScope scope;

	//Local<Number> timeout = Number::New(args[0]->NumberValue());
	int timeout = args[0]->NumberValue();

	idleCB = Local<Function>::Cast(args[1]);

	/*
	Local<Function> cb = Local<Function>::Cast(args[1]);
	const unsigned argc = 1;
	Local<Value> argv[argc] = { Local<Value>::New(Number::New(idleTime)) };
	//Local<Value> argv[argc] = { Local<Value>::New(timeout) };
	cb->Call(Context::GetCurrent()->Global(), argc, argv);
	*/


	Local<FunctionTemplate> t = FunctionTemplate::New(Emitter::New);
	t->InstanceTemplate()->SetInternalFieldCount(1);
	t->SetClassName(String::New("Emitter"));
	NODE_SET_PROTOTYPE_METHOD(t, "idle", Emitter::Idle);


	Local<Object> obj = Object::New();
	//obj->Set(String::NewSymbol("timeout"), timeout);
	//obj->Set(String::NewSymbol("activity"), FunctionTemplate::New(activity)->GetFunction());
	obj->Set(String::NewSymbol("Emitter"), t->GetFunction());

	idling(timeout);

	return scope.Close(obj);

	//return scope.Close(Undefined());
}

extern "C" void init(Handle<Object> exports, Handle<Object> module) {
	module->Set(String::NewSymbol("exports"),
		FunctionTemplate::New(idle)->GetFunction());
}

NODE_MODULE(idlerun, init);
