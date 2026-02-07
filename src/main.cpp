#include <pch.h>

// Patches
#include <Patches/AchievementsPatch.h>
#include <Patches/ActorIsHostileToActorPatch.h>
#include <Patches/BGSAIWorldLocationRefRadiusPatch.h>
#include <Patches/BSPreCulledObjectsPatch.h>
#include <Patches/CellInitPatch.h>
#include <Patches/CreateD3DAndSwapChainPatch.h>
#include <Patches/EncounterZoneResetPatch.h>
#include <Patches/ExperimentalPatch.h>
#include <Patches/ImageSpaceAdapterWarningPatch.h>
#include <Patches/INISettingCollectionPatch.h>
#include <Patches/InputSwitchPatch.h>
#include <Patches/InteriorNavCutPatch.h>
#include <Patches/MagicEffectApplyEventPatch.h>
#include <Patches/MaxStdIOPatch.h>
#include <Patches/MovementPlannerPatch.h>
#include <Patches/PipBoyLightInvPatch.h>
#include <Patches/SafeExitPatch.h>
#include <Patches/TESObjectREFRGetEncounterZonePatch.h>
#include <Patches/UnalignedLoadPatch.h>
#include <Patches/WorkbenchSwapPatch.h>

namespace Main
{
    // Fixes
    static REX::INI::Bool iActorIsHostileToActorPatch{              "Fixes"sv,          "ActorIsHostileToActor"sv,                          true        };
    static REX::INI::Bool iBGSAIWorldLocationRefRadiusPatch{        "Fixes"sv,          "BGSAIWorldLocationRefRadiusNull"sv,                true        };
    static REX::INI::Bool iCellInitPatch{                           "Fixes"sv,          "CellInit"sv,                                       true        };
    static REX::INI::Bool iCreateD3DAndSwapChainPatch{              "Fixes"sv,          "CreateD3DAndSwapChain"sv,                          true        };
    static REX::INI::Bool iEncounterZoneResetPatch{                 "Fixes"sv,          "EncounterZoneReset"sv,                             true        };
    static REX::INI::Bool iInteriorNavCutPatch{                     "Fixes"sv,          "InteriorNavCut"sv,                                 true        };
    static REX::INI::Bool iMagicEffectApplyEventPatch{              "Fixes"sv,          "MagicEffectApplyEvent"sv,                          true        };
    static REX::INI::Bool iMovementPlannerPatch{                    "Fixes"sv,          "MovementPlanner"sv,                                true        };
    static REX::INI::Bool iPipBoyLightInvPatch{                     "Fixes"sv,          "PipboyLightInvFix"sv,                              true        };
    static REX::INI::Bool iSafeExitPatch{                           "Fixes"sv,          "SafeExit"sv,                                       true        };
    static REX::INI::Bool iTESObjectREFRGetEncounterZonePatch{      "Fixes"sv,          "TESObjectREFRGetEncounterZone"sv,                  true        };
    static REX::INI::Bool iUnalignedLoadPatch{                      "Fixes"sv,          "UnalignedLoad"sv,                                  true        };
    static REX::INI::Bool iWorkbenchSwapPatch{                      "Fixes"sv,          "WorkBenchSwap"sv,                                  true        };
    static REX::INI::Bool iExperimentalPatch{                       "Fixes"sv,          "EnableExperimentalPatch"sv,                        false       };

    // Patches
    static REX::INI::Bool iAchievementsPatch{                       "Patches"sv,        "Achievements"sv,                                   true        };
    static REX::INI::Bool iBSPreCulledObjectsPatch{                 "Patches"sv,        "BSPreCulledObjects"sv,                             true        };
    static REX::INI::Bool iINISettingCollectionPatch{               "Patches"sv,        "INISettingCollection"sv,                           true        };
    static REX::INI::Bool iInputSwitchPatch{                        "Patches"sv,        "InputSwitch"sv,                                    true        };
    static REX::INI::I32  iMaxStdIOPatch{                           "Patches"sv,        "MaxStdIO"sv,                                       2048        };
    
    // Warnings
    static REX::INI::Bool iImageSpaceAdapterWarningPatch{           "Warnings"sv,       "ImageSpaceAdapter"sv,                              true        };
    
    // Helper Function
    template <typename Func>
    void ApplyPatch(std::string_view PatchName, bool ShouldApplyPatch, Func&& PatchInstallFunction)
    {
        if (!ShouldApplyPatch)
        {
            return;
        }

        REX::INFO("    Patching {}...", PatchName);

        if (PatchInstallFunction())
        {
            REX::INFO("        Successfully Patched!");
        }
        else {
            REX::WARN("        Failed to Patch!");
        }
    }

    // -------- PreLoad Patches -------- //

    void InstallPreLoadPatches()
    {
        REX::INFO("Installing PreLoad Patches...");
        
        ApplyPatch("Achievements",                      iAchievementsPatch.GetValue(),                          Patches::AchievementsPatch::InstallPreLoad                                              );
        ApplyPatch("ActorIsHostileToActor",             iActorIsHostileToActorPatch.GetValue(),                 Patches::ActorIsHostileToActorPatch::InstallPreLoad                                     );
        ApplyPatch("BGSAIWorldLocationRefRadius",       iBGSAIWorldLocationRefRadiusPatch.GetValue(),           Patches::BGSAIWorldLocationRefRadiusPatch::InstallPreLoad                               );
        ApplyPatch("BSPreCulledObjects",                iBSPreCulledObjectsPatch.GetValue(),                    Patches::BSPreCulledObjectsPatch::InstallPreLoad                                        );
        ApplyPatch("CellInit",                          iCellInitPatch.GetValue(),                              Patches::CellInitPatch::InstallPreLoad                                                  );
        ApplyPatch("CreateD3DAndSwapChain",             iCreateD3DAndSwapChainPatch.GetValue(),                 Patches::CreateD3DAndSwapChainPatch::InstallPreLoad                                     );
        ApplyPatch("ImageSpaceAdapterWarningPatch",     iImageSpaceAdapterWarningPatch.GetValue(),              Patches::ImageSpaceAdapterWarningPatch::InstallPreLoad                                  );
        ApplyPatch("INISettingCollection",              iINISettingCollectionPatch.GetValue(),                  Patches::INISettingCollectionPatch::InstallPreLoad                                      );
        ApplyPatch("InputSwitch",                       iInputSwitchPatch.GetValue(),                           Patches::InputSwitchPatch::InstallPreLoad                                               );
        ApplyPatch("MagicEffectApplyEvent",             iMagicEffectApplyEventPatch.GetValue(),                 Patches::MagicEffectApplyEventPatch::InstallPreLoad                                     );
        ApplyPatch("MaxStdIO",                          iMaxStdIOPatch.GetValue() > 512,                        [&]() { return Patches::MaxStdIOPatch::InstallPreLoad(iMaxStdIOPatch.GetValue()); }     );
        ApplyPatch("MovementPlanner",                   iMovementPlannerPatch.GetValue(),                       Patches::MovementPlannerPatch::InstallPreLoad                                           );
        ApplyPatch("PipBoyLightInv",                    iPipBoyLightInvPatch.GetValue(),                        Patches::PipBoyLightInvPatch::InstallPreLoad                                            );
        ApplyPatch("SafeExit",                          iSafeExitPatch.GetValue(),                              Patches::SafeExitPatch::InstallPreLoad                                                  );
        ApplyPatch("TESObjectREFRGetEncounterZone",     iTESObjectREFRGetEncounterZonePatch.GetValue(),         Patches::TESObjectREFRGetEncounterZonePatch::InstallPreLoad                             );
        ApplyPatch("UnalignedLoad",                     iUnalignedLoadPatch.GetValue(),                         Patches::UnalignedLoadPatch::InstallPreLoad                                             );
        ApplyPatch("WorkbenchSwap",                     iWorkbenchSwapPatch.GetValue(),                         Patches::WorkbenchSwapPatch::InstallPreLoad                                             );

        // Experimental
        ApplyPatch("Experimental",                      iExperimentalPatch.GetValue(),                          Patches::ExperimentalPatch::InstallPreLoad                                              );

        REX::INFO("Installed PreLoad Patches!");
    }

    // -------- GameDataReady Patches -------- //

    void InstallGameDataReadyPatches()
    {
        REX::INFO("Installing GameDataReady Patches...");

        ApplyPatch("InteriorNavCut",                    iInteriorNavCutPatch.GetValue(),                        Patches::InteriorNavCutPatch::RegisterNavMeshUpdateListener                             );

        REX::INFO("Installed GameDataReady Patches!");
    }

    // -------- PostInit Patches -------- //

    void InstallPostInitPatches()
    {
        REX::INFO("Installing PostInit Patches...");

        ApplyPatch("EncounterZoneReset",                iEncounterZoneResetPatch.GetValue(),                    Patches::EncounterZoneResetPatch::InstallPostInit                                       );
        ApplyPatch("InputSwitch",                       iInputSwitchPatch.GetValue(),                           Patches::InputSwitchPatch::InstallPostInit                                              );
        
        REX::INFO("Installed PostInit Patches!");
    }

    // -------- PostLoadGame Patches -------- //

    void InstallPostLoadGamePatches()
    {
        REX::INFO("Applying PostLoadGame Patches...");

        ApplyPatch("InteriorNavCut",                    iInteriorNavCutPatch.GetValue(),                        Patches::InteriorNavCutPatch::ForceNavMeshUpdate                                        );

        REX::INFO("Applied PostLoadGame Patches!");
    }

    // -------- F4SE Functions -------- //

    void F4SEMessageListener(F4SE::MessagingInterface::Message* a_msg)
    {
        switch (a_msg->type)
        {
        case F4SE::MessagingInterface::kGameDataReady:
            InstallGameDataReadyPatches();

            break;
        case F4SE::MessagingInterface::kGameLoaded:
        {
            InstallPostInitPatches();
            REX::INFO("Buffout 4 AE Initialized!");

            break;
        }
        case F4SE::MessagingInterface::kPostLoadGame:
            InstallPostLoadGamePatches();

            break;
        default:
            break;
        }
    }

    F4SE_PLUGIN_PRELOAD(const F4SE::LoadInterface* a_f4se)
    {
        // Init
        F4SE::Init(a_f4se);
        REX::INFO("Bufout 4 AE Initializing...");

        // Load the Config
        const auto config = REX::INI::SettingStore::GetSingleton();
        config->Init("Data/F4SE/Plugins/Buffout4AE.ini", "Data/F4SE/Plugins/Buffout4AECustom.ini");
        config->Load();

        // Get the Trampoline and Allocate
        auto& trampoline = REL::GetTrampoline();
        trampoline.create(512);

        // Listen for Messages (to Install PostInit Patches)
        auto MessagingInterface = F4SE::GetMessagingInterface();
        MessagingInterface->RegisterListener(F4SEMessageListener);
        REX::INFO("Started Listening for F4SE Message Callbacks.");

        // Install PreLoad Patches
        InstallPreLoadPatches();

        // Finished
        return true;
    }
}
