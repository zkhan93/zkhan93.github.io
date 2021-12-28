from typing import DefaultDict


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


def deserialize(arr):
    nodes = [None] * len(arr)
    for index, data in enumerate(arr):
        if data:
            node = nodes[index] or Node(data)
            left_index = index * 2 + 1
            right_index = index * 2 + 2
            if left_index < len(arr) and arr[left_index]:
                nodes[left_index] = Node(arr[left_index])
                node.left = nodes[left_index]
            if right_index < len(arr) and arr[right_index]:
                nodes[right_index] = Node(arr[right_index])
                node.right = nodes[right_index]
            nodes[index] = node
            
            if right_index == len(arr):
                break
    return nodes[0] if nodes else None

def serialize(root):
    h = get_height(root)
    arr = [None] * ((2**h) - 1)
    stack = [(0, root)]
    while stack:
        index, node = stack.pop()
        arr[index] = node.data
        if node.left:
            stack.append((index*2+1, node.left))
        if node.right:
            stack.append((index*2+2, node.right))
    return arr

def get_unit_space_gen():
        x = 0
        pow = 0
        while True:
            x = x + 2**pow
            pow += 1
            yield x

def print_tree(root):
    h = get_height(root)
    queue = [(1,root)]
    nodes_by_level = {l:[] for l in range(1,h+1)}
    while queue:
        level, node = queue.pop(0)
        nodes_by_level.get(level, []).append(str(node.data) if node else ' ')
        if node:
            queue.append((level+1, node.left))
            queue.append((level+1, node.right))
    
    node_width = max(len(n) for _, nodes in nodes_by_level.items() for n in nodes ) + 2
    space_char = ' '
    prefix_char = ' '
    fill_char = '-'
    
    cjust = lambda s, n: s.rjust(((n + len(s))//2), fill_char).ljust(n, fill_char)
    unit_space_gen = get_unit_space_gen()
    prefix_space_gen = get_unit_space_gen()
    lines = []
    for level in range(h, 0, -1):
        nodes = nodes_by_level[level]
        nodes = [cjust(v, node_width) for v in nodes]
        mul = next(unit_space_gen)
        space = space_char * mul * node_width
        line = space.join(nodes)
        if level == h:
            prefix = ""
        else:
            prefix = prefix_char * (next(prefix_space_gen) * node_width)
        lines.append(prefix + line)
    print('\n'.join(lines[::-1]))

# print_tree(deserialize([1,2,3,4,5,None,7,8,9,None,10,None,11,12,13]))