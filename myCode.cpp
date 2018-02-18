#include <exception>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <deque>

using namespace std;

deque<string> _messagesQueue;
std::condition_variable _cvMessages;
std::mutex _mMessages;

std::condition_variable _cv;
std::mutex _m;

void getMessageFromSocket()
{
	std::unique_lock<std::mutex> lckMessages(_mMessages, defer_lock); // not locked yet..
	string data = "test";
	lckMessages.lock(); // locking
	_messagesQueue.push_back(data);
	lckMessages.unlock();

	_cvMessages.notify_all(); // updating all (updateMessageQueue continuing)

	std::unique_lock<std::mutex> lck(_m);
	_cv.wait(lck);
	lckMessages.lock();
	_messagesQueue.pop_back(); // clearing messageQueue
	lckMessages.unlock();
}

void updateMessagesQueue(string user)
{
	string message;

	while (true)
	{
		std::unique_lock<std::mutex> lckMessages(_mMessages); // locking
		_cvMessages.wait(lckMessages); // wait for notify
		lckMessages.unlock();

		lckMessages.lock();
		message = _messagesQueue.front(); // getting message from front of messagesQueue
		lckMessages.unlock();

		_cv.notify_all();
	}
}


void main()
{
	int n = 0;

	while (n < 3)
	{
		std::thread t1(updateMessagesQueue, "user");
		getMessageFromSocket();
		t1.detach();
		n++;
	}
}
