#ifndef WELCOME_WELCOMEWINDOW_INCLUDED
#define WELCOME_WELCOMEWINDOW_INCLUDED

#include <gtkmm.h>

class WelcomeWindow : public Gtk::Window {
public:
	WelcomeWindow();
	virtual ~WelcomeWindow();

	void start_session();

private:
	Gtk::Alignment alignment;
	Gtk::Button login_button;
};

#endif
