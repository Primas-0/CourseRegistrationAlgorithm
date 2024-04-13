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
    //Test insertion for a normal case of min-heap. After a decent number of insertion (e.g. 300 nodes) we traverse the tree and check that the heap property is satisfied at every node.
    bool testHeapPropertyAfterInsertMINHEAP();

    //Test insertion for a normal case of max-heap. After a decent number of insertion (e.g. 300 nodes) we traverse the tree and check that the heap property is satisfied at every node.


    //Test removal for a normal case of min-heap. After a decent number of insertion (e.g. 300 nodes) we check whether all removals happen in the correct order.


    //Test removal for a normal case of max-heap. After a decent number of insertion (e.g. 300 nodes) we check whether all removals happen in the correct order.


    //Test all nodes in a leftist heap have the correct NPL value.


    //Test a leftist heap preserves the property of such a heap, i.e. at every node the NPL value of the left child is greater than or equal to the NPL value of the right child.


    //Test whether after changing the priority function a correct heap is rebuilt with the same data (nodes) and the different priority function.


    //Test merge of an empty queue (an edge case) with a normal queue. This is a call to the function RQueue::mergeWithQueue(RQueue& rhs) where rhs is a normally populated queue.


    //Test the RQueue class copy constructor for a normal case.


    //Test the RQueue class copy constructor for an edge case.


    //Test the RQueue class assignment operator for a normal case.


    //Test the RQueue class assignment operator for an edge case.


    //Test that attempting to dequeue an empty queue throws an out_of_range exception.


    //Test that attempting to merge queues with different priority functions throws a domain_error exception.


private:
    void insertMultipleStudents(RQueue &myQueue);

    bool checkHeapProperty(Node *node, prifn_t priorFunc, HEAPTYPE heapType);
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
        //recurse until both left and right sub-heaps satisfy heap property
        if (!checkHeapProperty(node->m_left, priorFunc, heapType) ||
            !checkHeapProperty(node->m_right, priorFunc, heapType)) {
            return false;
        }

        int currPriority = priorFunc(node->m_student);
        int leftChildPriority = 0;
        int rightChildPriority = 0;

        if (node->m_left != nullptr) {
            //if there is a left child, get its priority
            leftChildPriority = priorFunc(node->m_left->m_student);
        } else {
            //otherwise, assign a default priority
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
            //otherwise, assign a default priority
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
        //if comparison fails
        return false;
    }
    //if heap is empty
    return true;
}

bool Tester::testHeapPropertyAfterInsertMINHEAP() {
    RQueue myQueue(priorityFn2, MINHEAP, SKEW);
    insertMultipleStudents(myQueue);

    return checkHeapProperty(myQueue.m_heap, priorityFn2, MINHEAP);
}

int main() {
    Tester tester;

    cout << "\nTesting insertStudent - check whether heap property is satisfied after insertion into min-heap:" << endl;
    if (tester.testHeapPropertyAfterInsertMINHEAP()) {
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

