#include <string>
#include <rapidjson/document.h>
#include <optional>
#include <unordered_map>
#include <vector>
#include <functional>


enum MessageType {
    INIT,
    ECHO
};

struct MessageBody {
    std::string type;
    std::optional<int> messageId;
    std::optional<int> inReplyTo;
    std::optional<std::string> echo;
};

struct Message {
    std::string src;
    std::string dest;
    MessageBody body;
};

using Handler = std::function<std::vector<Message>(rapidjson::Document& document)>;

class Node {
    public:
        Node() = default;
        ~Node() = default;
        void run();

        void initialize_handler(const Handler& handler, const std::initializer_list<std::string> message_types);

        // parsers
        void parseSrcAndDest(Message& message, rapidjson::Document& document) const;
        void parseMessageTypeAndId(Message& message, rapidjson::Document& document) const;
        void parseEcho(Message &message, rapidjson::Document &document) const;

        std::string getTypeFromDocument(rapidjson::Document& document) const;
    private:

        const std::string& get_id() const;
        void set_id(const std::string id);

        std::vector<Message> initHandlerFunc(rapidjson::Document& document);

        // utilities
        rapidjson::Document createDocument(const Message& message) const; 
        void respond(rapidjson::Document& document) const;

        std::string id;

        enum STATE {
            WAITING_FOR_INIT,
            TAKING_MESSAGES
        };

        STATE currentState = STATE::WAITING_FOR_INIT;

        const std::string MESSAGE_PARSE_ERROR = "INCORRECT MESSAGE BODY";

        std::unordered_map<std::string, Handler> map;
};