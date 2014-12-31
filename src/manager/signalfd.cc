#include <cerrno>
#include <unistd.h>
#include <system_error>
#include "signalfd.hh"


using namespace std;


SignalFD::SignalFD(int signal) :
		signal(),
		signal_fd(-1) {
	sigemptyset(&mask);
	if (sigprocmask(SIG_BLOCK, nullptr, &original_mask)==-1)
		throw system_error(errno, system_category());
	add_signal(signal);
}


SignalFD::~SignalFD() {
	sigprocmask(SIG_SETMASK, &original_mask, nullptr);
	close(signal_fd);
}


void SignalFD::add_signal(int signal) {
	sigaddset(&mask, signal);
	if (sigprocmask(SIG_BLOCK, &mask, nullptr)==-1)
		throw system_error(errno, system_category());
	if ((signal_fd=signalfd(signal_fd, &mask, SFD_NONBLOCK|SFD_CLOEXEC))==-1)
		throw system_error(errno, system_category());
}
