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
                    rapidjson::Document responseDoc = response.toJSON();
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