#include "AirportBST.h"
#include <fstream>
#include <queue>
#include <stack>

AirportBST::~AirportBST() {
    clear();
}

bool AirportBST::insert(const Airport& airport) {
    if (root_ == nullptr) {
        root_ = new Node(airport);
        size_ = 1;
        return true;
    }

    Node* current = root_;
    while (current != nullptr) {
        if (airport.getIataCode() == current->airport.getIataCode()) {
            return false;
        }

        if (airport.getIataCode() < current->airport.getIataCode()) {
            if (current->left == nullptr) {
                current->left = new Node(airport);
                ++size_;
                return true;
            }
            current = current->left;
        } else {
            if (current->right == nullptr) {
                current->right = new Node(airport);
                ++size_;
                return true;
            }
            current = current->right;
        }
    }

    return false;
}

SearchResult AirportBST::find(const std::string& code) const {
    SearchResult result;
    const Node* current = root_;

    while (current != nullptr) {
        result.path.push_back(current->airport.getIataCode());
        if (code == current->airport.getIataCode()) {
            result.airport = &current->airport;
            return result;
        }
        current = code < current->airport.getIataCode()
                      ? current->left
                      : current->right;
    }

    return result;
}

DeleteResult AirportBST::remove(const std::string& code) {
    Node** link = &root_;

    while (*link != nullptr && (*link)->airport.getIataCode() != code) {
        link = code < (*link)->airport.getIataCode()
                   ? &((*link)->left)
                   : &((*link)->right);
    }

    if (*link == nullptr) {
        return {false, "code not found"};
    }

    Node* target = *link;
    if (target->left == nullptr && target->right == nullptr) {
        delete target;
        *link = nullptr;
        --size_;
        return {true, "D1: leaf"};
    }

    if (target->left != nullptr && target->right == nullptr) {
        const std::string childCode = target->left->airport.getIataCode();
        *link = target->left;
        delete target;
        --size_;
        return {true, "D2: only a left child, " + childCode};
    }

    if (target->left == nullptr && target->right != nullptr) {
        const std::string childCode = target->right->airport.getIataCode();
        *link = target->right;
        delete target;
        --size_;
        return {true, "D2: only a right child, " + childCode};
    }

    Node** successorLink = &(target->right);
    while ((*successorLink)->left) {
        successorLink = &((*successorLink)->left);
    }

    Node* successor = *successorLink;
    const std::string successorCode = successor->airport.getIataCode();
    target->airport = successor->airport;
    *successorLink = successor->right;
    delete successor;
    --size_;
    return {true, "D3: two children, successor " + successorCode};
}

const Airport* AirportBST::minimum() const {
    const Node* current = root_;
    if (current == nullptr) {
        return nullptr;
    }
    while (current->left) {
        current = current->left;
    }
    return &current->airport;
}

const Airport* AirportBST::maximum() const {
    const Node* current = root_;
    if (current == nullptr) {
        return nullptr;
    }
    while (current->right) {
        current = current->right;
    }
    return &current->airport;
}

std::string AirportBST::rootCode() const {
    return root_ != nullptr ? root_->airport.getIataCode() : "";
}

int AirportBST::height() const {
    if (root_ == nullptr) {
        return -1;
    }

    std::queue<std::pair<const Node*, int>> pending;
    pending.push({root_, 0});
    int maximumDepth = 0;

    while (!pending.empty()) {
        const Node* node = pending.front().first;
        const int depth = pending.front().second;
        pending.pop();
        if (depth > maximumDepth) {
            maximumDepth = depth;
        }
        if (node->left) {
            pending.push({node->left, depth + 1});
        }
        if (node->right) {
            pending.push({node->right, depth + 1});
        }
    }

    return maximumDepth;
}

double AirportBST::averageDepth() const {
    if (root_ == nullptr) {
        return 0.0;
    }

    std::queue<std::pair<const Node*, int>> pending;
    pending.push({root_, 0});
    long long totalDepth = 0;
    std::size_t nodesSeen = 0;

    while (!pending.empty()) {
        const Node* node = pending.front().first;
        const int depth = pending.front().second;
        pending.pop();
        totalDepth += depth;
        ++nodesSeen;
        if (node->left) {
            pending.push({node->left, depth + 1});
        }
        if (node->right) {
            pending.push({node->right, depth + 1});
        }
    }

    return static_cast<double>(totalDepth) / static_cast<double>(nodesSeen);
}

std::size_t AirportBST::size() const { return size_; }
bool AirportBST::empty() const { return size_ == 0; }

bool AirportBST::validate() const {
    return validateNode(root_, nullptr, nullptr);
}

bool AirportBST::validateNode(const Node* node,
                              const std::string* low,
                              const std::string* high) {
    if (node == nullptr) {
        return true;
    }

    const std::string& code = node->airport.getIataCode();
    if ((low != nullptr && code <= *low) ||
        (high != nullptr && code >= *high)) {
        return false;
    }

    const bool leftIsValid = validateNode(node->left, low, &code);
    const bool rightIsValid = validateNode(node->right, &code, high);
    return leftIsValid && rightIsValid;
}

std::vector<Airport> AirportBST::inorder() const {
    std::vector<Airport> result;
    result.reserve(size_);
    std::stack<const Node*> pending;
    const Node* current = root_;

    while (current || !pending.empty()) {
        while (current) {
            pending.push(current);
            current = current->left;
        }
        current = pending.top();
        pending.pop();
        result.push_back(current->airport);
        current = current->right;
    }

    return result;
}

std::vector<std::string> AirportBST::preorderCodes() const {
    std::vector<std::string> result;
    result.reserve(size_);
    if (root_ == nullptr) {
        return result;
    }

    std::stack<const Node*> pending;
    pending.push(root_);
    while (!pending.empty()) {
        const Node* node = pending.top();
        pending.pop();
        result.push_back(node->airport.getIataCode());
        if (node->right) {
            pending.push(node->right);
        }
        if (node->left) {
            pending.push(node->left);
        }
    }
    return result;
}

std::vector<std::vector<std::string>> AirportBST::levelOrder(int maximumLevels) const {
    std::vector<std::vector<std::string>> levels;
    if (root_ == nullptr || maximumLevels == 0) {
        return levels;
    }

    std::queue<std::pair<const Node*, int>> pending;
    pending.push({root_, 0});
    while (!pending.empty()) {
        const Node* node = pending.front().first;
        const int depth = pending.front().second;
        pending.pop();

        if (maximumLevels >= 0 && depth >= maximumLevels) {
            continue;
        }
        if (static_cast<int>(levels.size()) == depth) {
            levels.push_back({});
        }
        levels[depth].push_back(node->airport.getIataCode());

        if (node->left) {
            pending.push({node->left, depth + 1});
        }
        if (node->right) {
            pending.push({node->right, depth + 1});
        }
    }
    return levels;
}

RangeResult AirportBST::range(const std::string& from, const std::string& to) const {
    RangeResult result;
    if (from > to) {
        return result;
    }

    rangeNode(root_, from, to, result);
    return result;
}

void AirportBST::rangeNode(const Node* node,
                           const std::string& from,
                           const std::string& to,
                           RangeResult& result) {
    if (node == nullptr) {
        return;
    }

    ++result.nodesVisited;
    const std::string& code = node->airport.getIataCode();
    if (code > from) {
        rangeNode(node->left, from, to, result);
    }
    if (code >= from && code <= to) {
        result.airports.push_back(node->airport);
    }
    if (code < to) {
        rangeNode(node->right, from, to, result);
    }
}

bool AirportBST::savePreorder(const std::string& fileName) const {
    std::ofstream output(fileName);
    if (!output) {
        return false;
    }
    const std::vector<std::string> codes = preorderCodes();
    const int n = static_cast<int>(codes.size());
    for (int i = 0; i < n; i++) {
        output << codes[i] << '\n';
    }
    return static_cast<bool>(output);
}

AirportBST::Node* AirportBST::buildBalanced(const std::vector<Airport>& sorted,
                                            int begin,
                                            int end) {
    if (begin >= end) {
        return nullptr;
    }
    const int middle = begin + (end - begin) / 2;
    Node* node = new Node(sorted[middle]);
    node->left = buildBalanced(sorted, begin, middle);
    node->right = buildBalanced(sorted, middle + 1, end);
    return node;
}

void AirportBST::rebalance() {
    const std::vector<Airport> sorted = inorder();
    clear();
    root_ = buildBalanced(sorted, 0, static_cast<int>(sorted.size()));
    size_ = sorted.size();
}

void AirportBST::clear() {
    if (root_ == nullptr) {
        size_ = 0;
        return;
    }

    std::vector<Node*> pending;
    pending.push_back(root_);
    while (!pending.empty()) {
        Node* node = pending.back();
        pending.pop_back();
        if (node->left) {
            pending.push_back(node->left);
        }
        if (node->right) {
            pending.push_back(node->right);
        }
        delete node;
    }
    root_ = nullptr;
    size_ = 0;
}
