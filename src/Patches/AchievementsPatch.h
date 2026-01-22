#pragma once

namespace Patches::AchievementsPatch
{
	namespace detail
	{
		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				xor_(rax, rax);
				ret();
			}
		};
	}

	inline bool InstallPreLoad()
	{
		REL::Relocation<std::uintptr_t> Target{ REL::ID(2192323) };
		std::size_t Size = 0x6E;
		const auto Address = Target.address();
		REL::WriteSafeFill(Address, REL::INT3, Size);

		detail::Patch p;
		p.ready();

		assert(p.getSize() < Size);
		REL::WriteSafe(Address, std::span{ p.getCode<const std::byte*>(), p.getSize() });

		return true;
	}
}
