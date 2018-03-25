/**
 * @file node.h
 * @author Sayed Sadeed
 * @brief The Node module represents a "node" inside an AST.
 */

#ifndef NODE_H
#define NODE_H

#include <string.h>

/**
 * @brief We may have different types of Nodes. Each one corresponds to the unique production in the grammar. 
 */
enum NodeType {E_EOF_NODE, E_ASSIGNMENT_NODE, E_GROUP_NODE};

/**
 * @brief Node correlates to a node within a tree.
 * 
 * This is used to map tokens to an AST. Each node has a left value, right value and
 * its centre/root.
 */
struct Node {
    struct Node *left;
    char *value;
    struct Node *right;
};

/**
 * @brief NodeMgr manages holds all the nodes at the root level.
 * 
 * This provides a high level interfacing for the syntax tree. It is preferred to use this
 * for anything node related as it manages internal memory allocs and deallocs.
 * 
 */
typedef struct {
    struct Node **nodes; 
    size_t nodes_ctr;
    size_t nodes_cap;
} NodeMgr;

/**
 * @brief Create new node instance.
 * 
 * Will create a new node instance irrespective of NodeMgr.
 * 
 * @return Pointer to newly created node or null ptr if something went wrong.
 */
struct Node *Node_new(void);

/**
 * @brief Add an existing Node to the internal NodeMgr store.
 * 
 * This will allow the addition of externally created Node into the NodeMgr. 
 * Please note that it is expected *node will be a pointer to Node created on heap. To avoid unexpected 
 * behavior it is recommended to use Node_new() then add the returned pointer using this function.
 * 
 * @param node_mgr NodeMgr instance.
 * @param node Node instance to be added.
 * @return 0 if successful otherwise 1.
 */
 int NodeMgr_add_node(NodeMgr *node_mgr, struct Node *node);

/**
 * @brief Free all resources creates by node manager. Including node manager itself.
 *
 * @param node_mgr Pointer to the NodeMgr instance.
 * @return 1 if anything went wrong other return 0.
 */
int NodeMgr_free(NodeMgr *node_mgr);

/**
 * @brief Create node manager malloc'ed.
 * 
 * This function acts as a constructor for the Node manager.
 * 
 * @return newly created NodeMgr pointer.
 */
NodeMgr *NodeMgr_new(void);

/**
 * @brief Perform relloc on array of of nodes in Manager.
 * 
 * This function is to be used internally by NodeMg to allocate more
 * room if need be for node storage. 
 * 
 * @param node_mgr Instance of NodeMgr.
 * @return Newly allocated Node**.
 */
struct Node **grow_nodes(NodeMgr *node_mgr);

#endif
