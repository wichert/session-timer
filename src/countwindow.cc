#include <iostream>
#include <pangomm.h>
#include "countwindow.hh"
#include "gnome-sessionmanager.hh"
#include "xfce-sessionmanager.hh"

using namespace std;

const chrono::minutes lifetime(1);

CountWindow::CountWindow() :
		deadline(chrono::steady_clock::now()+lifetime),
		time_label(),
		timer_connection(),
		dbus_connection(DBus::Connection::SessionBus()) {
	// Use a splash screen as type. This implies we are hidden from
	// the taskbar and are always shown on top.
	set_type_hint(Gdk::WINDOW_TYPE_HINT_SPLASHSCREEN);
	stick();

	Pango::AttrList label_attributes;
	auto fontsize = Pango::Attribute::create_attr_size(Pango::SCALE*50);
	label_attributes.change(fontsize);

	time_label.set_use_markup(false);
	time_label.set_attributes(label_attributes);
	time_label.show();
	update_clock();
	add(time_label);

	sigc::slot<bool> slot = sigc::mem_fun(*this, &CountWindow::update_clock);
	sigc::connection conn = Glib::signal_timeout().connect_seconds(slot, 1);
}


CountWindow::~CountWindow() {
}


chrono::seconds CountWindow::time_remaining() const {
	auto remaining = deadline-chrono::steady_clock::now();
	return chrono::duration_cast<chrono::seconds>(remaining);
}


bool CountWindow::update_clock() {
	if (chrono::steady_clock::now()>=deadline) {
		logout();
		return false;
	}

	chrono::seconds remaining = time_remaining();
	Pango::AttrList label_attributes;
	auto lifetime_seconds = chrono::duration_cast<chrono::seconds>(lifetime).count();
	auto red = 65535 * (static_cast<float>(lifetime_seconds - remaining.count()) / lifetime_seconds);
	auto colour = Pango::Attribute::create_attr_foreground(red, 0, 0);
	label_attributes.change(colour);
	time_label.set_attributes(label_attributes);

	auto fontsize = Pango::Attribute::create_attr_size(Pango::SCALE*50);
	label_attributes.change(fontsize);

	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%02ld:%02ld", remaining.count()/60, remaining.count()%60);
	time_label.set_label(buffer);
	return true;
}

bool CountWindow::on_delete_event(GdkEventAny* event) {
	return true;
}


void CountWindow::logout() {
	XfceSessionManagerProxy xfce(dbus_connection);
	if (xfce.active())
		xfce.Logout(false, false);

	GnomeSessionManagerProxy gnome(dbus_connection);
	if (gnome.active())
		gnome.Logout(GnomeSessionManagerProxy::LogoutMode::NoConfirmation);

	// No known session manager found.. damn!
}
