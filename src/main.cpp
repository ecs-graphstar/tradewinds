#include <flecs.h>
#include <zmq.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <tradewinds.h>

using namespace tradewinds;

void embedding_response(std::map<std::string, msgpack::object>& res_map)
{
    std::string status = res_map["status"].as<std::string>();
    std::cout << status << std::endl;   
}

int main(int argc, char *argv[]) {

    flecs::world ecs;
    ecs.import<tradewinds::module>();
    // Flecs can be used with both ZMQ servers and clients

    ecs.set<ZMQContext>({ std::make_shared<zmq::context_t>(2) });

    // ecs.entity("PrimaryServer")
    //     .set<ZMQServer>({ "tcp://*:5555", zmq::socket_type::rep });

    auto pub = ecs.entity("GamePub")
        .set<ZMQServer>({ "tcp://*:5555", zmq::socket_type::pub });

    ecs.entity("GameSub")
        .set<ZMQClient>({ "tcp://localhost:5555", zmq::socket_type::sub })
        .set<ZMQSubscriber>({ "" }); // Subscribe to everything

    // flecs::entity client = ecs.entity("PrimaryClient")
    //     .set<ZMQClient>({ "ipc:///tmp/minilm_flecs_socket", zmq::socket_type::req });

    std::cout << "ECS World running. Listening on port 5555..." << std::endl;

    int frame_count = 0;
    while (ecs.progress()) {
        if (++frame_count % 100000 == 0) {
            pub.set<PublishedMessage>({ "news", "Hello from Flecs!" });
        }
    }
    
    return 0;
}