#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <stdio.h>
#include <stdlib.h>
#include <node.h>
#include <node_buffer.h>
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

uv_loop_t *loop;
uv_process_t child_req;
uv_process_options_t options;

LASTINPUTINFO lif;
DWORD tickCount;
Local<Function> idleCallback;

static unsigned bool Idle = false;
static const unsigned int interval = 1000;
static unsigned int Timeout,
					TimeoutSeconds,
					IdleTime = 0;

void GetIdleTime(uv_timer_t *handle, int status) {
	GetLastInputInfo(&lif);
	tickCount = GetTickCount();
	IdleTime = (tickCount - lif.dwTime) / 1000;

	if(IdleTime >= TimeoutSeconds && !Idle) {
		Idle = true;
		//printf("Idle %d seconds > %d seconds\n", IdleTime, TimeoutSeconds);
		//Handle<Value> argv[1] = { Number::New(IdleTime) };
		Handle<Value> argv[2] = {
			String::New("idle"),
			Number::New(IdleTime)
		};
		//idleCallback->Call(Context::GetCurrent()->Global(), 1, argv);
		Local<Object> global = Context::GetCurrent()->Global();
		Local<Object> process_object = global->Get(String::New("process"))->ToObject();
		MakeCallback(process_object, "emit", 2, argv);
	} else if(IdleTime < TimeoutSeconds && Idle) {
		Idle = false;
		Handle<Value> argv[2] = {
			String::New("active"),
			Number::New(IdleTime)
		};
		Local<Object> global = Context::GetCurrent()->Global();
		Local<Object> process_object = global->Get(String::New("process"))->ToObject();
		MakeCallback(process_object, "emit", 2, argv);
	}
}

int IdleLoop() {
	uv_loop_t *main_loop = uv_default_loop();
	uv_timer_t idler;

	uv_timer_init(main_loop, &idler);
	uv_timer_start(&idler, GetIdleTime, TimeoutSeconds, interval);
	//uv_unref((uv_handle_t*)&idler);
	return uv_run(main_loop, UV_RUN_DEFAULT);
}

int IdleInit() {
	lif.cbSize = sizeof(LASTINPUTINFO);

	return IdleLoop();
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

Handle<Value> Setup(const Arguments& args) {
	HandleScope scope;

	Timeout = args[0]->NumberValue();
	TimeoutSeconds = Timeout / 1000;

	idleCallback = Local<Function>::Cast(args[1]);

	Local<FunctionTemplate> t = FunctionTemplate::New(Emitter::New);
	t->InstanceTemplate()->SetInternalFieldCount(1);
	t->SetClassName(String::New("Emitter"));
	//t->InstanceTemplate()->SetAccessor(String::NewSymbol("state"), GetState, NULL);
	NODE_SET_PROTOTYPE_METHOD(t, "idle", Emitter::Idle);

	Local<Object> obj = Object::New();
	obj->Set(String::NewSymbol("time"), Number::New(IdleTime));
	obj->Set(String::NewSymbol("Emitter"), t->GetFunction());

	IdleInit();

	return scope.Close(obj);
}

extern "C" void init(Handle<Object> exports, Handle<Object> module) {
	module->Set(String::NewSymbol("exports"),
		FunctionTemplate::New(Setup)->GetFunction());
}

NODE_MODULE(idlerun, init);


/*
void Emit(const char* message) {
	NanScope();
	Handle<Value> args[1] = { String::New(message) };
	Emit(1, args);
}

void Emit(const char* message, Handle<Value>* arg) {
	NanScope();
	Handle<Value> args[2] = { String::New(message), *arg };
	Emit(2, args);
}

void Emit(int length, Handle<Value> *args) {
	NanScope();

	Local<Value> emit_v = NanObjectWrapHandle(this)->Get(NanSymbol("emit"));
	assert(emit_v->IsFunction());
	Local<Function> emit_f = emit_v.As<Function>();

	TryCatch tc;
	emit_f->Call(NanObjectWrapHandle(this), length, args);
	if(tc.HasCaught()) {
		FatalException(tc);
	}
}
*/
