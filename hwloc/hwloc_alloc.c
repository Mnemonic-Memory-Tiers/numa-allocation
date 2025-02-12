/* Example hwloc API program.
 *
 * See other examples under doc/examples/ in the source tree
 * for more details.
 *
 * Copyright © 2009-2016 Inria.  All rights reserved.
 * Copyright © 2009-2011 Université Bordeaux
 * Copyright © 2009-2010 Cisco Systems, Inc.  All rights reserved.
 * See COPYING in top-level directory.
 *
 * hwloc-hello.c
 */

#include "hwloc.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static void print_children(hwloc_topology_t topology, hwloc_obj_t obj,
                           int depth)
{
    char type[32], attr[1024];
    unsigned i;

    hwloc_obj_type_snprintf(type, sizeof(type), obj, 0);
    printf("%*s%s", 2*depth, "", type);
    if (obj->os_index != (unsigned) -1)
      printf("#%u", obj->os_index);
    hwloc_obj_attr_snprintf(attr, sizeof(attr), obj, " ", 0);
    if (*attr)
      printf("(%s)", attr);
    printf("\n");
    for (i = 0; i < obj->arity; i++) {
        print_children(topology, obj->children[i], depth + 1);
    }
}

int main(void)
{
    int depth;
    unsigned i, n;
    unsigned long size;
    int levels;
    char string[128];
    int topodepth;
    void *mem1, *mem2;
    hwloc_topology_t topology;
    hwloc_cpuset_t cpuset;
    hwloc_obj_t obj;

    /* Allocate and initialize topology object. */
    hwloc_topology_init(&topology);

    /* ... Optionally, put detection configuration here to ignore
       some objects types, define a synthetic topology, etc....

       The default is to detect all the objects of the machine that
       the caller is allowed to access.  See Configure Topology
       Detection. */

    /* Perform the topology detection. */
    hwloc_topology_load(topology);

    /* Optionally, get some additional topology information
       in case we need the topology depth later. */
    topodepth = hwloc_topology_get_depth(topology);

    /* Get last node. There's always at least one. */
    printf("Allocating from another NUMA node\n");
    n = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_NUMANODE);
    obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_NUMANODE, n - 1);

    size = 1024*1024;
    printf("%d NUMA nodes\n", n);
    printf("Allocating %d from %d (%s)\n", size, n - 1, obj->name);
    mem1 = hwloc_alloc_membind(topology, size, obj->nodeset,
                            HWLOC_MEMBIND_BIND, HWLOC_MEMBIND_BYNODESET);
    // printf("pointer from hwloc_alloc: %p\n", m);


    int local = 0;
    obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_NUMANODE, local);
    printf("Allocating %d from %d (%s)\n", size, local, obj->name);
    mem2 = hwloc_alloc_membind(topology, size, obj->nodeset,
                            HWLOC_MEMBIND_BIND, HWLOC_MEMBIND_BYNODESET);
    printf("remote allocation: %p\n", mem1);
    printf("local allocation: %p\n", mem2);

    // actually page in/read/write to memory
    *((uint32_t *) mem1) = 0xdeadbeef;
    *((uint32_t *) mem2) = 0xcafed00d;

    printf("values in memory: %x %x\n", *((uint32_t *) mem1), *((uint32_t *) mem2));

    //printf("exiting now...\n");
    printf("pid %ju\n", (uintmax_t)getpid()); 
    printf("wating ...\n");

    while(1);
    hwloc_free(topology, mem1, size);
    hwloc_free(topology, mem2, size);

    /* Destroy topology object. */
    hwloc_topology_destroy(topology);

    return 0;
}
