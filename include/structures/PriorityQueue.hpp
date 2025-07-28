#ifndef PRIORITYQUEUE_HPP
#define PRIORITYQUEUE_HPP

#include <iostream>

template <typename T>
class PriorityQueue {
private:
    struct Node {
        T data;
        int priority;
    };

    Node* heap;
    int capacity;
    int size;

    void swap(Node& a, Node& b);
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    PriorityQueue(int capacity = 100);
    ~PriorityQueue();

    void enqueue(const T& item, int priority);
    T dequeue();           // Removes item with highest priority
    T peek() const;        // Peek item with highest priority
    bool isEmpty() const;
    bool isFull() const;
    int getSize() const;
    void display() const;  // Display all items in priority order
};

template <typename T>
PriorityQueue<T>::PriorityQueue(int cap) {
    capacity = cap;
    size = 0;
    heap = new Node[capacity];
}

template <typename T>
PriorityQueue<T>::~PriorityQueue() {
    delete[] heap;
}

template <typename T>
bool PriorityQueue<T>::isEmpty() const {
    return size == 0;
}

template <typename T>
bool PriorityQueue<T>::isFull() const {
    return size == capacity;
}

template <typename T>
int PriorityQueue<T>::getSize() const {
    return size;
}

template <typename T>
void PriorityQueue<T>::swap(Node& a, Node& b) {
    Node temp = a;
    a = b;
    b = temp;
}

template <typename T>
void PriorityQueue<T>::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[index].priority > heap[parent].priority) {
            swap(heap[index], heap[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

template <typename T>
void PriorityQueue<T>::heapifyDown(int index) {
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        if (left < size && heap[left].priority > heap[largest].priority)
            largest = left;
        if (right < size && heap[right].priority > heap[largest].priority)
            largest = right;

        if (largest != index) {
            swap(heap[index], heap[largest]);
            index = largest;
        } else {
            break;
        }
    }
}

template <typename T>
void PriorityQueue<T>::enqueue(const T& item, int priority) {
    if (isFull()) return; // or throw
    heap[size].data = item;
    heap[size].priority = priority;
    heapifyUp(size);
    size++;
}

template <typename T>
T PriorityQueue<T>::dequeue() {
    if (isEmpty()) return T(); // or throw
    T item = heap[0].data;
    heap[0] = heap[size - 1];
    size--;
    heapifyDown(0);
    return item;
}

template <typename T>
T PriorityQueue<T>::peek() const {
    if (isEmpty()) return T();
    return heap[0].data;
}

template <typename T>
void PriorityQueue<T>::display() const {
    if (isEmpty()) {
        std::cout << "Priority Queue is empty.\n";
        return;
    }
    
    std::cout << "Priority Queue contents (Priority: Data):\n";
    for (int i = 0; i < size; i++) {
        std::cout << "Priority " << heap[i].priority << ": " << heap[i].data << "\n";
    }
    std::cout << "Total items: " << size << "\n";
}

#endif
