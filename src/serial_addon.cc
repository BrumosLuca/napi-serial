#include <stdio.h>
#include <stdlib.h>
#include <napi.h>
#include <assert.h>

extern "C"{
#include "portal.h"
}
#include <napi.h>

using namespace Napi;

Napi::Number Open(const Napi::CallbackInfo& info){
  int y = init();
  return Napi::Number::New(info.Env(), y);
}

Napi::Number Close(const Napi::CallbackInfo& info){
  int y = close();
  return Napi::Number::New(info.Env(), y);
}

Napi::Number ChangeSigma(const Napi::CallbackInfo& info) {
  int res;

  if(info.Length() < 1) res = -1;

  if(! info[0].IsNumber() ) res = -1;

  res = info[0].As<Napi::Number>();

  changeSigma(res);

  return info[0].As<Napi::Number>();
}

Napi::Value GetData(const Napi::CallbackInfo& info){
 
  Napi::Env env = info.Env();

  int * lastVals = update();

  if(lastVals == NULL) 
      return env.Null();
  else{
      Napi::Array result = Napi::Array::New(env, 4);
      for (size_t i = 0; i < 4; i++)
        result[i] = Napi::Number::New(env, lastVals[i]);
      return result;
  }
}


Napi::Object Init(Napi::Env env, Napi::Object exports) {

  exports.Set("open", Napi::Function::New(env, Open));
  exports.Set("close", Napi::Function::New(env, Close));
  exports.Set("getData", Napi::Function::New(env, GetData));
  exports.Set("changeSigma", Napi::Function::New(env, ChangeSigma) );

  return exports;
}

NODE_API_MODULE(addon, Init)
