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
#include <functional>
#include <sys/wait.h>

void controller() {
    while (true) {
        std::cout << "command> ";
        std::cout.flush();
        std::string command;
        std::getline(std::cin, command);
        std::istringstream iss(command);

        std::string cmd_type;
        iss >> cmd_type;

        if (cmd_type == "create") {
            int id;//, parent_id;
            iss >> id;// >> parent_id;

            // // Проверка на существование родителя
            // if (parent_id != -1 && !findNode(root, parent_id)) {
            //     std::cout << "Error: Parent not found\n";
            //     continue;
            // }

            // Создание нового узла
            pid_t pid = fork();
            if (pid == 0) {
                worker(id);
                exit(0);
            }

            if (!insertNode(root, id, pid)) {
                std::cout << "Error: Already exists\n";
                kill(pid, SIGKILL);
                continue;
            }
            std::cout << "Ok: " << pid << "\n";
        } else if (cmd_type == "exec") {
            int id, n;
            iss >> id >> n;

            auto node = findNode(root, id);
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
                node->socket.connect("tcp://127.0.0.1:" + std::to_string(5555 + id));
                zmq::message_t message(oss.str());
                node->socket.send(message, zmq::send_flags::none);

                zmq::message_t reply;
                if (node->socket.recv(reply, zmq::recv_flags::none)) {
                    std::cout << reply.to_string() << "\n";
                } else {
                    std::cout << "Error:" << id << ": Node is invaluable\n";
                }
            } catch (...) {
                std::cout << "Error:" << id << ": Node is invaluable\n";
            }
        } else if (cmd_type == "pingall") {
            std::unordered_set<int> unavailable_nodes;

            std::function<void(std::shared_ptr<Node>)> pingNode = [&](const std::shared_ptr<Node>& node) {
                if (!node) return;

                try {
                    node->socket.connect("tcp://127.0.0.1:" + std::to_string(5555 + node->id));
                    zmq::message_t message("ping");
                    node->socket.send(message, zmq::send_flags::none);

                    zmq::message_t reply;
                    if (!node->socket.recv(reply, zmq::recv_flags::none) &&
                        std::strcmp(reply.to_string().c_str(), "Ok") != 0) {
                        unavailable_nodes.insert(node->id);
                    }
                } catch (...) {
                    unavailable_nodes.insert(node->id);
                }

                pingNode(node->left);
                pingNode(node->right);
            };

            pingNode(root);

            if (unavailable_nodes.empty()) {
                std::cout << "Ok: -1\n";
            } else {
                auto e = unavailable_nodes.begin();
                std::cout << "Ok: " << *e;
                ++e;
                for (; e != unavailable_nodes.end(); ++e) {
                    std::cout << ";" << *e;
                }
                std::cout << "\n";
            }
        } else if (cmd_type == "exit") {
            terminateNodes(root);
            gl_context.close();
            exit(0);
        } else {
            std::cout << "Error: Unknown command\n";
        }
    }
}