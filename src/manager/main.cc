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

	setupLogging(config.count("debug"));

	shared_ptr<Poller> poller = make_shared<Poller>(); 
	RobustChild child(poller, {"invalid-command"});
	child.start();
	poller->runOnce();

	return 0;
}
