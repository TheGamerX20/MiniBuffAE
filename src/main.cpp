#include <pch.h>

// Patches
#include <Patches/EncounterZoneResetPatch.h>
#include <Patches/TESObjectREFRGetEncounterZonePatch.h>
#include <Patches/ActorIsHostileToActorPatch.h>
#include <Patches/UnalignedLoadPatch.h>
#include <Patches/CellInitPatch.h>
#include <Patches/ExperimentalPatch.h>

namespace Main
{
    // Config Options
    static REX::INI::Bool iEncounterZoneResetPatch{ "Patches"sv, "EnableEncounterZoneResetPatch"sv, true };
    static REX::INI::Bool iTESObjectREFRGetEncounterZonePatch{ "Patches"sv, "EnableTESObjectREFRGetEncounterZonePatch"sv, true };
    static REX::INI::Bool iActorIsHostileToActorPatch{ "Patches"sv, "EnableActorIsHostileToActorPatch"sv, true };
    static REX::INI::Bool iUnalignedLoadPatch{ "Patches"sv, "EnableUnalignedLoadPatch"sv, true };
    static REX::INI::Bool iCellInitPatch{ "Patches"sv, "EnableCellInitPatch"sv, true };
    static REX::INI::Bool iExperimentalPatch{ "Patches"sv, "EnableExperimentalPatch"sv, true };

    // -------- Normal Patches -------- //

    void InstallPatches()
    {
        // Patch TESObjectREFRGetEncounterZone
        if (iTESObjectREFRGetEncounterZonePatch.GetValue() == true)
        {
            if (Patches::TESObjectREFRGetEncounterZonePatch::Install())
            {
                REX::INFO("Achievements Patch Initialized!");
            }
        }

        // Patch ActorIsHostileToActor
        if (iActorIsHostileToActorPatch.GetValue() == true)
        {
            if (Patches::ActorIsHostileToActorPatch::Install())
            {
                REX::INFO("ActorIsHostileToActor Patched!");
            }
        }

        // Patch UnalignedLoad
        if (iUnalignedLoadPatch.GetValue() == true)
        {
            if (Patches::UnalignedLoadPatch::Install())
            {
                REX::INFO("UnalignedLoad Patched!");
            }
        }

        // Patch CellInit
        if (iCellInitPatch.GetValue() == true)
        {
            if (Patches::CellInitPatch::Install())
            {
                REX::INFO("CellInit Patched!");
            }
        }

        // Experimental Patch
        if (iExperimentalPatch.GetValue() == true)
        {
            if (Patches::ExperimentalPatch::Install())
            {
                REX::INFO("Experimental Patch Loaded, here be dragons!");
            }
        }
    }

    // -------- Late Patches -------- //

    void InstallLatePatches()
    {
        // Patch EncounterZoneResetPatch
        if (iEncounterZoneResetPatch.GetValue() == true)
        {
            if (Patches::EncounterZoneResetPatch::Install())
            {
                REX::INFO("EncounterZoneReset Patched!");
            }
        }
    }

    // -------- F4SE Functions -------- //

    void F4SEMessageListener(F4SE::MessagingInterface::Message* a_msg)
    {
        switch (a_msg->type)
        {
        case F4SE::MessagingInterface::kGameLoaded:
        {
            InstallLatePatches();
            break;
        }
        default:
            break;
        }
    }

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

        // Install Patches
        InstallPatches();

        // Listen for Messages (to Install Late Patches)
        auto MessagingInterface = F4SE::GetMessagingInterface();
        MessagingInterface->RegisterListener(F4SEMessageListener);
        REX::INFO("Started Listening for F4SE Message Callbacks.");

        // Finished
        REX::INFO("MiniBuff AE Initialized!");
        return true;
    }
}
