#ifndef MGR_ROBUSTCHILD_INCLUDED
#define MGR_ROBUSTCHILD_INCLUDED

#include <memory>
#include <initializer_list>
#include <string>
#include "signalfd.hh"
#include "poller.hh"

class RobustChild {
public:
	typedef std::vector<std::string> command_type;
	enum class State { ready, running, stopping, finished, failed };

	RobustChild() = delete;
	RobustChild(std::shared_ptr<Poller> poller, command_type&& command);
	virtual ~RobustChild();

	// No copying
	RobustChild(const RobustChild&) = delete;
	RobustChild& operator=(const RobustChild&) = delete;

	void start();
	void stop();

	State state;

private:
	void execChild();
	void onChildSignal(const signalfd_siginfo& info);

	std::shared_ptr<Poller> poller;
	command_type command;
	std::shared_ptr<SignalFD> signal_handler;
	pid_t pid;
	int error_count;
};

#endif
