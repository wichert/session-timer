#ifndef COUNTDOWN_COUNTWINDOW_INCLUDED
#define COUNTDOWN_COUNTWINDOW_INCLUDED

#include <dbus-c++/dbus.h>
#include <chrono>
#include <gtkmm.h>
#include <gdkmm.h>

class CountWindow : public Gtk::Window {
public:
	CountWindow();
	virtual ~CountWindow();

protected:
	void logout();
	bool update_clock();
	virtual bool on_delete_event(GdkEventAny*);
	std::chrono::seconds time_remaining() const;

	std::chrono::steady_clock::time_point deadline;
	Gtk::Label time_label;
	sigc::connection timer_connection;
	DBus::Connection dbus_connection;
};

#endif
