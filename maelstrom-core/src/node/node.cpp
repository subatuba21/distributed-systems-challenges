#include <node.hpp>
#include <cstdio>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "parsers.hpp"

namespace maelstrom
{
    const std::string &Node::get_id() const
    {
        return id;
    }

    void Node::set_id(const std::string id)
    {
        this->id = id;
    }

    rapidjson::Document Node::createDocument(const Message &message) const
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

        if (message.body.id.has_value())
        {
            rapidjson::Value id;
            id.SetString(message.body.id.value().c_str(), document.GetAllocator());
            body.AddMember("id", id, document.GetAllocator());
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

        initialize_handler([this](rapidjson::Document &message)
                           { return this->initHandlerFunc(message); }, {"init"});

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
            if (document.Parse(inputLine.c_str()).HasParseError())
            {
                std::cerr << ("Incorrect JSON \n");
            }

            std::string docType = parsers::getTypeFromDocument(document);

            std::cerr << docType << std::endl;

            if (this->map.find(docType) != this->map.end())
            {

                if (this->currentState == STATE::WAITING_FOR_INIT && docType != "init")
                {
                    continue;
                }

                const auto &responses = this->map[docType](document);

                std::cerr << "found " << std::to_string(responses.size()) << std::endl;

                for (const auto &response : responses)
                {
                    rapidjson::Document responseDoc = createDocument(response);
                    respond(responseDoc);
                }
            }

            // if (message.body.type == "init")
            // {
            // std::swap(message.src, message.dest);
            // message.body.inReplyTo = message.body.messageId;
            // message.body.type = "init_ok";
            // message.body.messageId.reset();
            // rapidjson::Document doc = createDocument(message);
            // respond(doc);
            // } else if (message.body.type == "echo") {
            //     parseEcho(message, document);
            //     std::swap(message.src, message.dest);
            //     message.body.inReplyTo = message.body.messageId;
            //     message.body.type = "echo_ok";
            //     message.body.messageId.reset();
            //     rapidjson::Document doc = createDocument(message);
            //     respond(doc);
            // }
        }
    }
}