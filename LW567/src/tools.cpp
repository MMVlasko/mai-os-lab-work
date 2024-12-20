#include <tools.h>

#include <iostream>
#include <string>
#include <thread>
#include <sys/types.h>
#include <csignal>
#include <worker.h>
#include <sys/wait.h>

Node::Node(int id) : id(id), pid(-1) {
    socket = zmq::socket_t(globalContext, zmq::socket_type::req);
    socket.set(zmq::sockopt::rcvtimeo, 2000);
    socket.set(zmq::sockopt::linger, 0);
    sockId = id;
    while (true) {
        try {
            socket.connect("tcp://127.0.0.1:" + std::to_string(5555 + sockId));
            break;
        } catch (zmq::error_t&) {
            ++sockId;
        }
    }
}

std::shared_ptr<Node> FindNode(std::shared_ptr<Node> root, int id) {
    if (!root) {
        return nullptr;
    }
    if (root->id == id) {
        return root;
    }
    if (id < root->id) {
        return FindNode(root->left, id);
    }
    return FindNode(root->right, id);
}

bool InsertNode(std::shared_ptr<Node>& root, int id) {
    if (root == nullptr) {
        try {
            root = std::make_shared<Node>(id);
            pid_t pid = fork();
            if (pid == 0) {
                Worker(id, root->sockId);
                exit(0);
            }
            root->pid = pid;
            return true;
        } catch (zmq::error_t&) {
            return false;
        }
        return true;
    }
    if (id == root->id) {
        return false;
    }
    if (id < root->id) {
        return InsertNode(root->left, id);
    }
    return InsertNode(root->right, id);
}

void PingNodes(const std::shared_ptr<Node>& node, std::unordered_set<int> &unavailable_nodes) {
    if (!node) {
        return;
    }

    try {
        zmq::message_t message("ping");
        node->socket.send(message, zmq::send_flags::none);

        zmq::message_t reply;
        if (!node->socket.recv(reply, zmq::recv_flags::none) &&
            std::strcmp(reply.to_string().c_str(), "Ok") != 0) {
            unavailable_nodes.insert(node->id);
            }
    } catch (zmq::error_t&) {
        unavailable_nodes.insert(node->id);
    }

    PingNodes(node->left, unavailable_nodes);
    PingNodes(node->right, unavailable_nodes);
};

void TerminateNodes(const std::shared_ptr<Node>& node) {
    if (!node) {
        return;
    }

    bool killed = false;

    if (waitpid(node->pid, nullptr, WNOHANG) != node->pid) {
        zmq::message_t message("exit");
        node->socket.send(message, zmq::send_flags::none);

        zmq::message_t reply;
        if (!node->socket.recv(reply, zmq::recv_flags::none) &&
            std::strcmp(reply.to_string().c_str(), "Ok") != 0) {
            killed = true;
            }
    }

    try {
        node->socket.close();
    } catch (const zmq::error_t& e) {
        std::cerr << "Error closing socket for node " << node->id << ": " << e.what() << "\n";
    }

    if (!killed) {
        kill(node->pid, SIGKILL);
        waitpid(node->pid, nullptr, 0);
    }

    TerminateNodes(node->left);
    TerminateNodes(node->right);
}