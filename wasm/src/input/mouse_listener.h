#ifndef _MOUSE_LISTENER_H_
#define _MOUSE_LISTENER_H_
#include <vector>

#include <SDL.h>
#include <SDL_events.h>

#include "common/util.h"

#define MAX_BTN_INDEX (MAX(MAX(MAX(MAX( \
	((uint8_t)SDL_BUTTON_LEFT),			\
	((uint8_t)SDL_BUTTON_MIDDLE)),		\
	((uint8_t)SDL_BUTTON_RIGHT)),		\
	((uint8_t)SDL_BUTTON_X1)),			\
	((uint8_t)SDL_BUTTON_X2)) - 1)

class mouse_listener {
	private:
		static inline uint64_t id_counter = 0;
		const uint64_t id = id_counter++;
		static inline std::vector<mouse_listener*> listeners{};
		static uint64_t btn_event_counters[MAX_BTN_INDEX + 1];
	protected:
		virtual bool on_event(std::pair<SDL_MouseButtonEvent* const, uint64_t>);
		virtual bool on_event(SDL_MouseMotionEvent* const event);
		virtual bool on_event(SDL_MouseWheelEvent* const event);
	public:
		const uint16_t layer;
		mouse_listener(uint16_t layer);
		virtual ~mouse_listener();

		static void publish_event(SDL_MouseButtonEvent* const event);
		static void publish_event(SDL_MouseMotionEvent* const event);
		static void publish_event(SDL_MouseWheelEvent* const event);
};

#endif