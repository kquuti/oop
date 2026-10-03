#pragma once

#include <cstddef>
#include <stdexcept>

#include "reposort/IList.h"

/// Реалізація списку на основі однозв'язного списку вузлів.
template <typename T>
class LinkedList : public IList<T> {
public:
    LinkedList() = default;

    // Копіювання заборонено, бо клас володіє вузлами в динамічній пам'яті.
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    ~LinkedList() override {
        clear();
    }

    void add(const T& item) override {
        Node* node = new Node(item);
        if (tail_ == nullptr) {
            // Список був порожній: новий вузол одночасно перший і останній.
            head_ = node;
            tail_ = node;
        } else {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    const T& get(std::size_t index) const override {
        checkIndex(index);
        return nodeAt(index)->value;
    }

    void set(std::size_t index, const T& item) override {
        checkIndex(index);
        nodeAt(index)->value = item;
    }

    void removeAt(std::size_t index) override {
        checkIndex(index);
        Node* toDelete = nullptr;
        if (index == 0) {
            toDelete = head_;
            head_ = head_->next;
            if (head_ == nullptr) {
                tail_ = nullptr;  // видалили єдиний елемент
            }
        } else {
            Node* previous = nodeAt(index - 1);
            toDelete = previous->next;
            previous->next = toDelete->next;
            if (toDelete == tail_) {
                tail_ = previous;  // видалили останній елемент
            }
        }
        delete toDelete;
        --size_;
    }

    std::size_t size() const override {
        return size_;
    }

private:
    struct Node {
        T value;
        Node* next;

        explicit Node(const T& v) : value(v), next(nullptr) {}
    };

    void checkIndex(std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("LinkedList: index out of range");
        }
    }

    // Проходить від початку списку до вузла з потрібним індексом.
    Node* nodeAt(std::size_t index) const {
        Node* current = head_;
        for (std::size_t i = 0; i < index; ++i) {
            current = current->next;
        }
        return current;
    }

    // Видаляє всі вузли.
    void clear() {
        while (head_ != nullptr) {
            Node* next = head_->next;
            delete head_;
            head_ = next;
        }
        tail_ = nullptr;
        size_ = 0;
    }

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};