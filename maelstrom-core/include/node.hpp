#include <string>
#include <rapidjson/document.h>
#include <optional>
#include <unordered_map>
#include <vector>
#include <functional>
#include <iostream>

namespace maelstrom
{
    enum MessageType
    {
        INIT,
        ECHO
    };

    struct Message;
    struct MessageBody;

    static void parseMessageTypeAndId(MessageBody &message, rapidjson::Document &document);
    static void parseSrcAndDest(Message &message, rapidjson::Document &document);

    struct MessageBody
    {
        std::string type;
        std::optional<int> messageId;
        std::optional<int> inReplyTo;
        std::optional<std::string> echo;
        std::optional<std::string> id;

        void addMessageBody(rapidjson::Document &document) const
        {
            document.AddMember("body", rapidjson::Value(rapidjson::kObjectType), document.GetAllocator());
            rapidjson::Value &body = document["body"];

            rapidjson::Value type;
            type.SetString(this->type.c_str(), document.GetAllocator());
            body.AddMember("type", type, document.GetAllocator());

            if (this->messageId.has_value())
            {
                rapidjson::Value msg_id;
                msg_id.SetInt(this->messageId.value());
                body.AddMember("msg_id", msg_id, document.GetAllocator());
            }

            if (this->inReplyTo.has_value())
            {
                rapidjson::Value in_reply_to;
                in_reply_to.SetInt(this->inReplyTo.value());
                body.AddMember("in_reply_to", in_reply_to, document.GetAllocator());
            }

            if (this->echo.has_value())
            {
                rapidjson::Value echo;
                echo.SetString(this->echo.value().c_str(), document.GetAllocator());
                body.AddMember("echo", echo, document.GetAllocator());
            }

            if (this->id.has_value())
            {
                rapidjson::Value id;
                id.SetString(this->id.value().c_str(), document.GetAllocator());
                body.AddMember("id", id, document.GetAllocator());
            }
        };

        void parseMessageBody(rapidjson::Document &document)
        {
            parseMessageTypeAndId(*this, document);
        }
    };

    struct Message
    {
        std::string src;
        std::string dest;
        MessageBody body;

        virtual rapidjson::Document toJSON() const
        {
            rapidjson::Document document;
            document.SetObject();

            rapidjson::Value src;
            src.SetString(this->src.c_str(), document.GetAllocator());
            document.AddMember("src", src, document.GetAllocator());

            rapidjson::Value dest;
            dest.SetString(this->dest.c_str(), document.GetAllocator());
            document.AddMember("dest", dest, document.GetAllocator());

            this->body.addMessageBody(document);

            return document;
        };

        virtual void parseJSON(rapidjson::Document &document)
        {
            parseSrcAndDest(*this, document);
            body.parseMessageBody(document);
        }
    };

    static const std::string MESSAGE_PARSE_ERROR = "ERROR";
    static void parseMessageTypeAndId(MessageBody &message, rapidjson::Document &document)
    {

        if (document.HasMember("body") && document["body"].IsObject())
        {
            const auto &body = document["body"].GetObject();

            if (body.HasMember("type") && body["type"].IsString())
            {
                message.type = body["type"].GetString();
            }
            else
            {
                throw std::runtime_error(maelstrom::MESSAGE_PARSE_ERROR);
            }

            if (body.HasMember("msg_id") && body["msg_id"].IsInt())
            {
                message.messageId = body["msg_id"].GetInt();
            }

            if (body.HasMember("in_reply_to") && body["in_reply_to"].IsInt())
            {
                message.inReplyTo = body["in_reply_to"].GetInt();
            }
        }
        else
        {
            throw std::runtime_error(maelstrom::MESSAGE_PARSE_ERROR);
        }
    }

    static void parseSrcAndDest(Message &message, rapidjson::Document &document)
    {

        if (document.HasMember("src") && document["src"].IsString())
        {
            message.src = document["src"].GetString();
            std::cerr << message.src << "\n";
        }
        else
        {
            throw std::runtime_error(maelstrom::MESSAGE_PARSE_ERROR);
        }

        if (document.HasMember("dest") && document["dest"].IsString())
        {
            message.dest = document["dest"].GetString();
        }
        else
        {
            throw std::runtime_error(maelstrom::MESSAGE_PARSE_ERROR);
        }
    }

    using Handler = std::function<std::vector<std::unique_ptr<Message>>(rapidjson::Document &document)>;

    class Node
    {
    public:
        Node() = default;
        ~Node() = default;
        void run();

        void initialize_handler(const Handler &handler, const std::initializer_list<std::string> message_types);

    private:
        const std::string &get_id() const;
        void set_id(const std::string id);

        std::vector<std::unique_ptr<Message>> initHandlerFunc(rapidjson::Document &document);

        void respond(rapidjson::Document &document) const;

        std::string id;

        enum STATE
        {
            WAITING_FOR_INIT,
            TAKING_MESSAGES
        };

        STATE currentState = STATE::WAITING_FOR_INIT;
        std::unordered_map<std::string, Handler> map;
    };
}