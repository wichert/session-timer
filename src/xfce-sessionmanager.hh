#ifndef COUNTDOWN_XFCE_SESSIONMANAGER_INCLUDED
#define COUNTDOWN_XFCE_SESSIONMANAGER_INCLUDED

#include "xfce-sessionmanager-proxy.hh"


class XfceSessionManagerProxy : public org::xfce::SessionManager_proxy,
	public DBus::IntrospectableProxy,
	public DBus::ObjectProxy {
public:
	XfceSessionManagerProxy(DBus::Connection &connection,
				const char *path="/org/xfce/SessionManager",
				const char* name="org.xfce.SessionManager") :
		DBus::ObjectProxy(connection, path, name) {
	}
};

#endif
