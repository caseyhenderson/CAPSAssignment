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

// TODO Server: Implement shared locks and whatever else is necessary to reach 1000 reqs/thread/second goal.
// Clean up code and remove spaghetti code
// Once 1000 requests/thread/second reliably achieved, server is done and testing can begin (for report)
// Can also split a few things into functions (i.e. topic Id iterator method)
void serverThreadFunction(TCPServer* server, ReceivedSocketData&& data);
string handleRequestTypes(string request);

vector<Request> allRequests;
map<string, vector<string>> megamap;
// this would become a map
// and then map.insert?

// so create a pair of <topicId> <vector.pushBack(request)>
// search at the start of every request (in parse) to check if its topicId is one of the map keys
// if it is, push_back to the vector of that space

// how does this work with postId
// get it by doing vector.size with the vector of messages
// so the pair has topicId, messageVector
// this will also replace topicIdCount as you can use vector.size() on the identified topicId

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
		// Introduce shared locks - how do we do this? DONE
		// Remove spaghetti code MOSTLY done but 95 is killing us, and we can do more
		// VECTOR to MAP data structure to hopefully get us there to 1000
		// Hopefully improve performance thusly
		// Multithreading does now work but isn't at 1000 yet
		//// CURRENT STATS (1/12/2023, 20:45 ): Total poster requests: 8257.
		//Average requests per poster thread : 8257.
		//		Total reader requests : 9465.
		//		Average requests per reader thread : 9465.
		//		Total requests : 17722.
		//		Average requests per thread : 8861.
		//		Average requests per thread per second : 885.982.
		// 1 Read, 1 Poster, unthrottled, 10 seconds

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


				// we might not need this with map - may be able to return vector.size()
	//			for (int i = 0; i < (allRequests.size()); i++)
	//			{
	//					shared.lock();
	//					// BAD LINE BELOW
	//					// How can we do this differently? and is it being called loads
	//					// map.at(i)?
	//					// if i corresponds to the key
	//					if (givenRequest.getTopicId() == allRequests.at(i).getTopicId())
	//					{
	//							topicIdCount++;
	//					}
	//					shared.unlock();
	///*					else
	//					{
	//							cout << "Don't add" << endl;
	//					}*/
	//			}
				map<string, vector<string >>::iterator it(megamap.find(givenRequest.getTopicId()));

				// CITATION TWO https://stackoverflow.com/questions/5689003/how-to-implode-a-vector-of-strings-into-a-string-the-elegant-way
				switch (initialCharacter)
				{
				case 'C':
						// call count on map vector with topic ID as key
						// but it needs to be called on the given messageVector (use iterator)
						shared.lock();
						if (it == megamap.end())
						{
								shared.unlock();
								return "0";
						}
						else
						{
								shared.unlock();
								return to_string(it->second.size());
						}
						//return to_string(topicIdCount);
				case 'P':
						// this gets replaced with the map changes
						// we'll insert under the same conditions, but it'll be messageVector.size() replacing topicIdCount

						// search to see if topicId already exists

						// could swap for count
						if (it == megamap.end())
						{
								unique.lock();
								vector<string> newVector = { givenRequest.getMessage() };
								megamap.insert(pair<string, vector<string>>(givenRequest.getTopicId(), newVector));
								unique.unlock(); 
								return "0";
								// this will always return 0
								// cout << it->second.size() << endl;
								// this is what we'll return ^
								// return the vector size thingy?
								// not found - create new pair and add to map
						}
						else
						{
								// need to push back in the space where the topic ID was found - ask about this
								// this is almost certainly where we're getting multiple from
								// this probably gets rewritten to push to topicId space?
								// vector<string> newVector = { givenRequest.getMessage() };
								int postId = it->second.size();
								// finding if the message is present in the vector already. If it is, we don't push back?
								if (find(it->second.begin(), it->second.end(), givenRequest.getMessage()) != it->second.end())
								{
										unique.lock();
										megamap.insert(pair<string, vector<string>>(givenRequest.getTopicId(), it->second));
										unique.unlock();

								}
								else
								{
										it->second.push_back(givenRequest.getMessage());
										unique.lock();
										megamap.insert(pair<string, vector<string>>(givenRequest.getTopicId(), it->second));
										unique.unlock();

								}
								return to_string(postId);
								// this is what we'll return ^
								// return vector size thingy?
								// how does this get added to the map? otherwise it'd only get added on the next POST
								// 
								// found = push_back on vector in space where topicId was found (find should give you iterator)
						}
						//if (topicIdCount == 0)
						//{
						//		givenRequest.setPostId(topicIdCount);
						//		unique.lock();

						//		allRequests.emplace_back(givenRequest);
						//		unique.unlock();
						//		return to_string(givenRequest.postId);
						//}
						//else {
						//		givenRequest.setPostId(topicIdCount);
						//		unique.lock();
						//		allRequests.emplace_back(givenRequest);
						//		unique.unlock();
						//		return to_string(givenRequest.postId);
						//}
				case 'R':
						// substitute the vector size for this maybe? need to see how this vector is created
						// if (messageVector.size() < givenRequest.getPostId())
						// map lookup by key to get the associated vector
						if (megamap.find(givenRequest.getTopicId()) != megamap.end())
						{
								shared.lock();
								// conflict here
								// can change to range based for loop
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




						//if (topicIdCount < givenRequest.getPostId())
						//{
						//		return "";
						//}
						//else
						//{
						//		// use givenRequest.gettopicId as the key to lookup in map
						//		// and then print all matches?
						//		// else 
						//		// return all values in map matching 
						//		shared.lock();
						//		for (int i = 0; i < allRequests.size(); i++)
						//		{
						//				if (allRequests.at(i).getTopicId() == givenRequest.getTopicId())
						//				{
						//						if (allRequests.at(i).getPostId() == givenRequest.getPostId())
						//						{
						//								shared.unlock();
						//								return allRequests.at(i).getMessage();
						//						}
						//				}
						//				else if ((allRequests.at(i).getTopicId() == givenRequest.getTopicId()) && (allRequests.at(i).getPostId() == givenRequest.getPostId()))
						//				{
						//						shared.unlock();
						//						return allRequests.at(i).getMessage();
						//				}
						//		}
						//		shared.unlock();
						//		return "";
						//}
				case 'L':
						// fix this, it's outrageously complicated for what it is
						// but it does work lol
						// think about what we're trying to do, and how we can speed it up


						// new approach
						// iterate through the map, listing all 'keys' (topicIds)
						// may have to do this still to remove dupes
						// but we'll see
						shared.lock();
						// topics need to be unique
						for (map<string, vector<string>>::iterator it = megamap.begin(); it != megamap.end(); ++it)
						{

								// if (it->first isn't already in listOfTopics)
								// // the version of this for our new DS
								//		if (listOfTopics.find(allRequests.at(i).getTopicId()) != std::string::npos)
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


						// shared.lock()
						//for (int i = 0, j = 0; i < allRequests.size(); i++)
						//{
						//		if (listOfTopics.find(allRequests.at(i).getTopicId()) != std::string::npos)
						//		{
						//				//cout << "Dupe found, don't add" << endl;
						//				// is it this?
						//		}
						//		else {
						//				if (listOfTopics.size() > 1)
						//				{
						//						listOfTopics.append("#");
						//				}
						//				listOfTopics.append("@");
						//				listOfTopics.append(allRequests.at(i).getTopicId());

						//		}
						//}
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