#include <cerrno>
#include <unistd.h>
#include <system_error>
#include "signalfd.hh"


using namespace std;


SignalFD::SignalFD(int signal, int flags) :
		signal(),
		signal_fd(-1) {
	sigemptyset(&mask);
	if (sigprocmask(SIG_BLOCK, nullptr, &original_mask)==-1)
		throw system_error(errno, system_category());
	add_signal(signal, flags);
}


SignalFD::~SignalFD() {
	sigprocmask(SIG_SETMASK, &original_mask, nullptr);
	close(signal_fd);
}


void SignalFD::add_signal(int signal, int flags) {
	if (flags) {
		struct sigaction act;
		sigaction(signal, nullptr, &act);
		act.sa_flags=flags;
		sigaction(signal, &act, nullptr);
	}

	sigaddset(&mask, signal);
	if (sigprocmask(SIG_BLOCK, &mask, nullptr)==-1)
		throw system_error(errno, system_category());
	if ((signal_fd=signalfd(signal_fd, &mask, SFD_NONBLOCK|SFD_CLOEXEC))==-1)
		throw system_error(errno, system_category());
}


void SignalFD::update() {
	signalfd_siginfo info;
	auto s = read(signal_fd, &info, sizeof(info));
	if (s!=sizeof(info))
		throw system_error(errno, system_category());
	signal(info);
}


boost::signals2::connection SignalFD::connect(const signal_type::slot_type &sub) {
	return signal.connect(sub);
}

