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

    Node** heap;  // Changed to pointer-to-pointer for proper memory management
    int capacity;
    int size;

    void swap(Node*& a, Node*& b);  // Updated signature for pointer swapping
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    PriorityQueue(int capacity = 100);
    ~PriorityQueue();
    
    // Copy constructor and assignment operator for proper deep copying
    PriorityQueue(const PriorityQueue& other);
    PriorityQueue& operator=(const PriorityQueue& other);

    void enqueue(const T& item, int priority);
    T dequeue();           // Removes item with highest priority
    T peek() const;        // Peek item with highest priority
    bool isEmpty() const;
    bool isFull() const;
    int getSize() const;
    void display() const;  // Display all items in priority order
    void clear();          // Clear the priority queue
    
    // Method to get all items with their priorities
    void getAllItemsWithPriority(T items[], int priorities[], int& count) const;
};

template <typename T>
PriorityQueue<T>::PriorityQueue(int cap) {
    capacity = cap;
    size = 0;
    heap = new Node*[capacity];
    for (int i = 0; i < capacity; i++) {
        heap[i] = nullptr;
    }
}

template <typename T>
PriorityQueue<T>::~PriorityQueue() {
    for (int i = 0; i < size; i++) {
        delete heap[i];
    }
    delete[] heap;
}

template <typename T>
PriorityQueue<T>::PriorityQueue(const PriorityQueue& other) {
    capacity = other.capacity;
    size = other.size;
    heap = new Node*[capacity];
    
    for (int i = 0; i < capacity; i++) {
        heap[i] = nullptr;
    }
    
    for (int i = 0; i < size; i++) {
        heap[i] = new Node{other.heap[i]->data, other.heap[i]->priority};
    }
}

template <typename T>
PriorityQueue<T>& PriorityQueue<T>::operator=(const PriorityQueue& other) {
    if (this != &other) {
        // Clean up existing data
        for (int i = 0; i < size; i++) {
            delete heap[i];
        }
        delete[] heap;
        
        // Copy from other
        capacity = other.capacity;
        size = other.size;
        heap = new Node*[capacity];
        
        for (int i = 0; i < capacity; i++) {
            heap[i] = nullptr;
        }
        
        for (int i = 0; i < size; i++) {
            heap[i] = new Node{other.heap[i]->data, other.heap[i]->priority};
        }
    }
    return *this;
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
void PriorityQueue<T>::swap(Node*& a, Node*& b) {
    Node* temp = a;
    a = b;
    b = temp;
}

template <typename T>
void PriorityQueue<T>::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[index]->priority > heap[parent]->priority) {
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

        if (left < size && heap[left]->priority > heap[largest]->priority)
            largest = left;
        if (right < size && heap[right]->priority > heap[largest]->priority)
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
    if (isFull()) {
        std::cerr << "Queue is full. Cannot enqueue.\n";
        return;
    }
    heap[size] = new Node{item, priority};
    size++;
    heapifyUp(size - 1);
}

template <typename T>
T PriorityQueue<T>::dequeue() {
    if (isEmpty()) return T(); // or throw
    T item = heap[0]->data;
    delete heap[0];
    heap[0] = heap[size - 1];
    heap[size - 1] = nullptr;
    size--;
    if (size > 0) {
        heapifyDown(0);
    }
    return item;
}

template <typename T>
T PriorityQueue<T>::peek() const {
    if (isEmpty()) return T();
    return heap[0]->data;
}

template <typename T>
void PriorityQueue<T>::display() const {
    if (isEmpty()) {
        std::cout << "Priority Queue is empty.\n";
        return;
    }
    
    std::cout << "Priority Queue contents (Priority: Data):\n";
    for (int i = 0; i < size; i++) {
        std::cout << "Priority " << heap[i]->priority << ": " << heap[i]->data << "\n";
    }
    std::cout << "Total items: " << size << "\n";
}

// Clear the priority queue
template <typename T>
void PriorityQueue<T>::clear() {
    for (int i = 0; i < size; i++) {
        delete heap[i];
    }
    delete[] heap;
    heap = new Node*[capacity];
    for (int i = 0; i < capacity; i++) {
        heap[i] = nullptr;
    }
    size = 0;
}

// Get all items with their priorities
template <typename T>
void PriorityQueue<T>::getAllItemsWithPriority(T items[], int priorities[], int& count) const {
    count = size;
    for (int i = 0; i < size; i++) {
        items[i] = heap[i]->data;
        priorities[i] = heap[i]->priority;
    }
}

#endif
