#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <node.h>
#include <capi_iotbus_i2c.h>
#include "i2c.h"

using namespace v8;

Persistent<Function> I2C::constructor;


I2C::I2C() {
}

I2C::~I2C() {
}

void I2C::Initialize(){
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "I2C"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "init", Init);
  NODE_SET_PROTOTYPE_METHOD(tpl, "stop", Stop);
  NODE_SET_PROTOTYPE_METHOD(tpl, "set_address", SetAddress);
  NODE_SET_PROTOTYPE_METHOD(tpl, "write", Write);
  NODE_SET_PROTOTYPE_METHOD(tpl, "read", Read);

  constructor.Reset(isolate, tpl->GetFunction());
  //exports->Set(String::NewFromUtf8(isolate, "I2C"), tpl->GetFunction());  
}

void I2C::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new MyObject(...)`
    I2C* obj = new I2C();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void I2C::NewInstance(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  const unsigned argc = 1;
  Handle<Value> argv[argc] = { args[0] };
  Local<Function> cons = Local<Function>::New(isolate, constructor);
  Local<Object> instance = cons->NewInstance(argc, argv);

  args.GetReturnValue().Set(instance);
}

void I2C::GetObjectValue(Local<Object> obj, iotbus_i2c_context_h *hnd){  
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  iotbus_i2c_context_h hnd_ptr;
  uintptr_t context_ptr = obj->Get(String::NewFromUtf8(isolate, "context_ptr"))->Int32Value();
  hnd_ptr = reinterpret_cast<iotbus_i2c_context_h>(context_ptr);  
  if(hnd_ptr){
    hnd_ptr->fd = obj->Get(String::NewFromUtf8(isolate, "fd"))->Int32Value();
    //printf("[GetObjectValue] context_ptr:%d, hnd=%p, hnd->fd:%d\n", context_ptr, hnd_ptr, hnd_ptr->fd);
  }
  *hnd = hnd_ptr;
}


void I2C::SetObjectValue(Local<Object> obj, iotbus_i2c_context_h hnd){
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if(hnd){
    uintptr_t context_ptr = reinterpret_cast<uintptr_t>(hnd);
    //printf("[SetObjectValue] context_ptr:%d, hnd=%p, hnd->fd:%d\n", context_ptr, hnd, hnd->fd);
    obj->Set(String::NewFromUtf8(isolate, "context_ptr"), Integer::New(isolate, context_ptr));
    obj->Set(String::NewFromUtf8(isolate, "fd"), Integer::New(isolate, hnd->fd));
  }  
}

void I2C::Init(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  int bus = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();  
  iotbus_i2c_context_h hnd = capi_iotbus_i2c_init(bus);

  Local<Object> obj = Object::New(isolate);
  SetObjectValue(obj, hnd);
  args.GetReturnValue().Set(obj);
}

void I2C::Stop(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  int ret;
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_i2c_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_i2c_stop(hnd);
  }
  else{
    printf("[ERROR] Fail to get iotbus_i2c_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void I2C::SetAddress(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() < 2) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }
  if (!args[0]->IsObject() || !args[1]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong argument type")));
    return;
  }

  int ret;
  int addr = args[1]->NumberValue();
  
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_i2c_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_i2c_set_address(hnd, addr);
    SetObjectValue(obj, hnd);
  }
  else{
    printf("[ERROR] Fail to get iotbus_i2c_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void I2C::Write(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() < 3) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }
  if (!args[0]->IsObject() || !args[2]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong argument type")));
    return;
  }

  int ret = -1;
  int len = args[2]->NumberValue();
  unsigned char *data = new unsigned char[len];
  
  if(args[1]->IsArray()){
    Local<Array> arr = Local<Array>::Cast(args[1]);
    for(int i=0; i<len; i++){
        data[i] = static_cast<unsigned char>(arr->Get(i)->Int32Value());
    }

    Local<Object> obj = Local<Object>::Cast(args[0]);
    iotbus_i2c_context_h hnd = NULL;
    GetObjectValue(obj, &hnd);
    if(hnd){
      ret = capi_iotbus_i2c_write(hnd, data, len);
    }
    else{
      printf("[ERROR] Fail to get iotbus_i2c_context_h !!\n");
      ret = -1;
    }
  }
  else{
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong argument type")));
    return;
  }
  delete []data;
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void I2C::Read(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() < 3) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }
  if (!args[0]->IsObject() || !args[2]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong argument type")));
    return;
  }

  int ret = -1;
  int len  = args[2]->NumberValue();
  unsigned char *data = new unsigned char[len];

  if(args[1]->IsArray()){
    Local<Array> arr = Local<Array>::Cast(args[1]);
    Local<Object> obj = Local<Object>::Cast(args[0]);

    iotbus_i2c_context_h hnd = NULL;  
    GetObjectValue(obj, &hnd);
    if(hnd){    
      ret = capi_iotbus_i2c_read(hnd, data, len);
    }
    else{
      printf("[ERROR] Fail to get iotbus_i2c_context_h !!\n");
      ret = -1;
    }

    for(int i=0; i<len; i++){
        arr->Set(i, Number::New(isolate, data[i]));
    }
  }
  else{
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong argument type")));
    return;
  }
  delete []data;
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CreateObject(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  I2C::NewInstance(args);
}

void InitAll(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  I2C::Initialize();

  exports->Set(String::NewFromUtf8(isolate, "create"), FunctionTemplate::New(isolate, CreateObject)->GetFunction());
}


NODE_MODULE(i2c, InitAll)

