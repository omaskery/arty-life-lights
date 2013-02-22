#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "mind.hpp"

namespace alife
{
	template<typename InnerState, typename Perception>
	class entity
	{
	public:
		entity(std::unique_ptr<mind<InnerState,Perception>> &&_mind, const InnerState &_state, const Perception &_perception)
			: mMind(std::move(_mind)), mState(_state), mPerception(_perception) {}
		inline InnerState &state() { return mState; }
		inline const InnerState &state() const { return mState; }
		inline void think() { mMind->think(mState, mPerception); }
		inline void update_perception(const Perception &_perception) { mPerception = _perception; }
	private:
		InnerState mState;
		Perception mPerception;
		std::unique_ptr<mind<InnerState,Perception>> mMind;
	};
};

#endif
