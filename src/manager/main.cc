#include <sys/wait.h>
#include <cerrno>
#include <csignal>
#include <iostream>
#include <system_error>
#include <boost/make_shared.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/program_options.hpp>
#include "null_deleter.hh"
#include "robustchild.hh"


using namespace std;
namespace po = boost::program_options;
namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;


/*
 * 1. Start the welcome command
 * 2. Keep track of state
 * 3. send regular messages to monitoring service
 */


enum class State {
	Idle,
	LoggedIn,
	Resetting,
	Error,
};


bool startWelcome() {
	pid_t pid = fork();
	if (pid==-1) {
		return false;
	}
}


void setupLogging(bool debug) {
	auto core = logging::core::get();

	if (debug) {
		auto backend = boost::make_shared<sinks::text_ostream_backend>();
		backend->add_stream(boost::shared_ptr<ostream>(&std::clog, boost::null_deleter()));
		backend->auto_flush(true);
		typedef sinks::synchronous_sink<sinks::text_ostream_backend> sink_t;
		core->add_sink(boost::make_shared<sink_t>(backend));

	} else {
		auto backend = boost::make_shared<sinks::syslog_backend>(
				keywords::facility=sinks::syslog::user);
		backend->set_severity_mapper(sinks::syslog::direct_severity_mapping<int>("Severity"));
		typedef sinks::synchronous_sink<sinks::syslog_backend> sink_t;
		core->add_sink(boost::make_shared<sink_t>(backend));
	}
}


void handleChildExit(int signal, siginfo_t* info, void* ucontext) {
	int status;
	int pid;

	if (info->si_signo!=SIGCHLD || info->si_code!=CLD_EXITED) {
		BOOST_LOG_TRIVIAL(warning) << "Invalid signal received, ignoring it";
		return;

	}
	if ((pid=waitpid(info->si_pid, &status, WNOHANG))==-1)
		throw system_error(errno, system_category());
	else if (pid==0) {
		// We got a signal, but no process was waiting to be reaped.
		// This should never happen, but we'll just assume a normal exit.
		BOOST_LOG_TRIVIAL(info) << "SIGCHLD signal received, but child process is not waiting";
		return;
	}

	if (info->si_status!=0) {
		BOOST_LOG_TRIVIAL(warning) << "Child process exited abnormally, restarting";
	}
}



int main(int argc, char *argv[]) {
	po::options_description desc("Allowed options");
	desc.add_options()
		("debug", "Enable debugging mode.")
		("help", "Display this help and exit.")
		;
	po::variables_map config;
	try {
		po::store(po::parse_command_line(argc, argv, desc), config);
	} catch (const po::error& e) {
		cerr << "Invalid commandline option: " << e.what() << endl
			<< endl
			<< desc << endl;
		return 1;
	}
	po::notify(config);
	if (config.count("help")) {
		cout << desc << endl;
		return 0;
	}

	shared_ptr<Poller> poller = make_shared<Poller>(); 
	RobustChild child(poller, {"invalid-command"});
	child.start();
	poller->run();

	return 0;
}
