/* compile: clang object_move.c -lnuma */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <assert.h>
#include <time.h>
#include <unistd.h>

/* third party */
#include <numa.h>
#include <numaif.h>

#define SECS_AS_NANOS 1000000000
#define BUFFER_SIZE 256
#define ITERATIONS 25

#define pass_or_exit(pass_cond, msg) \
    do { \
        if (!(pass_cond)) { \
            perror(msg); \
            exit(EXIT_FAILURE); \
        } \
    } while (0);

struct object {
    size_t size;
};

typedef enum {
    MEMCPY,
} migration_strategy_t;

typedef struct object * object_t;
typedef uint64_t nanos_t;

nanos_t elapsed_as_nanos(struct timespec* start, struct timespec* end);
nanos_t timed_object_move(size_t size, migration_strategy_t how, bool is_remote);
FILE* create_csv(char* name);
char* strategy_to_str(migration_strategy_t how);

void* alloc_on_node(size_t size, int node) {
    void* memory = NULL;
    memory = numa_alloc_onnode(size, node);
    return memory;
}

void* alloc_local(size_t size) {
    // either use numa_alloc_local
    // or the function we defined above with value 0
    void* memory = numa_alloc_local(size);
    //void* memory = alloc_on_node(size, 0);
    return memory;
}

void find_memory_node_for_addr(void* ptr) {
    int numa_node = -1;
    if(get_mempolicy(&numa_node, NULL, 0, ptr, MPOL_F_NODE | MPOL_F_ADDR) < 0) {
        perror("get_mempolicy failed");
    }
    printf("numa node: %d\n", numa_node);
}

// initalization of remote memory node
int REMOTE_NODE = -1;

int main(void) {
    // libnuma stuff
    bool has_numa = numa_available() != -1 ? true : false;
    printf("numa available? %s\n", has_numa ? "true" : "false");
    if (!has_numa) {
        fprintf(stderr, "exiting now ...");
        exit(EXIT_FAILURE);
    }

    // query nodes available
    int num_nodes = numa_num_configured_nodes();
    int max_node_num = numa_max_node();
    int min_node_num = 0;

    printf("number of nodes: %d, max node number: %d\n",
        num_nodes, max_node_num
    );

    assert(num_nodes == 2);

    // Is autonuma enabled?
    //
    // See: https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/html/virtualization_tuning_and_optimization_guide/sect-virtualization_tuning_optimization_guide-numa-auto_numa_balancing
    // Auto NUMA is enabled when:
    // - `numactl --hardware` shows multiple nodes
    // - `cat /proc/sys/kernel/numa_balancing` shows 1

    // numa_allocate_nodemask - zero filled mask
    // numa_bitmask_setbit
    // copy_bitmask_to_bitmask

    // bind current task to one node
    // uses nodemask
    // numa_bind()
    // numa_run_on_node
    int ret = 0;
    ret = numa_run_on_node(min_node_num);
    pass_or_exit(ret == 0, "failed to bind process to node");

    REMOTE_NODE = 1;


    // allocate remote and local memory
    // don't forget to call NUMA free
    size_t alloc_size = 4096 * 3;
    void* mem1 = alloc_on_node(alloc_size, REMOTE_NODE);
    void* mem2 = alloc_local(alloc_size);

    printf("remote allocation: %p\n", mem1);
    printf("local allocation: %p\n", mem2);

    // actually page in/read/write to memory
    *((uint32_t *) mem1) = 0xdeadbeef;
    *((uint32_t *) mem2) = 0xcafed00d;

    printf("values in memory: %x %x\n", *((uint32_t *) mem1), *((uint32_t *) mem2));

    find_memory_node_for_addr(mem1);
    find_memory_node_for_addr(mem2);

    //printf("exiting now...\n");
    printf("pid %ju\n", (uintmax_t)getpid()); 
    printf("wating ...\n");
    // while(1);

    return EXIT_SUCCESS;
}

