#include <boost/log/trivial.hpp>
#include <boost/network/protocol/http/client.hpp>
#include "state.hh"

using namespace std;
using namespace boost::network;


void StateTracker::set(State st, bool track) {
	state=st;

	if (!track)
		return;

	http::client::request request("http://www.attingo.nl/");
	request
		<< header ("Connection", "close")
		<< body("<?xml version=\"1.0\"?>\n<status>InUse</status>");
	http::client::options options;
	options.follow_redirects(false)
		.cache_resolved(true)
		.timeout(5);
	http::client client(options);
	auto response = client.put(request);
	int s = status(response);
	if (s!=200)
		BOOST_LOG_TRIVIAL(error) << "Failed to send status: server status " << s;
	if (response.body()!="OK")
		BOOST_LOG_TRIVIAL(error) << "Status server refused our update";
}

