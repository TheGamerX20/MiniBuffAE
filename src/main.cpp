#include <pch.h>

// Patches
#include <EncounterZoneResetPatch.h>
#include <TESObjectREFRGetEncounterZonePatch.h>
#include <ActorIsHostileToActorPatch.h>
#include <UnalignedLoadPatch.h>
#include <CellInitPatch.h>
#include <ExperimentalPatch.h>

namespace Main
{
    // Config Options
    static REX::INI::Bool iEncounterZoneResetPatch{ "Patches"sv, "EnableEncounterZoneResetPatch"sv, true };
    static REX::INI::Bool iTESObjectREFRGetEncounterZonePatch{ "Patches"sv, "EnableTESObjectREFRGetEncounterZonePatch"sv, true };
    static REX::INI::Bool iActorIsHostileToActorPatch{ "Patches"sv, "EnableActorIsHostileToActorPatch"sv, true };
    static REX::INI::Bool iUnalignedLoadPatch{ "Patches"sv, "EnableUnalignedLoadPatch"sv, true };
    static REX::INI::Bool iCellInitPatch{ "Patches"sv, "EnableCellInitPatch"sv, true };
    static REX::INI::Bool iExperimentalPatch{ "Patches"sv, "EnableExperimentalPatch"sv, true };

    F4SE_PLUGIN_PRELOAD(const F4SE::LoadInterface* a_f4se)
    {
        // Init
        F4SE::Init(a_f4se);
        REX::INFO("MiniBuff AE Initializing...");

        // Load the Config
        const auto config = REX::INI::SettingStore::GetSingleton();
        config->Init("Data/F4SE/Plugins/MiniBuffAE.ini", "Data/F4SE/Plugins/MiniBuffAECustom.ini");
        config->Load();

        // Get the Trampoline and Allocate
        auto& trampoline = REL::GetTrampoline();
        trampoline.create(128);

        // Patch EncounterZoneResetPatch
        if (iEncounterZoneResetPatch.GetValue() == true)
        {
            if (EncounterZoneResetPatch::Install())
            {
                REX::INFO("EncounterZoneReset Patched!");
            }
        }

        // Patch TESObjectREFRGetEncounterZone
        if (iTESObjectREFRGetEncounterZonePatch.GetValue() == true)
        {
            if (TESObjectREFRGetEncounterZonePatch::Install())
            {
                REX::INFO("Achievements Patch Initialized!");
            }
        }

        // Patch ActorIsHostileToActor
        if (iActorIsHostileToActorPatch.GetValue() == true)
        {
            if (ActorIsHostileToActorPatch::Install())
            {
                REX::INFO("ActorIsHostileToActor Patched!");
            }
        }

        // Patch UnalignedLoad
        if (iUnalignedLoadPatch.GetValue() == true)
        {
            if (UnalignedLoadPatch::Install())
            {
                REX::INFO("UnalignedLoad Patched!");
            }
        }

        // Patch CellInit
        if (iCellInitPatch.GetValue() == true)
        {
            if (CellInitPatch::Install())
            {
                REX::INFO("CellInit Patched!");
            }
        }

        // Experimental Patch
        if (iExperimentalPatch.GetValue() == true)
        {
            if (ExperimentalPatch::Install())
            {
                REX::INFO("Experimental Patch Loaded, here be dragons!");
            }
        }

        // Finished
        REX::INFO("MiniBuff AE Initialized!");
        return true;
    }
}
