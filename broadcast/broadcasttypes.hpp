#include "node.hpp"
#include "iostream"

namespace maelstrom
{

    struct broadcastMessage : Message
    {

        std::string message;

        void parseJSON(rapidjson::Document &document) override
        {
            Message::parseJSON(document);

            if (document.HasMember("body") && document["body"].IsObject())
            {
                const auto &body = document["body"].GetObject();
                if (body.HasMember("message") && body["message"].IsString())
                {
                    message = body["message"].GetString();
                }
            }
        }
    };

    struct readMessage : Message
    {

        std::vector<int> messages;

        rapidjson::Document toJSON() const override
        {
            rapidjson::Document document = Message::toJSON();

            std::cerr << "here1001" << "\n";

            if (document.HasMember("body") && document["body"].IsObject())
            {

                std::cerr << "here1111" << "\n";

                rapidjson::Value messagesVal;
                messagesVal.SetArray();
                for (int value : messages)
                {
                    messagesVal.PushBack(value, document.GetAllocator());
                }

                document["body"].AddMember("messages", messagesVal, document.GetAllocator());
            }

            return document;
        }
    };

    struct topologyMessage : Message
    {
        std::unordered_map<std::string, std::vector<std::string>> topology;

        void parseJSON(rapidjson::Document &document) override
        {
            Message::parseJSON(document);

            if (document.HasMember("body") && document["body"].IsObject())
            {
                const auto &body = document["body"].GetObject();
                if (body.HasMember("topology") && body["topology"].IsObject())
                {
                    const auto &topologyObj = body["topology"].GetObject();
                    for (auto it = topologyObj.MemberBegin(); it != topologyObj.MemberEnd(); ++it) {
                        std::string key = it->name.GetString();
                        std::vector<std::string> neighbors;

                        if (it->value.IsArray()) {
                            for (const auto& neighbor : it->value.GetArray()) {
                                neighbors.push_back(neighbor.GetString());
                            }
                        }

                        topology[key] = neighbors;
                    }
                }
            }
        }
    };

}