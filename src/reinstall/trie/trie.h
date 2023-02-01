#ifndef REINSTALL_TRIE_TRIE_H
#define REINSTALL_TRIE_TRIE_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <utility>

namespace reinstall {
namespace trie {

template <class T>
struct Node {
    typedef std::unordered_map<char, Node<T>> Map;

    std::string key;
    std::unique_ptr<Map> next;
    T data{};
    bool is_terminal;

    std::unique_ptr<Map> makeMapPtr() {
        return std::make_unique<Map>(Map{});
    }

    Node() {
        this->key = "";
        this->next = std::make_unique<Map>(Map{});
        this->is_terminal = false;
    }

    ~Node() = default;

    explicit Node(std::string key, bool is_terminal, T data)
        : key(std::move(key))
        , next(std::make_unique<Map>(Map{}))
        , data(std::move(data))
        , is_terminal(is_terminal)
    {}

    explicit Node(std::string  key, std::unique_ptr<Map> next, bool is_terminal, T data)
        : key(std::move(key))
        , next(std::move(next))
        , data(std::move(data))
        , is_terminal(is_terminal)
    {}

    Node(const Node& node) {
        this->key = node.key;
        this->is_terminal = node.is_terminal;
        this->data = node.data;

        this->next = std::make_unique<Map>(Map{});
        for (auto &i : *node.next) {
            this->next->insert(i);
        }
    }

    Node(Node&& node) noexcept {
        key.swap(node.key);
        next.swap(node.next);
        is_terminal = node.is_terminal;

        T tmp = std::move(data);
        data = std::move(node.data);
        node.data = std::move(tmp);
    }

    Node& operator=(const Node& other) {
        if (this == &other) {
            return *this;
        }

        this->key = other.key;
        this->is_terminal = other.is_terminal;
        this->data = other.data;
        this->next = std::make_unique<Map>(Map{});
        for (auto &i : *other.next) {
            this->next->insert(i);
        }

        return *this;
    }

    Node& operator=(Node&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        key.swap(other.key);
        next.swap(other.next);
        is_terminal = other.is_terminal;

        T tmp = std::move(data);
        data = std::move(other.data);
        other.data = std::move(tmp);

        return *this;
    }

    friend std::ostream& operator<<(std::ostream& out, const Node<T>& n) {
        return out << n.key;
    }
};

template <class T>
class Trie {
    typename Node<T>::Map head_ = {};
    bool insert_(Node<T>* node, std::string key, T data);
    std::pair<T, bool> find_(const Node<T>* node, std::string key);
    std::vector<T> findSeries_(const Node<T>* node, std::string key);
    void printTraverseNode_(std::ostream& out, Node<T>* node, int level);

public:
    Trie() = default;

    ~Trie() = default;
    bool insert(std::string key, T data);
    std::pair<T, bool> find(std::string key);
    std::vector<T> findSeries(std::string key);
    void printTraverse(std::ostream& out);

    static std::string::size_type maxSharedPrefixSize(std::string a, std::string b) {
        if (a.empty() || b.empty()) {
            return 0;
        }

        if (b.size() < a.size()) {
            a.swap(b);
        }

        for(std::string::size_type i = 0; i < a.size(); ++i) {
            if (a.substr(0, i + 1) != b.substr(0, i + 1)) {
                return i;
            }
        }

        return a.size();
    }
};

template <typename T>
bool Trie<T>::insert(std::string key, T data) {
    if (key.empty()) {
        return false;
    }

    char fchar = key.front();
    auto search = head_.find(fchar);
    if (search == head_.end()) { // first char edge not found
        return head_.insert({fchar, Node<T>{std::move(key), true, std::move(data)}}).second;
    }

    return insert_(&search->second, std::move(key), std::move(data));
}

template <typename T>
bool Trie<T>::insert_(Node<T> *node, std::string key, T data){
    if (key == node->key) { // duplicate key
        node->is_terminal = true;
        node->data = std::move(data);

        return true;
    }

    auto prefix_size = maxSharedPrefixSize(node->key, key);
    if (prefix_size != node->key.size()) { // key and node_key have shared prefix (but not whole node_key)
        auto new_key = node->key.substr(prefix_size, node->key.size() - prefix_size);
        node->key.erase(prefix_size, node->key.size() - prefix_size);

        char fchar = new_key.front();
        Node new_node{std::move(new_key), std::move(node->next), node->is_terminal, std::move(node->data)};

        node->is_terminal = false;
        node->next = node->makeMapPtr();
        node->next->insert({fchar, std::move(new_node)});
    }

    key.erase(0, prefix_size);
    if (key.empty()) {
        node->is_terminal = true;
        node->data = std::move(data);

        return true;
    }

    char fchar = key.front();
    auto search = node->next->find(fchar);
    if (search == node->next->end()) { // first char edge not found
        return node->next->insert({fchar, Node<T>{std::move(key), true, std::move(data)}}).second;
    }

    return insert_(&search->second, std::move(key), std::move(data));
}

template <typename T>
std::pair<T, bool> Trie<T>::find(std::string key) {
    if (key.empty()) {
        return std::make_pair(T{}, false);
    }

    char fchar = key.front();
    auto search = head_.find(fchar);
    if (search == head_.end()) { // first char edge not found
        return std::make_pair(T{}, false);
    }

    return find_(&search->second, std::move(key));
}

template <typename T>
std::pair<T, bool> Trie<T>::find_(const Node<T> *node, std::string key) {
    if (key == node->key) {
        if (node->is_terminal) {
            return std::make_pair(node->data, true);
        }

        return std::make_pair(T{}, false);
    }

    auto prefix_size = maxSharedPrefixSize(node->key, key);
    key.erase(0, prefix_size);
    
    if (key.empty()) {
        return std::make_pair(T{}, false);
    }

    char fchar = key.front();
    auto search = node->next->find(fchar);
    if (search == node->next->end()) { // first char edge not found
        return std::make_pair(T{}, false);
    }

    return find_(&search->second, key);
}

template <typename T>
std::vector<T> Trie<T>::findSeries(std::string key) {
    if (key.empty()) {
        return std::vector<T>{};
    }

    char fchar = key.front();
    auto search = head_.find(fchar);
    if (search == head_.end()) { // first char edge not found
        return std::vector<T>{};
    }

    return findSeries_(&search->second, std::move(key));
}

template <typename T>
std::vector<T> Trie<T>::findSeries_(const Node<T> *node, std::string key) {
    std::vector<T> result{};

    if (key == node->key) {
        if (node->is_terminal) {
            result.push_back(node->data);
        }

        return result;
    }

    auto prefix_size = maxSharedPrefixSize(node->key, key);
    if (prefix_size == node->key.size()) {
        if (node->is_terminal) {
            result.push_back(node->data);
        }
    }

    key.erase(0, prefix_size);
    if (key.empty()) {
        return result;
    }

    char fchar = key.front();
    auto search = node->next->find(fchar);
    if (search == node->next->end()) { // first char edge not found
        return result;
    }

    auto recursive_result = findSeries_(&search->second, key);
    result.insert(result.end(), recursive_result.begin(), recursive_result.end());

    return result;
}
template <typename T>
void Trie<T>::printTraverse(std::ostream& out) {
    out << "\n============= traverse trie =============\n";
    for (auto &i : head_) {
        out << "#" << i.first << "\n";
        printTraverseNode_(out, &i.second, 0);
    }
    out<< "\n============= traverse finish =============\n";
}

template <typename T>
void Trie<T>::printTraverseNode_(std::ostream& out, Node<T>* node, int level){
    std::string tabs;
    for (int i = 0; i < level; i++) {
        tabs.push_back(' ');
        tabs.push_back(' ');
    }
    out << tabs << ":" << level << " NODE(" << node->key << ") " << (node->is_terminal ? "-terminal-" : "") <<"\n";
    for (auto &i : *node->next) {
        out << tabs << ":" << level << " => " << i.first << "\n";
        printTraverseNode_(out, &i.second, level + 1);
    }
}

} // namespace trie
} // namespace reinstall

#endif //REINSTALL_TRIE_TRIE_H
