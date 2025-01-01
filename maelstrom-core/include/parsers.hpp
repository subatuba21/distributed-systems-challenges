#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace maelstrom
{
    namespace parsers
    {
        void parseSrcAndDest(Message &message, rapidjson::Document &document);
        void parseMessageTypeAndId(Message &message, rapidjson::Document &document);
        void parseEcho(Message &message, rapidjson::Document &document);
        std::string getTypeFromDocument(rapidjson::Document &document);
    }
}