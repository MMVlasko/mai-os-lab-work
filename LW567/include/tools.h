#pragma once

#include <memory>
#include <unordered_set>
#include <zmq.hpp>

inline zmq::context_t globalContext(1);

struct Node {
    int id;
    int pid;
    zmq::socket_t socket;

    std::shared_ptr<Node> left = nullptr;
    std::shared_ptr<Node> right = nullptr;

    Node(int id, int pid);
};

inline std::shared_ptr<Node> root = nullptr;

std::shared_ptr<Node> FindNode(std::shared_ptr<Node> root, int id);

bool InsertNode(std::shared_ptr<Node>& root, int id, int pid);

void PingNodes(const std::shared_ptr<Node>& node, std::unordered_set<int> &unavailable_nodes);

void TerminateNodes(const std::shared_ptr<Node>& node);