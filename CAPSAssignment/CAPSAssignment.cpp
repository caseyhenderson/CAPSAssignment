#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <mutex>

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
vector<Request> allRequests;
// vector<Request> listRequests;
vector<string> topics;
string listOfTopics = "";
mutex mutey;
// vector<PostRequest> postRequests;
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



		// lock and unlock on each thingy

		// switch case on request types
		// feel like we may need another layer of selection here






		if (Request::parse(request).valid) {
				Request givenRequest = Request::parse(request);
				char initialCharacter = givenRequest.requestType.front();
				//cout << "Initial Char " + initialCharacter << endl;
				int topicIdCount = 0;
				// could replace with range-based for loop (for int i: allRequests) ? or other more efficient iteration
				// there's probably a way of getting it to work with count, could use autos too
				// this absolutely needs optimising if it's gonna be the 'check topic count' operation
				// also needs separating out
				// VEC CITATION ONE https://stackoverflow.com/questions/2395275/how-to-navigate-through-a-vector-using-iterators-c
				// the iteration is what's breaking here
				// Do we need to do this before everything? probably not, so mess around with it for efficiency
				for (int i = 0; i < (allRequests.size()); i++)
				{
						// if it's already in topics, don't add it
						if (givenRequest.getTopicId() == allRequests.at(i).getTopicId())
						{
								// listRequests.emplace_back(allRequests.at(i));
								// cout << "List request " + to_string(i) + " is " + listRequests.at(i).getMessage() << endl;
								// only gets one message added to it, weirdly. check this
								// and it's always the first post request?
								// could be to do with where it is, or where the iterator is? 

								// should this only be happening on POST requests?
								//if (count(topics.begin(), topics.end(), givenRequest.getTopicId()) == 0)
								//{
								//		topics.emplace_back(allRequests.at(i).getTopicId());
								//		cout << "Unique topic found, and added" << endl;
								//}
								// topics.emplace_back(allRequests.at(i).getTopicId());
								// suppose it adds whenever a match is found, so in theory no? monitor
								topicIdCount++;
						}
	/*					else
						{
								cout << "Don't add" << endl;
						}*/
				}
				cout << "Number of matches for topic " + to_string(topicIdCount) << endl;
				// CITATION TWO https://stackoverflow.com/questions/5689003/how-to-implode-a-vector-of-strings-into-a-string-the-elegant-way
				const char* const separator = "#";
				ostringstream topicString;


				// ^ this may need to be switched to handle a DS of topicIds
				// ^ or a custom method
				// If they all have ^ then this needs to be [1]
				// could define a return value here and have it change based on selection - possible speed increase
				switch (initialCharacter)
				{
				case 'C':
						cout << "COUNT REQUEST" << endl;
						// this does the count?
						// and maybe the lock/unlock here - anywhere where we access DS put lock around it
						// take a topicId, return size
						// so we're returning a number here
						// Same comparison - the number of posts on a given topic, which is how we figure out new PostIDs
						// This operation is gonna be done a LOT so must be optimised with data structure choice (get working with vector, then choose and justify)
						// but in this case it just tells us how many there are in the specified topic - a find/search that just returns the number
						// where this operation should be done is another matter
						// if (size = 0) return 0, if not return size - literally just returning the size / count value (which is also technically the new post ID referenced in the below request handlers - can be made more generic)
						return to_string(topicIdCount);
				case 'P':
						cout << "POST REQUEST" << endl;
						//cout << "The topic ID is " + givenRequest.topicId + " and the get method returns " + givenRequest.getTopicId() << endl;
						if (topicIdCount == 0)
						{
								mutey.lock();
								givenRequest.setPostId(topicIdCount);
								// need setter method
	/*							for (int i = 0; i < allRequests.size(); i++)
								{
										if (givenRequest.getTopicId() == allRequests.at(i).getTopicId())
										{
												cout << "Dupe, no add";
										}
										else
										{
												topics.emplace_back(givenRequest.getTopicId());
										}
								}*/
								allRequests.emplace_back(givenRequest);
			/*					if (count(allRequests.begin(), allRequests.end(), givenRequest.getTopicId()) == 0)
								{
										topics.emplace_back(givenRequest.getTopicId());
										cout << "Unique topic found, and added" << endl;
								}*/
								mutey.unlock();
								// cout << "POST ID converted to string on the ZERO Request is" + to_string(givenRequest.postId) << endl;
								return to_string(givenRequest.postId);
								// create new topic somehow - presumably this creates new topicID
								// append to a topic - does this mean we need topic vectors? Investigate
								// Either way, we need to take this, do the same topic comparison in the structure to figure out how many posts there are, and then return the post ID
								// This is the 'alt' path where the topic does not already exist
								// need to be able to return an accurate postID - here, 0
								// if the topic does not exist, add the topicID to the vector here
						}
						else {
								// This is the 'main path' where the topic does already exist
								// As such, we need to do the same topic comparison in the structure to figure out what the post ID should be
								// And then return that +1
								// And add the request to the data structure
								mutey.lock();
								givenRequest.setPostId(topicIdCount);
								allRequests.emplace_back(givenRequest);
								mutey.unlock();
								// figure out postId here - change to getPostId()
								// cout << "PostId converted to string on the NON-ZERO is" + to_string(givenRequest.postId) <<endl;
								return to_string(givenRequest.postId);
								// find the amount of topicIds in vector and output
								// append to existing topic (lookup against them)
								// and an accurate postId here would be how many posts there already are on the topic
								// request.find(topicId) maybe? throw in the get method - need a count
						}
						// this does the post
						// Should probably have setter methods
						// and lock /unlock at the start and end for mutexes - implement next. Lock/unlock on access/write to data structures
						break;
				case 'R':
						// unlock
						// read 
						// take a topicID
						// return the message
						// need to implement the lookup here
						// and then lock? is this where it goes??
			/*			cout << "The topic ID is " + givenRequest.topicId << endl;
						cout << "The message of this READ request is " + givenRequest.message << endl;
						cout << "The getMessage request of this READ request is returning " + givenRequest.getMessage() << endl;*/
						// cout << "The topic ID count is " + to_string(topicIdCount) + " and the postId is " + to_string(givenRequest.postId) << endl;
						// Take the given request's topicID, and look it up in the existing data structure with all the pre-existing requests and their topicIds
						// this is why the vector may need to be replaced by a map.
						// iterate through vector, comparing the topicID. First to see if it exists, and also to see how many times it exists (hence getting the number of posts on the topic without needing postId)
						// Hence you will need a variable called topicCount or whatever for how many times it returns.
						// if (topicCount == 0 OR topicCount < givenRequest.postId) - make sure postId is being set correctly. In theory, the topic count should suffice without needing to literally get post IDs from the allRequests vector
						// separate topicIdCounts for each one? may not be reset properly
						cout << "READ REQUEST" << endl;
						cout << "The read request's topic ID count is " + to_string(topicIdCount) + " and the POST ID is " + to_string(givenRequest.getPostId()) << endl;
						// This should not be topic ID count, i don't think
						if (topicIdCount < givenRequest.getPostId())
						{
								// this should be if topicID is greater than the number of times it appears in the topics structure
								// OR if it isn't a thing at all (yet)
								// Hence we return a blank line
								// if(givenRequest.topicId > topicCount OR givenRequest.topicId is NOT in the vector
								// Need a comparison topicID method - could be in the getter? or could have a validity checker method etc

								return "";
								// include a comparison on postId too for the other criteria

						}
						// this is gonna need serious debugging - what if post Ids don't match?
						else
						{
								for (int i = 0; i < allRequests.size(); i++)
								{
										cout << "REQUEST NO: " + to_string(i) + " REQUEST: " + allRequests.at(i).toString() + " TOPIC ID: " + allRequests.at(i).getTopicId()+ " POST ID: " + to_string(allRequests.at(i).getPostId()) + "MESSAGE: " + allRequests.at(i).getMessage() + " Vector size " +to_string(allRequests.size()) << endl;
										mutey.lock();
										// the logic here is bugged, because this vector isn't actually broken
										if (allRequests.at(i).getTopicId() == givenRequest.getTopicId())
										{
												cout << "Matches topic ID " + allRequests.at(i).getMessage() << endl;
												// is this bit the problem? does it just not get out in time to iterate?
												if (allRequests.at(i).getPostId() == givenRequest.getPostId())
												{
														cout << "Matches topic and Post ID " + allRequests.at(i).getMessage() + " at " + to_string(i) << endl;
														mutey.unlock();
														return allRequests.at(i).getMessage();
												}
										}
										else if ((allRequests.at(i).getTopicId() == givenRequest.getTopicId()) && (allRequests.at(i).getPostId() == givenRequest.getPostId()))
										{
												mutey.unlock();
												return allRequests.at(i).getMessage();
										}
										mutey.unlock();
								}

								// 'normal path' if the topicID is fine#
								// 
								// then we just need to return the requested message
								// so we need to be capable of looking up by topicId and then postId
								// although here that should already be done, maybe.
								// basically: grab the relevant ones from the allRequests vector
								// first everything with the topicId, and then the postId
								// The Ids to be searched by come from the givenRequest
								// The ones to compare against come from whichever post requests have already gone in (or not)
								// TOMORROW: write out your examples of each request. Comments left should help with implementation.
								return "";
						}
				case 'L':

						// LIST request
						// This basically needs to print a vector of topicIds, but as a string
						// We might create a separate DS for topic Ids if it improves speed
						// In the mean time though, just iterate through allRequests and print every topicId that exists
						// Doesn't specify if they should be unique
						// But literally for (allRequests.size) output the topicIds into a string and return that
						// could possibly be done in the pre-existing method for other stuff but maybe not
						// Return the LIST
						// are we returning in right place? may need to break, or return once at the end of these - just change return depending
						// i.e. variable to return that changes depending on selection

						// created the listRequests vector for this - there is a better way, granted, but not today
						// actually LR vector not needed, you literally just need topicIds from everything
						// the iteration I'm going to put here should be able to be replaced by a single method that can also handle the iteration above
						// we're getting it working before we get it pretty.
						// see if we can add the hash separator there - how might we do this
						// hasn't been working cause any comparison with givenRequest is useless on a LIST request - it's just LIST
						// probably worth creating separate, unique LIST topic vector
						cout << "LIST REQUEST" << endl;
						cout << "Size of vector at this point is: "+to_string(allRequests.size())<<endl;
						cout << "Contents are " << listOfTopics << endl;
						// cout << "Size of topics vector is: " + to_string(topics.size());
						//for (int j = 0; j < topics.size(); j++)
						//{
						//		cout << "Topics vector contains " + topics.at(j) << endl;
						//}

						// fix this, it's outrageously complicated for what it is
						mutey.lock();
						for (int i = 0, j = 0; i < allRequests.size(); i++)
						{
								if (listOfTopics.find(allRequests.at(i).getTopicId()) != std::string::npos)
								{
										cout << "Dupe found, don't add" << endl;
										// is it this?
								}
								else {
										if (listOfTopics.size() > 1)
										{
												listOfTopics.append("#");
										}
										listOfTopics.append("@");
										// make conditonal - should only be added if separating words (don't worry about trailing here, it's removed at the end)
										listOfTopics.append(allRequests.at(i).getTopicId());
										// this is not adding hashtags properly
										cout << "List one: " + listOfTopics << endl;

										// this keeps being added and then removed.
								}
						}
						cout << "List one point 5: " + listOfTopics << endl;

										// this logic is broken and unwieldy
										// basically, add a hashtag unles it's going to be trailing.





						//				if (i = (allRequests.size()-1) && ((allRequests.size() - 1)>0))
						//				{
						//						cout << "Don't add the hashtag";
						//				}
						//				else
						//				{
						//						// remove this to go back up to 15
						//						// hashtags do need adding as separators though
						//						// TOMORROW: Fix 'READ' and 'LIST' - possibly the same issue with the data structure?
						//						cout << "Do add the hashtag, unless it's trailing";
						//						listOfTopics.append("#");
						//						if (listOfTopics.length() > 0)
						//						{
						//								std::string::iterator it = listOfTopics.end() - 1;
						//								cout << listOfTopics;
						//								if (*it == '#')
						//								{
						//										listOfTopics.erase(it);
						//								}
						//						}
						//				}
						//		}
						//}
		/*				if (listOfTopics.length() > 0)
						{
								std::string::iterator it = listOfTopics.end() - 1;
								cout << listOfTopics;
								if (*it == '#')
								{
										listOfTopics.erase(it);
								}
						}*/
						//copy(topics.begin(), topics.end(),
						//		ostream_iterator<string>(topicString, separator));
						//return topicString.str();
						

						// this is fine at killing trailing hashtags. the job now is to get them where they're actually needed.
						if (listOfTopics.length() > 0)
						{
								std::string::iterator it = listOfTopics.end() - 1;
								if (*it == '#')
								{
										listOfTopics.erase(it);
								}
						}
						cout << "List two: " + listOfTopics << endl;
						mutey.unlock();
						return listOfTopics;
				default:
						cout << "Not a valid request";
				}
				// exit is already handled - in theory
		}
		// may need to return something here
		// we'll need to return the changes / impacts to pass protocol verification
		// so something from the vector?

		return request;

		//else if (ReadRequest::parse(request).valid) {
		//		// take a topic ID from the request, and use it to look up topic
		//		// use postID from request to identify and return requested message
		//		// respond with the message
		//		ReadRequest readRequest = ReadRequest::parse(request);
		//		readRequest.getTopicId(); // use to look up topic and return message
		//		readRequest.getPostId(); // use to return relevant post
		//		// store as message and return - could be optimised
		//		// return message;
		//}

		//// Then handle LIST
		//// Return list of all topic IDs, separated by hash

		//else if (ListRequest::parse(request).valid)
		//{
		//		// cout all topic IDs - when we know how to get ALL of them
		//		// where we getting these from?
		//}

		//else if (CountRequest::parse(request).valid)
		//{
		//		CountRequest countRequest = CountRequest::parse(request);
		//		std::string topicId = countRequest.getTopicId();
		//		// look up in the post directory how many match the topicIds
		//		// return 0 if it doesn't exist.
		//}
		//// EXIT possibly already handled?
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
		// output total number too

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

						// reply needs to be blank
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