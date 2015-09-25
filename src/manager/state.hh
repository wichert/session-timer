#ifndef MGR_STATETRACKER_INCLUDED
#define MGR_STATETRACKER_INCLUDED

#include <string>

enum class State {
	Initializing=0,
	Idle,
	InUse,
	Resetting,
	Error,
};


inline std::string to_string(State st) {
	switch (st) {
	case State::Initializing:
		return "Initializing";
	case State::Idle:
		return "Idle";
	case State::InUse:
		return "InUse";
	case State::Resetting:
		return "Resetting";
	case State::Error:
		return "Unknown";
// PostgreSQL enum does not have error value, and migration on 8.4 is too painful
//		return "Error";
	}
	return "Unknown";
}


class StateTracker {
public:
	StateTracker() : state(State::Initializing) { }
	~StateTracker() { }

	State get() const noexcept {
		return state;
	}

	void set(State st, bool track=true);

	bool operator==(State st) {
		return state==st;
	}

	bool operator!=(State st) {
		return state!=st;
	}

private:
	State state;
};


#endif
