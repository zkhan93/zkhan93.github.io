import time
import random

class Node:
    def __init__(self, ip):
        self.ip = ip
        self.time = int(time.time())

    def __repr__(self):
        return f"Node({self.ip}, {self.time})"


def do(arr, item):
    item.time = time.time()
    count = 0
    for i in range(len(arr) - 1, -1, -1):
        tmp = arr[i]
        if tmp.ip == item.ip:
            if item.time - tmp.time >= 50:
                # only retain items after ith position
                arr = arr[i:]
                break
            else:
                count += 1
    if count < 2:
        arr.append(item)

arr = []
for _ in range(5):
    ip = random.randint(15, 20)
    print(ip)
    time.sleep(random.randrange(0, 1))
    do(arr, Node(ip))
    print(arr)
