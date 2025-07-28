#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <iostream>

template <typename T>
class Queue {
private:
    int front, rear, capacity, count;
    T* data;

public:
    Queue(int size = 100);
    ~Queue();

    bool isEmpty() const;
    bool isFull() const;
    void enqueue(const T& item);
    T dequeue();
    T peek() const;
    int size() const;
    void display() const;
};

template <typename T>
Queue<T>::Queue(int size) {
    capacity = size;
    front = 0;
    rear = -1;
    count = 0;
    data = new T[capacity];
}

template <typename T>
Queue<T>::~Queue() {
    delete[] data;
}

template <typename T>
bool Queue<T>::isEmpty() const {
    return count == 0;
}

template <typename T>
bool Queue<T>::isFull() const {
    return count == capacity;
}

template <typename T>
void Queue<T>::enqueue(const T& item) {
    if (isFull()) return; // or throw
    rear = (rear + 1) % capacity;
    data[rear] = item;
    count++;
}

template <typename T>
T Queue<T>::dequeue() {
    if (isEmpty()) return T(); // or throw
    T item = data[front];
    front = (front + 1) % capacity;
    count--;
    return item;
}

template <typename T>
T Queue<T>::peek() const {
    if (isEmpty()) return T(); // or throw
    return data[front];
}

template <typename T>
int Queue<T>::size() const {
    return count;
}

template <typename T>
void Queue<T>::display() const {
    if (isEmpty()) {
        std::cout << "Queue is empty.\n";
        return;
    }
    std::cout << "Queue contents: ";
    for (int i = 0; i < count; i++) {
        std::cout << data[(front + i) % capacity] << " ";
    }
    std::cout << "\n";
}

#endif
