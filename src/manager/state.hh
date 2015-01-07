#ifndef MGR_STATETRACKER_INCLUDED
#define MGR_STATETRACKER_INCLUDED

enum class State {
	Initializing,
	Idle,
	LoggingIn,
	InUse,
	Resetting,
	Error,
};


class StateTracker {
public:
	StateTracker() : state(State::Initializing) { }
	~StateTracker() { }

	State get() const noexcept {
		return state;
	}

	void set(State st, bool track=true) {
		// XXX Push state to monitoring server
		state=st;
	}

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
