#pragma once
namespace wf
{
	class ISystem
	{
	public:
		ISystem() = default;
		virtual ~ISystem() = default;

		virtual bool init() = 0;
		virtual void update(float dt) {}
		virtual void fixedUpdate(float dt) {}
		virtual void render(float dt) {}
		virtual void shutdown() {}
	};
}