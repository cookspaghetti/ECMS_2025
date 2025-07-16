#ifndef CIRCULARQUEUE_HPP
#define CIRCULARQUEUE_HPP

template <typename T>
class CircularQueue {
private:
    int front;
    int rear;
    int count;
    int capacity;
    T* data;

public:
    CircularQueue(int size = 100);
    ~CircularQueue();

    bool isEmpty() const;
    bool isFull() const;
    void enqueue(const T& item);
    T dequeue();
    T peek() const;
    int size() const;
};

template <typename T>
CircularQueue<T>::CircularQueue(int size) {
    capacity = size;
    data = new T[capacity];
    front = 0;
    rear = -1;
    count = 0;
}

template <typename T>
CircularQueue<T>::~CircularQueue() {
    delete[] data;
}

template <typename T>
bool CircularQueue<T>::isEmpty() const {
    return count == 0;
}

template <typename T>
bool CircularQueue<T>::isFull() const {
    return count == capacity;
}

template <typename T>
void CircularQueue<T>::enqueue(const T& item) {
    if (isFull()) return; // optionally throw error
    rear = (rear + 1) % capacity;
    data[rear] = item;
    count++;
}

template <typename T>
T CircularQueue<T>::dequeue() {
    if (isEmpty()) return T(); // optionally throw error
    T item = data[front];
    front = (front + 1) % capacity;
    count--;
    return item;
}

template <typename T>
T CircularQueue<T>::peek() const {
    if (isEmpty()) return T();
    return data[front];
}

template <typename T>
int CircularQueue<T>::size() const {
    return count;
}

#endif
