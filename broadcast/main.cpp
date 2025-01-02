#include "node.hpp"
#include "optional"
#include "vector"
#include "parsers.hpp"

#include <random>
#include <string>




int main()
{
    using namespace maelstrom;
    Node node{};

    Handler broadcasthandler = [node](rapidjson::Document &document)
    {
        Message message;
        parsers::parseMessageTypeAndId(message, document);
        parsers::parseSrcAndDest(message, document);
        std::swap(message.src, message.dest);
        message.body.inReplyTo = message.body.messageId;
        message.body.type = "broadcast_ok";
        message.body.messageId.reset();
        std::vector<Message> responses = {message};
        return responses;
    };

    node.initialize_handler(handler, {"broadcast"});
    node.run();
    return 0;
}
