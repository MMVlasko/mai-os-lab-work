#include <tools.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <thread>
#include <sys/types.h>
#include <csignal>
#include <sys/wait.h>

Node::Node(int id, int pid) : id(id), pid(pid) {
    socket = zmq::socket_t(gl_context, zmq::socket_type::req);
    socket.set(zmq::sockopt::rcvtimeo, 2000);
    socket.set(zmq::sockopt::linger, 0);
}

std::shared_ptr<Node> findNode(std::shared_ptr<Node> root, int id) {
    if (!root) return nullptr;
    if (root->id == id) return root;
    if (id < root->id) return findNode(root->left, id);
    return findNode(root->right, id);
}

bool insertNode(std::shared_ptr<Node>& root, int id, int pid) {
    if (root == nullptr) {
        root = std::make_shared<Node>(id, pid);
        return true;
    }
    if (id == root->id) return false;
    if (id < root->id) return insertNode(root->left, id, pid);
    return insertNode(root->right, id, pid);
}

void terminateNodes(const std::shared_ptr<Node>& node) {
    if (!node) return;

    if (waitpid(node->pid, nullptr, WNOHANG) != node->pid) {
        zmq::message_t message("exit");
        node->socket.send(message, zmq::send_flags::none);
    }

    try {
        node->socket.close();
    } catch (const zmq::error_t& e) {
        std::cerr << "Error closing socket for node " << node->id << ": " << e.what() << "\n";
    }

    if (waitpid(node->pid, nullptr, WNOHANG) != node->pid) {
        kill(node->pid, SIGKILL);
        waitpid(node->pid, nullptr, 0);
    }

    terminateNodes(node->left);
    terminateNodes(node->right);
}