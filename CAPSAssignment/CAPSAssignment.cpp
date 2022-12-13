#include <iostream>
#include <algorithm>
#include <string>
#include <thread>
#include <vector>

#include "config.h"
#include "TCPServer.h"
#include "TCPClient.h"
#include "RequestParser.h"

#define DEFAULT_PORT 12345
using namespace std;

bool terminateServer = false;

void serverThreadFunction(TCPServer* server, ReceivedSocketData&& data);
string handleRequestTypes(string request);

// define your mutexes? and vectors here
// will be one vector but let's get working requests first
// vector<Request> allRequests;
vector<PostRequest> postRequests;
// WARNING: Contains Agony, Pain and Suffering
// Yes, that's right - it's CAPS


string handleRequestTypes(string request)
{
		// taking a request, can we also take a request type so we can switch/case?
		// // Determine if valid
		// Then do request-specific behaviour from protocol.html
		// Then return 
		// First handle POST requests

		// possible character-based approach to use switch / case
		// lock for vector data structure which you emplace back
		// may also use map
		// topicId as unique key - each one is vector
		// hold requests in vectors

		// if it's a post request throw into vector
		// need a structure for topics etc - map


		// SKETCH

		// if Request::parse(request).valid
		// check what it is
		// then do necessary behaviour
		// so
				// post requests go into the vector (with a topicId)
				// read requests come from the vector (takes topicId)
		    // count requests take vector.size in some form (taking topicId)
				// list requests take all topicIds from the data structure - could be all the keys











		if (PostRequest::parse(request).valid) {
				PostRequest postRequest = PostRequest::parse(request);
				// will change this to check against existing IDs when I know how to get them
				// this will be list of existing topics
				if (postRequest.getTopicId() == "")
				{
						postRequest.postId = 0;
						postRequests.emplace_back(postRequest);

						// create new topic somehow - presumably this creates new topicID
				}
				else {
						postRequests.emplace_back(postRequest);
						// append to existing topic (lookup against them)
				}

				// append message to topic if exists - use topicID to determine
				// respond with PostID - 0 if topic does not exist, and new topic created (how? is a new topicID enough)
		}
		else if (ReadRequest::parse(request).valid) {
				// take a topic ID from the request, and use it to look up topic
				// use postID from request to identify and return requested message
				// respond with the message
				ReadRequest readRequest = ReadRequest::parse(request);
				readRequest.getTopicId(); // use to look up topic and return message
				readRequest.getPostId(); // use to return relevant post
				// store as message and return - could be optimised
				// return message;
		}

		// Then handle LIST
		// Return list of all topic IDs, separated by hash

		else if (ListRequest::parse(request).valid)
		{
				// cout all topic IDs - when we know how to get ALL of them
				// where we getting these from?
		}

		else if (CountRequest::parse(request).valid)
		{
				CountRequest countRequest = CountRequest::parse(request);
				std::string topicId = countRequest.getTopicId();
				// look up in the post directory how many match the topicIds
				// return 0 if it doesn't exist.
		}
		// EXIT possibly already handled?
		return "we'll implmement this later";
}


int main()
{
		TCPServer server(DEFAULT_PORT);

		ReceivedSocketData receivedData;

		std::vector<std::thread> serverThreads;

		std::cout << "Starting server. Send \"exit\" (without quotes) to terminate." << std::endl;

		while (!terminateServer)
		{
				receivedData = server.accept();

				if (!terminateServer)
				{
						serverThreads.emplace_back(serverThreadFunction, &server, receivedData);
				}
		}

		for (auto& th : serverThreads)
				th.join();
		// count them here
		// output total number tooo

		std::cout << "Server terminated." << std::endl;

		return 0;
}

void serverThreadFunction(TCPServer* server, ReceivedSocketData&& data)
{
		unsigned int socketIndex = (unsigned int)data.ClientSocket;

		do {
				// fun request parsing things will happen here! Yay!
				// But first, ten acronyms to sum up my feelings about this assignment
				// Complete Absolute Pain System
				// Crappy Agonising Painful Situation
				// C++ Agonises, Pains and Smashes (my soul)
				// Crushingly Awful Painful Situation
				// Crisis (of) Agonisingly Painful Stuff
				// Contains Agony, Pain (and) Suffering
				// Crying Agonisingly, Painfully (and) Slowly
				// C++ - Awful, Painful (and) Shockingly (bad)
				// Critically Awful Punishing Situation
				// Crappy Assignment of Pain and Suffering
				// Cripplingly Awful Pain and Suffering
				// We'll have our requestParser function drop in here so we're handling ALL not just exit lol

				server->receiveData(data, 0);
				// replace with handleRequestTypes (which tbf probably doesn't need to handle exit now? maybe?
				if (data.request != "" && data.request != "exit" && data.request != "EXIT")
				{
						// std::cout << "[" << socketIndex << "] Bytes received: " << data.request.size() << std::endl;
						// std::cout << "[" << socketIndex << "] Data received: " << data.request << std::endl;

						data.reply = handleRequestTypes(data.request);
						// std::reverse(data.reply.begin(), data.reply.end());

						server->sendReply(data);
				}
				else if (data.request == "exit" || data.request == "EXIT")
				{
						//std::cout << "[" << socketIndex << "] Data received: " << data.request << std::endl;
						// std::cout << "[" << socketIndex << "] Exiting... Bye bye!" << std::endl;

						// reply may just need to be blank
						data.reply = data.request;
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