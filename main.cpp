#include "unionfind.h"
#include "stack.h"
#include "queue.h"
#include "queue_policy_based.h"
#include "sorts.h"
#include "5algs.h"
#include "priority_queue.h"
#include "bst.h"
#include "llrb.h"
#include "hash_table.h"
#include "threads.h"
#include "applications/percolation.h"
#include "simple_deque.h"
#include "random_queue.h"

int main() {
    testUF();
    testStack();
    testQueue();
    testPolicyBasedQueues();
    testSorts();
    testGrahamScan();
    test5Algs();
    testPriorityQueue();
    testBST();
    testLLRB();
    testHashTable();
    testThreads();
    PercolationStats(50, 100);
    testDeque();
    testRandomQueue();
    return 0;
}