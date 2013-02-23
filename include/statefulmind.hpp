#ifndef STATEFULMIND_HPP
#define STATEFULMIND_HPP

#include "mind.hpp"

namespace alife
{
	template<typename InnerState, typename Perception>
	class stateful_mind : public mind<InnerState,Perception>
	{
	public:
		stateful_mind(std::unique_ptr<mind<InnerState,Perception>> &&_state)
			: mState(std::move(_state)) {}
		virtual std::unique_ptr<mind<InnerState,Perception>> think(InnerState &_state, const Perception &_perception) override {
			if(mState)
			{
				auto next = mState->think(_state, _perception);
				if(next) mState = std::move(next);
			}

			return std::unique_ptr<mind<InnerState,Perception>>(nullptr);
		}
	private:
		std::unique_ptr<mind<InnerState,Perception>> mState;
	};
};

#endif