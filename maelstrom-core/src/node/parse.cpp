#include <node.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <iostream>


void Node::parseSrcAndDest(Message &message, rapidjson::Document &document) const
{

    if (document.HasMember("src") && document["src"].IsString())
    {
        message.src = document["src"].GetString();
        std::cerr << message.src << "\n";
    }
    else
    {
        throw std::runtime_error(MESSAGE_PARSE_ERROR);
    }

    if (document.HasMember("dest") && document["dest"].IsString())
    {
        message.dest = document["dest"].GetString();
    }
    else
    {
        throw std::runtime_error(MESSAGE_PARSE_ERROR);
    }
}

void Node::parseMessageTypeAndId(Message &message, rapidjson::Document &document) const
{

    if (document.HasMember("body") && document["body"].IsObject())
    {
        const auto &body = document["body"].GetObject();

        if (body.HasMember("type") && body["type"].IsString())
        {
            message.body.type = body["type"].GetString();
        }
        else
        {
            throw std::runtime_error(MESSAGE_PARSE_ERROR);
        }

        if (body.HasMember("msg_id") && body["msg_id"].IsInt())
        {
            message.body.messageId = body["msg_id"].GetInt();
        }

        if (body.HasMember("in_reply_to") && body["in_reply_to"].IsInt()) {
            message.body.inReplyTo = body["in_reply_to"].GetInt();
        }
    }
    else
    {
        throw std::runtime_error(MESSAGE_PARSE_ERROR);
    }
}

void Node::parseEcho(Message &message, rapidjson::Document &document) const
{

    if (document.HasMember("body") && document["body"].IsObject())
    {
        const auto &body = document["body"].GetObject();

        if (body.HasMember("echo") && body["echo"].IsString())
        {
            message.body.echo = body["echo"].GetString();
        }
        else
        {
            throw std::runtime_error(MESSAGE_PARSE_ERROR);
        }
    }
    else
    {
        throw std::runtime_error(MESSAGE_PARSE_ERROR);
    }
}

std::string Node::getTypeFromDocument(rapidjson::Document& document) const {
    if (document.HasMember("body") && document["body"].IsObject())
    {
        const auto &body = document["body"].GetObject();

        if (body.HasMember("type") && body["type"].IsString())
        {
            return body["type"].GetString();
        }
    }
}
