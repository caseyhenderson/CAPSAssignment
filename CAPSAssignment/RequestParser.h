#ifndef __REQUESTPARSER_H
#define __REQUESTPARSER_H

#include <string>
// make a generic request type
class Request
{
public:
		Request();
		Request(std::string requestType);
		Request(std::string requestType, std::string topicId, std::string message);
		Request(std::string requestType, std:: string topicId, std::string message, int postId);
		~Request();
		static Request parse(std::string request);
		std::string getTopicId() { return this->topicId; }
		std::string getMessage() { return this->message; }
		std::string toString();
		std::string getRequestType() {
				return this->requestType;
		}
		int getPostId() {
				return this->postId;}
		void setPostId(int givenPostId) {
				 this->postId = givenPostId;
		};
		void setValid(Request request, bool valid);
		std::string topicId;
		std::string message;
		std::string requestType;
		int postId;
		// check if valid is being used
		// next stage is to take the setters to inline as seen above with the getters.
		bool valid;
};

//class PostRequest
//{
//public:
//		PostRequest();
//		~PostRequest();
//		static PostRequest parse(std::string request);
//		std::string getTopicId();
//		std::string getMessage();
//		std::string toString();
//		int getPostId();
//		std::string topicId;
//		std::string message;
//		int postId;
//		bool valid;
//};
//
//class ReadRequest
//{
//public:
//		ReadRequest();
//		~ReadRequest();
//		static ReadRequest parse(std::string request);
//		std::string getTopicId();
//		int getPostId();
//		std::string toString();
//		std::string topicId;
//		int postId;
//		bool valid;
//};
//
//class CountRequest
//{
//public:
//		CountRequest();
//		~CountRequest();
//		static CountRequest parse(std::string request);
//		std::string getTopicId();
//		std::string toString();
//
//		std::string topicId;
//		bool valid;
//};
//
//class ListRequest
//{
//public:
//		ListRequest();
//		~ListRequest();
//		static ListRequest parse(std::string request);
//		std::string toString();
//		bool valid;
//};

class ExitRequest
{
public:
		ExitRequest();
		~ExitRequest();
		static ExitRequest parse(std::string request);
		std::string toString();
		bool valid;
};

#endif //__REQUESTPARSER_H