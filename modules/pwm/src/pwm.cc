#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <node.h>
#include <capi_iotbus_pwm.h>
#include "pwm.h"

using namespace v8;

Persistent<Function> PWM::constructor;


PWM::PWM() {
}

PWM::~PWM() {
}

void PWM::Initialize(){
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "PWM"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "set_duty_cycle", SetDutyCycle);
  NODE_SET_PROTOTYPE_METHOD(tpl, "set_period", SetPeriod);
  NODE_SET_PROTOTYPE_METHOD(tpl, "set_enabled", SetEnabled);
  NODE_SET_PROTOTYPE_METHOD(tpl, "is_enabled", IsEnabled);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_duty_cycle", GetDutyCycle);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_period", GetPeriod);

  constructor.Reset(isolate, tpl->GetFunction());
  //exports->Set(String::NewFromUtf8(isolate, "PWM"), tpl->GetFunction());  
}

void PWM::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new MyObject(...)`
    PWM* obj = new PWM();
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

void PWM::NewInstance(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  const unsigned argc = 1;
  Handle<Value> argv[argc] = { args[0] };
  Local<Function> cons = Local<Function>::New(isolate, constructor);
  Local<Object> instance = cons->NewInstance(argc, argv);

  args.GetReturnValue().Set(instance);
}

void PWM::CastingStateInttoEnum(int state, iotbus_pwm_state_e *state_enum){
  if(state_enum){
    switch(state){
      case 0:
        *state_enum = IOTBUS_PWM_DISABLE;
        break;
      case 1:
        *state_enum = IOTBUS_PWM_ENABLE;
        break;
      default:
        *state_enum = IOTBUS_PWM_DISABLE;
        break;
    }
  }
}

void PWM::CastingStateEnumtoInt(iotbus_pwm_state_e state_enum, int *state){
  if(state){
    switch(state_enum){
      case IOTBUS_PWM_DISABLE:
        *state = 0;
        break;
      case IOTBUS_PWM_ENABLE:
        *state = 1;
        break;
      default:
        *state = 0;
        break;
    }
  }
}

void PWM::GetObjectValue(Local<Object> obj, iotbus_pwm_context_h *hnd){
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate); 

  iotbus_pwm_context_h hnd_ptr;
  uintptr_t context_ptr = obj->Get(String::NewFromUtf8(isolate, "context_ptr"))->Int32Value();
  hnd_ptr = reinterpret_cast<iotbus_pwm_context_h>(context_ptr);  
  if(hnd_ptr){
    hnd_ptr->device = obj->Get(String::NewFromUtf8(isolate, "device"))->Int32Value();
    hnd_ptr->channel = obj->Get(String::NewFromUtf8(isolate, "channel"))->Int32Value();
    hnd_ptr->period = obj->Get(String::NewFromUtf8(isolate, "period"))->Int32Value();
    hnd_ptr->duty_cycle = obj->Get(String::NewFromUtf8(isolate, "duty_cycle"))->Int32Value();
    hnd_ptr->enabled = obj->Get(String::NewFromUtf8(isolate, "enabled"))->Int32Value();
    //printf("[GetObjectValue] context_ptr:%d, hnd=%p, device:%d, channel:%d, period:%d, duty_cycle:%d, enabled:%d\n", context_ptr, hnd_ptr, hnd_ptr->device, hnd_ptr->channel, hnd_ptr->period, hnd_ptr->duty_cycle, hnd_ptr->enabled);
  }
  *hnd = hnd_ptr;
}


void PWM::SetObjectValue(Local<Object> obj, iotbus_pwm_context_h hnd){
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if(hnd){
    uintptr_t context_ptr = reinterpret_cast<uintptr_t>(hnd);
    //printf("[SetObjectValue] context_ptr:%d, hnd=%p, device:%d, channel:%d, period:%d, duty_cycle:%d, enabled:%d\n", context_ptr, hnd, hnd->device, hnd->channel, hnd->period, hnd->duty_cycle, hnd->enabled);
    obj->Set(String::NewFromUtf8(isolate, "context_ptr"), Integer::New(isolate, context_ptr));
    obj->Set(String::NewFromUtf8(isolate, "device"), Integer::New(isolate, hnd->device));
    obj->Set(String::NewFromUtf8(isolate, "channel"), Integer::New(isolate, hnd->channel));
    obj->Set(String::NewFromUtf8(isolate, "period"), Integer::New(isolate, hnd->period));
    obj->Set(String::NewFromUtf8(isolate, "duty_cycle"), Integer::New(isolate, hnd->duty_cycle));    
    obj->Set(String::NewFromUtf8(isolate, "enabled"), Integer::New(isolate, hnd->enabled));
  }  
}

void PWM::Open(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() < 2) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }
  if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong argument type")));
    return;
  }

  int device = args[0]->NumberValue();
  int channel = args[1]->NumberValue();
  iotbus_pwm_context_h hnd = capi_iotbus_pwm_open(device, channel);

  Local<Object> obj = Object::New(isolate);
  SetObjectValue(obj, hnd);
  args.GetReturnValue().Set(obj);
}

void PWM::Close(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  int ret;
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_pwm_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_pwm_close(hnd);
  }
  else{
    printf("[ERROR] Fail to get iotbus_pwm_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void PWM::SetDutyCycle(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  int ret;

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
  
  int duty_cycle = args[1]->NumberValue();
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_pwm_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_pwm_set_duty_cycle(hnd, duty_cycle);
    SetObjectValue(obj, hnd);
  }
  else{
    printf("[ERROR] Fail to get iotbus_pwm_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void PWM::SetPeriod(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  int ret;

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
  
  int period = args[1]->NumberValue();
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_pwm_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_pwm_set_period(hnd, period);
    SetObjectValue(obj, hnd);
  }
  else{
    printf("[ERROR] Fail to get iotbus_pwm_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void PWM::SetEnabled(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  int ret;

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
  
  int state = args[1]->NumberValue();
  iotbus_pwm_state_e state_enum;
  CastingStateInttoEnum(state, &state_enum);  

  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_pwm_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_pwm_set_enabled(hnd, state_enum);
    SetObjectValue(obj, hnd);
  }
  else{
    printf("[ERROR] Fail to get iotbus_pwm_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void PWM::IsEnabled(const v8::FunctionCallbackInfo<Value>& args) {
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
  
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_pwm_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);

  int enabled;  
  if(hnd){
    enabled = capi_iotbus_pwm_is_enabled(hnd);
    SetObjectValue(obj, hnd);
  }
  else{
    printf("[ERROR] Fail to get iotbus_pwm_context_h !!\n");
    enabled = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, enabled));
}

void PWM::GetDutyCycle(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  int ret;
  int duty_cycle;

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
  
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_pwm_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_pwm_get_duty_cycle(hnd, &duty_cycle);
  }
  else{
    printf("[ERROR] Fail to get iotbus_pwm_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, duty_cycle));
}

void PWM::GetPeriod(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  int ret;
  int period;

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
  
  Local<Object> obj = Local<Object>::Cast(args[0]);
  iotbus_pwm_context_h hnd = NULL;
  GetObjectValue(obj, &hnd);
  if(hnd){
    ret = capi_iotbus_pwm_get_period(hnd, &period);
  }
  else{
    printf("[ERROR] Fail to get iotbus_pwm_context_h !!\n");
    ret = -1;
  }
  args.GetReturnValue().Set(Number::New(isolate, period));
}

void CreateObject(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  PWM::NewInstance(args);
}

void InitAll(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  PWM::Initialize();

  exports->Set(String::NewFromUtf8(isolate, "create"), FunctionTemplate::New(isolate, CreateObject)->GetFunction());
}


NODE_MODULE(pwm, InitAll)

