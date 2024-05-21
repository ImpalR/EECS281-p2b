// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure
            // to initialize it here.
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
            {}

            // Description: Allows access to the element at that Node's
            // position.  There are two versions, getElt() and a dereference
            // operator, use whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data
            // members of this Node class from within the PairingPQ class.
            // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
            // function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *parent;
    }; // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, count{ 0 }, root{ nullptr } {} // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, count{ 0 }, root{ nullptr } {
        while (start != end) {
            push(*start);
            ++start;
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare }, root{ nullptr } {
        count = 0;
        std::deque<Node*> cntr;
        cntr.push_back(other.root);
        while (not cntr.empty()) {
            Node *temp = cntr.front();
            if (temp->child != nullptr) cntr.push_back(temp->child);
            if (temp->sibling != nullptr) cntr.push_back(temp->sibling);
            push(temp->elt);
            cntr.pop_front();
        }
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        PairingPQ temp(rhs);
        std::swap(count, temp.count);
        std::swap(root, temp.root);
        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        if (root == nullptr) return;
        std::deque<Node*> cntr;
        cntr.push_back(root);
        while (not cntr.empty()) {
            Node *temp = cntr.front();
            if (temp->child != nullptr)   cntr.push_back(temp->child);
            if (temp->sibling != nullptr) cntr.push_back(temp->sibling);
            delete temp;
            cntr.pop_front();
        }
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        if (root == nullptr) return;
        std::deque<Node*> cntr;
        cntr.push_back(root);
        while (not cntr.empty()) {
            Node *temp = cntr.front();
            cntr.pop_front();
            if (temp->child != nullptr) cntr.push_back(temp->child);
            if (temp->sibling != nullptr) cntr.push_back(temp->sibling);
            temp->sibling = nullptr;
            temp->child = nullptr;
            if (temp->parent == nullptr) root = temp;
            else {
                temp->parent = nullptr;
                root = meld(root, temp);
            }
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        if (root->child == nullptr) {
            delete root;
            root = nullptr;
            --count;
            return;
        }
        Node *temp = root->child;
        delete root;
        --count;
        if (temp == nullptr) root = nullptr;

        std::deque<Node*> cntr;
        while (temp->sibling != nullptr) {
            cntr.push_back(temp);
            temp = temp->sibling;
        }
        if (temp->sibling == nullptr) cntr.push_back(temp);

        while (cntr.size() > 1) {
            Node *front = cntr.front();
            cntr.pop_front();
            front->sibling = nullptr;
            front->parent = nullptr;
            Node *second = cntr.front();
            cntr.pop_front();
            second->sibling = nullptr;
            second->parent = nullptr;
            cntr.push_back(meld(front, second));
        }

        root = cntr.front();
        cntr.pop_front();
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return root->elt;
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return count;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return count == 0;
    } // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        node->elt = new_value;
        Node *temp1 = node->parent;
        if (root == node) return;

        if (this->compare(temp1->elt, node->elt)) {
            if (temp1->child == node) {
                temp1->child = node->sibling;
                node->sibling = nullptr;
                node->parent = nullptr;
                root = meld(root, node);
            } else {
                Node *temp2 = temp1->child;
                while (temp2->sibling != node) {
                    temp2 = temp2->sibling;
                }
                temp2->sibling = node->sibling;
                node->sibling = nullptr;
                node->parent = nullptr;
                root = meld(root, node);
            }
        }
    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node* addNode(const TYPE &val) {
        Node *temp = new Node{val};
        ++count;
        if (root == nullptr) root = temp;
        else if (root != nullptr and temp != nullptr) root = meld(root, temp);
        return temp;
    } // addNode()


private:
    uint count = 0;
    Node *root = nullptr;
    Node *meld(Node *a, Node *b) {
        if (this->compare(a->elt, b->elt)) {
            a->sibling = b->child;
            b->child = a;
            a->parent = b;
            return b;
        } else {
            b->sibling = a->child;
            a->child = b;
            b->parent = a;
            return a;
        }
    }
};


#endif // PAIRINGPQ_H
