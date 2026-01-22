#pragma once

namespace Patches::MagicEffectApplyEventPatch
{
	namespace detail
	{
		struct ProcessEvent
		{
			static RE::BSEventNotifyControl thunk(RE::GameScript::CombatEventHandler& a_self, const RE::TESMagicEffectApplyEvent& a_event, RE::BSTEventSource<RE::TESMagicEffectApplyEvent>* a_source)
			{
				return a_event.target ? func(a_self, a_event, a_source) : RE::BSEventNotifyControl::kContinue;
			}

			static inline REL::Relocation<decltype(thunk)> func;
		};
	}

	inline bool InstallPreLoad()
	{
		REL::Relocation<std::uintptr_t> Target{ REL::ID(RE::GameScript::CombatEventHandler::VTABLE[1]) };
		detail::ProcessEvent::func = Target.write_vfunc(0x1, detail::ProcessEvent::thunk);

		return true;
	}
}
