#include "optional"
#include "vector"
#include "set"
#include "parsers.hpp"
#include "broadcasttypes.hpp"

#include <random>
#include <string>
#include <list>


int main()
{
    using namespace maelstrom;
    Node node{};

    std::set<int> saved;
    std::vector<std::string> neighbors;
    int messageIdCounter = 0;
    int messageCounter = 0;

    std::list<std::shared_ptr<broadcastMessage>> retryList;
    std::unordered_map<int, std::list<std::shared_ptr<broadcastMessage>>::iterator> retryMap;

    Handler broadcasthandler = [&saved, &node, &neighbors, &messageIdCounter, &retryList, &retryMap](rapidjson::Document &document)
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
        std::vector<std::shared_ptr<maelstrom::Message>> responses;
        responses.emplace_back(std::make_shared<maelstrom::Message>(message));

        if (alreadySeen) {
            // no need to propagate
            return responses;
        }


        for (auto& neighbor : neighbors) {
            auto newMessage = std::make_shared<broadcastMessage>();
            newMessage->src = node.get_id();
            newMessage->dest = neighbor;
            newMessage->body.messageId = messageIdCounter;
            newMessage->body.type = "broadcast";
            
            newMessage->message = messageInt;

            // add to retry
            retryList.push_back(newMessage);
            retryMap[messageIdCounter] = std::prev(retryList.end());

            responses.push_back(newMessage);

            messageIdCounter++;
        }


        // messages to neighbor nodes
        return responses;
    };

    Handler readHandler = [&saved, &node](rapidjson::Document &document)
    {
        auto message = std::make_shared<readMessage>();
        message->parseJSON(document);
        std::swap(message->src, message->dest);
        message->body.inReplyTo = message->body.messageId;
        message->body.type = "read_ok";
        message->body.messageId.reset();
        message->messages = std::vector<int>(saved.begin(), saved.end());
        std::vector<std::shared_ptr<maelstrom::Message>> responses;
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
        
        std::vector<std::shared_ptr<maelstrom::Message>> responses;
        responses.emplace_back(std::make_shared<maelstrom::Message>(message));
        return responses;
    };

    Handler broadcastOkHandler = [&messageCounter, &retryMap, &retryList](rapidjson::Document &document) {
        Message broadCastOkayMessage;
        broadCastOkayMessage.parseJSON(document);

        if (broadCastOkayMessage.body.inReplyTo.has_value() && retryMap.count(broadCastOkayMessage.body.inReplyTo.value()) != 0) {
            retryList.erase(retryMap[broadCastOkayMessage.body.inReplyTo.value()]);
            retryMap.erase(broadCastOkayMessage.body.inReplyTo.value());
        }

        std::cerr << "broadcast_OK YAY";

        
        return std::vector<std::shared_ptr<maelstrom::Message>>{};
    };


    // Handler retryHandler = [&messageCounter](rapidjson::Document &document) {
    //     messageCounter += 1;


    // };

    node.initialize_handler(broadcasthandler, {"broadcast"});
    node.initialize_handler(topologyHandler, {"topology"});
    node.initialize_handler(readHandler, {"read"});
    node.initialize_handler(broadcastOkHandler, {"broadcast_ok"});

    node.run();
    return 0;
}
