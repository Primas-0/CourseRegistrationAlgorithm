// UMBC - CMSC 341 - Spring 2024 - Proj3
#include "rqueue.h"

RQueue::RQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
    m_heap = nullptr;
    m_size = 0;
    m_priorFunc = priFn;
    m_heapType = heapType;
    m_structure = structure;
}

RQueue::~RQueue() {
    clear();
}

void RQueue::clear() {
    //deallocate all memory
    destroyHeap(m_heap);

    //re-initialize member variables
    m_heap = nullptr;
    m_size = 0;
}

void RQueue::destroyHeap(Node *node) {
    //if node is not nullptr
    if (node) {
        //recursively deallocate memory for left and right sub-heaps
        destroyHeap(node->m_left);
        destroyHeap(node->m_right);
        delete node;
    }
}

RQueue::RQueue(const RQueue &rhs) {
    //mirror member variables
    m_size = rhs.m_size;
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;

    //make current object a deep copy of rhs
    copyNodes(rhs.m_heap, m_heap);
}

void RQueue::copyNodes(Node *sourceNode, Node *&destinationNode) {
    if (sourceNode == nullptr) {
        destinationNode = nullptr;
    } else {
        //allocate memory and copy over the data from each node
        destinationNode = new Node(sourceNode->m_student);

        //preorder traversal of heap
        copyNodes(sourceNode->m_left, destinationNode->m_left);
        copyNodes(sourceNode->m_right, destinationNode->m_right);
    }
}

HEAPTYPE RQueue::getHeapType() const {
    return m_heapType;
}

RQueue &RQueue::operator=(const RQueue &rhs) {
    //if self-assignment, return current object without any changes
    if (this == &rhs) {
        return *this;
    }

    //otherwise, destroy current object
    clear();

    //mirror member variables
    m_size = rhs.m_size;
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;

    //make current object a deep copy of rhs
    copyNodes(rhs.m_heap, m_heap);

    return *this;
}

void RQueue::mergeWithQueue(RQueue &rhs) {
    //protection against self-merging
    if (this == &rhs) {
        return;
    }

    //merge host queue with rhs if conditions are met
    if (m_structure == LEFTIST && rhs.m_structure == LEFTIST && m_priorFunc == rhs.m_priorFunc &&
        m_heapType == rhs.m_heapType) {
        m_heap = mergeLEFTIST(m_heap, rhs.m_heap);
    } else if (m_structure == SKEW && rhs.m_structure == SKEW && m_priorFunc == rhs.m_priorFunc &&
               m_heapType == rhs.m_heapType) {
        m_heap = mergeSKEW(m_heap, rhs.m_heap);
    } else {
        throw domain_error("Cannot merge queues with different priority functions or different data structures");
    }

    //leave rhs empty
    rhs.m_heap = nullptr;
    rhs.m_size = 0;
}

Node *RQueue::mergeLEFTIST(Node *lhs, Node *rhs) {
    if (lhs == nullptr) {
        //base case 1
        return rhs;
    } else if (rhs == nullptr) {
        //base case 2
        return lhs;
    } else if (priorityCheck(lhs, rhs)) {
        //if lhs has higher priority
        lhs->m_right = mergeLEFTIST(lhs->m_right, rhs);

        if (lhs->m_left == nullptr) {
            //if left side is empty, move right child to left side
            lhs->m_left = lhs->m_right;
            lhs->m_right = nullptr;
        } else {
            //if the NPL value of right child is larger than that of left child, swap them
            if (lhs->m_right->m_npl > lhs->m_left->m_npl) {
                Node *temp = lhs->m_left;
                lhs->m_left = lhs->m_right;
                lhs->m_right = temp;
            }
            //update NPL values
            lhs->m_npl = lhs->m_right->m_npl + 1;
        }
        return lhs;
    } else {
        //if rhs has higher priority, swap lhs and rhs and call merge recursively
        return mergeLEFTIST(rhs, lhs);
    }
}

Node *RQueue::mergeSKEW(Node *lhs, Node *rhs) {
    if (lhs == nullptr) {
        //base case 1
        return rhs;
    } else if (rhs == nullptr) {
        //base case 2
        return lhs;
    } else if (priorityCheck(lhs, rhs)) {
        //if lhs has higher priority, swap its children, then recursively merge rhs and the left sub-heap of lhs
        //replace the left subtree of lhs with the result of the recursive merge
        Node *temp = lhs->m_right;
        lhs->m_right = lhs->m_left;
        lhs->m_left = mergeSKEW(rhs, temp);

        //once all merges are complete, return
        return lhs;
    } else {
        //if rhs has higher priority, swap lhs and rhs and call merge recursively
        return mergeSKEW(rhs, lhs);
    }
}

bool RQueue::priorityCheck(Node *lhs, Node *rhs) {
    //compares the priority of two nodes based on heap type
    if ((m_heapType == MINHEAP && m_priorFunc(lhs->m_student) <= m_priorFunc(rhs->m_student)) ||
        (m_heapType == MAXHEAP && m_priorFunc(lhs->m_student) >= m_priorFunc(rhs->m_student))) {
        return true;
    }
    return false;
}

void RQueue::insertStudent(const Student &student) {
    //make a temporary heap with the same properties as the current one (to ensure merge can occur)
    RQueue tempQueue(m_priorFunc, m_heapType, m_structure);

    //allocate memory for new node using passed-in student object, make it the root of temporary heap
    tempQueue.m_heap = new Node(student);

    //merge the two heaps, essentially inserting node into current heap
    mergeWithQueue(tempQueue);

    m_size++;
}

int RQueue::numStudents() const {
    return m_size;
}

prifn_t RQueue::getPriorityFn() const {
    return m_priorFunc;
}

Student RQueue::getNextStudent() {
    //throw error if queue is empty
    if (m_size == 0) {
        throw out_of_range("Queue is empty");
    }

    //get the highest priority student from root node
    Student highestPriorityStudent = m_heap->m_student;

    //save the left and right sub-heaps
    Node *lhs = m_heap->m_left;
    Node *rhs = m_heap->m_right;

    //remove the root node from the heap
    delete m_heap;

    if (m_size == 1) {
        //if there's only one node in the heap, empty the heap
        m_heap = nullptr;
    } else {
        //otherwise, maintain min-heap or max-heap property by merging the two sub-heaps
        if (m_structure == LEFTIST) {
            m_heap = mergeLEFTIST(lhs, rhs);
        } else {
            m_heap = mergeSKEW(lhs, rhs);
        }
    }

    //update size of heap
    m_size--;

    return highestPriorityStudent;
}

void RQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
    m_priorFunc = priFn;
    m_heapType = heapType;

    Node *oldNode = m_heap;
    m_heap = nullptr;
    rebuildHeap(oldNode);
}

void RQueue::setStructure(STRUCTURE structure) {
    m_structure = structure;

    Node *oldNode = m_heap;
    m_heap = nullptr;
    rebuildHeap(oldNode);
}

void RQueue::rebuildHeap(Node *oldNode) {
    if (oldNode != nullptr) {
        //postorder traversal
        rebuildHeap(oldNode->m_left);
        rebuildHeap(oldNode->m_right);

        //remove inserted nodes from old heap
        oldNode->m_left = nullptr;
        oldNode->m_right = nullptr;

        //insert each node into new "heap"
        insertPointer(oldNode, m_heap);
    }
}

void RQueue::insertPointer(Node *oldNode, Node *newNode) {
    if (m_structure == LEFTIST) {
        m_heap = mergeLEFTIST(oldNode, newNode);
    } else {
        m_heap = mergeSKEW(oldNode, newNode);
    }
}

STRUCTURE RQueue::getStructure() const {
    return m_structure;
}

void RQueue::printStudentsQueue() const {
    cout << "Contents of the queue: \n";

    //print the contents of the queue using preorder traversal
    preorderPrint(m_heap);
}

void RQueue::preorderPrint(Node *node) const {
    if (node != nullptr) {
        //visit all nodes and print each student's details
        cout << "[" << m_priorFunc(node->m_student) << "] Student name: " << node->m_student.m_name << ", Major: "
             << node->m_student.getMajorStr() << ", Gender: " << node->m_student.getGenderStr() << ", Level: "
             << node->m_student.getLevelStr() << "\n";
        preorderPrint(node->m_left);
        preorderPrint(node->m_right);
    }
}

void RQueue::dump() const {
    if (m_size == 0) {
        cout << "Empty heap.\n";
    } else {
        dump(m_heap);
    }
    cout << endl;
}

void RQueue::dump(Node *pos) const {
    if (pos != nullptr) {
        cout << "(";
        dump(pos->m_left);
        if (m_structure == SKEW)
            cout << m_priorFunc(pos->m_student) << ":" << pos->m_student.m_name;
        else
            cout << m_priorFunc(pos->m_student) << ":" << pos->m_student.m_name << ":" << pos->m_npl;
        dump(pos->m_right);
        cout << ")";
    }
}

ostream &operator<<(ostream &sout, const Student &student) {
    sout << "Student name: " << student.m_name
         << ", Major: " << student.getMajorStr()
         << ", Gender: " << student.getGenderStr()
         << ", Level: " << student.getLevelStr();
    return sout;
}

ostream &operator<<(ostream &sout, const Node &node) {
    sout << node.m_student;
    return sout;
}