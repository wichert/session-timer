#ifndef MGR_SIGNALFD_INCLUDED
#define MGR_SIGNALFD_INCLUDED

#include <csignal>
#include <sys/signalfd.h>
#include <boost/signals2.hpp>


class SignalFD {
public:
	SignalFD() = delete;
	SignalFD(int signal);
	~SignalFD();

	void add_signal(int signal);

	int fd() {
		return signal_fd;
	}

	void update();

	boost::signals2::signal<void(signalfd_siginfo)> signal;

private:
	sigset_t original_mask;
	sigset_t mask;
	int signal_fd;
};


#endif

