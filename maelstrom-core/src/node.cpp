#include <node.hpp>
#include <cstdio>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

const std::string &Node::get_id() const
{
    return id;
}

void Node::set_id(const std::string id)
{
    this->id = id;
}

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

rapidjson::Document Node::createDocument(Message &message) const
{
    rapidjson::Document document;
    document.SetObject();

    rapidjson::Value src;
    src.SetString(message.src.c_str(), document.GetAllocator());
    document.AddMember("src", src, document.GetAllocator());

    rapidjson::Value dest;
    dest.SetString(message.dest.c_str(), document.GetAllocator());
    document.AddMember("dest", dest, document.GetAllocator());

    document.AddMember("body", rapidjson::Value(rapidjson::kObjectType), document.GetAllocator());
    rapidjson::Value &body = document["body"];

    rapidjson::Value type;
    type.SetString(message.body.type.c_str(), document.GetAllocator());
    body.AddMember("type", type, document.GetAllocator());

    if (message.body.messageId.has_value())
    {
        rapidjson::Value msg_id;
        msg_id.SetInt(message.body.messageId.value());
        body.AddMember("msg_id", msg_id, document.GetAllocator());
    }

    if (message.body.inReplyTo.has_value())
    {
        rapidjson::Value in_reply_to;
        in_reply_to.SetInt(message.body.inReplyTo.value());
        body.AddMember("in_reply_to", in_reply_to, document.GetAllocator());
    }

    if (message.body.echo.has_value())
    {
        rapidjson::Value echo;
        echo.SetString(message.body.echo.value().c_str(), document.GetAllocator());
        body.AddMember("echo", echo, document.GetAllocator());
    }

    return document;
}

void Node::respond(rapidjson::Document &document) const
{
    // Serialize and output the response
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::cout << buffer.GetString() << std::endl;
}

void Node::run()
{
    std::cerr << "Node has started.";
    std::string inputLine;
    while (true)
    {
        std::getline(std::cin, inputLine);
        std::cerr << "input " << inputLine + "\n";
        if (inputLine.empty())
        {
            break;
        }

        rapidjson::Document document;
        Message message;
        if (document.Parse(inputLine.c_str()).HasParseError())
        {
            std::cerr << ("Incorrect JSON \n");
        }

        parseMessageTypeAndId(message, document);
        parseSrcAndDest(message, document);
        if (message.body.type == "init")
        {
            std::swap(message.src, message.dest);
            message.body.inReplyTo = message.body.messageId;
            message.body.type = "init_ok";
            message.body.messageId.reset();
            rapidjson::Document doc = createDocument(message);
            respond(doc);
        } else if (message.body.type == "echo") {
            parseEcho(message, document);
            std::swap(message.src, message.dest);
            message.body.inReplyTo = message.body.messageId;
            message.body.type = "echo_ok";
            message.body.messageId.reset();
            rapidjson::Document doc = createDocument(message);
            respond(doc);
        }
    }
}