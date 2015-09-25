#include <unistd.h>
#include <sstream>
#include <boost/format.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include "state.hh"

using namespace std;


string hostname() {
	char buffer[HOST_NAME_MAX];

	if (gethostname(buffer, HOST_NAME_MAX)==0)
		return buffer;
	return "unknown";
}

constexpr auto body_format = "<?xml version=\"1.0\"?>\n<status>%s</status>";

void StateTracker::set(State st, bool track) {
	if (state==st)
		return;

	state = st;

	istringstream body((boost::format(body_format) % to_string(st)).str());
	auto url = (boost::format("http://statusmonitor.aas.attingo.nl/desktops/%s") % hostname()).str();

	if (!track)
		return;

	try {
		curlpp::Cleanup cleanup;
		curlpp::Easy request;
		stringstream response;

		using namespace curlpp::Options;
		request.setOpt<Url>(url);
//		request.setOpt(new Verbose(true));
		request.setOpt(new Upload(true));
		request.setOpt(new ReadStream(&body));
		request.setOpt(new InfileSize(body.str().size()));
		request.setOpt(WriteStream(&response));

		request.perform();

		auto code = curlpp::infos::ResponseCode::get(request);
		if (code!=200)
			cerr << "Status push failed with code " << code << endl;
		else if (response.str()!="OK")
			cerr << "Status push rejected: " << response << endl;
	} catch (const curlpp::RuntimeError &e) {
		cerr << "Failed to send status: server status " << e.what() << endl;
	} catch (const curlpp::LogicError &e) {
		cerr << "Failed to send status: server status " << e.what() << endl;
	}
}

