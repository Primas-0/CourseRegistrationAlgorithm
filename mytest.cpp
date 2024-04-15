#include "rqueue.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
#include <ctime>

using namespace std;

enum RANDOM {
    UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE
};

class Random {
public:
    Random(int min, int max, RANDOM type = UNIFORMINT, int mean = 50, int stdev = 20) : m_min(min), m_max(max),
                                                                                        m_type(type) {
        if (type == NORMAL) {
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor
            m_normdist = std::normal_distribution<>(mean, stdev);
        } else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min, max);
        } else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double) min, (double) max);
        } else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }

    void setSeed(int seedNum) {
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> &array) {
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i <= m_max; i++) {
            array.push_back(i);
        }
        shuffle(array.begin(), array.end(), m_generator);
    }

    void getShuffle(int array[]) {
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i <= m_max; i++) {
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it = temp.begin(); it != temp.end(); it++) {
            array[i] = *it;
            i++;
        }
    }

    int getRandNum() {
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if (m_type == NORMAL) {
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while (result < m_min || result > m_max)
                result = m_normdist(m_generator);
        } else if (m_type == UNIFORMINT) {
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum() {
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result * 100.0) / 100.0;
        return result;
    }

    string getRandString(int size) {
        // the parameter size specifies the length of string we ask for
        // to use ASCII char the number range in constructor must be set to 97 - 122
        // and the Random type must be UNIFORMINT (it is default in constructor)
        string output = "";
        for (int i = 0; i < size; i++) {
            output = output + (char) getRandNum();
        }
        return output;
    }

private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};

int priorityFn1(const Student &student);
int priorityFn2(const Student &student);

class Tester {
public:
    bool testHeapPropertyAfterInsertMINHEAP();
    bool testHeapPropertyAfterInsertMAXHEAP();

    bool testRemovalOrderMINHEAP();
    bool testRemovalOrderMAXHEAP();

    bool testNPLValue();
    bool testLEFTISTProperty();

    bool testSetPriorityFn();

    bool testMergeWithQueueEdge();

    bool testCopyConstructorNormal();
    bool testCopyConstructorEdge();

    bool testAssignmentNormal();
    bool testAssignmentEdge();

    //Test that attempting to dequeue an empty queue throws an out_of_range exception.
    bool testRemovalError();

    //Test that attempting to merge queues with different priority functions throws a domain_error exception.
    bool testMergeWithQueueError();

private:
    void insertMultipleStudents(RQueue &myQueue);
    bool checkHeapProperty(Node *node, prifn_t priorFunc, HEAPTYPE heapType);
    bool checkRemovalOrder(RQueue &myQueue);
    bool checkNPLValue(Node *node);
    bool checkLEFTISTProperty(Node *node);
    void storeDataInVector(vector<Node *> &dataVector, Node *node);
    bool checkHeapEquivalence(Node *source, Node *destination);
};

void Tester::insertMultipleStudents(RQueue &myQueue) {
    Random randNameObject(97, 122);
    Random randLevelObject(0, 3);
    Random randMajorObject(0, 4);
    Random randGroupObject(0, 3);
    Random randRaceObject(0, 2);
    Random randGenderObject(0, 2);
    Random randIncomeObject(0, 4);
    Random randHighSchoolObject(0, 2);

    int studentSize = 300;

    for (int i = 0; i < studentSize; i++) {
        Student student(randNameObject.getRandString(5), randLevelObject.getRandNum(),
                        randMajorObject.getRandNum(), randGroupObject.getRandNum(),
                        randRaceObject.getRandNum(), randGenderObject.getRandNum(),
                        randIncomeObject.getRandNum(), randHighSchoolObject.getRandNum());
        myQueue.insertStudent(student);
    }
}

bool Tester::checkHeapProperty(Node *node, prifn_t priorFunc, HEAPTYPE heapType) {
    if (node != nullptr) {
        //recursively check whether both left and right sub-heaps satisfy heap property
        checkHeapProperty(node->m_left, priorFunc, heapType);
        checkHeapProperty(node->m_right, priorFunc, heapType);

        int currPriority = priorFunc(node->m_student);
        int leftChildPriority = 0;
        int rightChildPriority = 0;

        if (node->m_left != nullptr) {
            //if there is a left child, get its priority
            leftChildPriority = priorFunc(node->m_left->m_student);
        } else {
            //otherwise, assign the default priority according to heaptype
            if (heapType == MINHEAP) {
                leftChildPriority = MAX;
            } else {
                leftChildPriority = MIN;
            }
        }

        if (node->m_right != nullptr) {
            //if there is a right child, get its priority
            rightChildPriority = priorFunc(node->m_right->m_student);
        } else {
            //otherwise, assign the default priority according to heaptype
            if (heapType == MINHEAP) {
                rightChildPriority = MAX;
            } else {
                rightChildPriority = MIN;
            }
        }

        //compare priority values to ensure heap property
        if ((heapType == MINHEAP && currPriority <= leftChildPriority && currPriority <= rightChildPriority) ||
            (heapType == MAXHEAP && currPriority >= leftChildPriority && currPriority >= rightChildPriority)) {
            return true;
        }
        //test fails if any comparisons are false
        return false;
    }
    //test passes if heap is empty
    return true;
}

bool Tester::checkRemovalOrder(RQueue &myQueue) {
    //remove root and save its priority
    Student prevStudent = myQueue.getNextStudent();
    int prevPriority = myQueue.m_priorFunc(prevStudent);

    int removeSize = 98;

    for (int i = 0; i < removeSize; i++) {
        //remove the next node and save its priority
        Student currStudent = myQueue.getNextStudent();
        int currPriority = myQueue.m_priorFunc(currStudent);

        //compare the priorities of the previous and current removed nodes
        if ((myQueue.m_heapType == MINHEAP && currPriority < prevPriority) ||
            (myQueue.m_heapType == MAXHEAP && currPriority > prevPriority)) {
            //test fails if a lower priority node is removed before a higher priority
            return false;
        }

        //advance the "previous" node to be the current node, and repeat the process
        prevStudent = currStudent;
        prevPriority = currPriority;
    }

    //test passes if all the nodes were removed in the correct order
    return true;
}

bool Tester::checkNPLValue(Node *node) {
    if (node != nullptr) {
        //recursively check whether both left and right sub-heaps have correct NPL values
        checkNPLValue(node->m_left);
        checkNPLValue(node->m_right);

        int leftChildNPL = 0;
        int rightChildNPL = 0;

        //get NPL values of left and right children
        if (node->m_left != nullptr) {
            leftChildNPL = node->m_left->m_npl;
        } else {
            leftChildNPL = -1;
        }
        if (node->m_right != nullptr) {
            rightChildNPL = node->m_right->m_npl;
        } else {
            rightChildNPL = -1;
        }

        //calculate expected NPL value for each node
        int correctNPL = min(leftChildNPL, rightChildNPL) + 1;

        if (node->m_npl != correctNPL) {
            //test fails if any NPL values differ from expected values
            return false;
        }
        //test passes if all NPL values match
        return true;
    }
    //test passes if heap is empty
    return true;
}

bool Tester::checkLEFTISTProperty(Node *node) {
    if (node != nullptr) {
        //recursively check whether both left and right sub-heaps satisfy leftist property
        checkLEFTISTProperty(node->m_left);
        checkLEFTISTProperty(node->m_right);

        int leftChildNPL = 0;
        int rightChildNPL = 0;

        //get NPL values of left and right children
        if (node->m_left != nullptr) {
            leftChildNPL = node->m_left->m_npl;
        } else {
            leftChildNPL = -1;
        }
        if (node->m_right != nullptr) {
            rightChildNPL = node->m_right->m_npl;
        } else {
            rightChildNPL = -1;
        }

        if (leftChildNPL >= rightChildNPL) {
            return true;
        }
        return false;
    }
    //test passes if heap is empty
    return true;
}

void Tester::storeDataInVector(vector<Node *> &dataVector, Node *node) {
    if (node != nullptr) {
        //insert each node pointer into a vector
        dataVector.push_back(node);
        storeDataInVector(dataVector, node->m_left);
        storeDataInVector(dataVector, node->m_right);
    }
    //order the node pointers in non-descending order of memory address
    sort(dataVector.begin(), dataVector.end());
}

bool Tester::checkHeapEquivalence(Node *source, Node *destination) {
    if (source != nullptr && destination != nullptr) {
        //recursively check whether both left and right sub-heaps are equivalent
        checkHeapEquivalence(source->m_left, destination->m_left);
        checkHeapEquivalence(source->m_right, destination->m_right);

        if (source->m_student == destination->m_student) {
            return true;
        }
        return false;
    }
    return true;
}

bool Tester::testHeapPropertyAfterInsertMINHEAP() {
    RQueue myQueue(priorityFn2, MINHEAP, SKEW);
    insertMultipleStudents(myQueue);

    return checkHeapProperty(myQueue.m_heap, priorityFn2, MINHEAP);
}

bool Tester::testHeapPropertyAfterInsertMAXHEAP() {
    RQueue myQueue(priorityFn1, MAXHEAP, SKEW);
    insertMultipleStudents(myQueue);

    return checkHeapProperty(myQueue.m_heap, priorityFn1, MAXHEAP);
}

bool Tester::testRemovalOrderMINHEAP() {
    RQueue myQueue(priorityFn2, MINHEAP, SKEW);
    insertMultipleStudents(myQueue);

    return checkRemovalOrder(myQueue) && checkHeapProperty(myQueue.m_heap, priorityFn2, MINHEAP);
}

bool Tester::testRemovalOrderMAXHEAP() {
    RQueue myQueue(priorityFn1, MAXHEAP, SKEW);
    insertMultipleStudents(myQueue);

    return checkRemovalOrder(myQueue) && checkHeapProperty(myQueue.m_heap, priorityFn1, MAXHEAP);
}

bool Tester::testNPLValue() {
    RQueue myQueue(priorityFn1, MAXHEAP, LEFTIST);
    insertMultipleStudents(myQueue);

    return checkNPLValue(myQueue.m_heap) && checkHeapProperty(myQueue.m_heap, priorityFn1, MAXHEAP);
}

bool Tester::testLEFTISTProperty() {
    RQueue myQueue(priorityFn1, MAXHEAP, LEFTIST);
    insertMultipleStudents(myQueue);

    return checkLEFTISTProperty(myQueue.m_heap) && checkHeapProperty(myQueue.m_heap, priorityFn1, MAXHEAP);
}

bool Tester::testSetPriorityFn() {
    RQueue myQueue(priorityFn1, MAXHEAP, SKEW);
    insertMultipleStudents(myQueue);

    vector<Node *> initialData;
    vector<Node *> finalData;

    //save data before change in one vector
    storeDataInVector(initialData, myQueue.m_heap);

    //change priority function and heap type
    myQueue.setPriorityFn(priorityFn2, MINHEAP);

    //save data after change in a different vector
    storeDataInVector(finalData, myQueue.m_heap);

    //verify whether the data is the same, the priority function has been changed, and heap property is maintained
    if (initialData != finalData || myQueue.m_priorFunc != priorityFn2 ||
        !checkHeapProperty(myQueue.m_heap, priorityFn2, MINHEAP)) {
        return false;
    }
    return true;
}

bool Tester::testMergeWithQueueEdge() {
    RQueue emptyQueue(priorityFn1, MAXHEAP, SKEW);

    RQueue normalQueue(priorityFn1, MAXHEAP, SKEW);
    insertMultipleStudents(normalQueue);

    int normalQueueSize = normalQueue.m_size;

    emptyQueue.mergeWithQueue(normalQueue);

    //size of empty queue should be updated, empty queue should no longer be empty, and normal queue should be empty
    if (emptyQueue.m_size != normalQueueSize || emptyQueue.m_heap == nullptr || normalQueue.m_heap != nullptr) {
        return false;
    }
    return true;
}

bool Tester::testCopyConstructorNormal() {
    RQueue myQueue(priorityFn1, MAXHEAP, SKEW);
    insertMultipleStudents(myQueue);

    //copy a normal queue
    RQueue copyQueue(myQueue);

    //heaps and member variables should be identical
    if (!checkHeapEquivalence(myQueue.m_heap, copyQueue.m_heap) || myQueue.m_size != copyQueue.m_size ||
        myQueue.m_priorFunc != copyQueue.m_priorFunc || myQueue.m_heapType != copyQueue.m_heapType ||
        myQueue.m_structure != copyQueue.m_structure) {
        return false;
    }
    return true;
}

bool Tester::testCopyConstructorEdge() {
    RQueue myQueue(priorityFn1, MAXHEAP, SKEW);

    //copy an empty queue
    RQueue copyQueue(myQueue);

    //heaps and member variables should be identical
    if (!checkHeapEquivalence(myQueue.m_heap, copyQueue.m_heap) || myQueue.m_size != copyQueue.m_size ||
        myQueue.m_priorFunc != copyQueue.m_priorFunc || myQueue.m_heapType != copyQueue.m_heapType ||
        myQueue.m_structure != copyQueue.m_structure) {
        return false;
    }
    return true;
}

bool Tester::testAssignmentNormal() {
    RQueue sourceQueue(priorityFn1, MAXHEAP, SKEW);
    insertMultipleStudents(sourceQueue);

    RQueue destQueue(priorityFn1, MAXHEAP, SKEW);
    insertMultipleStudents(destQueue);

    //assign one normal queue to another
    destQueue = sourceQueue;

    //heaps and member variables should be identical
    if (!checkHeapEquivalence(sourceQueue.m_heap, destQueue.m_heap) ||
        sourceQueue.m_size != destQueue.m_size || sourceQueue.m_priorFunc != destQueue.m_priorFunc ||
        sourceQueue.m_heapType != destQueue.m_heapType || sourceQueue.m_structure != destQueue.m_structure) {
        return false;
    }
    return true;
}

bool Tester::testAssignmentEdge() {
    RQueue sourceQueue(priorityFn1, MAXHEAP, SKEW);
    insertMultipleStudents(sourceQueue);

    RQueue destQueue(priorityFn1, MAXHEAP, SKEW);

    //assign a normal queue to an empty queue
    destQueue = sourceQueue;

    //heaps and member variables should be identical
    if (!checkHeapEquivalence(sourceQueue.m_heap, destQueue.m_heap) ||
        sourceQueue.m_size != destQueue.m_size || sourceQueue.m_priorFunc != destQueue.m_priorFunc ||
        sourceQueue.m_heapType != destQueue.m_heapType || sourceQueue.m_structure != destQueue.m_structure) {
        return false;
    }
    return true;
}

int main() {
    Tester tester;

    cout << "\nTesting insertStudent - check whether heap property is satisfied after insertion into min-heap:" << endl;
    if (tester.testHeapPropertyAfterInsertMINHEAP()) {
        cout << "\tTest passed!" << endl;
    } else {
        cout << "\t***Test failed!***" << endl;
    }
    cout << "Testing insertStudent - check whether heap property is satisfied after insertion into max-heap:" << endl;
    if (tester.testHeapPropertyAfterInsertMAXHEAP()) {
        cout << "\tTest passed!" << endl;
    } else {
        cout << "\t***Test failed!***" << endl;
    }

    cout << "\nTesting getNextStudent - check whether all min-heap removals happen in the correct order:" << endl;
    if (tester.testRemovalOrderMINHEAP()) {
        cout << "\tTest passed!" << endl;
    } else {
        cout << "\t***Test failed!***" << endl;
    }
    cout << "Testing getNextStudent - check whether all max-heap removals happen in the correct order:" << endl;
    if (tester.testRemovalOrderMAXHEAP()) {
        cout << "\tTest passed!" << endl;
    } else {
        cout << "\t***Test failed!***" << endl;
    }

    cout << "\nTesting leftist heap - check whether all nodes in a leftist heap have the correct NPL value:" << endl;
    if (tester.testNPLValue()) {
        cout << "\tTest passed!" << endl;
    } else {
        cout << "\t***Test failed!***" << endl;
    }
    cout << "Testing leftist heap - check whether leftist heap preserves the property of such a heap:" << endl;
    if (tester.testLEFTISTProperty()) {
        cout << "\tTest passed!" << endl;
    } else {
        cout << "\t***Test failed!***" << endl;
    }

    cout << "\nTesting setPriorityFn - check whether after changing the priority function a correct heap is rebuilt "
            "with the same data and the different priority function:" << endl;
    if (tester.testSetPriorityFn()) {
        cout << "\tTest passed!" << endl;
    } else {
        cout << "\t***Test failed!***" << endl;
    }

    cout << "\nTesting mergeWithQueue - successfully merges an empty queue with a normal queue:" << endl;
    if (tester.testMergeWithQueueEdge()) {
        cout << "\tTest passed!" << endl;
    } else {
        cout << "\t***Test failed!***" << endl;
    }

    cout << "\nTesting copy constructor - normal:" << endl;
    if (tester.testCopyConstructorNormal()) {
        cout << "\tTest passed!" << endl;
    } else {
        cout << "\t***Test failed!***" << endl;
    }
    cout << "Testing copy constructor - edge:" << endl;
    if (tester.testCopyConstructorEdge()) {
        cout << "\tTest passed!" << endl;
    } else {
        cout << "\t***Test failed!***" << endl;
    }

    cout << "\nTesting assignment operator - normal:" << endl;
    if (tester.testAssignmentNormal()) {
        cout << "\tTest passed!" << endl;
    } else {
        cout << "\t***Test failed!***" << endl;
    }
    cout << "Testing assignment operator - edge:" << endl;
    if (tester.testAssignmentEdge()) {
        cout << "\tTest passed!" << endl;
    } else {
        cout << "\t***Test failed!***" << endl;
    }

    return 0;
}

int priorityFn1(const Student &student) {
    //this function works with a MAXHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [0-10]
    //level + major + group
    //the highest priority would be 3+4+3 = 10
    //the lowest priority would be 0+0+0 = 0
    //the larger value means the higher priority
    int priority = student.getLevel() + student.getMajor() + student.getGroup();
    return priority;
}

int priorityFn2(const Student &student) {
    //this function works with a MINHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [0-10]
    //race + gender + income + highschool
    //the highest priority would be 0+0+0+0 = 0
    //the lowest priority would be 2+2+4+2 = 10
    //the smaller value means the higher priority
    int priority = student.getRace() + student.getGender() + student.getIncome() + student.getHighschool();
    return priority;
}

