#include <iostream>
#include <regex>
#include "RequestParser.h"
using namespace std;


Request::Request() : requestType(""), topicId(""), message(""), valid(true) 
{}
Request::Request(std::string requestType) : requestType(requestType), topicId(""), message(""), valid(true) {}
Request::Request(std::string requestType, std::string topicId, std::string message) : requestType(requestType), topicId(topicId), message(message), postId(0), valid(true) {}
Request::Request(std::string requestType, std::string topicId, std::string message, int postId) : requestType(requestType), topicId(topicId), message(message), postId(postId), valid(true) {}

Request::~Request()
{ }

Request Request::parse(std::string request)
{
		// throw all parsing in here
		// selection based on type 
		// remember you'll need to assign a type too
		// get examples of each request
		// must return a request
		const auto indexVal = request.find("@");
		if (indexVal != std::string::npos) {
				std::string requestType = request.substr(0, indexVal);
				//cout << "Request Type: " + requestType << endl;
				// handle Post, Read, Count
				// return new request each time
				if (requestType == "POST" || requestType == "READ" || requestType == "COUNT")
				{
						requestType = requestType.substr(0, 1);
						if (requestType == "C") {
								// request newRequest
								Request newRequest(requestType, request.substr(6, request.size()), "");
								return newRequest;
						}
						else {
								// POST and READ
								// // possible need for separation as these are getting confused a bit
								// review LATER
								// these 2 are getting mixed up? possible need for a swap
								int postId = 0;
								const std::string topicId = request.substr(5, request.find("#") - 5);
								const std::string message = request.substr(6 + topicId.size(), request.size());
								// do we need one of these for postId?
								// as post and READ will always have one?or will they
								// // READ deffo will
								// do topic lookup here with the postId - it shouldn't always be zero
								// may need a set method
								if (requestType == "P") {
										// postId lookup here - maybe - instead of in main?
										Request newRequest(requestType, topicId, message, postId);
										return newRequest;
								}
								// cout << "POST ID is:" + to_string(postId) + " Message is: " + message << endl;
								// the code needed to find the message of a POST request and the postId of a READ request is the same, and hence we can re-use it here
								postId = stoi(message);
								//cout << "The message is " << message << " and the topicId is " << topicId << endl;
								Request newRequest(requestType, topicId, message, postId);
								return newRequest;
						}
				}
		}
		else if (request.find("LIST") != std::string::npos) {
				// substr alternatives
				//cout << "This is a LIST request" << endl;
				std::string requestType = request.substr(0, 1);
				Request newRequest(requestType);
				return newRequest;
		}
		Request invalid;
		invalid.valid = false;
		return invalid;
}

std::string Request::toString()
{
		// replace POST with string type
		// may need selection here
		return std::string(requestType) + topicId + std::string("#") + message;
}

void Request::setValid(Request request, bool valid)
{
		request.valid = valid;
}
