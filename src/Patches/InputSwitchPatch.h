#pragma once

namespace Patches::InputSwitchPatch
{
	namespace detail
	{
		class DeviceSwapHandler : public RE::BSInputEventUser
		{
		public:
			[[nodiscard]] static DeviceSwapHandler* GetSingleton()
			{
				static DeviceSwapHandler singleton;
				return &singleton;
			}

			DeviceSwapHandler(const DeviceSwapHandler&) = delete;
			DeviceSwapHandler& operator=(const DeviceSwapHandler&) = delete;

			bool ShouldHandleEvent(const RE::InputEvent* a_event) override
			{
				if (a_event)
				{
					const auto& event = *a_event;
					auto input = Device::none;
					switch (*event.device)
					{
					case RE::INPUT_DEVICE::kKeyboard:
					case RE::INPUT_DEVICE::kMouse:
						input = Device::kbm;
						break;
					case RE::INPUT_DEVICE::kGamepad:
						input = Device::gamepad;
						break;
					}

					if (input != Device::none && _active.device != input)
					{
						_active.device = input;
						UpdateControls();
					}

					if (const auto id = event.As<RE::IDEvent>(); id)
					{
						auto& control = id->strUserEvent;
						const auto trySet = [&](auto& a_device) noexcept
						{
							if (const auto mouse = event.As<RE::MouseMoveEvent>(); mouse && (mouse->mouseInputX != 0 || mouse->mouseInputY != 0))
							{
								a_device = input;
							}
							else
							{
								a_device = input;
							}
						};

						if (control == _strings.look)
						{
							trySet(_active.looking);
						}
					}

					if (_proxied.controlMap)
					{
						_proxied.controlMap->SetIgnoreKeyboardMouse(false);
					}
				}

				return false;
			}

			[[nodiscard]] bool IsGamepadActiveDevice() const noexcept { return _active.device == Device::gamepad; }
			[[nodiscard]] bool IsGamepadActiveLooking() const noexcept { return _active.looking == Device::gamepad; }

		private:
			using AddMessage_t = void(RE::UIMessageQueue&, const RE::BSFixedString&, RE::UI_MESSAGE_TYPE);
			using UpdateGamepadDependentButtonCodes_t = void(bool);

			DeviceSwapHandler() = default;

			enum class Device
			{
				none,
				kbm,
				gamepad
			};

			void UpdateControls()
			{
				if (_proxied.ui && _proxied.msgq)
				{
					RE::BSAutoReadLock _{ _proxied.menuMapRWLock };
					for (const auto& [menu, entry] : _proxied.ui->menuMap)
					{
						_proxied.addMessage(*_proxied.msgq, menu, RE::UI_MESSAGE_TYPE::kUpdateController);
					}
				}

				_proxied.updateGamepadDependentButtonCodes(IsGamepadActiveDevice());
			}

			struct
			{
				RE::UI*& ui{ *reinterpret_cast<RE::UI**>(REL::ID(4796314).address()) }; // OG: 548587, NG: 2689028
				RE::BSReadWriteLock& menuMapRWLock{ *reinterpret_cast<RE::BSReadWriteLock*>(REL::ID(2707105).address()) }; // OG: 578487
				RE::UIMessageQueue*& msgq{ *reinterpret_cast<RE::UIMessageQueue**>(REL::ID(4796377).address()) }; // OG: 82123, NG: 2689091
				RE::ControlMap*& controlMap{ *reinterpret_cast<RE::ControlMap**>(REL::ID(4799307).address()) }; // OG: 325206, NG: 2692014
				AddMessage_t* const addMessage{ reinterpret_cast<AddMessage_t*>(REL::ID(2284929).address()) }; // OG: 1182019
				UpdateGamepadDependentButtonCodes_t* const updateGamepadDependentButtonCodes{ reinterpret_cast<UpdateGamepadDependentButtonCodes_t*>(REL::ID(4483350).address()) }; // OG: 190238, NG: 2249714
			} _proxied;  // this runs on a per-frame basis, so try to optimize perfomance

			struct
			{
				RE::BSFixedString look{ "Look" };
			} _strings;  // use optimized pointer comparison instead of slow string comparison

			struct
			{
				std::atomic<Device> device{ Device::none };
				std::atomic<Device> looking{ Device::none };
			} _active;
		};

		inline void RefreshCursor(RE::PipboyMenu& a_self)
		{
			using UIMF = RE::UI_MENU_FLAGS;

			const auto controls = RE::ControlMap::GetSingleton();
			const auto cursor = RE::MenuCursor::GetSingleton();
			const auto manager = RE::PipboyManager::GetSingleton();

			bool cursorEnabled = false;
			if (REL::Relocation<RE::PIPBOY_PAGES*> curPage{ REL::ID(4804316) }; *curPage == RE::PIPBOY_PAGES::kMap) // OG: 1287022, NG: 2696944
			{
				cursorEnabled = !a_self.showingModalMessage;
			}

			const auto gamepadMenuing = DeviceSwapHandler::GetSingleton()->IsGamepadActiveDevice();
			if (!gamepadMenuing)
			{
				cursorEnabled = true;
			}

			const auto usedCursor = a_self.UsesCursor();
			const auto assignedCursor = a_self.AssignsCursorToRenderer();
			a_self.UpdateFlag(UIMF::kUsesCursor, cursorEnabled);
			a_self.UpdateFlag(UIMF::kAssignCursorToRenderer, gamepadMenuing);
			a_self.pipboyCursorEnabled = cursorEnabled;

			if (usedCursor != a_self.UsesCursor() && cursor)
			{
				if (a_self.UsesCursor())
				{
					cursor->RegisterCursor();
				}
				else
				{
					cursor->UnregisterCursor();
				}
			}

			if (controls)
			{
				using Context = RE::UserEvents::INPUT_CONTEXT_ID;
				const auto reset = [&](Context a_context, bool a_condition)
				{
					while (controls->PopInputContext(a_context)) {}
					if (a_condition)
					{
						controls->PushInputContext(a_context);
					}
				};

				reset(Context::kThumbNav, gamepadMenuing);
				reset(Context::kLThumbCursor, gamepadMenuing && cursorEnabled);
			}

			if (cursorEnabled && cursor && manager)
			{
				if (gamepadMenuing)
				{
					if (assignedCursor != a_self.AssignsCursorToRenderer())
					{
						cursor->CenterCursor();
					}

					manager->UpdateCursorConstraint(true);
				}
				else
				{
					cursor->ClearConstraints();
					if (const auto model = RE::FlatScreenModel::GetSingleton(); model)
					{
						constexpr stl::enumeration flags{ UIMF::kAssignCursorToRenderer };
						RE::BSUIMessageData::SendUIStringUIntMessage(RE::CursorMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, model->customRendererName, flags.underlying());
					}
				}
			}

			if (const auto ui = RE::UI::GetSingleton(); ui)
			{
				ui->RefreshCursor();
			}
		}

		inline void InstallRefreshCursorPatch()
		{
			// PipboyMenu::RefreshCursor
			{
				const auto target = REL::ID(2224196).address(); // OG: 1533778, 0xE9
				stl::asm_jump(target, 0xE9, reinterpret_cast<std::uintptr_t>(RefreshCursor));
			}

			// PipboyMenu::ProcessMessage - skip redundant input context switching
			{
				const auto target = REL::ID(2224181).address(); // OG: 643948, 0x57C, 0x64
				REL::WriteSafeFill(target + 0x5FB, REL::NOP, 0x64);
			}

			// PipboyMenu::PipboyMenu - skip assigning cursor to renderer
			{
				const auto target = REL::ID(2224179).address(); // OG: 712982, 0x132, 0x17
				REL::WriteSafeFill(target + 0x139, REL::NOP, 0x17);
			}
		}

		inline void DisableDisconnectHandler()
		{
			REL::ID target{ 2249389 }; // OG: 548136, 0x98, 0x4E, NG: 2249389, 0x9C, 0x4E
			REL::WriteSafeFill(target.address() + 0x94, REL::NOP, 0x4E);
		}

		inline void DisableKBMIgnore()
		{
			const auto target = REL::ID(2268334).address(); // OG: 647956, 0x39, 0x6
			REL::WriteSafeFill(target + 0x40, REL::NOP, 0x7);
		}

		inline bool IsGamepadConnected(const RE::BSInputDeviceManager&)
		{
			const auto handler = DeviceSwapHandler::GetSingleton();
			return handler->IsGamepadActiveDevice();
		}

		inline void InstallGamepadConnectedPatch()
		{
			const auto target = REL::ID(2268387).address(); // OG: 609928, 0x25
			constexpr std::size_t size = 0x25;
			REL::WriteSafeFill(target, REL::INT3, size);
			stl::asm_jump(target, size, reinterpret_cast<std::uintptr_t>(IsGamepadConnected));
		}

		inline bool UsingGamepad(const RE::BSInputDeviceManager&)
		{
			const auto handler = DeviceSwapHandler::GetSingleton();
			return handler->IsGamepadActiveDevice();
		}

		inline void InstallUsingGamepadPatch()
		{
			const auto target = REL::ID(2268386).address(); // OG: 875683, 0x25
			constexpr std::size_t size = 0x25;
			REL::WriteSafeFill(target, REL::INT3, size);
			stl::asm_jump(target, size, reinterpret_cast<std::uintptr_t>(UsingGamepad));
		}

		inline bool UsingGamepadLook(const RE::BSInputDeviceManager&)
		{
			const auto handler = DeviceSwapHandler::GetSingleton();
			return handler->IsGamepadActiveLooking();
		}

		inline void InstallGamepadLookPatches()
		{
			const auto patch = [](REL::ID a_base, std::size_t a_offset)
			{
				auto& trampoline = REL::GetTrampoline();
				trampoline.write_call<5>(a_base.address() + a_offset, UsingGamepadLook);
			};

			patch(REL::ID(2223308), 0x58); // OG: 1349441,	0x58	// LevelUpMenu::ZoomGrid
			patch(REL::ID(2234801), 0x3C); // OG: 455462,	0x43	// PlayerControls::ProcessLookInput
			patch(REL::ID(2234829), 0x56); // OG: 53721,	0x56	// PlayerControlsUtils::ProcessLookControls
			patch(REL::ID(2248276), 0x1F); // OG: 1262531,	0x1F	// FirstPersonState::CalculatePitchOffsetChaseValue
		}

		static void PipboyMenuPreDtor()
		{
			if (const auto controls = RE::ControlMap::GetSingleton(); controls)
			{
				using RE::UserEvents::INPUT_CONTEXT_ID::kLThumbCursor;
				while (controls->PopInputContext(kLThumbCursor)) {}
			}
		}

		struct DtorPatch : Xbyak::CodeGenerator
		{
			DtorPatch(std::uintptr_t a_ret)
			{
				push(rcx);
				sub(rsp, 0x8);   // alignment
				sub(rsp, 0x20);  // function call
				mov(rax, reinterpret_cast<std::uintptr_t>(PipboyMenuPreDtor));
				call(rax);
				add(rsp, 0x20);
				add(rsp, 0x8);
				pop(rcx);

				// restore
				mov(ptr[rsp + 0x8], rbx);
				mov(ptr[rsp + 0x10], rbp);

				mov(rax, a_ret);
				jmp(rax);
			}
		};

		inline void InstallPipboyMenuStatePatches()
		{
			const auto patch = []<class T>(std::in_place_type_t<T>, REL::ID a_func, std::size_t a_size)
			{
				assert(a_size >= 6);

				const auto target = a_func.address();
				REL::WriteSafeFill(target, REL::NOP, a_size);

				T p{ target + a_size };
				p.ready();

				auto& trampoline = REL::GetTrampoline();
				trampoline.write_jmp<6>(target, trampoline.allocate(p));
			};

			patch(std::in_place_type<DtorPatch>, REL::ID(2224180), 0xA); // OG: 405150, 0xA
		}
	}

	inline bool InstallPreLoad()
	{
		detail::DisableDisconnectHandler();
		detail::DisableKBMIgnore();
		detail::InstallRefreshCursorPatch();
		detail::InstallGamepadConnectedPatch();
		detail::InstallUsingGamepadPatch();
		detail::InstallGamepadLookPatches();
		detail::InstallPipboyMenuStatePatches();

		return true;
	}

	inline bool InstallPostInit()
	{
		if (const auto Controls = RE::MenuControls::GetSingleton(); Controls)
		{
			Controls->handlers.insert(Controls->handlers.begin(), detail::DeviceSwapHandler::GetSingleton());
			return true;
		}

		return false;
	}
}
