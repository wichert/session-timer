#include <iostream>
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


CountWindow::CountWindow(const chrono::minutes _lifetime, const chrono::minutes _idle_timeout ) :
		lifetime(_lifetime),
		idle_timeout(_idle_timeout),
		deadline(chrono::steady_clock::now()+lifetime),
		content_vbox(Gtk::ORIENTATION_VERTICAL, 5),
		time_header(),
		time_label(),
		deadline_warning_shown(false),
		timer_connection(),
		dbus_connection(DBus::Connection::SessionBus()),
		screensaver(),
		deadline_warning_dialog(nullptr),
		idle_warning_dialog(nullptr) {
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

	sigc::slot<bool> slot = sigc::mem_fun(*this, &CountWindow::update);
	sigc::connection conn = Glib::signal_timeout().connect_seconds(slot, 1);
}


CountWindow::~CountWindow() {
}


void CountWindow::show_deadline_warning() {
	deadline_warning_dialog = new Gtk::MessageDialog(*this,
			"Your session will end in two minutes.",
			false,
			Gtk::MESSAGE_WARNING,
			Gtk::BUTTONS_OK,
			true);
	deadline_warning_dialog->set_secondary_text(
			"You will automatically be logged out soon. Plesae make "
			"sure all finish your work before this happens. When "
			"you are logged out <b>all your data will be deleted</b>.",
			true);

	deadline_warning_dialog->signal_response().connect([&] (int response_id) {
		if (response_id==GTK_RESPONSE_OK)
			deadline_warning_dialog->close();
	});

	deadline_warning_dialog->set_application(get_application());
	deadline_warning_dialog->show();
}


void CountWindow::show_idle_warning() {
	if (idle_warning_dialog)
		return;

	idle_warning_dialog=new Gtk::MessageDialog(*this,
			"Your have been idle for 4 minutes.",
			false,
			Gtk::MESSAGE_WARNING,
			Gtk::BUTTONS_OK,
			true);
	idle_warning_dialog->set_secondary_text(
			"You do not seem to be using this computer anymore. If you "
			"remain idle <b>you will be logged automatcially in one "
			"minute</b> if you remain idle. Please note that when "
			"this happens <b>all your data will be deleted</b>.",
			true);

	idle_warning_dialog->signal_response().connect([&] (int response_id) {
		if (response_id==GTK_RESPONSE_OK) {
			idle_warning_dialog->close();
			idle_warning_dialog=nullptr;
		}
	});

	idle_warning_dialog->set_application(get_application());
	idle_warning_dialog->show();
}


bool CountWindow::update() {
	update_clock();
	check_idle_time();
	return true;;
}


void CountWindow::check_idle_time() {
	chrono::milliseconds idle_time(screensaver.idle_time());

	if (idle_time>idle_timeout) {
		logout();
		return;
	}

	auto warn_at = (chrono::duration_cast<chrono::milliseconds>(idle_timeout)*4)/5;
	if (idle_time>warn_at)
		show_idle_warning();
}


chrono::seconds CountWindow::time_remaining() const {
	auto remaining = deadline-chrono::steady_clock::now();
	return chrono::duration_cast<chrono::seconds>(remaining);
}


void CountWindow::update_clock() {
	if (chrono::steady_clock::now()>=deadline) {
		logout();
		return;
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

	if (!deadline_warning_shown && remaining<warn_at) {
		deadline_warning_shown=true;
		show_deadline_warning();
	}
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
