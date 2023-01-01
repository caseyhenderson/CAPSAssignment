#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <mutex>
#include <shared_mutex>

#include "config.h"
#include "TCPServer.h"
#include "TCPClient.h"
#include "RequestParser.h"

#define DEFAULT_PORT 12345
using namespace std;

bool terminateServer = false;

// TODO Server: Implement shared locks and whatever else is necessary to reach 1000 reqs/thread/second goal.
// Clean up code and remove spaghetti code
// Once 1000 requests/thread/second reliably achieved, server is done and testing can begin (for report)
// Can also split a few things into functions (i.e. topic Id iterator method)
void serverThreadFunction(TCPServer* server, ReceivedSocketData&& data);
string handleRequestTypes(string request);

vector<Request> allRequests;
vector<string> topics;
string listOfTopics = "";
shared_mutex mutey;

// this will be replaced with a shared_mutex, but we need to figure out how that's done first.
// Will potentially need to refactor this into a class (would be a good idea anyway, OOP principles)
// In that case, may need to separate certain vars from the class
// class XXXX, etc
// how would that impact methods?
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
		// count them here
		// output total number too
		std::cout << "Server terminated." << std::endl;

		return 0;
}

string handleRequestTypes(string request)
{
		// TODO:
		// Fix weirdness with postId being saved to message on Read Requests DONE - seemingly (was a parsing thing - the same thing that's used to get the message on POST requests can be used to get the PostId on read requests)
		// Introduce shared locks - how do we do this?
		// Remove spaghetti code
		// Hopefully improve performance thusly
		// Multithreading does now work but is S L O W
		// CURRENT STATS (30/12/2022, 01:11): Total poster requests: 2228.
/*			Total reader requests : 2124.
				Average requests per reader thread : 1062.
				Total requests : 4352.
				Average requests per thread : 1088.
				Average requests per thread per second : 108.724.*/		/*Average requests per poster thread : 1114.*/

		if (Request::parse(request).valid) {
				Request givenRequest = Request::parse(request);
				char initialCharacter = givenRequest.requestType.front();
				int topicIdCount = 0;

				// VEC CITATION ONE https://stackoverflow.com/questions/2395275/how-to-navigate-through-a-vector-using-iterators-c
				// Do we need to do this before everything? probably not, so mess around with it for efficiency
				std::shared_lock<std::shared_mutex> shared(mutey, std::defer_lock);
				std::unique_lock<std::shared_mutex> unique(mutey, std::defer_lock);
				// possibly need to add defer_lock
				// also, it's probably time to change vector to map 

				for (int i = 0; i < (allRequests.size()); i++)
				{
						shared.lock();
						// BAD LINE BELOW
						// How can we do this differently? and is it being called loads
						if (givenRequest.getTopicId() == allRequests.at(i).getTopicId())
						{
								topicIdCount++;
						}
						shared.unlock();
	/*					else
						{
								cout << "Don't add" << endl;
						}*/
				}

			
				// CITATION TWO https://stackoverflow.com/questions/5689003/how-to-implode-a-vector-of-strings-into-a-string-the-elegant-way
				switch (initialCharacter)
				{
				case 'C':
						return to_string(topicIdCount);
				case 'P':
						if (topicIdCount == 0)
						{
								givenRequest.setPostId(topicIdCount);
								unique.lock();
								allRequests.emplace_back(givenRequest);
								unique.unlock();
								return to_string(givenRequest.postId);
						}
						else {
								givenRequest.setPostId(topicIdCount);
								unique.lock();
								allRequests.emplace_back(givenRequest);
								unique.unlock();
								return to_string(givenRequest.postId);
						}
						break;
				case 'R':
						if (topicIdCount < givenRequest.getPostId())
						{
								return "";
						}
						else
						{
								shared.lock();
								for (int i = 0; i < allRequests.size(); i++)
								{
										if (allRequests.at(i).getTopicId() == givenRequest.getTopicId())
										{
												if (allRequests.at(i).getPostId() == givenRequest.getPostId())
												{
														shared.unlock();
														return allRequests.at(i).getMessage();
												}
										}
										else if ((allRequests.at(i).getTopicId() == givenRequest.getTopicId()) && (allRequests.at(i).getPostId() == givenRequest.getPostId()))
										{
												shared.unlock();
												return allRequests.at(i).getMessage();
										}
								}
								shared.unlock();
								return "";
						}
				case 'L':
						// fix this, it's outrageously complicated for what it is
						// but it does work lol
						// think about what we're trying to do, and how we can speed it up
						shared.lock();
						for (int i = 0, j = 0; i < allRequests.size(); i++)
						{
								if (listOfTopics.find(allRequests.at(i).getTopicId()) != std::string::npos)
								{
										//cout << "Dupe found, don't add" << endl;
										// is it this?
								}
								else {
										if (listOfTopics.size() > 1)
										{
												listOfTopics.append("#");
										}
										listOfTopics.append("@");
										listOfTopics.append(allRequests.at(i).getTopicId());

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
		// fix this, shouldn't be needed
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