#ifndef GPIO_H
#define GPIO_H

#include <node.h>
#include <node_object_wrap.h>
#include <capi_iotbus_gpio.h>

using namespace v8;

class GPIO : public node::ObjectWrap {
 public:
  static void Initialize();
  static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  explicit GPIO();
  ~GPIO();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetObjectValue(Local<Object> obj, iotbus_gpio_context_h hnd);
  static void GetObjectValue(Local<Object> obj, iotbus_gpio_context_h *hnd);
  static void CastingDirectionInttoEnum(int dir, iotbus_gpio_direction_e *dir_enum);
  static void CastingDirectionEnumtoInt(iotbus_gpio_direction_e dir_enum, int *dir);
  static void CastingEdgeInttoEnum(int edge, iotbus_gpio_edge_e *edge_enum);
  static void CastingEdgeEnumtoInt(iotbus_gpio_edge_e edge_enum, int *edge);
  static void Open(const v8::FunctionCallbackInfo<Value>& args);
  static void Close(const v8::FunctionCallbackInfo<Value>& args);
  static void SetDirection(const v8::FunctionCallbackInfo<Value>& args);
  static void SetEdgeMode(const v8::FunctionCallbackInfo<Value>& args);
  static void SetDriveMode();
  static void RegisterCallback();
  static void UnregisterCallback();
  static void Read(const v8::FunctionCallbackInfo<Value>& args);
  static void Write(const v8::FunctionCallbackInfo<Value>& args);
  static void GetDirection(const v8::FunctionCallbackInfo<Value>& args);
  static void GetPin(const v8::FunctionCallbackInfo<Value>& args);
  static void GetEdgeMode(const v8::FunctionCallbackInfo<Value>& args);
  static void GetDriveMode();

  static v8::Persistent<v8::Function> constructor;
};
#endif