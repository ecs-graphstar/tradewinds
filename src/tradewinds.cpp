#include <tradewinds.h>

namespace tradewinds {

module::module(flecs::world& ecs) 
{
    ecs.module<module>();

    ecs.component<ZMQContext>();
    ecs.component<ZMQServer>();

    ecs.observer<ZMQServer>()
        .event(flecs::OnSet)
        .each([](flecs::entity e, ZMQServer& server) {
            // Prevent re-initialization if the socket already exists
            if (server.socket) return;

            auto ctx_comp = e.world().ensure<ZMQContext>();

            server.socket = std::make_unique<zmq::socket_t>(*(ctx_comp.ctx), server.type);
            server.socket->bind(server.addr);
        });

    ecs.observer<ZMQClient>()
        .event(flecs::OnSet)
        .each([](flecs::entity e, ZMQClient& server) {
            // Prevent re-initialization if the socket already exists
            if (server.socket) return;

            auto ctx_comp = e.world().ensure<ZMQContext>();

            server.socket = std::make_unique<zmq::socket_t>(*(ctx_comp.ctx), server.type);
            server.socket->connect(server.addr);
        });

    ecs.observer<ZMQClient, ZMQSubscriber>()
        .event(flecs::OnSet)
        .each([](flecs::entity e, ZMQClient& client, ZMQSubscriber& sub) {
            if (client.type == zmq::socket_type::sub) {
                client.socket->set(zmq::sockopt::subscribe, sub.topic);
                std::cout << "Subscribed to topic: " << sub.topic << std::endl;
            }
        });

    ecs.system<ZMQClient>("ZMQSubscriberSystem")
        .each([](flecs::entity e, ZMQClient& client) {
            if (client.type == zmq::socket_type::sub) {
                zmq::message_t topic_msg;
                
                // Non-blocking check for the first part (Topic)
                auto res = client.socket->recv(topic_msg, zmq::recv_flags::dontwait);
                
                if (res) {
                    std::string topic = topic_msg.to_string();
                    
                    // If there's more data (the payload), receive it
                    if (topic_msg.more()) {
                        zmq::message_t payload_msg;
                        client.socket->recv(payload_msg, zmq::recv_flags::none);
                        
                        std::cout << "[" << e.name() << "] Received on topic '" 
                                << topic << "': " << payload_msg.to_string() << std::endl;
                    } else {
                        std::cout << "[" << e.name() << "] Received topic only: " << topic << std::endl;
                    }
                }
            }
        });

    ecs.system<ZMQServer, const PublishedMessage>()
        .each([](flecs::entity e, ZMQServer& server, const PublishedMessage& msg) {
            if (server.type == zmq::socket_type::pub) {
                zmq::message_t topic_msg(msg.topic.size());
                memcpy(topic_msg.data(), msg.topic.data(), msg.topic.size());
                
                zmq::message_t payload_msg(msg.payload.size());
                memcpy(payload_msg.data(), msg.payload.data(), msg.payload.size());

                server.socket->send(topic_msg, zmq::send_flags::sndmore);
                server.socket->send(payload_msg, zmq::send_flags::none);

                e.remove<PublishedMessage>(); 
            }
        });

    ecs.observer<SendMapRequest, ZMQClient>()
        .event(flecs::OnSet)
        .each([](flecs::entity e, SendMapRequest& req, ZMQClient& client) {
            msgpack::sbuffer sbuf;
            msgpack::packer<msgpack::sbuffer> pk(&sbuf);

            pk.pack_map(req.data.size());
            for (const auto& [key, value] : req.data) {
                pk.pack(key);
                pk.pack(value);
            }

            zmq::message_t request(sbuf.data(), sbuf.size());
            auto result = client.socket->send(request, zmq::send_flags::dontwait);
            e.remove<SendMapRequest>();
            // TODO: AwaitRequestResult
            // if (result) {
            //     std::cout << "Request sent successfully." << std::endl;
            // }
        });

    ecs.system<AwaitResponse, ZMQClient>()
        .without<SendMapRequest>()
        .each([](flecs::entity e, AwaitResponse& onResponse, ZMQClient& client)
        {
            zmq::message_t reply;
            auto res = client.socket->recv(reply, zmq::recv_flags::dontwait);
            if (res) {
                msgpack::object_handle oh = msgpack::unpack((const char*)reply.data(), reply.size());
                msgpack::object obj = oh.get();
                auto res_map = obj.as<std::map<std::string, msgpack::object>>();
                onResponse.response_function(res_map);
                e.remove<AwaitResponse>();
            }
        });

    ecs.system<ZMQServer>("ZMQListenSystem")
        .each([](flecs::entity e, ZMQServer& server) {
            // Only attempt to receive if it's a REP (Reply) socket for this example
            if (server.type == zmq::socket_type::rep) {
                zmq::message_t request;
                
                // Use dontwait to ensure the ECS loop remains high-performance
                auto result = server.socket->recv(request, zmq::recv_flags::dontwait);

                if (result) {
                    std::cout << "[" << e.name() << "] Received: " << request.to_string() << std::endl;

                    // Simple Echo/Reply logic
                    std::string reply_str = "World";
                    zmq::message_t reply(reply_str.size());
                    memcpy(reply.data(), reply_str.data(), reply_str.size());
                    
                    server.socket->send(reply, zmq::send_flags::none);
                }
            }
        });

}

}