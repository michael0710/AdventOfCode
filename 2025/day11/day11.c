#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "../../puzzle_solver.h"

typedef struct StrNodeType NodeType;

struct StrNodeType
{
    uint16_t id;
    uint16_t* connIds;
    NodeType** connRefs;
    size_t connsCount;
    int64_t cache;
};

void free_rainer(NodeType* nodes, size_t count)
{
    for (size_t i=0; i<count; i++)
    {
        if (nodes != NULL)
        {
            free(nodes[i].connIds);
            nodes[i].connIds = NULL;
            free(nodes[i].connRefs);
            nodes[i].connRefs = NULL;
            nodes[i].connsCount = 0;
        }
    }
}

void clear_cache(NodeType* nodes, size_t count)
{
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Clearing cache for next step ... ");
#endif
    for (size_t i=0; i<count; i++)
    {
        nodes[i].cache = -1;
    }
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("done\n");
#endif
}

int compar_node_w_id(const void *a, const void *b)
{
    int64_t rv = (int64_t)(*((uint16_t*)a)) - (int64_t)(((NodeType*)b)->id);
    return (rv > INT_MAX) ? INT_MAX : ((rv < INT_MIN) ? INT_MIN : (int)(rv));
}

int compar_nodes(const void *a, const void *b)
{
    int64_t rv = (int64_t)(((NodeType*)a)->id) - (int64_t)(((NodeType*)b)->id);
    return (rv > INT_MAX) ? INT_MAX : ((rv < INT_MIN) ? INT_MIN : (int)(rv));
}

int64_t find_no_of_paths(volatile NodeType* startNode,
                         const NodeType* endNode)
{
#if 0
#ifdef VERBOSE_DEBUG_OUTPUT
/** DICTIONARY:
 * 44958 "you"
 * 37431 "svr"
 * 32213 "out"
 * 17220 "dac"
 * 20009 "fft"
 */
    if (startNode->id = 44958)
    {
        printf("Reached node 'you'\n");
    }
    else if (startNode->id = 37431)
    {
        printf("Reached node 'svr'\n");
    }
    else if (startNode->id = 32213)
    {
        printf("Reached node 'out'\n");
    }
    else if (startNode->id = 17220)
    {
        printf("Reached node 'dac'\n");
    }
    else if (startNode->id = 20009)
    {
        printf("Reached node 'fft'\n");
    }
#endif
#endif
    if (startNode->id == endNode->id)
    {
        return 1;
    }

    int64_t rv = 0;
    for (size_t i=0; i<startNode->connsCount; i++)
    {
        if ((startNode->connRefs[i]->cache) == -1)
        {
            startNode->connRefs[i]->cache = find_no_of_paths(startNode->connRefs[i], endNode);
        }
        rv += startNode->connRefs[i]->cache;
    }
    return rv;
}

int solve_puzzle(FILE* input)
{
#define MAX_INPUT_LINE_LENGTH   128
#define REALLOC_STEP_SIZE        2
    int funcRv = 0;

    char tempBuf[MAX_INPUT_LINE_LENGTH];
    size_t nodeInputSize = REALLOC_STEP_SIZE;
    NodeType* nodeInput = (NodeType*)malloc(sizeof(NodeType)*nodeInputSize);
    size_t nodeInputCounter = 0;
    bool leaveNow = false;
    while ((funcRv == 0) && (leaveNow == false))
    {
        char* rv = fgets(tempBuf, MAX_INPUT_LINE_LENGTH, input);
        if (rv == NULL)
        {
            strncpy(tempBuf, "out:", 4);
            tempBuf[4] = '\0';
            leaveNow = true;
        }
        if (strnlen(tempBuf, MAX_INPUT_LINE_LENGTH) >= (MAX_INPUT_LINE_LENGTH-1))
        {
            funcRv = -1;
            printf("ERROR: Input line too long!\n");
            break;
        }

        if (nodeInputCounter >= nodeInputSize)
        {
            nodeInputSize += REALLOC_STEP_SIZE;
            NodeType* newNodeInput = (NodeType*)realloc(nodeInput, nodeInputSize*sizeof(NodeType));
            if (newNodeInput == NULL)
            {
                funcRv = -1;
                printf("ERROR: realloc() failed!\n");
                break;
            }
            nodeInput = newNodeInput;
        }

        /* get node id */
        char* startptr = tempBuf;
        char* endptr = NULL;
        nodeInput[nodeInputCounter].id = strtoul(startptr, &endptr, 36);
        if ((*endptr) != ':')
        {
            funcRv = -1;
            printf("ERROR: expected ':' after node id!\n");
            break;
        }
        endptr++;
        nodeInput[nodeInputCounter].connsCount = 0;
        nodeInput[nodeInputCounter].connIds = NULL;
        while (funcRv == 0)
        {
            startptr = endptr;
            uint16_t tempId = strtoul(startptr, &endptr, 36);
            if (startptr == endptr)
            {
#ifdef VERBOSE_DEBUG_OUTPUT
                printf("    end of node %zu with id=%lu reached\n", nodeInputCounter, nodeInput[nodeInputCounter].id);
#endif
                break;
            }
            nodeInput[nodeInputCounter].connsCount++;
            uint16_t* newConnIds = (uint16_t*)realloc(nodeInput[nodeInputCounter].connIds, nodeInput[nodeInputCounter].connsCount*sizeof(uint16_t));
            if (newConnIds == NULL)
            {
                funcRv = -1;
                printf("ERROR: realloc() failed for connection ids of node %zu!\n", nodeInputCounter);
                break;
            }
            nodeInput[nodeInputCounter].connIds = newConnIds;
            nodeInput[nodeInputCounter].connIds[nodeInput[nodeInputCounter].connsCount-1] = tempId;
        }
        if (funcRv != 0)
        {
            break;
        }
        nodeInputCounter++;
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("The input consists of %zu nodes\n", nodeInputCounter);
    printf("The nodes are:\n");
    for (size_t i=0; i<nodeInputCounter; i++)
    {
        printf("  %lu has connections to\n", nodeInput[i].id);
        for (size_t k=0; k<nodeInput[i].connsCount; k++)
        {
            printf("    %lu\n", nodeInput[i].connIds[k]);
        }
    }
#endif

    /* if we sort the nodes by id here, we can use bsearch() instead of
    iterating through the whole array */
    qsort(nodeInput, nodeInputCounter, sizeof(NodeType), compar_nodes);

    /* search the node ids and make direct references */
    for (size_t i=0; i<nodeInputCounter; i++)
    {
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("Establishing references of node id %lu\n", nodeInput[i].id);
#endif
        nodeInput[i].connRefs = (NodeType**)malloc(sizeof(NodeType*)*nodeInput[i].connsCount);
        if (nodeInput[i].connRefs == NULL)
        {
            funcRv = -1;
            printf("ERROR: malloc for nodeRefs failed for node with id %lu", nodeInput[i].id);
            break;
        }
        for (size_t k=0; k<nodeInput[i].connsCount; k++)
        {
            NodeType* startNode = (NodeType*)bsearch(&(nodeInput[i].connIds[k]), nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
            nodeInput[i].connRefs[k] = startNode;
        }
    }

    clear_cache(nodeInput, nodeInputCounter);

#if BUILD_SOLUTION == 1
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Finding paths from 'you' to 'out' ... ");
#endif
    uint16_t startId = 44958; /* "you" converted with base 36 integer */
#elif BUILD_SOLUTION == 2
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Finding paths from 'fft' to 'out' ... ");
#endif
    uint16_t startId = 20009; /* "svr" converted with base 36 integer */
#endif
    uint16_t destId = 32213; /* "out" converted with base 36 integer */
    int64_t possiblePaths = 0;

    if (funcRv == 0)
    {
        NodeType* startNode = (NodeType*)bsearch(&startId, nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
        NodeType* endNode = (NodeType*)bsearch(&destId, nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
        if (   (startNode != NULL)
            && (endNode != NULL))
        {
            possiblePaths = find_no_of_paths(startNode, endNode);
        }
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("%llu\n", possiblePaths);
#endif

/** DICTIONARY:
 * 44958 "you"
 * 37431 "svr"
 * 32213 "out"
 * 17220 "dac"
 * 20009 "fft"
 */

#if BUILD_SOLUTION == 2
    clear_cache(nodeInput, nodeInputCounter);
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Finding paths from 'dac' to 'out' ... ");
#endif

    startId = 17220; /* "dac" converted with base 36 integer */
    destId = 32213; /* "out" converted with base 36 integer */
    int64_t possiblePathsDacOut = 0;

    if (funcRv == 0)
    {
        NodeType* startNode = (NodeType*)bsearch(&startId, nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
        NodeType* endNode = (NodeType*)bsearch(&destId, nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
        if (   (startNode != NULL)
            && (endNode != NULL))
        {
            possiblePathsDacOut = find_no_of_paths(startNode, endNode);
        }
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("%llu\n", possiblePathsDacOut);
#endif

    clear_cache(nodeInput, nodeInputCounter);

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Finding paths from 'fft' to 'dac' ... ");
#endif

    startId = 20009; /* "fft" converted with base 36 integer */
    destId = 17220; /* "dac" converted with base 36 integer */
    int64_t possiblePathsFftDac = 0;

    if (funcRv == 0)
    {
        NodeType* startNode = (NodeType*)bsearch(&startId, nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
        NodeType* endNode = (NodeType*)bsearch(&destId, nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
        if (   (startNode != NULL)
            && (endNode != NULL))
        {
            possiblePathsFftDac = find_no_of_paths(startNode, endNode);
        }
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("%llu\n", possiblePathsFftDac);
#endif
    clear_cache(nodeInput, nodeInputCounter);
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Finding paths from 'dac' to 'fft' ... ");
#endif

    startId = 17220; /* "dac" converted with base 36 integer */
    destId = 20009; /* "fft" converted with base 36 integer */
    int64_t possiblePathsDacFft = 0;

    if (funcRv == 0)
    {
        NodeType* startNode = (NodeType*)bsearch(&startId, nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
        NodeType* endNode = (NodeType*)bsearch(&destId, nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
        if (   (startNode != NULL)
            && (endNode != NULL))
        {
            possiblePathsDacFft = find_no_of_paths(startNode, endNode);
        }
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("%llu\n", possiblePathsDacFft);
#endif
    clear_cache(nodeInput, nodeInputCounter);
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Finding paths from 'svr' to 'dac' ... ");
#endif

    startId = 37431; /* "svr" converted with base 36 integer */
    destId = 17220; /* "dac" converted with base 36 integer */
    int64_t possiblePathsSvrDac = 0;

    if (funcRv == 0)
    {
        NodeType* startNode = (NodeType*)bsearch(&startId, nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
        NodeType* endNode = (NodeType*)bsearch(&destId, nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
        if (   (startNode != NULL)
            && (endNode != NULL))
        {
            possiblePathsSvrDac = find_no_of_paths(startNode, endNode);
        }
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("%llu\n", possiblePathsSvrDac);
#endif
    clear_cache(nodeInput, nodeInputCounter);
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Finding paths from 'svr' to 'fft' ... ");
#endif

    startId = 37431; /* "svr" converted with base 36 integer */
    destId = 20009; /* "fft" converted with base 36 integer */
    int64_t possiblePathsSvrFft = 0;

    if (funcRv == 0)
    {
        NodeType* startNode = (NodeType*)bsearch(&startId, nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
        NodeType* endNode = (NodeType*)bsearch(&destId, nodeInput, nodeInputCounter, sizeof(NodeType), compar_node_w_id);
        if (   (startNode != NULL)
            && (endNode != NULL))
        {
            possiblePathsSvrFft = find_no_of_paths(startNode, endNode);
        }
    }
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("%llu\n", possiblePathsSvrFft);
#endif

#if 0
    possiblePaths /* fft out */
    possiblePathsDacOut
    possiblePathsFftDac
    possiblePathsDacFft
    possiblePathsSvrDac
    possiblePathsSvrFft
#endif
    possiblePaths = possiblePathsSvrDac*possiblePathsDacFft*possiblePaths
                + possiblePathsSvrFft*possiblePathsFftDac*possiblePathsDacOut;
#endif
    free_rainer(nodeInput, nodeInputCounter);
    free(nodeInput);

    printf("There are %lld different paths from me to out\n", possiblePaths);
    return funcRv;
}
