#pragma once

namespace Patches::SafeExitPatch
{
	namespace detail
	{
		inline void Shutdown()
		{
			REX::W32::TerminateProcess(REX::W32::GetCurrentProcess(), EXIT_SUCCESS);
		}
	}

	inline bool InstallPreLoad()
	{
		auto& trampoline = REL::GetTrampoline();
		REL::Relocation<std::uintptr_t> Target{ REL::ID(4812562), REL::Offset(0x20) };
		trampoline.write_call<5>(Target.address(), detail::Shutdown);

		return true;
	}
}
