#include <cerrno>
#include <stdexcept>
#include <system_error>
#include <unistd.h>
#include <boost/log/trivial.hpp>
#include "robustchild.hh"

using namespace std;

RobustChild::RobustChild(shared_ptr<Poller> poller, command_type&& command) :
		poller(poller),
		command(command),
		signal_handler(make_shared<SignalFD>(SIGCHLD)),
		status(State::ready),
		pid(-1),
		error_count(0) {
	signal_handler->connect([&](const signalfd_siginfo& info) {
			return onChildSignal(info);
			});
	poller->insert(signal_handler);
}


RobustChild::~RobustChild() {
	if (status==State::running)
		stop();
	poller->erase(signal_handler);
}


void RobustChild::start() {
	if (status==State::running || status==State::stopping)
		throw logic_error("Can  not start a running process.");
	status=State::running;
	if ((pid=fork())==-1) {
		status=State::failed;
		throw system_error(errno, generic_category());
	} else if (pid==0) {
		try {
			execChild();
		} catch(const exception&) { 
			status=State::failed;
			throw;
		}
	}
}


void RobustChild::stop() {
	if (status==State::stopping)
		return;
	if (status!=State::running)
		throw logic_error("Can  not stop a process which is not running.");
	status=State::stopping;
	if (kill(pid, SIGTERM)==-1)
		throw system_error(errno, generic_category());
}


void RobustChild::execChild() {
	const char** argv = new const char*[command.size()+2];
	for (int i=0; i<command.size(); i++)
		argv[i]=command[i];
	argv[command.size()]=nullptr;
	execv(argv[0], const_cast<char* const*>(argv));
	delete []argv;
	throw system_error(errno, generic_category());
}


void RobustChild::onChildSignal(const signalfd_siginfo& info) {
	if (info.ssi_signo!=SIGCHLD) {
		BOOST_LOG_TRIVIAL(error) << "Invalid signal received: " << info.ssi_signo;
		return;
	}

	bool can_retry = false;

	switch  (info.ssi_code) {
		case CLD_EXITED:
			if (info.ssi_status!=0) {
				BOOST_LOG_TRIVIAL(warning) << "Child exited with exit code " << info.ssi_status;
				status=State::failed;
				error_count++;
			} else {
				BOOST_LOG_TRIVIAL(info) << "Child exited normally";
				status=State::finished;
			}
			break;
		case CLD_KILLED:
		case CLD_DUMPED:
			BOOST_LOG_TRIVIAL(warning) << "Child killed with signal " << info.ssi_status;
			status=State::failed;
			error_count++;
			can_retry=true;
			break;
	}

	if (can_retry && status==State::failed && error_count<5) {
		BOOST_LOG_TRIVIAL(info) << "Restarting child process";
		start();
	}
}

