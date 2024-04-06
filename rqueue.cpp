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
    if (m_structure == LEFTIST && rhs.m_structure == LEFTIST && m_priorFunc == rhs.m_priorFunc) {
        m_heap = mergeLEFTIST(m_heap, rhs.m_heap);
    } else if (m_structure == SKEW && rhs.m_structure == SKEW && m_priorFunc == rhs.m_priorFunc) {
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
}

int RQueue::numStudents() const {
    return m_size;
}

prifn_t RQueue::getPriorityFn() const {
    return m_priorFunc;
}

Student RQueue::getNextStudent() {
    //TODO: This function extracts (removes the node) and returns the highest priority student from the queue. It must
    // maintain the min-heap or max-heap property. The function throws an out_of_range exception if the queue is empty
    // when the function is called.

}

void RQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
    //TODO: This function sets a new priority function and its corresponding heap type (min-heap or max-heap). It must
    // rebuild the heap! Note: it is the responsibility of the caller to pass compatible arguments priFn and heapType.

}

void RQueue::setStructure(STRUCTURE structure) {
    //TODO: This function sets the data structure, i.e. it is either SKEW or LEFTIST. It must rebuild a heap with the
    // new structure using the nodes in the current data structure. Note: rebuild means transferring nodes not
    // recreating nodes.

}

STRUCTURE RQueue::getStructure() const {
    return m_structure;
}

void RQueue::printStudentsQueue() const {
    //TODO: It prints the contents of the queue using preorder traversal. Although the first student printed should have
    // the highest priority, the remaining students will not necessarily be in priority order. Please refer to the
    // sample output file (driver.txt) for the format of this function's output.

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