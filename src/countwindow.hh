#ifndef COUNTDOWN_COUNTWINDOW_INCLUDED
#define COUNTDOWN_COUNTWINDOW_INCLUDED

#include <dbus-c++/dbus.h>
#include <chrono>
#include <gtkmm.h>
#include <gdkmm.h>
#include "idle.hh"


class CountWindow : public Gtk::Window {
public:
	CountWindow(const std::chrono::minutes _lifetime=std::chrono::minutes(30),
			const std::chrono::minutes _idle_timeout=std::chrono::minutes(5));
	virtual ~CountWindow();

protected:
	void logout();
	void show_deadline_warning();
	void show_idle_warning();
	bool update();
	void check_idle_time();
	void update_clock();
	virtual bool on_delete_event(GdkEventAny*);
	std::chrono::seconds time_remaining() const;

	std::chrono::minutes lifetime;
	std::chrono::minutes idle_timeout;
	std::chrono::steady_clock::time_point deadline;
	Gtk::Box content_vbox;
	Gtk::Label time_header;
	Gtk::Label time_label;
	bool deadline_warning_shown;
	sigc::connection timer_connection;
	DBus::Connection dbus_connection;
	XScreenSaver screensaver;
	Gtk::MessageDialog *deadline_warning_dialog;
	Gtk::MessageDialog *idle_warning_dialog;
};

#endif
