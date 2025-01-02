#include "optional"
#include "vector"
#include "parsers.hpp"
#include "broadcasttypes.hpp"

#include <random>
#include <string>


int main()
{
    using namespace maelstrom;
    Node node{};

    std::vector<int> saved;

    Handler broadcasthandler = [&saved, node](rapidjson::Document &document)
    {
        broadcastMessage message;
        message.parseJSON(document);

        saved.push_back(message.message);

        std::swap(message.src, message.dest);
        message.body.inReplyTo = message.body.messageId;
        message.body.type = "broadcast_ok";
        message.body.messageId.reset();
        std::vector<std::unique_ptr<maelstrom::Message>> responses;
        responses.emplace_back(std::make_unique<maelstrom::Message>(message));
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
        message->messages = saved;
        std::vector<std::unique_ptr<maelstrom::Message>> responses;
        responses.emplace_back(std::move(message));
        return responses;
    };

    Handler topologyHandler = [node](rapidjson::Document &document)
    {
        topologyMessage message;
        message.parseJSON(document);
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
