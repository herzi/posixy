#include <cerrno>   // errno
#include <unistd.h> // link()
#include <node.h>   // NODE_MODULE()

using namespace v8;

static Handle<Value>
Link(const Arguments& args)
{
    HandleScope scope;
    int status;

    Local<Function> cb = Local<Function>::Cast(args[2]);
    Local<Value> argv[] = {
        Local<Value>::New(Null())
    };

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

    cb->Call(Context::GetCurrent()->Global(), sizeof(argv)/sizeof(*argv), argv);

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
