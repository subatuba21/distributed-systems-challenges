#include "optional"
#include "vector"
#include "set"
#include "parsers.hpp"
#include "broadcasttypes.hpp"

#include <random>
#include <string>


int main()
{
    using namespace maelstrom;
    Node node{};

    std::set<int> saved;
    std::vector<std::string> neighbors;
    int messageIdCounter = 0;

    Handler broadcasthandler = [&saved, &node, &neighbors, &messageIdCounter](rapidjson::Document &document)
    {
        broadcastMessage message;
        message.parseJSON(document);

        std::string src = message.src;
        std::string dest = message.dest;
        int messageInt = message.message;

        bool alreadySeen = saved.find(messageInt) != saved.end();
        saved.insert(messageInt);

        // response to client
        std::swap(message.src, message.dest);
        message.body.inReplyTo = message.body.messageId;
        message.body.type = "broadcast_ok";
        message.body.messageId.reset();
        std::vector<std::unique_ptr<maelstrom::Message>> responses;
        responses.emplace_back(std::make_unique<maelstrom::Message>(message));

        if (alreadySeen) {
            // no need to propagate
            return responses;
        }


        for (auto& neighbor : neighbors) {
            auto newMessage = std::make_unique<broadcastMessage>();
            newMessage->src = node.get_id();
            newMessage->dest = neighbor;
            newMessage->body.messageId = messageIdCounter;
            newMessage->body.type = "broadcast";
            messageIdCounter++;
            newMessage->message = messageInt;

            responses.emplace_back(std::move(newMessage));
        }


        // messages to neighbor nodes
        return responses;
    };

    Handler readHandler = [&saved, node](rapidjson::Document &document)
    {
        auto message = std::make_unique<readMessage>();
        message->parseJSON(document);
        std::swap(message->src, message->dest);
        message->body.inReplyTo = message->body.messageId;
        message->body.type = "read_ok";
        message->body.messageId.reset();
        message->messages = std::vector<int>(saved.begin(), saved.end());
        std::vector<std::unique_ptr<maelstrom::Message>> responses;
        responses.emplace_back(std::move(message));
        return responses;
    };

    Handler topologyHandler = [&neighbors, &node](rapidjson::Document &document)
    {
        topologyMessage message;
        message.parseJSON(document);

        for (auto& neighbor : message.topology[node.get_id()]) {
            neighbors.push_back(neighbor);
        }

        std::swap(message.src, message.dest);
        message.body.inReplyTo = message.body.messageId;
        message.body.type = "topology_ok";
        message.body.messageId.reset();
        
        std::vector<std::unique_ptr<maelstrom::Message>> responses;
        responses.emplace_back(std::make_unique<maelstrom::Message>(message));
        return responses;
    };

    node.initialize_handler(broadcasthandler, {"broadcast"});
    node.initialize_handler(topologyHandler, {"topology"});
    node.initialize_handler(readHandler, {"read"});

    node.run();
    return 0;
}
