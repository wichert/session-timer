#ifndef COUNTDOWN_XFCE_SESSIONMANAGER_INCLUDED
#define COUNTDOWN_XFCE_SESSIONMANAGER_INCLUDED

#include "xfce-sessionmanager-proxy.hh"


class XfceSessionManagerProxy : public org::xfce::Session::Manager_proxy,
	public DBus::IntrospectableProxy,
	public DBus::ObjectProxy {
public:
	enum class State : int {
		Startup=0,
		Idle,
		Checkpoint,
		Shutdown,
		ShutdownPhase2
	};

	XfceSessionManagerProxy(DBus::Connection &connection,
				const char *path="/org/xfce/SessionManager",
				const char* name="org.xfce.SessionManager") :
		DBus::ObjectProxy(connection, path, name) {
	}

	virtual void ShutdownCancelled() {
	}

	virtual void ClientRegistered(const std::string&) {
	}

	virtual void StateChanged(const uint32_t&, const uint32_t&) {
	}

	void Logout(bool show_dialog, bool allow_save) {
		org::xfce::Session::Manager_proxy::Logout(show_dialog, allow_save);
	}

	bool active() {
		try {
			State state = static_cast<State>(GetState());
			return state!=State::Shutdown && state!=State::ShutdownPhase2;
		} catch (const DBus::Error &) {
			return false;
		}
	}
};

#endif
