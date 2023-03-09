#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <shared_mutex>

#include "config.h"
#include "TCPServer.h"
#include "TCPClient.h"
#include "RequestParser.h"

#define DEFAULT_PORT 12345
using namespace std;

bool terminateServer = false;
void serverThreadFunction(TCPServer* server, ReceivedSocketData&& data);
string handleRequestTypes(string request);

vector<Request> allRequests;
map<string, vector<string>> requestsMap;

string listOfTopics = "";
shared_mutex mutexShared;

int main()
{
		TCPServer server(DEFAULT_PORT);

		ReceivedSocketData receivedData;

		std::vector<std::thread> serverThreads;


		while (!terminateServer)
		{
				receivedData = server.accept();
				if (!terminateServer)
				{
						std::cout << "Server started." << endl;
						serverThreads.emplace_back(serverThreadFunction, &server, receivedData);
				}
		}
		for (auto& th : serverThreads)
				th.join();
		std::cout << "Server terminated." << std::endl;

		return 0;
}

string handleRequestTypes(string request)
{
		if (Request::parse(request).valid) {
				Request givenRequest = Request::parse(request);
				char initialCharacter = givenRequest.requestType.front();
				int topicIdCount = 0;
				std::shared_lock<std::shared_mutex> shared(mutexShared, std::defer_lock);
				std::unique_lock<std::shared_mutex> unique(mutexShared, std::defer_lock);
	
				map<string, vector<string >>::iterator it(requestsMap.find(givenRequest.getTopicId()));

				switch (initialCharacter)
				{
				case 'C':
						// call count on map vector with topic ID as key
						// but it needs to be called on the given messageVector (use iterator)
						shared.lock();
						if (it == requestsMap.end())
						{
								shared.unlock();
								return "0";
						}
						else
						{
								shared.unlock();
								return to_string(it->second.size());
						}
				case 'P':

						// search to see if topicId already exists. If it doesn't, we insert and return 0.
						if (it == requestsMap.end())
						{
								unique.lock();
								vector<string> newVector = { givenRequest.getMessage() };
								requestsMap.insert(pair<string, vector<string>>(givenRequest.getTopicId(), newVector));
								unique.unlock(); 
								return "0";
						}
						else
						{
								// need to push back in the space where the topic ID was found
								int postId = it->second.size();
								if (find(it->second.begin(), it->second.end(), givenRequest.getMessage()) != it->second.end())
								{
										unique.lock();
										requestsMap.insert(pair<string, vector<string>>(givenRequest.getTopicId(), it->second));
										unique.unlock();

								}
								else
								{
										it->second.push_back(givenRequest.getMessage());
										unique.lock();
										requestsMap.insert(pair<string, vector<string>>(givenRequest.getTopicId(), it->second));
										unique.unlock();

								}
								return to_string(postId);
						}
				case 'R':
						// map lookup by key to get the associated vector
						if (requestsMap.find(givenRequest.getTopicId()) != requestsMap.end())
						{
								shared.lock();
								for (int i = 0; i<it->second.size(); i++)
								{
										if (i == givenRequest.getPostId())
										{
												shared.unlock();
												return it->second.at(i);
										}
								}
								shared.unlock();
								return "";
						}
						else
						{
								return "";
						}
				case 'L':
						shared.lock();
						// topics need to be unique
						for (map<string, vector<string>>::iterator it = requestsMap.begin(); it != requestsMap.end(); ++it)
						{

								if (listOfTopics.find(it->first) != std::string::npos)
								{
										// Dupe found, don't add
								}
								else
								{
										if (listOfTopics.size() > 1)
										{
												listOfTopics.append("#");
										}
										listOfTopics.append("@");
										listOfTopics.append(it->first);
								}
						}
						shared.unlock();
						if (listOfTopics.length() > 0)
						{
								std::string::iterator it = listOfTopics.end() - 1;
								if (*it == '#')
								{
										listOfTopics.erase(it);
								}
						}
						return listOfTopics;
				default:
						cout << "Not a valid request";
				}
		}
		return request;
}


void serverThreadFunction(TCPServer* server, ReceivedSocketData&& data)
{
		unsigned int socketIndex = (unsigned int)data.ClientSocket;

		do {

				server->receiveData(data, 0);
				if (data.request != "" && data.request != "exit" && data.request != "EXIT")
				{
						data.reply = handleRequestTypes(data.request);
						server->sendReply(data);
				}
				else if (data.request == "exit" || data.request == "EXIT")
				{
						// reply needs to be blank
						data.reply = "";
						server->sendReply(data);
				}
		} while (data.request != "exit" && data.request != "EXIT" && !terminateServer);

		if (!terminateServer && (data.request == "exit" || data.request == "EXIT"))
		{
				terminateServer = true;

				TCPClient tempClient(std::string("127.0.0.1"), DEFAULT_PORT);
				tempClient.OpenConnection();
				tempClient.CloseConnection();
		}

		server->closeClientSocket(data);
}
