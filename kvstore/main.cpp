#include "optional"
#include "vector"
#include "set"
#include "kvstoretypes.hpp"

#include <random>
#include <string>
#include <list>


int main()
{
    using namespace maelstrom;
    Node node{};

    std::unordered_map<int, int> store;

    Handler txnHandler = [&store](rapidjson::Document& document) {

        txnMessage message;
        message.parseJSON(document);


        auto newMessage = std::make_shared<txnMessage>();
        newMessage->src = message.dest;
        newMessage->dest = message.src;
        newMessage->body.inReplyTo = message.body.messageId;
        newMessage->body.type = "txn_ok";
        
        for (auto operation : message.operations) {
            if (operation.op_type == operation::READ) {
                if (store.find(operation.key) != store.end()) {
                    std::cerr << store[operation.key] << " FOUND \n";
                }
            } else if (operation.op_type == operation::WRITE) {
                store[operation.key] = operation.value;
            }

            newMessage->operations.push_back(operation);
        }

        std::vector<std::shared_ptr<Message>> responses;
        responses.push_back(newMessage);
        return responses;
    };

    node.initialize_handler(txnHandler, {"txn"});
    node.run();
}