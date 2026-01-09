#pragma once

namespace Patches::EncounterZoneResetPatch
{
	namespace detail
	{
		class Sink : public RE::BSTEventSink<RE::CellAttachDetachEvent>
		{
		public:
			[[nodiscard]] static Sink* GetSingleton()
			{
				static Sink singleton;
				return std::addressof(singleton);
			}

		private:
			Sink() = default;
			Sink(const Sink&) = delete;
			Sink(Sink&&) = delete;
			~Sink() = default;
			Sink& operator=(const Sink&) = delete;
			Sink& operator=(Sink&&) = delete;

			RE::BSEventNotifyControl ProcessEvent(const RE::CellAttachDetachEvent& a_event, RE::BSTEventSource<RE::CellAttachDetachEvent>*) override
			{
				switch (*a_event.type)
				{
					case RE::CellAttachDetachEvent::EVENT_TYPE::kPreDetach:
					{
						const auto cell = a_event.cell;
						const auto ez = cell ? cell->GetEncounterZone() : nullptr;
						const auto calendar = RE::Calendar::GetSingleton();
						if (ez && calendar)
						{
							ez->SetDetachTime(static_cast<std::uint32_t>(calendar->GetHoursPassed()));
						}
					}
					break;
				default:
					break;
				}

				return RE::BSEventNotifyControl::kContinue;
			}
		};
	}

	inline bool Install()
	{
		auto& Cells = RE::CellAttachDetachEventSource::CellAttachDetachEventSourceSingleton::GetSingleton();
		Cells.source.RegisterSink(detail::Sink::GetSingleton());

		return true;
	}
}
