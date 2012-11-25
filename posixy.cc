#include <cerrno>   // errno
#include <cstdlib>  // malloc()
#include <cstring>  // strlen()
#include <node.h>   // NODE_MODULE()
#include <unistd.h> // link()

using namespace v8;

struct LinkMarshal
{
    Persistent<Function>  callback;
};

static Handle<Value>
Link(const Arguments& args)
{
    HandleScope scope;
    int status;
    LinkMarshal* marshal;

    Local<Function> callback = Local<Function>::Cast(args[2]);
    Local<Value> argv[] = {
        Local<Value>::New(Null())
    };

    marshal = new LinkMarshal;
    marshal->callback = Persistent<Function>::New(callback);

    String::Utf8Value from(args[0]->ToString());
    String::Utf8Value to(args[1]->ToString());
    status = link(*from, *to);
    if (status) {
#define pattern "cannot link %s to %s"
        size_t length = strlen(pattern) + strlen(*from) + strlen(*to) + 1;
        char* buf = (char*)malloc(length);
        size_t written = snprintf(buf, length, pattern, *from, *to);
        assert (written < length);
        argv[0] = node::ErrnoException(errno, "link()", buf, NULL);
        free(buf);
    }

    TryCatch  trap;

    marshal->callback->Call(Context::GetCurrent()->Global(), sizeof(argv)/sizeof(*argv), argv);
    if (trap.HasCaught()) {
        node::FatalException(trap);
    }

    marshal->callback.Dispose();
    delete marshal;

    return scope.Close(Undefined());
}

void
Init(Handle<Object> target)
{
    target->Set(String::NewSymbol("link"),
            FunctionTemplate::New(Link)->GetFunction());
}

NODE_MODULE(posixy, Init)

// vim:set sw=4 et:
