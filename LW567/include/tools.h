#pragma once

#include <memory>
#include <zmq.hpp>

inline zmq::context_t gl_context(1);

struct Node {
    int id;
    int pid;
    zmq::socket_t socket;

    std::shared_ptr<Node> left = nullptr;
    std::shared_ptr<Node> right = nullptr;

    Node(int id, int pid);
};

inline std::shared_ptr<Node> root = nullptr;

std::shared_ptr<Node> findNode(std::shared_ptr<Node> root, int id);

bool insertNode(std::shared_ptr<Node>& root, int id, int pid);

void terminateNodes(const std::shared_ptr<Node>& node);