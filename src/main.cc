#include <dbus-c++/dbus.h>
#include <dbus-c++/glib-integration.h>
#include <gtkmm/application.h>
#include "countwindow.hh"


namespace {
	DBus::Glib::BusDispatcher dbus_dispatcher;
}

int main(int argc, char *argv[]) {
	auto app = Gtk::Application::create(argc, argv, "nl.attingo.countdown");

	if (!Glib::thread_supported())
		Glib::thread_init();

	DBus::_init_threading();
	DBus::default_dispatcher=&dbus_dispatcher;

	CountWindow window;
	return app->run(window);
}

