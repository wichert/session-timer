#include <stdexcept>
#include "idle.hh"
#include <gdk/gdkx.h>

using namespace std;


XScreenSaver::XScreenSaver() : display(0), info(0) {
	display=gdk_x11_display_get_xdisplay(gdk_display_get_default());
	root_window=DefaultRootWindow(display);

	int event_base, err_base;
	if (!XScreenSaverQueryExtension(display, &event_base, &err_base))
		throw runtime_error("No screensaver support present.");
	if (!(info=XScreenSaverAllocInfo()))
		throw runtime_error("No X11 memory available");
}


XScreenSaver::~XScreenSaver() {
	if (info)
		XFree(info);
}


chrono::milliseconds XScreenSaver::idle_time() const {
	XScreenSaverQueryInfo(display, root_window, info);
	return chrono::milliseconds(info->idle);
}
