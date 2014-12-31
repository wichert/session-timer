#ifndef MGR_ROBUSTCHILD_INCLUDED
#define MGR_ROBUSTCHILD_INCLUDED

#include <initializer_list>
#include "signalfd.hh"

class RobustChild {
public:
	RobustChild() = delete;
	RobustChild(std::initializer_list<const char*> command);
	virtual ~RobustChild();
	void start();
	void stop();

private:
	void execChild();


	std::vector<const char*> command;
	SignalFD signal_handler;
	enum class State { ready, running, stopping, finished, failed };
	State status;
	pid_t pid;
};

#endif
