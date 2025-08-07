#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include <iostream>

template <typename T>
class DynamicArray {
private:
    T* data;
    int capacity;
    int size;
    
    void resize() {
        int newCapacity = capacity * 2;
        T* newData = new T[newCapacity];
        
        for (int i = 0; i < size; ++i) {
            newData[i] = data[i];
        }
        
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    // Default constructor
    DynamicArray() : data(nullptr), capacity(0), size(0) {
        capacity = 10; // Initial capacity
        data = new T[capacity];
    }
    
    // Constructor with initial capacity
    explicit DynamicArray(int initialCapacity) : data(nullptr), capacity(initialCapacity), size(0) {
        if (initialCapacity <= 0) initialCapacity = 10;
        data = new T[capacity];
    }
    
    // Copy constructor
    DynamicArray(const DynamicArray& other) : data(nullptr), capacity(other.capacity), size(other.size) {
        data = new T[capacity];
        for (int i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
    }
    
    // Assignment operator
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data;
            
            capacity = other.capacity;
            size = other.size;
            data = new T[capacity];
            
            for (int i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    
    // Destructor
    ~DynamicArray() {
        delete[] data;
    }
    
    // Add element to the end
    void push_back(const T& item) {
        if (size >= capacity) {
            resize();
        }
        data[size++] = item;
    }
    
    // Remove last element
    void pop_back() {
        if (size > 0) {
            --size;
        }
    }
    
    // Access element by index
    T& operator[](int index) {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
    
    // Access element by index (const version)
    const T& operator[](int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
    
    // Get element at index with bounds checking
    T& at(int index) {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
    
    // Get element at index with bounds checking (const version)
    const T& at(int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
    
    // Get current size
    int getSize() const {
        return size;
    }
    
    // Check if empty
    bool empty() const {
        return size == 0;
    }
    
    // Clear all elements
    void clear() {
        size = 0;
    }
    
    // Get first element
    T& front() {
        if (size == 0) {
            throw std::out_of_range("Array is empty");
        }
        return data[0];
    }
    
    // Get first element (const version)
    const T& front() const {
        if (size == 0) {
            throw std::out_of_range("Array is empty");
        }
        return data[0];
    }
    
    // Get last element
    T& back() {
        if (size == 0) {
            throw std::out_of_range("Array is empty");
        }
        return data[size - 1];
    }
    
    // Get last element (const version)
    const T& back() const {
        if (size == 0) {
            throw std::out_of_range("Array is empty");
        }
        return data[size - 1];
    }
    
    // Insert element at specific position
    void insert(int index, const T& item) {
        if (index < 0 || index > size) {
            throw std::out_of_range("Index out of range");
        }
        
        if (size >= capacity) {
            resize();
        }
        
        // Shift elements to the right
        for (int i = size; i > index; --i) {
            data[i] = data[i - 1];
        }
        
        data[index] = item;
        ++size;
    }
    
    // Remove element at specific position
    void erase(int index) {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
        
        // Shift elements to the left
        for (int i = index; i < size - 1; ++i) {
            data[i] = data[i + 1];
        }
        
        --size;
    }
    
    // Find element and return index (-1 if not found)
    int find(const T& item) const {
        for (int i = 0; i < size; ++i) {
            if (data[i] == item) {
                return i;
            }
        }
        return -1;
    }
    
    // Append all elements from another DynamicArray
    void append(const DynamicArray& other) {
        for (int i = 0; i < other.getSize(); ++i) {
            push_back(other[i]);
        }
    }
    
    // Get capacity
    int getCapacity() const {
        return capacity;
    }
    
    // Reserve capacity
    void reserve(int newCapacity) {
        if (newCapacity > capacity) {
            T* newData = new T[newCapacity];
            
            for (int i = 0; i < size; ++i) {
                newData[i] = data[i];
            }
            
            delete[] data;
            data = newData;
            capacity = newCapacity;
        }
    }
    
    // Iterator-like functionality
    T* begin() { return data; }
    T* end() { return data + size; }
    const T* begin() const { return data; }
    const T* end() const { return data + size; }
    
    // Display contents (for debugging)
    void display() const {
        std::cout << "DynamicArray [size=" << size << ", capacity=" << capacity << "]: ";
        for (int i = 0; i < size; ++i) {
            std::cout << data[i];
            if (i < size - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
};

#endif
