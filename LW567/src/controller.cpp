#include <controller.h>
#include <tools.h>
#include <worker.h>

#include <iostream>
#include <string>
#include <unordered_set>
#include <memory>
#include <sstream>
#include <thread>
#include <zmq.hpp>
#include <cstdlib>
#include <sys/types.h>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>
#include <unordered_map>
#include <future>
#include <vector>

std::string Exec(const std::shared_ptr<Node>& node, const int id, const std::string& oss) {
    std::string result;
    try {
        zmq::message_t message(oss);
        node->socket.send(message, zmq::send_flags::none);

        zmq::message_t reply;
        if (node->socket.recv(reply, zmq::recv_flags::none)) {
            result = reply.to_string() + "\n";
        } else {
            result = "Error:" + std::to_string(id) + ": Node is invaluable\n";
        }
    } catch (zmq::error_t&) {
        result = "Error:" + std::to_string(id) + ": Node is invaluable\n";
    } catch (std::exception& e) {
        result = "Error:" + std::to_string(id) + ": " + e.what() + "\n";
    }
    return result;
}

void Controller(std::istream &stream, bool test) {
    std::vector<std::future<std::string>> futures;

    while (true) {
        if (!test) {
            std::cout << "command> ";
            std::cout.flush();
        }
        std::string command;
        if (std::getline(stream, command)) {
            std::istringstream iss(command);

            std::string cmdType;
            iss >> cmdType;

            if (cmdType == "create") {
                int id;
                iss >> id;

                pid_t pid = fork();
                if (pid == 0) {
                    Worker(id);
                    exit(0);
                }

                if (!InsertNode(root, id, pid)) {
                    std::cout << "Error: Already exists\n";
                    kill(pid, SIGKILL);
                    continue;
                }
                std::cout << "Ok: " << pid << "\n";
            } else if (cmdType == "exec") {
                int id, n;
                iss >> id >> n;

                auto node = FindNode(root, id);
                if (!node) {
                    std::cout << "Error:" << id << ": Not found\n";
                    continue;
                }

                std::ostringstream oss;
                oss << "exec " << n;
                for (int i = 0; i < n; ++i) {
                    int num;
                    iss >> num;
                    oss << " " << num;
                }
                try {
                    futures.push_back(std::async(std::launch::async, Exec, node, id, oss.str()));
                } catch (std::exception& e) {
                    std::cout << "Error:" << id << ": " << e.what() << std::endl;
                }
            } else if (cmdType == "pingall") {
                std::unordered_set<int> unavailableNodes;

                PingNodes(root, unavailableNodes);

                if (unavailableNodes.empty()) {
                    std::cout << "Ok: -1\n";
                } else {
                    auto e = unavailableNodes.begin();
                    std::cout << "Ok: " << *e;
                    ++e;
                    for (; e != unavailableNodes.end(); ++e) {
                        std::cout << ";" << *e;
                    }
                    std::cout << "\n";
                }
            } else if (cmdType == "exit") {
                TerminateNodes(root);
                globalContext.close();
                break;
            } else {
                std::cout << "Error: Unknown command\n";
            }
        }

        for (auto it = futures.begin(); it != futures.end();) {
            if (it->wait_for(std::chrono::milliseconds(50)) == std::future_status::ready) {
                std::cout << it->get();
                it = futures.erase(it);
            } else {
                ++it;
            }
        }
    }
}