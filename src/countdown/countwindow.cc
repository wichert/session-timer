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

enum {
	STRUT_LEFT = 0,
	STRUT_RIGHT,
	STRUT_TOP,
	STRUT_BOTTOM,
	STRUT_LEFT_START_Y,
	STRUT_LEFT_END_Y,
	STRUT_RIGHT_START_Y,
	STRUT_RIGHT_END_Y,
	STRUT_TOP_START_X,
	STRUT_TOP_END_X,
	STRUT_BOTTOM_START_X,
	STRUT_BOTTOM_END_X,
	N_STRUTS
};


CountWindow::CountWindow(const chrono::minutes _lifetime, const chrono::minutes _idle_timeout ) :
		lifetime(_lifetime),
		idle_timeout(_idle_timeout),
		deadline(chrono::steady_clock::now()+lifetime),
		content_grid(),
		time_label(),
		logout_button(),
		deadline_warning_shown(false),
		timer_connection(),
		dbus_connection(DBus::Connection::SessionBus()),
		screensaver(),
		deadline_warning_dialog(nullptr),
		idle_warning_dialog(nullptr) {
	set_type_hint(Gdk::WINDOW_TYPE_HINT_DOCK);
	set_skip_taskbar_hint();
	set_skip_pager_hint();
	set_accept_focus(false);
	set_resizable(false);
	set_keep_above();
	set_decorated(false);
	stick();

	Gdk::Geometry geometry;
	geometry.min_height=geometry.max_height=30;
	geometry.min_width=geometry.max_width=get_screen()->get_width();
	set_geometry_hints(*this, geometry, Gdk::HINT_MIN_SIZE | Gdk::HINT_MAX_SIZE);

	time_label.set_justify(Gtk::JUSTIFY_CENTER);
	time_label.set_padding(5, 0);
	content_grid.attach(time_label, 0, 0, 1, 1);
	update_clock();

	logout_button.set_label("Logout");
	logout_button.set_image_from_icon_name(GTK_STOCK_QUIT);
	logout_button.set_always_show_image();
	content_grid.attach_next_to(logout_button, time_label, Gtk::POS_RIGHT, 1, 1);

	add(content_grid);
	show_all_children();

	// Reserve the top part of the screen so the window manager will never
	// put anything on top of it.
	signal_realize().connect([this] () {
		auto wm_strut_partial_atom = gdk_atom_intern_static_string("_NET_WM_STRUT_PARTIAL");
		auto cardinal_atom = gdk_atom_intern_static_string("CARDINAL");
		gulong struts[N_STRUTS];
		struts[STRUT_LEFT]=struts[STRUT_RIGHT]=struts[STRUT_BOTTOM]=0;
		struts[STRUT_TOP]=30;
		struts[STRUT_TOP_START_X]=0;
		struts[STRUT_TOP_END_X]=get_screen()->get_width();
		gdk_property_change(get_window()->gobj(),
				wm_strut_partial_atom, cardinal_atom, 32, GDK_PROP_MODE_REPLACE,
				reinterpret_cast<const guchar*>(&struts), N_STRUTS);
	});

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

	char buffer[64];
	snprintf(buffer, sizeof(buffer), "Time remaining: <b>%02ld:%02ld</b>",
			remaining.count()/60, remaining.count()%60);
	time_label.set_markup(buffer);

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
