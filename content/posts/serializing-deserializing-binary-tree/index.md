---
title: "Serializing & Deserializing Binary Tree"
date: 2021-12-09T13:46:58+05:30
tags: [binary-tree]
draft: true
---

We are going to serialize a binary tree to an array,
A binary tree is a tree data structure, each node of which can have 0-2 children(s).

this is a binary tree for visual understanding
```
     1
    / \
   /   \
  2     \
 / \     3
4   5   / \
       9   \
            8
           / \
          6   7
```


we can represent this tree in an array 

```python

class Node:
    data = None
    left = None
    right = None

    def __init__(self, data):
        self.data = data

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


def serialize(root):
    h = get_height()
    arr = [None] * ((2**h) - 1)
    stack = [(0, head)]
    while stack:
        index, node = stack.pop()
        arr[index] = node.data
        if node.left:
            stack.append((index*2+1, node.left))
        if node.right:
            stack.append((index*2+2, node.right))
    return arr

def deserialize(arr):
    nodes = []
    for index, data in enumerate(arr):
        if data:
            node = Node(data)
            left_index = index * 2 + 1
            right_index = index * 2 + 2
            if left_index < len(arr) and arr[left_index]:
                node.left = Node(arr[left_index])
            if right_index < len(arr) and arr[right_index]:
                node.right = Node(arr[right_index])
            nodes[index] = node
            
            if right_index == len(arr):
                break
    return node[0] if nodes else None
```