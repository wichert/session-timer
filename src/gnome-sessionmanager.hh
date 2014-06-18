#ifndef COUNTDOWN_GNOME_SESSIONMANAGER_INCLUDED
#define COUNTDOWN_GNOME_SESSIONMANAGER_INCLUDED

#include "gnome-sessionmanager-proxy.hh"


class GnomeSessionManagerProxy : public org::gnome::SessionManager_proxy,
	public DBus::IntrospectableProxy,
	public DBus::ObjectProxy {
public:
	enum class LogoutMode : int {
		Normal=0,
		NoConfirmation,
		Force,
	};

	GnomeSessionManagerProxy(DBus::Connection &connection,
				const char *path="/org/gnome/SessionManager",
				const char* name="org.gnome.SessionManager") :
		DBus::ObjectProxy(connection, path, name) {
	}

	virtual void SessionOver() {
	}

	virtual void SessionRunning() {
	}

	virtual void InhibitorRemoved(const DBus::Path&) {
	}

	virtual void InhibitorAdded(const DBus::Path&) {
	}

	virtual void ClientRemoved(const DBus::Path&) {
	}

	virtual void ClientAdded(const DBus::Path&) {
	}

	void Logout(LogoutMode mode) {
		const uint32_t m = static_cast<uint32_t>(mode);
		org::gnome::SessionManager_proxy::Logout(m);
	}
};

#endif
