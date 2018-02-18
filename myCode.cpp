#include <WinSock2.h>
#include "Server.h"
#include <exception>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

deque<string> _messagesQueue;
std::condition_variable _cvMessages;
std::mutex _mMessages;

void getMessageFromSocket(SOCKET clientSocket)
{
	std::unique_lock<std::mutex> lckMessages(_mMessages, defer_lock); // not locked yet..

	lckMessages.lock(); // locking
	_messagesQueue.push_back(data);
	lckMessages.unlock(); 

	_cvMessages.notify_all(); // updating all (updateMessageQueue continuing)

	lckMessages.lock();
	_messagesQueue.pop_back(); // clearing messageQueue
	lckMessages.unlock();
}

void updateMessagesQueue(SOCKET sc, string user)
{
	string message;

	while (true)
	{
		std::unique_lock<std::mutex> lckMessages(_mMessages); // locking
		if (_messagesQueue.empty()) // if messageQueue empty
			_cvMessages.wait(lckMessages); // wait for notify
		lckMessages.unlock();

		lckMessages.lock();
		message = _messagesQueue.front(); // getting message from front of messagesQueue
		Helper::updateFile(PATH, message); // updating file...
		lckMessages.unlock();
	}
}


void main()
{
	std::thread t1(updateMessagesQueue, someSocket, "user");
	getMessageFromSocket(someSocket);
}
