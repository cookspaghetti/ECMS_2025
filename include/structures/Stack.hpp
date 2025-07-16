#ifndef STACK_HPP
#define STACK_HPP

template <typename T>
class Stack {
private:
    int capacity;
    int topIndex;
    T* data;

public:
    Stack(int size = 100);
    ~Stack();

    void push(const T& item);
    T pop();
    T peek() const;

    bool isEmpty() const;
    bool isFull() const;
    int size() const;
};

template <typename T>
Stack<T>::Stack(int size) {
    capacity = size;
    topIndex = -1;
    data = new T[capacity];
}

template <typename T>
Stack<T>::~Stack() {
    delete[] data;
}

template <typename T>
void Stack<T>::push(const T& item) {
    if (isFull()) return; // optionally throw
    data[++topIndex] = item;
}

template <typename T>
T Stack<T>::pop() {
    if (isEmpty()) return T(); // or throw
    return data[topIndex--];
}

template <typename T>
T Stack<T>::peek() const {
    if (isEmpty()) return T(); // or throw
    return data[topIndex];
}

template <typename T>
bool Stack<T>::isEmpty() const {
    return topIndex == -1;
}

template <typename T>
bool Stack<T>::isFull() const {
    return topIndex == capacity - 1;
}

template <typename T>
int Stack<T>::size() const {
    return topIndex + 1;
}

#endif
