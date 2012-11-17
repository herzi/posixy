#include <node.h>

using namespace v8;

static Handle<Value>
RunCallback(const Arguments& args)
{
    HandleScope scope;

    Local<Function> cb = Local<Function>::Cast(args[0]);
    Local<Value> argv[] = {
        Local<Value>::New(String::New("hello world"))
    };
    cb->Call(Context::GetCurrent()->Global(), sizeof(argv)/sizeof(*argv), argv);

    return scope.Close(Undefined());
}

void
Init(Handle<Object> target)
{
    target->Set(String::NewSymbol("runCallback"),
            FunctionTemplate::New(RunCallback)->GetFunction());
}

NODE_MODULE(posixy, Init)

// vim:set sw=4 et:
