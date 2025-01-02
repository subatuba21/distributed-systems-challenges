#include "node.hpp"
#include "optional"
#include "vector"
#include "parsers.hpp"
#include <rapidjson/document.h>


namespace maelstrom {

    void parseEcho(Message &message, rapidjson::Document &document) {

        if (document.HasMember("body") && document["body"].IsObject())
        {
            const auto &body = document["body"].GetObject();

            if (body.HasMember("echo") && body["echo"].IsString())
            {
                message.body.echo = body["echo"].GetString();
            }
            else
            {
                throw std::runtime_error(maelstrom::MESSAGE_PARSE_ERROR);
            }
        }
        else
        {
            throw std::runtime_error(maelstrom::MESSAGE_PARSE_ERROR);
        }
    }

}

int main()
{

    using namespace maelstrom;
    Node node{};

    Handler echoHandler = [node](rapidjson::Document &document)
    {
        Message message;
        message.parseJSON(document);
        parseEcho(message, document);
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
