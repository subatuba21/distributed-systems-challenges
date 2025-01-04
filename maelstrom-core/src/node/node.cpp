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
        return this->id;
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

                std::vector<std::shared_ptr<Message>> responses;

                for (auto function : this->map[docType])
                {
                    auto funcResponses = function(document);
                    for (const auto &response : funcResponses)
                    {
                        responses.push_back(response);
                    }
                }

                for (const auto &response : responses)
                {
                    rapidjson::Document responseDoc = response->toJSON();
                    respond(responseDoc);
                }
            }
        }
    }
}