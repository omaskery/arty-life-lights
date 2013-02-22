#ifndef MIND_HPP
#define MIND_HPP

#include <memory>

namespace alife
{
	template<typename InnerState, typename Perception>
	class mind
	{
	public:
		virtual std::unique_ptr<mind<InnerState,Perception>> think(InnerState &_state, const Perception &_perception) = 0;
	};
};

#endif
