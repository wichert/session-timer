#include <iostream>
#include <boost/program_options.hpp>
#include <dbus-c++/dbus.h>
#include <dbus-c++/glib-integration.h>
#include <gtkmm/application.h>
#include <gtkmm.h>
#include "welcomewindow.hh"

using namespace std;
namespace po = boost::program_options;

namespace {
	DBus::Glib::BusDispatcher dbus_dispatcher;
}

int main(int argc, char *argv[]) {
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Display this help and exit.")
		;
	po::variables_map config;
	try {
		po::store(po::parse_command_line(argc, argv, desc), config);
	} catch (const po::error& e) {
		cerr << "Invalid commandline option: " << e.what() << endl
			<< endl
			<< desc << endl;
		return 1;
	}
	po::notify(config);

	if (config.count("help")) {
		cout << desc << endl;
		return 0;
	}

	auto app = Gtk::Application::create("nl.attingo.welcome");

	if (!Glib::thread_supported())
		Glib::thread_init();

	DBus::_init_threading();
	DBus::default_dispatcher=&dbus_dispatcher;

	WelcomeWindow window;

	auto css = Gtk::CssProvider::create();
	try {
		css->load_from_path("welcome.css");
	} catch (const Glib::Error &e) {
		cerr << "Error loading CSS: " << e.what() << endl;
		return 1;
	}
	auto screen = Gdk::Screen::get_default();
	auto ctx = Gtk::StyleContext::create();
	ctx->add_provider_for_screen(screen, css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	if (app->run(window, 1, argv)!=0)
		return 1;
	return 0;
}

