#include <string>
#include <rapidjson/document.h>
#include <optional>


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

using Handler = std::function<std::vector<std::unique_ptr<Message>>(std::shared_ptr<Message>&)>;

class Node {
    public:
        Node() = default;
        ~Node() = default;
        void run();
        const std::string& get_id() const;
        void set_id(const std::string id);
        void initialize_handler(const Handler& handler, const std::initializer_list<const std::string&> message_types);
    private:
        
        void parseSrcAndDest(Message& message, rapidjson::Document& document) const;
        void parseMessageTypeAndId(Message& message, rapidjson::Document& document) const;
        rapidjson::Document createDocument(Message& message) const; 
        void respond(rapidjson::Document& document) const;

        std::string id;

        enum STATE {
            WAITING_FOR_INIT,
            TAKING_MESSAGES
        };

        STATE currentState = STATE::WAITING_FOR_INIT;

        const std::string MESSAGE_PARSE_ERROR = "INCORRECT MESSAGE BODY";


};