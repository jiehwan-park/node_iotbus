#ifndef I2C_H
#define I2C_H

#include <node.h>
#include <node_object_wrap.h>

using namespace v8;

class I2C : public node::ObjectWrap {
 public:
  static void Initialize();
  static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  explicit I2C();
  ~I2C();


  static iotbus_i2c_context_h i2c_hnd;

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetObjectValue(Local<Object> obj, iotbus_i2c_context_h hnd);
  static void GetObjectValue(Local<Object> obj, iotbus_i2c_context_h *hnd);
  static void Init(const v8::FunctionCallbackInfo<Value>& args);
  static void Stop(const v8::FunctionCallbackInfo<Value>& args);
  static void SetAddress(const v8::FunctionCallbackInfo<Value>& args);
  static void Write(const v8::FunctionCallbackInfo<Value>& args);
  static void Read(const v8::FunctionCallbackInfo<Value>& args);

  static v8::Persistent<v8::Function> constructor;
};
#endif