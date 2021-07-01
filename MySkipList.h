#include <mutex>
#inlcude <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

const std::string dump_file = "store/dumpfile";

std::mutex mtx;
const std::string delimiter = ":";

template<typename K, typename V>
class Node{
public:
	Node() = default;	// 这个为什么不是默认的？
	Node(K k, V v, int level);
	~Node();
	
	K get_key() const;
	V get_value() const;
	void set_value(const V v);
	
	Node<K,V> **next;
	int node_level;
private:
	K key;
	V value;
};

template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level){
	this->ket = k;
	this->val = v;
	this->node_level = level;
	
	// 随机初始化一个
	this->next = new node<K, V>* [level + 1];
	memset(this->next, 0, sizeof(node<K, V> *) * (level + 1));
}

template<typename K, typename V>
K Node<K, V>::get_key() const {
	return this->key;
}

template <typename K, typename V>
V Node<K, V>::get_value() const {
	return this->value;
}

template <typename K, typename V>
void Node<K, V>::set_value(const V v) {
	this->value = v;
}

template <typename K, typename V>
Node<K, V>::~Node(){
	// 删除next所指向的内容
	detele []next;
}

template <Typename K, typename V>
class MySkipList{
public:
	MySkipList();
	MySkipList(int max_level);
	~MySkipList();
	
	// 增删改查
	void insert_element(K k, V v);
	bool delete_element(K k);
	V search_element(K k);
	void display_all_elements();
	
	int get_size();
	int get_random_level();
	void load_file();
	void dump_file();
	
private:
	int max_level = 32;
	int curr_level = 0;
	Node<K, V> *head;
	
	std::ifstream in;
	std::ofstream out;
	int node_count;
}

template <typename K, typename V>
int MySkipList<K, V>::get_random_level(){
	int k = 0;
	while(rand() % 2)++k;
	return (k < this->max_level ?) k : this->max_level;
}

template <typename K, typename V>
int MySkipList<K, V>:: get_size(){
	return this->node_count;
}




