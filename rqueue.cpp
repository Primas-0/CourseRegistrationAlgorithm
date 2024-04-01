// UMBC - CMSC 341 - Spring 2024 - Proj3
#include "rqueue.h"
RQueue::RQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {

}
RQueue::~RQueue() {

}
void RQueue::clear() {

}
RQueue::RQueue(const RQueue& rhs) {

}
HEAPTYPE RQueue::getHeapType() const {

}
RQueue& RQueue::operator=(const RQueue& rhs) {

}
void RQueue::mergeWithQueue(RQueue& rhs) {

}
void RQueue::insertStudent(const Student& student) {

}
int RQueue::numStudents() const
{

}
prifn_t RQueue::getPriorityFn() const {

}
Student RQueue::getNextStudent() {

}
void RQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {

}
void RQueue::setStructure(STRUCTURE structure){

}
STRUCTURE RQueue::getStructure() const {

}
void RQueue::printStudentsQueue() const {

}

void RQueue::dump() const {
    if (m_size == 0) {
        cout << "Empty heap.\n" ;
    } else {
        dump(m_heap);
    }
    cout << endl;
}
void RQueue::dump(Node *pos) const {
    if ( pos != nullptr ) {
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

ostream& operator<<(ostream& sout, const Student& student) {
    sout << "Student name: " << student.m_name
         << ", Major: " << student.getMajorStr()
         << ", Gender: " << student.getGenderStr()
         << ", Level: " << student.getLevelStr();
    return sout;
}
ostream& operator<<(ostream& sout, const Node& node) {
    sout << node.m_student;
    return sout;
}