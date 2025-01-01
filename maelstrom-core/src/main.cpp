#include "node.hpp"
#include "optional"
#include "vector"



int main() {

    Node node{};

    Handler echoHandler = [node](rapidjson::Document& document) {
        Message message;
        node.parseMessageTypeAndId(message, document);
        node.parseSrcAndDest(message, document);
        node.parseEcho(message, document);
        std::swap(message.src, message.dest);
        message.body.inReplyTo = message.body.messageId;
        message.body.type = "echo_ok";
        message.body.messageId.reset();
        std::vector<Message> responses = {message};
        return responses;
    };

    node.initialize_handler(echoHandler, {"echo"});
    node.run();
    return 0;
}

