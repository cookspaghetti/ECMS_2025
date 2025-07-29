// #ifndef CIRCULARQUEUE_HPP
// #define CIRCULARQUEUE_HPP

// template <typename T>
// class CircularQueue {
// private:
//     int front;
//     int rear;
//     int count;
//     int capacity;
//     T* data;

// public:
//     CircularQueue(int size = 100);
//     ~CircularQueue();

//     bool isEmpty() const;
//     bool isFull() const;
//     void enqueue(const T& item);
//     T dequeue();
//     T peek() const;
//     int size() const;
// };

// template <typename T>
// CircularQueue<T>::CircularQueue(int size) {
//     capacity = size;
//     data = new T[capacity];
//     front = 0;
//     rear = -1;
//     count = 0;
// }

// template <typename T>
// CircularQueue<T>::~CircularQueue() {
//     delete[] data;
// }

// template <typename T>
// bool CircularQueue<T>::isEmpty() const {
//     return count == 0;
// }

// template <typename T>
// bool CircularQueue<T>::isFull() const {
//     return count == capacity;
// }

// template <typename T>
// void CircularQueue<T>::enqueue(const T& item) {
//     if (isFull()) return; // optionally throw error
//     rear = (rear + 1) % capacity;
//     data[rear] = item;
//     count++;
// }

// template <typename T>
// T CircularQueue<T>::dequeue() {
//     if (isEmpty()) return T(); // optionally throw error
//     T item = data[front];
//     front = (front + 1) % capacity;
//     count--;
//     return item;
// }

// template <typename T>
// T CircularQueue<T>::peek() const {
//     if (isEmpty()) return T();
//     return data[front];
// }

// template <typename T>
// int CircularQueue<T>::size() const {
//     return count;
// }

// #endif

// // #ifndef CIRCULARQUEUE_HPP
// // #define CIRCULARQUEUE_HPP

// // #include <iostream>
// // #include <stdexcept>

// // template <typename T>
// // class CircularQueue {
// // private:
// //     int front;
// //     int rear;
// //     int count;
// //     int capacity;
// //     T* data;

// // public:
// //     CircularQueue(int size = 100);
// //     ~CircularQueue();
    
// //     // Copy constructor
// //     CircularQueue(const CircularQueue& other);
    
// //     // Copy assignment operator
// //     CircularQueue& operator=(const CircularQueue& other);

// //     bool isEmpty() const;
// //     bool isFull() const;
// //     void enqueue(const T& item);
// //     T dequeue();
// //     T peek() const;
// //     int size() const;
// // };

// // template <typename T>
// // CircularQueue<T>::CircularQueue(int size) {
// //     if (size <= 0) {
// //         std::cerr << "[ERROR] CircularQueue: Invalid size " << size << ", using default 100\n";
// //         size = 100;
// //     }
    
// //     capacity = size;
// //     std::cout << "[DEBUG] CircularQueue: Allocating array for " << capacity << " elements\n";
    
// //     try {
// //         data = new T[capacity];
// //         std::cout << "[DEBUG] CircularQueue: Memory allocated successfully at " << data << "\n";
// //     } catch (const std::bad_alloc& e) {
// //         std::cerr << "[ERROR] CircularQueue: Failed to allocate memory: " << e.what() << "\n";
// //         throw;
// //     }
    
// //     front = 0;
// //     rear = -1;
// //     count = 0;
    
// //     std::cout << "[DEBUG] CircularQueue: Initialized with capacity=" << capacity 
// //               << ", front=" << front << ", rear=" << rear << ", count=" << count << "\n";
// // }

// // template <typename T>
// // CircularQueue<T>::~CircularQueue() {
// //     std::cout << "[DEBUG] CircularQueue: Destructor called, data=" << data << "\n";
// //     if (data != nullptr) {
// //         delete[] data;
// //         data = nullptr;
// //         std::cout << "[DEBUG] CircularQueue: Memory deallocated\n";
// //     }
// // }

// // template <typename T>
// // CircularQueue<T>::CircularQueue(const CircularQueue& other) {
// //     std::cout << "[DEBUG] CircularQueue: Copy constructor called\n";
// //     capacity = other.capacity;
// //     front = other.front;
// //     rear = other.rear;
// //     count = other.count;
    
// //     if (capacity > 0) {
// //         data = new T[capacity];
// //         std::cout << "[DEBUG] CircularQueue: Copy constructor allocated " << capacity << " elements\n";
        
// //         // Copy all elements
// //         for (int i = 0; i < capacity; ++i) {
// //             data[i] = other.data[i];
// //         }
// //     } else {
// //         data = nullptr;
// //     }
// // }

// // template <typename T>
// // CircularQueue<T>& CircularQueue<T>::operator=(const CircularQueue& other) {
// //     std::cout << "[DEBUG] CircularQueue: Copy assignment operator called\n";
    
// //     if (this != &other) {  // Self-assignment check
// //         // Clean up existing resources
// //         if (data != nullptr) {
// //             delete[] data;
// //             data = nullptr;
// //         }
        
// //         // Copy from other
// //         capacity = other.capacity;
// //         front = other.front;
// //         rear = other.rear;
// //         count = other.count;
        
// //         if (capacity > 0) {
// //             data = new T[capacity];
// //             std::cout << "[DEBUG] CircularQueue: Copy assignment allocated " << capacity << " elements\n";
            
// //             // Copy all elements
// //             for (int i = 0; i < capacity; ++i) {
// //                 data[i] = other.data[i];
// //             }
// //         } else {
// //             data = nullptr;
// //         }
// //     }
    
// //     return *this;
// // }

// // template <typename T>
// // bool CircularQueue<T>::isEmpty() const {
// //     return count == 0;
// // }

// // template <typename T>
// // bool CircularQueue<T>::isFull() const {
// //     return count == capacity;
// // }

// // template <typename T>
// // void CircularQueue<T>::enqueue(const T& item) {
// //     // Check if data pointer is valid
// //     if (data == nullptr) {
// //         std::cerr << "[ERROR] CircularQueue::enqueue: data pointer is null!\n";
// //         throw std::runtime_error("CircularQueue data pointer is null");
// //     }
    
// //     // Check if full
// //     if (isFull()) {
// //         std::cerr << "[ERROR] CircularQueue::enqueue: Queue is full (count=" << count 
// //                   << ", capacity=" << capacity << ")\n";
// //         return; // or throw error
// //     }
    
// //     // Calculate new rear position with bounds checking
// //     int newRear = (rear + 1) % capacity;
    
// //     // Validate the calculated position
// //     if (newRear < 0 || newRear >= capacity) {
// //         std::cerr << "[ERROR] CircularQueue::enqueue: Invalid newRear=" << newRear 
// //                   << " (capacity=" << capacity << ")\n";
// //         throw std::runtime_error("Invalid rear position calculated");
// //     }
    
// //     // Update rear
// //     rear = newRear;
    
// //     // Assign the item with bounds check
// //     try {
// //         std::cout << "[DEBUG] CircularQueue::enqueue: About to assign item to data[" << rear << "]\n";
// //         data[rear] = item;
// //         std::cout << "[DEBUG] CircularQueue::enqueue: Successfully assigned item\n";
// //     } catch (const std::exception& e) {
// //         std::cerr << "[ERROR] CircularQueue::enqueue: Exception during assignment: " << e.what() << "\n";
// //         throw;
// //     } catch (...) {
// //         std::cerr << "[ERROR] CircularQueue::enqueue: Unknown exception during assignment\n";
// //         throw;
// //     }
    
// //     // Update count
// //     count++;
// // }

// // template <typename T>
// // T CircularQueue<T>::dequeue() {
// //     std::cout << "[DEBUG] CircularQueue::dequeue: Starting with count=" << count 
// //               << ", front=" << front << "\n";
    
// //     if (isEmpty()) {
// //         std::cout << "[DEBUG] CircularQueue::dequeue: Queue is empty, returning default T()\n";
// //         return T(); // or throw error
// //     }
    
// //     if (data == nullptr) {
// //         std::cerr << "[ERROR] CircularQueue::dequeue: data pointer is null!\n";
// //         throw std::runtime_error("CircularQueue data pointer is null");
// //     }
    
// //     if (front < 0 || front >= capacity) {
// //         std::cerr << "[ERROR] CircularQueue::dequeue: Invalid front=" << front 
// //                   << " (capacity=" << capacity << ")\n";
// //         throw std::runtime_error("Invalid front position");
// //     }
    
// //     T item = data[front];
// //     std::cout << "[DEBUG] CircularQueue::dequeue: Retrieved item from data[" << front << "]\n";
    
// //     front = (front + 1) % capacity;
// //     count--;
    
// //     std::cout << "[DEBUG] CircularQueue::dequeue: Updated front to " << front 
// //               << ", count to " << count << "\n";
    
// //     return item;
// // }

// // template <typename T>
// // T CircularQueue<T>::peek() const {
// //     if (isEmpty()) {
// //         std::cout << "[DEBUG] CircularQueue::peek: Queue is empty\n";
// //         return T();
// //     }
    
// //     if (data == nullptr) {
// //         std::cerr << "[ERROR] CircularQueue::peek: data pointer is null!\n";
// //         throw std::runtime_error("CircularQueue data pointer is null");
// //     }
    
// //     if (front < 0 || front >= capacity) {
// //         std::cerr << "[ERROR] CircularQueue::peek: Invalid front=" << front 
// //                   << " (capacity=" << capacity << ")\n";
// //         throw std::runtime_error("Invalid front position");
// //     }
    
// //     return data[front];
// // }

// // template <typename T>
// // int CircularQueue<T>::size() const {
// //     return count;
// // }

// // #endif


// File: structures/CircularQueue.hpp
#ifndef CIRCULARQUEUE_HPP
#define CIRCULARQUEUE_HPP

#include <stdexcept>

/**
 * A simple fixed-capacity circular queue implementation.
 * Supports enqueue, dequeue, peek, and capacity checks.
 */

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
};

#endif // CIRCULARQUEUE_HPP
