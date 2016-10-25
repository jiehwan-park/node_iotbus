#ifndef PWM_H
#define PWM_H

#include <node.h>
#include <node_object_wrap.h>

using namespace v8;

class PWM : public node::ObjectWrap {
 public:
  static void Initialize();
  static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  explicit PWM();
  ~PWM();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void CastingStateInttoEnum(int state, iotbus_pwm_state_e *state_enum);
  static void CastingStateEnumtoInt(iotbus_pwm_state_e state_enum, int *state);
  static void GetObjectValue(Local<Object> obj, iotbus_pwm_context_h *hnd);
  static void SetObjectValue(Local<Object> obj, iotbus_pwm_context_h hnd);
  static void Open(const v8::FunctionCallbackInfo<Value>& args);
  static void Close(const v8::FunctionCallbackInfo<Value>& args);
  static void SetDutyCycle(const v8::FunctionCallbackInfo<Value>& args);
  static void SetPeriod(const v8::FunctionCallbackInfo<Value>& args);
  static void SetEnabled(const v8::FunctionCallbackInfo<Value>& args);
  static void IsEnabled(const v8::FunctionCallbackInfo<Value>& args);
  static void GetDutyCycle(const v8::FunctionCallbackInfo<Value>& args);
  static void GetPeriod(const v8::FunctionCallbackInfo<Value>& args);

  static v8::Persistent<v8::Function> constructor;
};
#endif