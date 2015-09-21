#include <gdk/gdk.h>
#include "welcomewindow.hh"

WelcomeWindow::WelcomeWindow() :
	alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0, 0),
	login_button()
{
	auto screen = Gdk::Screen::get_default();

	// For situationsn without a window manager
	move(0, 0);
	resize(screen->get_width(), screen->get_height());
	// Just in case we do have a window manager
	fullscreen();

	set_name("root");

	login_button.set_name("login-button");
	login_button.set_label("Login");
	login_button.signal_clicked().connect(
			sigc::mem_fun(*this, &WelcomeWindow::start_session));

	alignment.set_name("container");
	alignment.add(login_button);
	add(alignment);
	show_all_children();
}


WelcomeWindow::~WelcomeWindow() {
}


void WelcomeWindow::start_session() {
	hide();
}

