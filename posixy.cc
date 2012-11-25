#include <cerrno>   // errno
#include <cstdlib>  // malloc()
#include <cstring>  // strlen()
#include <node.h>   // NODE_MODULE()
#include <unistd.h> // link()

using namespace v8;

struct LinkMarshal
{
    Persistent<Function>  callback;
    Persistent<String>    from;
    Persistent<String>    to;
    int                   status;
    int                   errorNumber;
};

static void
LinkHandler (uv_work_t* work)
{
    LinkMarshal* marshal = static_cast<LinkMarshal*>(work->data);

    String::Utf8Value from(marshal->from);
    String::Utf8Value to(marshal->to);
    marshal->status = link(*from, *to);
    if (marshal->status) {
        marshal->errorNumber = errno;
    }
}

static Handle<Value>
Link(const Arguments& args)
{
    HandleScope scope;
    LinkMarshal* marshal;

    Local<Function> callback = Local<Function>::Cast(args[2]);
    Local<Value> argv[] = {
        Local<Value>::New(Null())
    };

    marshal = new LinkMarshal;
    marshal->callback    = Persistent<Function>::New(callback);
    marshal->from        = Persistent<String>::New(Local<String>::Cast(args[0]));
    marshal->to          = Persistent<String>::New(Local<String>::Cast(args[1]));
    marshal->status      = 0;
    marshal->errorNumber = 0;

    uv_work_t* task = new uv_work_t();
    task->data = marshal;

    LinkHandler(task);

    delete task;

    TryCatch  trap;

    if (marshal->status) {
        String::Utf8Value from(marshal->from);
        String::Utf8Value to(marshal->to);
#define pattern "cannot link %s to %s"
        size_t length = strlen(pattern) + strlen(*from) + strlen(*to) + 1;
        char* buf = (char*)malloc(length);
        size_t written = snprintf(buf, length, pattern, *from, *to);
        assert (written < length);
        argv[0] = node::ErrnoException(marshal->errorNumber, "link()", buf, NULL);
        free(buf);
    }

    marshal->callback->Call(Context::GetCurrent()->Global(), sizeof(argv)/sizeof(*argv), argv);
    if (trap.HasCaught()) {
        node::FatalException(trap);
    }

    marshal->callback.Dispose();
    marshal->from.Dispose();
    marshal->to.Dispose();
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
