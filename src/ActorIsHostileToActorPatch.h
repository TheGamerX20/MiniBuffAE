#pragma once

namespace ActorIsHostileToActorPatch
{
	namespace detail
	{
		inline bool IsHostileToActor(RE::BSScript::IVirtualMachine* a_vm, std::uint32_t a_stackID, RE::Actor* a_self, RE::Actor* a_actor)
		{
			if (!a_actor)
			{
				RE::GameScript::LogFormError(a_actor, "Cannot call IsHostileToActor with a None actor", a_vm, a_stackID);
				return false;
			}
			else
			{
				return a_self->GetHostileToActor(a_actor);
			}
		}
	}

	inline bool Install()
	{
		constexpr std::size_t Size = 0x10;
		REL::Relocation<std::uintptr_t> Target{ REL::ID(4486975) };

		REL::WriteSafeFill(Target.address(), REL::INT3, Size);
		stl::asm_jump(Target.address(), Size, reinterpret_cast<std::uintptr_t>(detail::IsHostileToActor));

		return true;
	}
}
