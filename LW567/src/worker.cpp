#include <worker.h>

#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <thread>
#include <zmq.hpp>
#include <cstdlib>
#include <sys/types.h>
#include <csignal>
#include <sys/wait.h>

void Worker(int id, int sockId) {
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    try {
        socket.bind("tcp://127.0.0.1:" + std::to_string(5555 + sockId));
    } catch (zmq::error_t&) {
        context.close();
        exit(-1);
    }

    while (true) {
        zmq::message_t request;
        if (socket.recv(request, zmq::recv_flags::none)) {
            std::string msg(static_cast<char*>(request.data()), request.size());

            std::istringstream iss(msg);
            std::string command;
            iss >> command;
            if (!std::strcmp(command.c_str(), "exec")) {
                int n, sum = 0;
                iss >> n;
                for (int i = 0; i < n; ++i) {
                    int num;
                    iss >> num;
                    sum += num;
                }
                std::ostringstream oss;
                oss << "Ok:" << id << ": " << sum;
                socket.send(zmq::buffer(oss.str()), zmq::send_flags::none);
            } else if (!std::strcmp(command.c_str(), "ping")) {
                socket.send(zmq::buffer("Ok"), zmq::send_flags::none);
            } else if (!std::strcmp(command.c_str(), "exit")) {
                socket.send(zmq::buffer("Ok"), zmq::send_flags::none);
                socket.close();
                context.close();
                exit(0);
            } else {
                socket.send(zmq::buffer("!" + command + "!"), zmq::send_flags::none);
            }
        }
    }
}