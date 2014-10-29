#ifndef MGR_NULL_DELETER_INCLUDED
#define MGR_NULL_DELETER_INCLUDED

#include <boost/version.hpp>

#if BOOST_VERSION > 105600
#include <boost/core/null_deleter.hpp>
#else
namespace boost {
	struct null_deleter {
		void operator()(void const*) const {}
	};
}
#endif

#endif

