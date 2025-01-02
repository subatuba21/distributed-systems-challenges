#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace maelstrom
{
    namespace parsers
    {
        std::string getTypeFromDocument(rapidjson::Document &document);
    }
}