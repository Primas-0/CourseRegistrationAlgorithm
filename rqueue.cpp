// UMBC - CMSC 341 - Spring 2024 - Proj3
#include "rqueue.h"

RQueue::RQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
    m_heap = nullptr;
    m_size = 0;
    m_priorFunc = priFn;
    m_heapType = heapType;
    m_structure = structure;
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

void RQueue::copyNodes(Node *sourceNode, Node *&destinationNode) {
    if (sourceNode == nullptr) {
        destinationNode = nullptr;
    } else {
        //allocate memory and copy over the data from each node
        destinationNode = new Node(*sourceNode);

        //preorder traversal of heap
        copyNodes(sourceNode->m_left, destinationNode->m_left);
        copyNodes(sourceNode->m_right, destinationNode->m_right);
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
    //TODO: This function merges the host queue with the rhs; it leaves rhs empty; it transfers all nodes from rhs to
    // the current heap. Two heaps can only be merged if they have the same priority function and they are of the same
    // data structure. If the user attempts to merge queues with different priority functions, or two different data
    // structures a domain_error exception should be thrown. This function requires protection against self-merging.
    // Merging a heap with itself is not allowed.

}

void RQueue::insertStudent(const Student &student) {
    //TODO: Inserts a student into the queue. Must maintain the min-heap or the max-heap property depending on the
    // settings. Moreover, if the selected data structure is leftist heap, it needs to maintain a correct value of Null
    // Path Length (NPL) in the node.

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