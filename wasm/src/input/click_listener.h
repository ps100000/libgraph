#ifndef _CLICK_LISTENER_H_
#define _CLICK_LISTENER_H_

#include <functional>

#ifdef __EMSCRIPTEN__
    #include <glm.hpp>
#else
    #include <glm/glm.hpp>
#endif

#include "mouse_listener.h"

class click_listener: public mouse_listener {
	private:
		bool on_event(std::pair<SDL_MouseButtonEvent* const, uint64_t> event) override;
		uint64_t last_down_event_id;
	public:
		std::function<bool(std::pair<SDL_MouseButtonEvent* const, uint64_t>)> is_consumed;
		std::function<void(SDL_MouseButtonEvent* const)> on_click;

		click_listener(uint16_t layer = 0);
		click_listener(glm::vec2 pos, float width, float height, uint16_t layer);
		click_listener(glm::vec2 pos, float radius, uint16_t layer);
};

#endif
