#include "mouse_listener.h"

#include <algorithm>
#include <iostream>

#include "common/singleton.h"
#include "gl/window_info.h"

uint64_t mouse_listener::btn_event_counters[] = {0,0,0,0,0};

bool mouse_listener::on_event(std::pair<SDL_MouseButtonEvent* const, uint64_t>){
	return false;
}

bool mouse_listener::on_event(SDL_MouseMotionEvent* const){
	return false;
}

bool mouse_listener::on_event(SDL_MouseWheelEvent* const){
	return false;
}


mouse_listener::mouse_listener(uint16_t layer): layer(layer){
	listeners.insert(
		std::find_if(
			listeners.begin(),
			listeners.end(),
			[layer](mouse_listener* a){
				return a->layer > layer;
			}),
		this);
}

mouse_listener::~mouse_listener(){
	std::vector<mouse_listener*>::iterator listener = std::find_if(
		listeners.begin(),
		listeners.end(),
		[this](mouse_listener* a){
			return a->id == id;
		});
	// in theory this erase could crash if listener == listeners.end() but it should never happen.
	listeners.erase(listener);
}

void mouse_listener::publish_event(SDL_MouseButtonEvent* const event){
	event->y = singleton<window_info>::value.h - event->y;
	for(auto& listener : listeners){
		if(listener->on_event({event, btn_event_counters[event->button]})){
			break;
		}
	}
	btn_event_counters[event->button]++;
}

void mouse_listener::publish_event(SDL_MouseMotionEvent* const event){
	for(auto& listener : listeners){
		if(listener->on_event(event)){
			break;
		}
	}
}

void mouse_listener::publish_event(SDL_MouseWheelEvent* const event){
	for(auto& listener : listeners){
		if(listener->on_event(event)){
			break;
		}
	}
}
