#ifndef MGR_ROBUSTCHILD_INCLUDED
#define MGR_ROBUSTCHILD_INCLUDED

#include <initializer_list>
#include "signalfd.hh"

class RobustChild {
public:
	RobustChild() = delete;
	RobustChild(std::initializer_list<const char*> command);
	virtual ~RobustChild();

	// No copying
	RobustChild(const RobustChild&) = delete;
	RobustChild& operator=(const RobustChild&) = delete;

	void start();
	void stop();

private:
	void execChild();
	void onChildSignal(const signalfd_siginfo& info);

	std::vector<const char*> command;
	SignalFD signal_handler;
	enum class State { ready, running, stopping, finished, failed };
	State status;
	pid_t pid;
};

#endif
