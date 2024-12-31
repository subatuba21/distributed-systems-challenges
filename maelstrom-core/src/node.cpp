#include <node.hpp>
#include <cstdio>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

const std::string& Node::get_id() const {
    return id;
}

void Node::set_id(const std::string id) {
    this->id = id;
}

void Node::parseSrcAndDest(Message& message, rapidjson::Document& document) const
{

    if (document.HasMember("src") && document["src"].IsString()) {
        message.src = document["src"].GetString();
    } else {
        throw std::runtime_error(MESSAGE_PARSE_ERROR); 
    }

    if (document.HasMember("dest") && document["dest"].IsString()) {
        message.dest = document["dest"].GetString();
    } else {
        throw std::runtime_error(MESSAGE_PARSE_ERROR); 
    }
}

void Node::parseMessageTypeAndId(Message& message, rapidjson::Document& document) const {

    if (document.HasMember("body") && document["body"].IsObject()) {
        const auto& body = document["body"].GetObject();

        if (body.HasMember("type") && body["type"].IsString()) {
            message.body.type = body["type"].GetString();
        } else {
            throw std::runtime_error(MESSAGE_PARSE_ERROR);
        }

        if (body.HasMember("type") && body["type"].IsString()) {
            message.body.type = body["type"].GetString();
        } else {
            throw std::runtime_error(MESSAGE_PARSE_ERROR);
        }
        
    } else {
        throw std::runtime_error(MESSAGE_PARSE_ERROR);
    }
}

void Node::respond(rapidjson::Document &document) const
{
    // Serialize and output the response
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::cout << buffer.GetString() << std::endl;
}

void Node::run() {
    std::cerr << "Node has started.";
    std::string inputLine;
    while (true) {
        std::getline(std::cin, inputLine);
        std::cerr << inputLine + "\n";
        rapidjson::Document document;
        Message message;
        parseMessageTypeAndId(message, document);
        parseSrcAndDest(message, document);
        if (message.body.type == "init") {

        } else {

        }
    }
}