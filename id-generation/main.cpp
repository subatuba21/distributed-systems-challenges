#include "node.hpp"
#include "optional"
#include "vector"
#include "parsers.hpp"

#include <random>
#include <string>

inline std::string generateRandomID(std::size_t length = 16)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    // A random_device can be expensive or limited on some platforms;
    // for higher performance, you could seed your engine once per program run.
    std::random_device rd;  
    std::mt19937 generator(rd());  
    // Uniform distribution range: [0, number_of_characters - 1]
    std::uniform_int_distribution<std::size_t> dist(0, sizeof(alphanum) - 2);

    std::string result;
    result.reserve(length);
    for (std::size_t i = 0; i < length; ++i)
    {
        result.push_back(alphanum[dist(generator)]);
    }
    return result;
}


int main()
{

    using namespace maelstrom;
    Node node{};

    Handler idhandler = [node](rapidjson::Document &document)
    {
        Message message;
        message.parseJSON(document);
        std::swap(message.src, message.dest);
        message.body.inReplyTo = message.body.messageId;
        message.body.type = "generate_ok";
        message.body.messageId.reset();
        message.body.id = generateRandomID();
        std::vector<Message> responses = {message};
        return responses;
    };

    node.initialize_handler(idhandler, {"generate"});
    node.run();
    return 0;
}
