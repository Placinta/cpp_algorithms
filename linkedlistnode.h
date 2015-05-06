//
// Created by Placinta on 5/6/15.
//

#ifndef ALGS_LINKEDLISTNODE_H
#define ALGS_LINKEDLISTNODE_H

#include <iostream>

template <typename T>
struct LinkedListNode {
    T item;
    std::shared_ptr<LinkedListNode> next;

    LinkedListNode() : next(nullptr) {}
    LinkedListNode(T value) : item(value), next(nullptr) {}
};

template <typename T>
void customLinkedListDeleter(LinkedListNode<T>* node) {
    std::cout << "Item deleted" << std::endl;
    delete node;
    node = nullptr;
}

#endif //ALGS_LINKEDLISTNODE_H
