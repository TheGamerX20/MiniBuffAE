#pragma once

namespace Patches::MaxStdIOPatch
{
	static REX::INI::I32 iMaxStdIO{ "MaxStdIOPatch"sv, "MaxStdIO"sv, 2048 };

	inline bool InstallPreLoad()
	{
		const auto handle = REX::W32::GetModuleHandleW(L"api-ms-win-crt-runtime-l1-1-0.dll");
		const auto proc = handle ? reinterpret_cast<decltype(&_setmaxstdio)>(REX::W32::GetProcAddress(handle, "_setmaxstdio")) : nullptr;
		
		if (proc != nullptr)
		{
			const auto get = reinterpret_cast<decltype(&_getmaxstdio)>(REX::W32::GetProcAddress(handle, "_getmaxstdio"));
			const auto old = get();
			const auto result = proc(static_cast<int>(iMaxStdIO.GetValue()));
			
			if (get)
			{
				REX::INFO("        Set MaxStdIO to {} from {}", result, old);
			}
			else
			{
				REX::INFO("        Set MaxStdIO to {}", result);
			}
		}
		else
		{
			REX::WARN("        Failed to set MaxStdIO!");
		}

		return true;
	}
}
