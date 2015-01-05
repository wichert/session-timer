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
		pid(-1) {
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
	BOOST_LOG_TRIVIAL(info) << "RobustChild signal handler called";
}

