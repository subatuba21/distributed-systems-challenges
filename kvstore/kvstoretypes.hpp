#include "node.hpp"
#include <vector>
#include <string>
#include "rapidjson/document.h"

namespace maelstrom
{

    struct operation
    {
        enum type
        {
            READ,
            WRITE
        };

        type op_type;
        int key;
        int value;

        operation(type op_type, int key, int value = 0)
            : op_type(op_type), key(key), value(value) {}
    };

    struct txnMessage : Message
    {
        std::vector<operation> operations;

        void parseJSON(rapidjson::Document &document) override
        {
            if (document.HasMember("body") && document["body"].IsObject())
            {
                const auto &body = document["body"].GetObject();
                if (body.HasMember("txn") && body["txn"].IsArray())
                {
                    const auto &txn = body["txn"].GetArray();
                    for (const auto &item : txn)
                    {
                        if (item.IsArray() && item.Size() >= 3)
                        {
                            std::string op_type_str = item[0].GetString();
                            int key = item[1].GetInt();
                            int value = item[2].IsNull() ? 0 : item[2].GetInt();

                            if (op_type_str == "r")
                            {
                                operations.emplace_back(operation::READ, key);
                            }
                            else if (op_type_str == "w")
                            {
                                operations.emplace_back(operation::WRITE, key, value);
                            }
                        }
                    }
                }
            }
        }

        rapidjson::Document toJSON() const override
        {
            rapidjson::Document document = Message::toJSON();

            if (document.HasMember("body") && document["body"].IsObject())
            {
                const auto &body = document["body"].GetObject();

                rapidjson::Value vxn;
                vxn.SetArray();

                for (const auto& operation : operations) {
                    rapidjson::Value jsonOperation;
                    jsonOperation.SetArray();

                    rapidjson::Value typeValue;
                    if (operation.op_type == operation::type::READ) {
                        typeValue.SetString("r", document.GetAllocator());
                    } else if (operation.op_type == operation::type::WRITE) {
                        typeValue.SetString("w", document.GetAllocator());
                    }

                    jsonOperation.PushBack(typeValue, document.GetAllocator());

                    rapidjson::Value keyValue;
                    keyValue.SetInt(operation.key);

                    jsonOperation.PushBack(keyValue, document.GetAllocator());

                    rapidjson::Value valueValue;
                    valueValue.SetInt(operation.value);

                    jsonOperation.PushBack(valueValue, document.GetAllocator());
                }
                body.AddMember("vxn", vxn, document.GetAllocator());
            }

            return document;
        }
    };

}