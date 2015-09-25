#ifndef MGR_POLLER_INCLUDED
#define MGR_POLLER_INCLUDED

#include <chrono>
#include <memory>
#include <set>

class Pollee {
public:
	virtual ~Pollee() {}
	virtual int fd() const = 0;
	virtual void update() = 0;
};


class Poller {
public:
	typedef std::set<std::shared_ptr<Pollee>> _cont_type;

	typedef _cont_type::key_type key_type;
	typedef _cont_type::value_type value_type;
	typedef _cont_type::size_type size_type;
	typedef _cont_type::iterator iterator;
	typedef _cont_type::const_iterator const_iterator;

private:
	_cont_type fds;
	int epoll_fd;

	void add_to_epoll(std::shared_ptr<Pollee> pollee);
	void remove_from_epoll(std::shared_ptr<Pollee> pollee);

public:
	Poller();
	~Poller();

	Poller(const Poller&) = delete;
	Poller& operator=(const Poller&) = delete;

	void runForever();
	int runOnce(int timeout=-1);  // Return the event count
	int runOnce(std::chrono::milliseconds timeout) {
		return runOnce(timeout.count());
	}

	iterator begin() noexcept {
		return fds.begin();
	}

	const_iterator begin() const noexcept {
		return fds.begin();
	}

	const_iterator cbegin() const noexcept {
		return fds.begin();
	}

	iterator end() noexcept {
		return fds.end();
	}

	const_iterator end() const noexcept {
		return fds.end();
	}

	const_iterator cend() const noexcept {
		return fds.end();
	}

	bool empty() const noexcept {
		return fds.empty();
	}

	size_type size() const noexcept {
		return fds.size();
	}

	size_type max_size() const noexcept {
		return fds.max_size();
	}

	void clear() noexcept {
		fds.clear();
	}

	std::pair<iterator, bool> insert(const value_type& value) {
		add_to_epoll(value);
		return fds.insert(value); // Remove from epoll on error?
	}

	std::pair<iterator, bool> insert(value_type&& value) {
		add_to_epoll(value);
		return fds.insert(value); // Remove from epoll on error?
	}

	iterator erase(const_iterator pos) {
		return fds.erase(pos);
	}

	iterator erase(const_iterator first, const_iterator last) {
		return fds.erase(first, last);
	}

	size_type erase(const key_type& key) {
		return fds.erase(key);
	}

	size_type count(const key_type& key) const {
		return fds.count(key);
	}

	iterator find(const key_type& key) {
		return fds.find(key);
	}

	const_iterator find(const key_type& key) const {
		return fds.find(key);
	}

};

#endif

