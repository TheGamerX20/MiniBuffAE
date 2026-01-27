#pragma once

namespace Patches::BGSAIWorldLocationRefRadiusPatch
{
	namespace detail
	{
		struct Patch : Xbyak::CodeGenerator
		{
			explicit Patch(std::uintptr_t a_dest, std::uintptr_t a_rtn)
			{
				Xbyak::Label contLab;
				Xbyak::Label retLab;

				// code clobbered at target is placed here
				movss(qword[rbx + 0x10], xmm0);
				// end clobbered code
				test(rsi, rsi);    // nullptr check on rsi
				jz("returnFunc");  // jump to returnFunc if rsi is null
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

	inline bool InstallPreLoad()
	{
		const auto base = REL::ID(2188379).address();

		REL::Relocation<std::uintptr_t> target{ base + 0x4E };
		REL::Relocation<std::uintptr_t> resume{ target.address() + 0x5 };
		REL::Relocation<std::uintptr_t> returnAddr{ base + 0xF8 };

		const auto instructionBytes = resume.address() - target.address();
		for (std::size_t i = 0; i < instructionBytes; i++)
		{
			REL::WriteSafeData(target.address() + i, REL::NOP);
		}

		detail::Patch p{ resume.address(), returnAddr.address() };

		p.ready();

		auto& trampoline = REL::GetTrampoline();
		trampoline.write_jmp<5>(target.address(), trampoline.allocate(p));

		return true;
	}
}
