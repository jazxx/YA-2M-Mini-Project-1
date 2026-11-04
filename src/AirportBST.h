#ifndef AIRPORT_BST_H
#define AIRPORT_BST_H
#include "Airport.h"
#include <cstddef>
#include <string>
#include <vector>

struct SearchResult {
    const Airport* airport = nullptr;
    std::vector<std::string> path;

    bool found() const { return airport != nullptr; }
    std::size_t comparisons() const { return path.size(); }
};

struct DeleteResult {
    bool deleted = false;
    std::string caseDescription;
};

struct RangeResult {
    std::vector<Airport> airports;
    std::size_t nodesVisited = 0;
};

class AirportBST {
public:
    AirportBST() = default;
    ~AirportBST();

    AirportBST(const AirportBST&) = delete;
    AirportBST& operator=(const AirportBST&) = delete;

    bool insert(const Airport& airport);
    SearchResult find(const std::string& code) const;
    DeleteResult remove(const std::string& code);

    const Airport* minimum() const;
    const Airport* maximum() const;
    std::string rootCode() const;

    int height() const;
    double averageDepth() const;
    std::size_t size() const;
    bool empty() const;

    bool validate() const;
    std::vector<Airport> inorder() const;
    std::vector<std::string> preorderCodes() const;
    std::vector<std::vector<std::string>> levelOrder(int maximumLevels = -1) const;
    RangeResult range(const std::string& from, const std::string& to) const;

    bool savePreorder(const std::string& fileName) const;
    void rebalance();
    void clear();

private:
    struct Node {
        explicit Node(const Airport& value) : airport(value) {}

        Airport airport;
        Node* left = nullptr;
        Node* right = nullptr;
    };

    static Node* buildBalanced(const std::vector<Airport>& sorted,
                               int begin,
                               int end);
    static bool validateNode(const Node* node,
                             const std::string* low,
                             const std::string* high);
    static void rangeNode(const Node* node,
                          const std::string& from,
                          const std::string& to,
                          RangeResult& result);

    Node* root_ = nullptr;
    std::size_t size_ = 0;
};

#endif
