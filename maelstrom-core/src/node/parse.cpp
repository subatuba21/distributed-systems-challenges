#include <node.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <iostream>
#include "parsers.hpp"

namespace maelstrom
{
    namespace parsers
    {
        const std::string MESSAGE_PARSE_ERROR = "INCORRECT MESSAGE BODY";

        std::string getTypeFromDocument(rapidjson::Document &document)
        {
            if (document.HasMember("body") && document["body"].IsObject())
            {
                const auto &body = document["body"].GetObject();

                if (body.HasMember("type") && body["type"].IsString())
                {
                    return body["type"].GetString();
                }
            }
        }
    }
}
