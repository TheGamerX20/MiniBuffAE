#pragma once

namespace Patches::ExperimentalPatch
{
	namespace detail
	{
		struct Patch : Xbyak::CodeGenerator
		{
			explicit Patch(std::uintptr_t ReturnAddress)
			{
				// Check Nullptr
				test(rcx, rcx);
				jz("nullptr");

				// Normal Behavior
				mov(rdi, ptr[rcx + 0x0C0]);
				jmp("continue");

				// We had a Nullptr
				L("nullptr");
				ret();

				// We did not have a Nullptr
				L("continue");

				// Jump to Original Code
				jmp(ptr[rip]);
				dq(ReturnAddress);
			}
		};
	}

	inline bool Install()
	{
		auto& trampoline = REL::GetTrampoline();

		REL::Relocation<std::uintptr_t> Target{ REL::ID(2220806), REL::Offset(0x10) };
		std::uintptr_t ReturnAddress = Target.address() + 0x7;

		detail::Patch p{ ReturnAddress };
		p.ready();

		REL::WriteSafeFill(Target.address(), 0x90, 7);
		trampoline.write_jmp<5>(Target.address(), trampoline.allocate(p));

		return true;
	}
}
