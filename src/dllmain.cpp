#include <string>

#include <Mod/CppUserModBase.hpp>

#include "handler.cpp"

class PrimeAnnouncer : public RC::CppUserModBase {
private:
  int last_tick_attempt = 0;
  int current_tick = 0;
  const int tick_delay = 1200;

public:
  PrimeAnnouncer() : CppUserModBase()
  {
    ModName = STR("PrimeAnnouncer");
    ModVersion = STR("1.0");
    ModDescription = STR("Tell players when they do something to prime tasks related");
    ModAuthors = STR("Shiza");
  }

  auto on_unreal_init() -> void override {
    PrimeChecker::Initialize();
  }

  auto on_update() -> void override {
    if (++current_tick < last_tick_attempt + tick_delay) return;
    last_tick_attempt = current_tick;
    PrimeChecker::Fire();
  }
};

#define KISMET_DEBUGGER_MOD_API __declspec(dllexport)
extern "C"
{
  KISMET_DEBUGGER_MOD_API RC::CppUserModBase* start_mod()
  {
    return new PrimeAnnouncer();
  }

  KISMET_DEBUGGER_MOD_API void uninstall_mod(RC::CppUserModBase* mod)
  {
    delete mod;
  }
}