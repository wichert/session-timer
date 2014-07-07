#include <sys/types.h>
#include <csignal>
#include <cerrno>
#include <iostream>
#include <system_error>
#include <pangomm.h>
#include "countwindow.hh"
#include "gnome-sessionmanager.hh"
#include "xfce-sessionmanager.hh"

using namespace std;

const chrono::minutes warn_at(2);

CountWindow::CountWindow(int _lifetime) :
		lifetime(_lifetime),
		deadline(chrono::steady_clock::now()+lifetime),
		content_vbox(Gtk::ORIENTATION_VERTICAL, 5),
		time_header(),
		time_label(),
		warning_shown(false),
		timer_connection(),
		dbus_connection(DBus::Connection::SessionBus()) {
	// Use a splash screen as type. This implies we are hidden from
	// the taskbar and are always shown on top.
	set_type_hint(Gdk::WINDOW_TYPE_HINT_SPLASHSCREEN);
	stick();

	time_header.set_justify(Gtk::JUSTIFY_CENTER);
	time_header.set_alignment(Gtk::ALIGN_FILL);
	time_header.set_padding(5, 0);
	time_header.set_text("Time remaining");
	content_vbox.pack_start(time_header, Gtk::PACK_SHRINK);

	Pango::AttrList label_attributes;
	auto fontsize = Pango::Attribute::create_attr_size(Pango::SCALE*50);
	label_attributes.change(fontsize);

	time_label.set_use_markup(false);
	time_label.set_attributes(label_attributes);
	update_clock();
	content_vbox.pack_start(time_label, Gtk::PACK_SHRINK);
	add(content_vbox);
	show_all_children();

	sigc::slot<bool> slot = sigc::mem_fun(*this, &CountWindow::update_clock);
	sigc::connection conn = Glib::signal_timeout().connect_seconds(slot, 1);
}


CountWindow::~CountWindow() {
}


void CountWindow::show_warning() {
	Gtk::MessageDialog dialog(*this,
			"Your session will end in two minutes.",
			false,
			Gtk::MESSAGE_WARNING,
			Gtk::BUTTONS_OK,
			true);
	dialog.set_secondary_text(
			"You will automatically be logged out soon. Plesae make "
			"sure all finish your work before this happens. When "
			"you are logged out <b>all your data will be deleted</b>.",
			true);
	dialog.run();
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
	time_label.set_text(buffer);

	if (!warning_shown && remaining<warn_at) {
		warning_shown=true;
		show_warning();
	}

	return true;
}


bool CountWindow::on_delete_event(GdkEventAny* event) {
	return true;
}


void CountWindow::logout() {
	try {
		XfceSessionManagerProxy xfce(dbus_connection);
		if (xfce.active()) {
			xfce.Logout(false, false);
			return;
		}
	} catch (const DBus::Error&) {
	}

	try {
		GnomeSessionManagerProxy gnome(dbus_connection);
		if (gnome.active()) {
			gnome.Logout(GnomeSessionManagerProxy::LogoutMode::NoConfirmation);
			return;
		}
	} catch (const DBus::Error&) {
	}

	// No (supported) session manager found. So lets fall back to a
	// somewhat brutal but effective approach: kill all our processes.
	if (kill(-1, SIGTERM)==-1) {
		cerr << "FATAL: failed to send TERM signal: " << strerror(errno);
		throw system_error(error_code(errno, generic_category()));
	}
}
