#include <iostream>
#include <boost/program_options.hpp>
#include <dbus-c++/dbus.h>
#include <dbus-c++/glib-integration.h>
#include <gtkmm/application.h>
#include "countwindow.hh"

using namespace std;
namespace po = boost::program_options;

namespace {
	DBus::Glib::BusDispatcher dbus_dispatcher;
}

int main(int argc, char *argv[]) {
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Display this help and exit.")
		("period", po::value<int>()->value_name("MINUTES")->default_value(60), "Maximum session period in minutes")
		("idle-timeout", po::value<int>()->value_name("MINUTES")->default_value(5), "Maximum idle timeout in minutes")
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

	auto app = Gtk::Application::create("nl.attingo.countdown");

	if (!Glib::thread_supported())
		Glib::thread_init();

	DBus::_init_threading();
	DBus::default_dispatcher=&dbus_dispatcher;

	CountWindow window(chrono::minutes(config["period"].as<int>()), chrono::minutes(config["idle-timeout"].as<int>()));

	auto css = Gtk::CssProvider::create();
	try {
		css->load_from_path(DATADIR "countdown.css");
	} catch (const Glib::Error &e) {
		cerr << "Error loading CSS: " << e.what() << endl;
		return 1;
	}
	auto screen = Gdk::Screen::get_default();
	auto ctx = Gtk::StyleContext::create();
	ctx->add_provider_for_screen(screen, css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	return app->run(window, 1, argv);
}

