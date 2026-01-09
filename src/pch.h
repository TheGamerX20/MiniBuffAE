#pragma once

#include <RE/Fallout.h>
#include <F4SE/F4SE.h>
#include <REX/REX/INI.h>
#include <xbyak/xbyak.h>

using namespace std::literals;

namespace stl
{
	namespace detail
	{
		struct asm_patch : Xbyak::CodeGenerator
		{
			asm_patch(std::uintptr_t a_dst)
			{
				mov(rax, a_dst);
				jmp(rax);
			}
		};
	}

	inline void asm_jump(std::uintptr_t a_from, [[maybe_unused]] std::size_t a_size, std::uintptr_t a_to)
	{
		detail::asm_patch p{ a_to };
		p.ready();
		assert(p.getSize() <= a_size);
		REL::WriteSafe(a_from, std::span{ p.getCode<const std::byte*>(), p.getSize() });
	}

	template <std::size_t N, class T>
	void write_thunk_call(std::uintptr_t a_src)
	{
		auto& trampoline = REL::GetTrampoline();
		T::func = trampoline.write_call<N>(a_src, T::thunk);
	}
}