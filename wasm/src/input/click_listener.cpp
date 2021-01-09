#include "click_listener.h"

#include <iostream>

uint64_t last_down_event_id;
std::function<bool(std::pair<SDL_MouseButtonEvent* const, uint64_t>)> is_consumed;
std::function<void(SDL_MouseButtonEvent* const)> on_click;


bool click_listener::on_event(std::pair<SDL_MouseButtonEvent* const, uint64_t> event){
	if(is_consumed(event)){
		if(event.first->state == SDL_PRESSED){
			last_down_event_id = event.second;
		}else if(
			event.first->state == SDL_RELEASED
			&& last_down_event_id == event.second - 1){
			on_click(event.first);
		}
		return true;
	}
	return false;
}

click_listener::click_listener(uint16_t layer): mouse_listener(layer){}

click_listener::click_listener(
	glm::vec2 pos,
	float width,
	float height,
	uint16_t layer): mouse_listener(layer){
	is_consumed = [pos, width, height](std::pair<SDL_MouseButtonEvent* const, uint64_t> event){
		return event.first->x >= pos.x
			&& event.first->x <= pos.x + width
			&& event.first->y >= pos.y
			&& event.first->y <= pos.y + height;
	};
}

click_listener::click_listener(
	glm::vec2 pos,
	float radius,
	uint16_t layer): mouse_listener(layer){
	is_consumed = [pos, radius](std::pair<SDL_MouseButtonEvent* const, uint64_t> event){
		return pow(event.first->x - pos.x, 2)
		+ pow(event.first->y - pos.y, 2) < radius * radius;
	};
}
