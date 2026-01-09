#pragma once

namespace CellInitPatch
{
	namespace detail
	{
		inline RE::BGSLocation* GetLocation(const RE::TESObjectCELL* a_cell)
		{
			const auto xLoc =
				a_cell && a_cell->extraList ?
				a_cell->extraList->GetByType<RE::ExtraLocation>() :
				nullptr;
			auto loc = xLoc ? xLoc->location : nullptr;

			if (loc && a_cell && !a_cell->IsInitialized())
			{
				auto id = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(a_cell));
				const auto file = a_cell->GetFile();
				RE::TESForm::AddCompileIndex(id, file);
				loc = RE::TESForm::GetFormByID<RE::BGSLocation>(id);
			}

			return loc;
		}

		struct Patch : Xbyak::CodeGenerator
		{
			explicit Patch(std::uintptr_t a_target)
			{
				mov(rcx, rbx);  // rbx == TESObjectCELL*
				mov(rdx, a_target);
				jmp(rdx);
			}
		};
	}

	inline bool Install()
	{
		REL::Relocation<std::uintptr_t> Target{ REL::ID(2200179), REL::Offset(0x3E) };

		detail::Patch p{ reinterpret_cast<std::uintptr_t>(detail::GetLocation) };
		p.ready();

		auto& trampoline = REL::GetTrampoline();
		trampoline.write_call<5>(Target.address(), trampoline.allocate(p));

		return true;
	}
}
