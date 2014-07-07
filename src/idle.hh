#ifndef COUNTDOWN_IDLE_INCLUDED
#define COUNTDOWN_IDLE_INCLUDED

#include <chrono>
#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>

class XScreenSaver {
public:
	XScreenSaver();
	~XScreenSaver();

	XScreenSaver(const XScreenSaver&) = delete;
	XScreenSaver& operator=(const XScreenSaver&) = delete;

	std::chrono::milliseconds idle_time() const;

private:
	Display* display;
	Window root_window;
	XScreenSaverInfo* info;
};

#endif

