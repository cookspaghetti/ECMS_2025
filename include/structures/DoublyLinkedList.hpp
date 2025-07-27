#ifndef DOUBLY_LINKED_LIST_HPP
#define DOUBLY_LINKED_LIST_HPP

#include <iostream>

template <typename T>
class DoublyLinkedList {
private:
    struct Node {
        T data;
        Node* prev;
        Node* next;

        Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int size;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}
    
    ~DoublyLinkedList() {
        clear();
    }

    void append(const T& value) {
        Node* newNode = new Node(value);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        size++;
    }

    void prepend(const T& value) {
        Node* newNode = new Node(value);
        if (!head) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        size++;
    }

    void removeAt(int index) {
        if (index < 0 || index >= size) return;

        Node* current = head;
        for (int i = 0; i < index; ++i) current = current->next;

        if (current->prev) current->prev->next = current->next;
        else head = current->next;

        if (current->next) current->next->prev = current->prev;
        else tail = current->prev;

        delete current;
        size--;
    }

    T* get(int index) const {
        if (index < 0 || index >= size) return nullptr;

        Node* current = head;
        for (int i = 0; i < index; ++i) current = current->next;

        return &current->data;
    }

    int getSize() const {
        return size;
    }

    void clear() {
        while (head) {
            Node* tmp = head;
            head = head->next;
            delete tmp;
        }
        tail = nullptr;
        size = 0;
    }

    void display() const {
        Node* current = head;
        while (current) {
            std::cout << current->data.name << " (ID: " << current->data.id << ")\n";
            current = current->next;
        }
    }

    // Iteration support
    class Iterator {
    private:
        Node* current;
    public:
        Iterator(Node* node) : current(node) {}
        T& operator*() { return current->data; }
        Iterator& operator++() { current = current->next; return *this; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
    };

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
};

#endif
