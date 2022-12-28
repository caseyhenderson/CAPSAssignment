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
								const std::string topicId = request.substr(5, request.find("#") - 5);
								const std::string message = request.substr(6 + topicId.size(), request.size());
								// do topic lookup here with the postId - it shouldn't always be zero
								// may need a set method
								if (requestType == "P") {
										int postId = 0;
										// postId lookup here - maybe - instead of in main?
										Request newRequest(requestType, topicId, message, postId);
								}
								//cout << "The message is " << message << " and the topicId is " << topicId << endl;
								Request newRequest(requestType, topicId, message);
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

std::string Request::getTopicId()
{
		return topicId;
}

std::string Request::getMessage()
{
		return message;
}

int Request::getPostId()
{
		return postId;
}

void Request::setPostId(int givenPostId)
{
		postId = givenPostId;
}


std::string Request::toString()
{
		// replace POST with string type
		// may need selection here
		return std::string(requestType) + topicId + std::string("#") + message;
}

std::string Request::getRequestType()
{
		return requestType;
}

void Request::setValid(Request request, bool valid)
{
		request.valid = valid;
}


// can eventually remove all of the below in favour of the one generic 
// remove regex for big speed
PostRequest::PostRequest() : valid(0)
{ }

PostRequest::~PostRequest()
{ }

PostRequest PostRequest::parse(std::string request)
{
		std::regex postRegex("^POST(@[^@#]*)#(.+)$");
		std::smatch postMatch;
		PostRequest post;

		if (!std::regex_match(request, postMatch, postRegex, std::regex_constants::match_default))
		{
				post.valid = 0;
				return post;
		}

		post.topicId = postMatch[1];
		post.message = postMatch[2];
		post.valid = 1;

		return post;
}

std::string PostRequest::getTopicId()
{
		return topicId;
}

std::string PostRequest::getMessage()
{
		return message;
}

int PostRequest::getPostId()
{
		return postId;
}

std::string PostRequest::toString()
{
		return std::string("POST") + topicId + std::string("#") + message;
}

ReadRequest::ReadRequest() : valid(0)
{ }

ReadRequest::~ReadRequest()
{ }

ReadRequest ReadRequest::parse(std::string request)
{
		std::regex readRegex("^READ(@[^@#]*)#([0-9]+)$");
		std::smatch readMatch;
		ReadRequest read;

		if (!std::regex_match(request, readMatch, readRegex, std::regex_constants::match_default))
		{
				read.valid = 0;
				return read;
		}

		read.topicId = readMatch[1];
		read.postId = std::stoi(readMatch[2]);
		read.valid = 1;

		return read;
}

std::string ReadRequest::getTopicId()
{
		return topicId;
}

int ReadRequest::getPostId()
{
		return postId;
}

std::string ReadRequest::toString()
{
		return std::string("READ") + topicId + std::string("#") + std::to_string(postId);
}

CountRequest::CountRequest() : valid(0)
{ }

CountRequest::~CountRequest()
{ }

CountRequest CountRequest::parse(std::string request)
{
		std::regex countRegex("^COUNT(@[^@#]*)$");
		std::smatch countMatch;
		CountRequest count;

		if (!std::regex_match(request, countMatch, countRegex, std::regex_constants::match_default))
		{
				count.valid = 0;
				return count;
		}

		count.topicId = countMatch[1];
		count.valid = 1;

		return count;
}

std::string CountRequest::getTopicId()
{
		return topicId;
}

std::string CountRequest::toString()
{
		return std::string("COUNT") + topicId;
}

ListRequest::ListRequest() : valid(0)
{ }

ListRequest::~ListRequest()
{ }

ListRequest ListRequest::parse(std::string request)
{
		std::regex listRegex("^LIST$");
		std::smatch listMatch;
		ListRequest list;

		if (!std::regex_match(request, listMatch, listRegex, std::regex_constants::match_default))
		{
				list.valid = 0;
				return list;
		}

		list.valid = 1;

		return list;
}

std::string ListRequest::toString()
{
		return std::string("LIST");
}

ExitRequest::ExitRequest() : valid(0)
{ }

ExitRequest::~ExitRequest()
{ }

ExitRequest ExitRequest::parse(std::string request)
{
		std::regex exitRegex("^EXIT$");
		std::smatch exitMatch;
		ExitRequest exitReq;

		if (!std::regex_match(request, exitMatch, exitRegex, std::regex_constants::match_default))
		{
				exitReq.valid = 0;
				return exitReq;
		}

		exitReq.valid = 1;

		return exitReq;
}

std::string ExitRequest::toString()
{
		return std::string("EXIT");
}
