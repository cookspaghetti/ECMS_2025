#ifndef CIRCULARQUEUE_HPP
#define CIRCULARQUEUE_HPP

#include <stdexcept>

template <typename T>
class CircularQueue {
private:
    int front;      // index of next item to dequeue
    int rear;       // index of last item enqueued
    int count;      // current number of items
    int capacity;   // maximum number of items
    T* data;        // underlying array storage

public:
    // Construct a queue of given size (default 100)
    CircularQueue(int size = 100)
      : front(0), rear(size - 1), count(0), capacity(size) {
        data = new T[capacity];
    }

    // Copy constructor
    CircularQueue(const CircularQueue& other)
      : front(other.front),
        rear(other.rear),
        count(other.count),
        capacity(other.capacity) {
        data = new T[capacity];
        for (int i = 0; i < capacity; ++i)
            data[i] = other.data[i];
    }

    // Copy assignment operator
    CircularQueue& operator=(const CircularQueue& other) {
        if (this != &other) {
            delete[] data;
            front = other.front;
            rear = other.rear;
            count = other.count;
            capacity = other.capacity;
            data = new T[capacity];
            for (int i = 0; i < capacity; ++i)
                data[i] = other.data[i];
        }
        return *this;
    }

    // Destructor
    ~CircularQueue() {
        delete[] data;
    }

    // Check if queue is empty
    bool isEmpty() const {
        return count == 0;
    }

    // Check if queue is full
    bool isFull() const {
        return count == capacity;
    }

    // Current number of items
    int size() const {
        return count;
    }

    // Enqueue an item at the rear
    void enqueue(const T& item) {
        if (isFull()) {
            throw std::overflow_error("CircularQueue::enqueue: queue is full");
        }
        // advance rear index
        rear = (rear + 1) % capacity;
        data[rear] = item;
        ++count;
    }

    // Dequeue an item from the front
    T dequeue() {
        if (isEmpty()) {
            throw std::underflow_error("CircularQueue::dequeue: queue is empty");
        }
        T item = data[front];
        front = (front + 1) % capacity;
        --count;
        return item;
    }

    // Peek at the front item without removing
    T peek() const {
        if (isEmpty()) {
            throw std::underflow_error("CircularQueue::peek: queue is empty");
        }
        return data[front];
    }

    // Clear the queue
    void clear() {
        front = 0;
        rear = capacity - 1;
        count = 0;
    }
};

#endif // CIRCULARQUEUE_HPP
