#include <cerrno>
#include <unistd.h>
#include <sys/epoll.h>
#include <system_error>
#include "poller.hh"

using namespace std;

Poller::Poller() : fds(), epoll_fd(-1) {
	if ((epoll_fd=epoll_create1(EPOLL_CLOEXEC))==-1)
		throw system_error(errno, system_category());
}


Poller::~Poller() {
	close(epoll_fd);
}


void Poller::add_to_epoll(shared_ptr<Pollee> pollee) {
	epoll_event ev;
	ev.events=EPOLLIN;
	ev.data.ptr=pollee.get();
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pollee->fd(), &ev)==-1)
		if (errno!=EEXIST)
			throw system_error(errno, system_category());
}

void Poller::remove_from_epoll(shared_ptr<Pollee> pollee) {
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pollee->fd(), nullptr)==-1)
		if (errno!=ENOENT)
			throw system_error(errno, system_category());
}


void Poller::runForever() {
	while (!empty())
		runOnce();
}



void Poller::runOnce() {
	if (empty())
		return;

	epoll_event events[10];
	int event_count, i;

	if ((event_count=epoll_wait(epoll_fd, events, 10, -1))==-1)
		throw system_error(errno, system_category());
	for (i=0; i<event_count; i++) {
		auto p = reinterpret_cast<Pollee*>(events[i].data.ptr);
		p->update();
	}
}

