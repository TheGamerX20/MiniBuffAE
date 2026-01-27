#pragma once

namespace Patches::PipBoyLightInvPatch
{
	namespace detail
	{
		struct Patch : Xbyak::CodeGenerator
		{
			explicit Patch(std::uintptr_t a_dest, std::uintptr_t a_rtn, std::uintptr_t a_rbx_offset)
			{
				Xbyak::Label contLab;
				Xbyak::Label retLab;

				test(rbx, rbx);
				jz("returnFunc");
				mov(rcx, dword[rbx + a_rbx_offset]);
				test(rcx, rcx);
				jz("returnFunc");
				test(rax, rax);
				jz("returnFunc");
				jmp(ptr[rip + contLab]);

				L("returnFunc");
				jmp(ptr[rip + retLab]);

				L(contLab);
				dq(a_dest);

				L(retLab);
				dq(a_rtn);
			}
		};
	}

	inline bool InstallPostInit()
	{
		const auto base = REL::ID(2233255).address();

		REL::Relocation<std::uintptr_t> target{ base + 0xC92 };
		REL::Relocation<std::uintptr_t> resume{ target.address() + 0x7 };
		REL::Relocation<std::uintptr_t> returnAddr{ base + 0xD87 };

		const auto instructionBytes = resume.address() - target.address();
		for (std::size_t i = 0; i < instructionBytes; i++)
		{
			REL::WriteSafeData(target.address() + i, REL::NOP);
		}

		detail::Patch p{ resume.address(), returnAddr.address(), 0xC40 };
		p.ready();

		auto& trampoline = REL::GetTrampoline();
		trampoline.write_jmp<5>(target.address(), trampoline.allocate(p));

		return true;
	}
}
