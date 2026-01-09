#pragma once

namespace TESObjectREFRGetEncounterZonePatch
{
	namespace detail
	{
		template <class T>
		struct GetEncounterZone
		{
			static T* thunk(const RE::BSTSmartPointer<RE::ExtraDataList>& a_in)
			{
				const auto& ref = *REX::ADJUST_POINTER<RE::TESObjectREFR>(&a_in, -static_cast<std::ptrdiff_t>(offsetof(RE::TESObjectREFR, RE::TESObjectREFR::extraList)));
				auto ptr = ref.extraList ? func(*ref.extraList) : nullptr;

				const auto addr = reinterpret_cast<std::uintptr_t>(ptr);
				if (!ref.IsInitialized() &&
					((addr & 0xFFFF'FFFF'0000'0000) == 0) &&
					((addr & 0x0000'0000'FFFF'FFFF) != 0))
				{
					auto id = static_cast<std::uint32_t>(addr);
					RE::TESForm::AddCompileIndex(id, ref.GetFile());
					ptr = RE::TESForm::GetFormByID<T>(id);
				}

				return ptr;
			}

			static inline REL::Relocation<T*(const RE::ExtraDataList&)> func;
		};
	}

	inline bool Install()
	{
		REL::Relocation<std::uintptr_t> Target{ REL::ID(2202627) };

		REL::WriteSafeFill(Target.address() + 0xE, static_cast<std::uint8_t>(0x8D), 1); // mov -> lea
		stl::write_thunk_call<5, detail::GetEncounterZone<RE::BGSEncounterZone>>(Target.address() + 0x14);

		REL::WriteSafeFill(Target.address() + 0x92, static_cast<std::uint8_t>(0x8D), 1); // mov -> lea
		stl::write_thunk_call<5, detail::GetEncounterZone<RE::BGSLocation>>(Target.address() + 0x98);

		return true;
	}
}
