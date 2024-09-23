// Design and implement a concurrent hash map
// that supports efficient read and write operations.

// TODO: Lock free data hash map.

#include <bits/stdc++.h>
using namespace std;
mutex coutMtx;

template<typename K, typename V>
struct Node {
    Node<K,V>* left;
    Node<K,V>* right;
    K key;
    V val;
};

template<typename K, typename V>
class DoublyLinkedList {
public:
    bool addNode(Node<K, V>* node) {
        if(node == nullptr) return false;
        node->right = head->right;
        node->left = head;
        head->right->left = node;
        head->right = node;
        return true;
    }

    bool removeNode(Node<K, V>* node) {
        if(node == nullptr) return false;
        node->left->right = node->right;
        node->right->left = node->left;
        node->left = nullptr;
        node->right = nullptr;
        return true;
    }

    Node<K,V>* getNode(const K& key) {
        Node<K, V>* node = head->right;
        while(node && node != tail) {
            if(node->key == key) {
                return node;
            }
            node = node->right;
        }
        return nullptr;
    }

    DoublyLinkedList() {
        head->right = tail;
        tail->left = head;
    }

private:
    Node<K,V> nodes[2];
    Node<K,V>* head = &nodes[0];
    Node<K,V>* tail = &nodes[1];    
};

template<typename K, typename V>
class HashMap {
public:
    bool get(const K &key, V& val) {
        int bucketID = hash<K>()(key) % maxSize;
        DoublyLinkedList<K,V>* bucket = &Buckets[bucketID];
        shared_lock<shared_mutex> lock(mtx[bucketID]);
        Node<K, V>* node = bucket->getNode(key);
        if(node != nullptr) {
            val = node->val;
            return true;
        }
        return false;
    }

    bool insert(const K &key, const V &val) {
        int bucketID = hash<K>()(key) % maxSize;
        DoublyLinkedList<K,V>* bucket = &Buckets[bucketID];

        unique_lock<shared_mutex> lock(mtx[bucketID]);
        Node<K, V>* node = bucket->getNode(key);
        if(node != nullptr) {
            node->val = val;
            return true;
        }

        node = getNewNode();
        if(!node) return false;

        node->key = key;
        node->val = val;
        return bucket->addNode(node);
    }

    bool erase(const K& key) {
        int bucketID = hash<K>()(key) % maxSize;
        DoublyLinkedList<K,V>* bucket = &Buckets[bucketID];
        unique_lock<shared_mutex> lock(mtx[bucketID]);
        Node<K, V>* node = bucket->getNode(key);
        if(node == nullptr) return false;

        if(!bucket->removeNode(node)) {
            return false;
        }
        addFreeNode(node);
        return true;
    }

    HashMap(const int& maxNodes) {
        nodes = new Node<K,V>[maxNodes];
        for(int i = 0; i < maxNodes; i++) {
            freeNodes.push(&nodes[i]);
        }
    }

private:
    static const uint64_t maxSize = 10;
    Node<K,V>* nodes;
    stack<Node<K,V>*> freeNodes;
    DoublyLinkedList<K,V> Buckets[maxSize];
    shared_mutex mtx[maxSize];

    Node<K,V>* getNewNode() {
        if(freeNodes.empty()) {
            return nullptr;
        }
        Node<K,V>* node = freeNodes.top();
        freeNodes.pop();
        return node;
    }
    void addFreeNode(Node<K,V>* node) {
        freeNodes.push(node);
    }
};


void print(const string& msg) {
    lock_guard<mutex> lock(coutMtx);
    cout<<msg<<endl; 
}

void doWriterWork(HashMap<int, int> * mp) {
    while(1) {
        this_thread::sleep_for(1s);
        int key = rand() % 10;
        int val = key;
        if(mp->insert(key, val)) {
            char msg[100];
            sprintf(msg, "Inserted key %d and value %d", key, val);
            print(msg);
        }
    }
}

void doWork(HashMap<int, int>* mp) {
    while(1) {
        this_thread::sleep_for(1s);
        int key = rand() % 10;
        int val;
        if(mp->get(key, val)) {
            char msg[100];
            sprintf(msg, "For key: %d got value: %d", key, val);
            print(msg);
        }
    }
}

int main() {
    HashMap<int, int> *mp = new HashMap<int, int>(100);
    vector<thread*> threads;
    
    int num_thread = 4;
    for(int i = 0; i < num_thread; i++) {
        thread *t;
        if(i == 0) {
            t = new thread(doWriterWork, mp);
        } else {
            t = new thread(doWork, mp);
        }

        threads.push_back(t);
    }

    for(int i = 0; i < num_thread; i++) {
        threads[i]->join();
    }
}