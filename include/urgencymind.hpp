#ifndef URGENCYMIND_HPP
#define URGENCYMIND_HPP

#include <vector>

#include "mind.hpp"

namespace alife
{
	template<typename InnerState, typename Perception>
	class urgency_rated_mind
	{
	public:
		urgency_rated_mind(std::unique_ptr<mind<InnerState,Perception>> _mind)
			: mMind(_mind) {}
		inline void think(InnerState &_state, const Perception &_perception) { mMind->think(_state, _perception); }
		virtual float urgency(const InnerState &_state, const Perception &_perception) = 0;
	private:
		std::unique_ptr<mind<InnerState,Perception>> mMind;
	};

	template<typename InnerState, typename Perception>
	class urgency_mind : public mind<InnerState,Perception>
	{
	public:
		void add_mind(std::unique_ptr<urgency_rated_mind<InnerState,Perception>> _mind) { mMinds.push_back(std::move(_mind)); }
		virtual std::unique_ptr<mind<InnerState,Perception>> think(InnerState &_state, const Perception &_perception) override {
			std::unique_ptr<urgency_rated_mind<InnerState,Perception>> most_urgent;
			float most_urgency;

			for(auto mind : mMinds)
			{
				float check_urgency = mind->urgency(_state, _perception);
				if(!most_urgent || check_urgency > most_urgency)
				{
					most_urgent = mind;
					most_urgency = check_urgency;
				}
			}

			if(most_urgent) most_urgent->think(_state, _perception);

			return std::unique_ptr<mind<InnerState,Perception>>(nullptr);
		}
	private:
		std::vector<std::unique_ptr<urgency_rated_mind<InnerState,Perception>>> mMinds;
	};
};

#endif