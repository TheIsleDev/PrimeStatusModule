#include <string>

#include <Mod/CppUserModBase.hpp>

#include "updator_handler.cpp"

class PrimeAnnouncer : public RC::CppUserModBase {
public:
    PrimeAnnouncer() : CppUserModBase()
    {
        ModName = STR("PrimeAnnouncer");
        ModVersion = STR("1.0");
        ModDescription = STR("Tell players when they do something to prime tasks related");
        ModAuthors = STR("Shiza");
    }

    auto on_ui_init() -> void override {
        PrimeChecker::Initialize();
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