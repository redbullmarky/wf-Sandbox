#pragma once
#include "Events/Base.h"

#include <functional>
#include <memory>
#include <SDL3/SDL.h>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace wf
{
	class EventDispatcher
	{
	public:
	private:
		struct ListenerBase
		{
			virtual ~ListenerBase() = default;
			virtual bool tryInvoke(BaseEvent* event) = 0;
		};

		template<typename T>
		struct Listener : ListenerBase
		{
			explicit Listener(std::function<void(T&)> cb) : callback(std::move(cb)) {}
			std::function<void(T&)> callback;

			bool tryInvoke(BaseEvent* event) override
			{
				if (auto ptr = dynamic_cast<T*>(event)) {
					callback(*ptr);
					return true;
				}
				return false;
			}
		};

	public:
		EventDispatcher() = default;
		~EventDispatcher() = default;

		/**
		 * @brief Trigger an event with no args
		 */
		template<typename T>
		void dispatch()
		{
			T event{};
			dispatch(event);
		}

		/**
		 * @brief Trigger an event with the details passed along
		 */
		template<typename T>
		void dispatch(T& event)
		{
			BaseEvent* basePtr = &event;  // now event *must* derive BaseEvent

			for (auto& [type, vec] : listeners) {
				for (auto& listener : vec) {
					listener->tryInvoke(basePtr);
				}
			}
		}

		/**
		 * @brief Regular listener registration
		 */
		template<typename T>
		void on(std::function<void(T&)> callback)
		{
			auto& vec = listeners[typeid(T)];
			vec.emplace_back(std::make_unique<Listener<T>>(std::move(callback)));
		}

		/**
		 * @brief Registration of callbacks that don't require args passing - just the fact the event occured is enough
		 */
		template<typename T>
		void on(std::function<void()> callback)
		{
			// Wrap the no-arg callback to a function that accepts T& and ignores it
			on<T>([cb = std::move(callback)](T&) {
				cb();
				});
		}

	private:
		std::unordered_map<std::type_index, std::vector<std::unique_ptr<ListenerBase>>> listeners;
	};
}