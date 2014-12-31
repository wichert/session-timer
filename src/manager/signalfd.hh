#ifndef MGR_SIGNALFD_INCLUDED
#define MGR_SIGNALFD_INCLUDED

#include <csignal>
#include <sys/signalfd.h>
#include <boost/signals2.hpp>


class SignalFD {
public:
	typedef boost::signals2::signal<void(const signalfd_siginfo&)> signal_type;

	SignalFD() = delete;
	SignalFD(int signal);
	~SignalFD();

	// No copying
	SignalFD(const SignalFD&) = delete;
	SignalFD& operator=(const SignalFD&) = delete;

	void add_signal(int signal);

	int fd() {
		return signal_fd;
	}

	void update();
	boost::signals2::connection connect(const signal_type::slot_type &sub);

private:
	boost::signals2::signal<void(const signalfd_siginfo&)> signal;
	sigset_t original_mask;
	sigset_t mask;
	int signal_fd;
};


#endif

