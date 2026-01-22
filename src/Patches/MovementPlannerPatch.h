#pragma once

namespace Patches::MovementPlannerPatch
{
	namespace detail
	{
		struct CanWarpOnPathFailure
		{
			static bool thunk(const RE::Actor* a_actor)
			{
				return a_actor ? func(a_actor) : true;
			}

			static inline REL::Relocation<decltype(thunk)> func;
		};
	}

	inline bool InstallPreLoad()
	{
		REL::Relocation<std::uintptr_t> Target{ REL::ID(2234683), 0x30 };
		stl::write_thunk_call<5, detail::CanWarpOnPathFailure>(Target.address());

		return true;
	}
}
