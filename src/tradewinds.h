// Tradewinds is a module for flecs that supports networking with ZeroMQ in flecs
// through data oriented component observers and callbacks

#ifndef TRADEWINDS_H
#define TRADEWINDS_H

#include <flecs.h>
#include <zmq.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <unistd.h>
#include <fcntl.h>
#include <functional>
#include <map>

#include <msgpack.hpp>

#ifdef __cplusplus
extern "C" {
#endif

namespace tradewinds {

struct ZMQContext { 
    std::shared_ptr<zmq::context_t> ctx; 
};

struct ZMQClient {
    std::string addr;
    zmq::socket_type type;
    std::unique_ptr<zmq::socket_t> socket;
};
struct ZMQServer {
    std::string addr;
    zmq::socket_type type;
    std::unique_ptr<zmq::socket_t> socket;
};

struct ZMQSubscriber {
    std::string topic;
};

struct PublishedMessage {
    std::string topic;
    std::string payload;
};

struct AwaitResponse
{
    std::function<void(std::map<std::string, msgpack::object>&)> response_function;
};

struct SendMapRequest 
{
    std::map<std::string, std::string> data;
};

struct module {
    module(flecs::world& world);
};

}

#ifdef __cplusplus
}
#endif

#endif

