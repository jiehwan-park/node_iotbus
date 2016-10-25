#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <node.h>
#include <capi_iotbus_gpio.h>
#include "gpio.h"

using namespace v8;

Persistent<Function> GPIO::constructor;


GPIO::GPIO() {
}

GPIO::~GPIO() {
}

void GPIO::Initialize(){
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "GPIO"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "set_direction", SetDirection);
  NODE_SET_PROTOTYPE_METHOD(tpl, "set_edge_mode", SetEdgeMode);
  //NODE_SET_PROTOTYPE_METHOD(tpl, "set_drive_mode", SetDriveMode);
  //NODE_SET_PROTOTYPE_METHOD(tpl, "register_callback", RegisterCallback);
  //NODE_SET_PROTOTYPE_METHOD(tpl, "unregister_callback", UnregisterCallback);
  NODE_SET_PROTOTYPE_METHOD(tpl, "read", Read);
  NODE_SET_PROTOTYPE_METHOD(tpl, "write", Write);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_direction", GetDirection);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_pin", GetPin);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_edge_mode", GetEdgeMode);
  //NODE_SET_PROTOTYPE_METHOD(tpl, "get_drive_mode", GetDriveMode);

  constructor.Reset(isolate, tpl->GetFunction());
  //exports->Set(String::NewFromUtf8(isolate, "GPIO"), tpl->GetFunction());  
}

void GPIO::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new MyObject(...)`
    GPIO* obj = new GPIO();
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

void GPIO::NewInstance(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  const unsigned argc = 1;
  Handle<Value> argv[argc] = { args[0] };
  Local<Function> cons = Local<Function>::New(isolate, constructor);
  Local<Object> instance = cons->NewInstance(argc, argv);

  args.GetReturnValue().Set(instance);
}

void GPIO::CastingDirectionInttoEnum(int dir, iotbus_gpio_direction_e *dir_enum){
  if(dir_enum){
    switch(dir){
      case 0:
        *dir_enum = IOTBUS_GPIO_DIRECTION_NONE;
        break;
      case 1:
        *dir_enum = IOTBUS_GPIO_DIRECTION_OUT;
        break;
      case 2:
        *dir_enum = IOTBUS_GPIO_DIRECTION_IN;
        break;
      default:
        *dir_enum = IOTBUS_GPIO_DIRECTION_OUT;
        break;
    }
  }
}

void GPIO::CastingDirectionEnumtoInt(iotbus_gpio_direction_e dir_enum, int *dir){
  if(dir){
    switch(dir_enum){
      case IOTBUS_GPIO_DIRECTION_NONE:
        *dir = 0;
        break;
      case IOTBUS_GPIO_DIRECTION_OUT:
        *dir = 1;
        break;
      case IOTBUS_GPIO_DIRECTION_IN:
        *dir = 2;
        break;
      default:
        *dir = 0;
        break;
    }
  }
}

void GPIO::CastingEdgeInttoEnum(int edge, iotbus_gpio_edge_e *edge_enum){
  if(edge_enum){
    switch(edge){
      case 0:
        *edge_enum = IOTBUS_GPIO_EDGE_NONE;
        break;
      case 1:
        *edge_enum = IOTBUS_GPIO_EDGE_BOTH;
        break;
      case 2:
        *edge_enum = IOTBUS_GPIO_EDGE_RISING;
        break;
      case 3:
        *edge_enum = IOTBUS_GPIO_EDGE_FALLING;
        break;
      default:
        *edge_enum = IOTBUS_GPIO_EDGE_NONE;
        break;
    }
  }
}

void GPIO::CastingEdgeEnumtoInt(iotbus_gpio_edge_e edge_enum, int *edge){
  if(edge){
    switch(edge_enum){
      case IOTBUS_GPIO_EDGE_NONE:
        *edge = 0;
        break;
      case IOTBUS_GPIO_EDGE_BOTH:
        *edge = 1;
        break;
      case IOTBUS_GPIO_EDGE_RISING:
        *edge = 2;
        break;
      case IOTBUS_GPIO_EDGE_FALLING:
        *edge = 3;
        break;
      default:
        *edge = 0;
        break;
    }
  }
}

void GPIO::GetObjectValue(Local<Object> obj, iotbus_gpio_context_h *hnd){  
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  iotbus_gpio_context_h hnd_ptr;
  uintptr_t context_ptr = obj->Get(String::NewFromUtf8(isolate, "context_ptr"))->Int32Value();
  hnd_ptr = reinterpret_cast<iotbus_gpio_context_h>(context_ptr);  

  if(hnd_ptr){
    hnd_ptr->pin = obj->Get(String::NewFromUtf8(isolate, "pin"))->Int32Value();
    int dir_local = obj->Get(String::NewFromUtf8(isolate, "dir"))->Int32Value();
    CastingDirectionInttoEnum(dir_local, &hnd_ptr->dir);
    int edge_local = obj->Get(String::NewFromUtf8(isolate, "edge"))->Int32Value();
    CastingEdgeInttoEnum(edge_local, &hnd_ptr->edge);
    //hnd_ptr->drive = obj->Get(String::NewFromUtf8(isolate, "drive"))->Int32Value();
    //printf("[GetObjectValue] context_ptr:%d, hnd=%p, pin:%d, dir:%d, edge:%d\n", context_ptr, hnd_ptr, hnd_ptr->pin, hnd_ptr->dir, hnd_ptr->edge);
  }
  *hnd = hnd_ptr;
}


void GPIO::SetObjectValue(Local<Object> obj, iotbus_gpio_context_h hnd){
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if(hnd){
    uintptr_t context_ptr = reinterpret_cast<uintptr_t>(hnd);
    //printf("[SetObjectValue] context_ptr:%d, hnd=%p, hnd->pin:%d, hnd->dir:%d, hnd->edge:%d\n", context_ptr, hnd, hnd->pin, hnd->dir, hnd->edge);
    obj->Set(String::NewFromUtf8(isolate, "context_ptr"), Integer::New(isolate, context_ptr));
    obj->Set(String::NewFromUtf8(isolate, "pin"), Integer::New(isolate, hnd->pin));
    obj->Set(String::NewFromUtf8(isolate, "dir"), Integer::New(isolate, hnd->dir));
    obj->Set(String::NewFromUtf8(isolate, "edge"), Integer::New(isolate, hnd->edge));
    //obj->Set(String::NewFromUtf8(isolate, "drive"), Integer::New(isolate, hnd->drive));    
  }  
}

void GPIO::Open(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  int pin = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();  
  iotbus_gpio_context_h hnd = capi_iotbus_gpio_open(pin);

  Local<Object> obj = Object::New(isolate);
  SetObjectValue(obj, hnd);
  args.GetReturnValue().Set(obj);
}

void GPIO::Close(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  int ret;
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_gpio_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_gpio_close(hnd);
  }
  else{
    printf("[ERROR] Fail to get iotbus_gpio_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void GPIO::SetDirection(const v8::FunctionCallbackInfo<Value>& args) {
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
  int dir = args[1]->NumberValue();
  iotbus_gpio_direction_e dir_enum;
  CastingDirectionInttoEnum(dir, &dir_enum);  
  
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_gpio_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_gpio_set_direction(hnd, dir_enum);
    SetObjectValue(obj, hnd);
  }
  else{
    printf("[ERROR] Fail to get iotbus_gpio_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void GPIO::SetEdgeMode(const v8::FunctionCallbackInfo<Value>& args) {
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
  int edge = args[1]->NumberValue();
  iotbus_gpio_edge_e edge_enum;
  CastingEdgeInttoEnum(edge, &edge_enum);

  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_gpio_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_gpio_set_edge_mode(hnd, edge_enum);
    SetObjectValue(obj, hnd);
  }
  else{
    printf("[ERROR] Fail to get iotbus_gpio_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void GPIO::SetDriveMode() {
  // Not implemented
}

void GPIO::RegisterCallback() {
  // Not implemented
}

void GPIO::UnregisterCallback() {
  // Not implemented
}

void GPIO::Read(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }
  if (!args[0]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong argument type")));
    return;
  }

  int ret;
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_gpio_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_gpio_read(hnd);
  }
  else{
    printf("[ERROR] Fail to get iotbus_gpio_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void GPIO::Write(const v8::FunctionCallbackInfo<Value>& args) {
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
  int value = args[1]->NumberValue();

  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_gpio_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_gpio_write(hnd, value);
  }
  else{
    printf("[ERROR] Fail to get iotbus_gpio_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void GPIO::GetDirection(const v8::FunctionCallbackInfo<Value>& args) {
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

  int ret=0;
  int dir_int=0;
  iotbus_gpio_direction_e dir;  
  
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_gpio_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_gpio_get_direction(hnd, &dir);
    CastingDirectionEnumtoInt(dir, &dir_int);
  }
  else{
    printf("[ERROR] Fail to get iotbus_gpio_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, dir_int));
}


void GPIO::GetPin(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }
  if (!args[0]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong argument type")));
    return;
  }

  int ret;
  
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_gpio_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_gpio_get_pin(hnd);
  }
  else{
    printf("[ERROR] Fail to get iotbus_gpio_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, hnd->pin));
}

void GPIO::GetEdgeMode(const v8::FunctionCallbackInfo<Value>& args) {
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

  int ret=0;
  int edge_int=0;
  iotbus_gpio_edge_e edge;  
  
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_gpio_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_gpio_get_edge_mode(hnd, &edge);
    CastingEdgeEnumtoInt(edge, &edge_int);
  }
  else{
    printf("[ERROR] Fail to get iotbus_gpio_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, edge_int));
}

void GPIO::GetDriveMode() {
  // Not supported
}


void CreateObject(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  GPIO::NewInstance(args);
}

void InitAll(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  GPIO::Initialize();

  exports->Set(String::NewFromUtf8(isolate, "create"), FunctionTemplate::New(isolate, CreateObject)->GetFunction());
}


NODE_MODULE(gpio, InitAll)

