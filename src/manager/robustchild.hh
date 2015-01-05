#ifndef MGR_ROBUSTCHILD_INCLUDED
#define MGR_ROBUSTCHILD_INCLUDED

#include <memory>
#include <initializer_list>
#include "signalfd.hh"
#include "poller.hh"

class RobustChild {
public:
	typedef std::vector<const char*> command_type;

	RobustChild() = delete;
	RobustChild(std::shared_ptr<Poller> poller, command_type&& command);
	virtual ~RobustChild();

	// No copying
	RobustChild(const RobustChild&) = delete;
	RobustChild& operator=(const RobustChild&) = delete;

	void start();
	void stop();

private:
	void execChild();
	void onChildSignal(const signalfd_siginfo& info);

	std::shared_ptr<Poller> poller;
	command_type command;
	std::shared_ptr<SignalFD> signal_handler;
	enum class State { ready, running, stopping, finished, failed };
	State status;
	pid_t pid;
};

#endif
