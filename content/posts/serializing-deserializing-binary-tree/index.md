---
title: "Serializing & Deserializing Binary Tree"
date: 2021-12-09T13:46:58+05:30
tags: [binary-tree]
---

We are going to serialize a binary tree to an array,
A binary tree is a tree data structure. Each node can have 0-2 children(s).

visualization of a binary tree
```
        2
       / \
      /   \
     /     \
    1       3
   / \     / \
  0   7   9   1
 /   / \     / \
2   1   0   8   8
```

### Representation of a Node
A node should contain the data(value) and reference to child nodes. 
A simple class like `Node` below will work for us.

```python
class Node:
    data = None
    left = None
    right = None

    def __init__(self, data):
        self.data = data

```

### Height of a Binary Tree
Height is the max level of nodes in a tree. In the example above tree's height is 4.
Given a reference to a tree's root node, we can find its height as below

```python
def get_height(root):
    if root is None:
        return 0
    stack = [(1, root)]
    max_level = 1
    while stack:
        level, node = stack.pop()
        if level > max_level:
            max_level = level 
        if node.left:
            stack.append((level + 1, node.left))
        if node.right:
            stack.append((level + 1, node.right))
    return max_level
```
We use a stack to traverse the tree. Each entry in the stack contains the node's level and the node itself.
As we traverse the tree we keep track of the highest level seen so far. We add 1 to the current level while adding children of the current node to a stack. In the end we will have the tree's height in max level.

An interesting thing to note here is we can also use Queue to traverse the tree, however, a sibling will be visited earlier than a child, hence making it a BFS (Breadth-First Search) instead of a DFS (Depth-First Search)
We have to traverse the complete tree, hence, the time complexity of finding out the height of the tree will be O(n)

### Serialization
We can represent a binary tree with an array, where left and right children of a node at index `n` can be placed at `(n * 2) + 1` and `(n * 2) + 2` index respectively.

To represent a Binary Tree we would need an array to hold all the nodes.
Our array length is dependent on the tree's height and since a binary tree might have some missing subtree,
but the array has to have space for all items as if the tree is a complete binary tree.
The length of this array will be `2**h - 1`, `h` being the tree's height.

Once we have size of array finalized, we can traverse the tree to fill our array.
```python
def serialize(root):
    h = get_height(root)
    arr = [None] * ((2**h) - 1)
    stack = [(0, root)]
    while stack:
        index, node = stack.pop()
        arr[index] = node.data
        if node.left:
            stack.append((index*2 + 1, node.left))
        if node.right:
            stack.append((index*2 + 2, node.right))
    return arr
```
Our stack will contain tuples of 2 items, a node with its index.
We start with a stack like `[(0, root)]`. In one iteration, we pop one item from the stack to get `node` and `index` and assign node's data to `arr[index]`, after that we place children of the node to the top of stack.


### De-Serialization
we should be able to create the tree back from the array we got above.

```python
def deserialize(arr):
    nodes = [None] * len(arr)
    for index, data in enumerate(arr):
        if data:
            node = nodes[index] or Node(data)
            left_index = index * 2 + 1
            right_index = index * 2 + 2
            if left_index < len(arr) and arr[left_index]:
                node.left = Node(arr[left_index])
            if right_index < len(arr) and arr[right_index]:
                node.right = Node(arr[right_index])
            nodes[index] = node
            
            if right_index == len(arr):
                break
    return nodes[0] if nodes else None
```
we start by iterating through the array in each iteration we create a `Node` object with the current value and check if child positions do not exceed the array length then initialize those child nodes and link them to parent.

and with that we were able to serialize and deserialize the tree by calculating its height if not given then traversing through the tree once.

