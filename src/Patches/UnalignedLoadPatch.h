#pragma once

namespace Patches::UnalignedLoadPatch
{
	namespace detail
	{
		inline void ApplySkinningToGeometry()
		{
			REL::Relocation<std::uintptr_t> Target{ REL::ID(2277131), REL::Offset(0x192) };

			std::uint32_t Value = 0x10;
			REL::WriteSafe(Target.address(), &Value, sizeof(Value));
		}
	}

	inline bool InstallPreLoad()
	{
		detail::ApplySkinningToGeometry();

		return true;
	}
}
