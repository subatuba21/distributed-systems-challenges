#include "node.hpp"
#include "parsers.hpp"

namespace maelstrom
{

    void Node::initialize_handler(const Handler &handler, const std::initializer_list<std::string> message_types)
    {
        for (const std::string &str : message_types)
        {

            // has to be a new message type
            if (this->map.find(str) == this->map.end())
            {
                this->map[str] = handler;
            }
        }
    }

    std::vector<std::unique_ptr<Message>> Node::initHandlerFunc(rapidjson::Document &document)
    {

        Message message;
        message.parseJSON(document);

        std::swap(message.src, message.dest);
        message.body.inReplyTo = message.body.messageId;
        message.body.type = "init_ok";
        message.body.messageId.reset();

        this->currentState = STATE::TAKING_MESSAGES;
        std::vector<std::unique_ptr<maelstrom::Message>> responses;
        responses.emplace_back(std::make_unique<maelstrom::Message>(message));
        return responses;

    }

}