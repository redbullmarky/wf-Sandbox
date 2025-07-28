#pragma once
namespace wf
{
	class Application
	{
	public:
		Application() = default;
		virtual ~Application() = default;

		virtual bool init() = 0;
		virtual void run() = 0;
		virtual void shutdown() = 0;
	};
}