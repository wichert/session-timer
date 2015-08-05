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
#include "state.hh"


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


struct config_data {
	bool debug;
	string welcome_command;
	string countdown_command;
	string xsession_command;
};


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


void runLoginState(shared_ptr<Poller> poller, StateTracker &state, const config_data &config) {
	auto login = make_shared<RobustChild>(poller, initializer_list<string>{config.welcome_command});
	state.set(State::Idle);
	login->start();

	while (state==State::Idle) {
		poller->runOnce();
		switch (login->state) {
			case RobustChild::State::ready:
			case RobustChild::State::running:
				// Non-login related interruption
				break;
			case RobustChild::State::stopping: // Should not happen since we never call stop()
				BOOST_LOG_TRIVIAL(error) << "Login process stopping unexpectedly";
				state.set(State::Error);
				break;
			case RobustChild::State::finished:
				BOOST_LOG_TRIVIAL(info) << "Login process exited without error";
				state.set(State::LoggingIn, false);
				break;
			case RobustChild::State::failed:
				BOOST_LOG_TRIVIAL(warning) << "Login process exited with error";
				state.set(State::Error);
				break;
		}
	}
}


void runDesktopState(shared_ptr<Poller> poller, StateTracker &state, const config_data &config) {
	auto xsession = make_shared<RobustChild>(poller, initializer_list<string>{config.xsession_command});
	auto countdown = make_shared<RobustChild>(poller, initializer_list<string>{config.countdown_command});
	state.set(State::InUse);
	xsession->start();
	countdown->start();

	while (state==State::InUse) {
		poller->runOnce();
		switch (xsession->state) {
			case RobustChild::State::ready:
			case RobustChild::State::running:
				break;
			case RobustChild::State::stopping: // Should not happen since we never call stop()
				state.set(State::Error);
				break;
			case RobustChild::State::finished:
				BOOST_LOG_TRIVIAL(info) << "Xsession lougout";
				state.set(State::Resetting);
				break;
			case RobustChild::State::failed:
				BOOST_LOG_TRIVIAL(warning) << "Xsession aborted, forcing logout";
				state.set(State::Error);
				break;
		}

		switch (countdown->state) {
			case RobustChild::State::ready:
			case RobustChild::State::running:
				// Non-login related interruption
				break;
			case RobustChild::State::stopping: // Should not happen since we never call stop()
				BOOST_LOG_TRIVIAL(error) << "Countdown process stopping unexpectedly";
				state.set(State::Error);
				break;
			case RobustChild::State::finished:
				BOOST_LOG_TRIVIAL(info) << "Countdown reached zero, logging out";
				state.set(State::Resetting);
				break;
			case RobustChild::State::failed:
				BOOST_LOG_TRIVIAL(warning) << "Countdown aborted, forcing logout";
				state.set(State::Error);
				break;
		}
	}
}


void run(const config_data config) {
	auto poller = make_shared<Poller>();
	StateTracker state;

	do {
		switch (state.get()) {
			case State::Initializing:
			case State::Idle:
				runLoginState(poller, state, config);
				break;
			case State::LoggingIn:
			case State::InUse:
				runDesktopState(poller, state, config);
				break;
			case State::Resetting:
			case State::Error:
				break;
		}
	} while (state!=State::Error);
}


int main(int argc, char *argv[]) {
	po::options_description desc("Allowed options");
	config_data config {false};

	desc.add_options()
		("debug",
		 "Enable debugging mode.")
		("help", "Display this help and exit.")
		("welcome",
		 po::value<string>(&config.welcome_command)->default_value(LIBEXEC_PATH "/welcome"),
		 "Path to welcome command")
		("countdown",
		 po::value<string>(&config.countdown_command)->default_value(LIBEXEC_PATH "/countdown"),
		 "Path to countdown command")
		("xsession",
		 po::value<string>(&config.xsession_command)->default_value(XSESSION_PATH),
		 "Path for XSession command")
		;
	po::variables_map po_config;
	try {
		po::store(po::parse_command_line(argc, argv, desc), po_config);
	} catch (const po::error& e) {
		cerr << "Invalid commandline option: " << e.what() << endl
			<< endl
			<< desc << endl;
		return 1;
	}
	po::notify(po_config);
	if (po_config.count("help")) {
		cout << desc << endl;
		return 0;
	}
	config.debug=!!po_config.count("debug");

	setupLogging(config.debug);
	run(config);
	return 0;
}
