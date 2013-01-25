#include <cerrno>   // errno
#include <cstdlib>  // malloc()
#include <cstring>  // strlen()
#include <node.h>   // NODE_MODULE()
#include <unistd.h> // link()

using namespace v8;

struct LinkMarshal
{
    Persistent<Function>  callback;
    char*    from;
    char*    to;
    int                   status;
    int                   errorNumber;
};

static void
LinkHandler (uv_work_t* work)
{
    LinkMarshal* marshal = (LinkMarshal*)work->data;

    marshal->status = link(marshal->from, marshal->to);
    if (marshal->status) {
        marshal->errorNumber = errno;
    }
}

static void
LinkAfter (uv_work_t* work)
{
    LinkMarshal* marshal = static_cast<LinkMarshal*>(work->data);
    delete work;

    TryCatch  trap;

    Local<Value> argv[] = {
        Local<Value>::New(Null())
    };

    if (marshal->status) {
#define pattern "cannot link %s to %s"
        size_t length = strlen(pattern) + strlen(marshal->from) + strlen(marshal->to) + 1;
        char* buf = (char*)malloc(length);
        size_t written = snprintf(buf, length, pattern, marshal->from, marshal->to);
        assert (written < length);
        argv[0] = node::ErrnoException(marshal->errorNumber, "link()", buf, NULL);
        free(buf);
    }

    marshal->callback->Call(Context::GetCurrent()->Global(), sizeof(argv)/sizeof(*argv), argv);
    if (trap.HasCaught()) {
        node::FatalException(trap);
    }

    marshal->callback.Dispose();
    free(marshal->from);
    free(marshal->to);
    delete marshal;
}

static Handle<Value>
Link (const Arguments& args)
{
    HandleScope scope;
    LinkMarshal* marshal;

    Local<Function> callback = Local<Function>::Cast(args[2]);

    String::Utf8Value from(Local<String>::Cast(args[0]));
    String::Utf8Value to(Local<String>::Cast(args[1]));

    marshal = new LinkMarshal;
    marshal->callback    = Persistent<Function>::New(callback);
    marshal->from        = strdup(*from);
    marshal->to          = strdup(*to);
    marshal->status      = 0;
    marshal->errorNumber = 0;

    uv_work_t* work = new uv_work_t();
    work->data = marshal;

    uv_queue_work(uv_default_loop(), work, LinkHandler, LinkAfter);
    return scope.Close(Undefined());
}

struct MkTempMarshal
{
    Persistent<Function>  callback;
    int                   status;
    int                   errorNumber;
    char                * result;
};

static void
MkTempHandler (uv_work_t* work)
{
    MkTempMarshal* marshal = (MkTempMarshal*)work->data;
    char* buf = strdup("/tmp/posixy.XXXXXX"); // FIXME: let the user specify this
    // FIXME: use mkstemps()

    marshal->result = mktemp(buf);
    if (!marshal->result) {
        free(buf);
        marshal->errorNumber = errno;
    }
}

static void
MkTempAfter (uv_work_t* work)
{
    MkTempMarshal* marshal = static_cast<MkTempMarshal*>(work->data);
    delete work;

    TryCatch  trap;

    Local<Value> argv[] = {
        Local<Value>::New(Null()),
        Local<Value>::New(Null())
    };

    if (!marshal->result) {
        argv[0] = node::ErrnoException(marshal->errorNumber, "link()", "Cannot create a temporary file", NULL);
    } else {
        Local<Object> o = Object::New();
        argv[1] = o;
        o->Set(String::New("path"), String::New(marshal->result));
        free(marshal->result);
    }

    marshal->callback->Call(Context::GetCurrent()->Global(), sizeof(argv)/sizeof(*argv), argv);
    if (trap.HasCaught()) {
        node::FatalException(trap);
    }

    marshal->callback.Dispose();
    delete marshal;
}

static Handle<Value>
MkTemp (const Arguments& args)
{
    HandleScope scope;
    MkTempMarshal* marshal;

    Local<Function> callback = Local<Function>::Cast(args[0]);

    marshal = new MkTempMarshal;
    marshal->callback    = Persistent<Function>::New(callback);
    marshal->status      = 0;
    marshal->errorNumber = 0;

    uv_work_t* work = new uv_work_t();
    work->data = marshal;
    uv_queue_work(uv_default_loop(), work, MkTempHandler, MkTempAfter);

    return scope.Close(Undefined());
}

void
Init (Handle<Object> target)
{
    target->Set(String::NewSymbol("link"),
            FunctionTemplate::New(Link)->GetFunction());
    target->Set(String::NewSymbol("mktemp"),
            FunctionTemplate::New(MkTemp)->GetFunction());
}

NODE_MODULE(posixy, Init)

// vim:set sw=4 et:
