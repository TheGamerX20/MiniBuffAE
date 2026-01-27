#pragma once

#include <dxgi.h>

namespace Patches::CreateD3DAndSwapChainPatch
{
	namespace detail
	{
		::HRESULT GetDisplayModeList(::IDXGIOutput& a_this, ::DXGI_FORMAT a_enumFormat, ::UINT a_flags, ::UINT* a_numModes, ::DXGI_MODE_DESC* a_desc)
		{
			const auto result = a_this.GetDisplayModeList(a_enumFormat, a_flags, a_numModes, a_desc);

			const auto modes = std::span(a_desc, *a_numModes);
			const auto end = std::stable_partition(
				modes.begin(),
				modes.end(),
				[](const ::DXGI_MODE_DESC& a_desc) {
					return a_desc.RefreshRate.Denominator != 0;
				});
			*a_numModes = static_cast<::UINT>(end - modes.begin());

			return result;
		}
	}

	inline bool InstallPreLoad()
	{
		const auto target = REL::Relocation<std::uintptr_t>(REL::ID(4492363), REL::Offset(0x10B)).address();
		auto& trampoline = REL::GetTrampoline();
		trampoline.write_call<5>(target, detail::GetDisplayModeList);

		return true;
	}
}
