---
title: "Understanding Linux load average"
date: 2021-11-26T21:48:41+05:30
tag: linux
---

```bash
>>> uptime
09:20:04 up 13:22,  5 users,  load average: 0.11, 0.31, 0.42
```

Load average are those three number, its these to give and idea of how busy the system is.

It shows load for last 1, 5 and 15 mins,
A common misconception is that it just shows how busy the CPU is, however it also takes other IO operation into consideration.


#### [This](https://www.brendangregg.com/blog/2017-08-08/linux-load-averages.html) blog post explains it beautifully what those three numbers mean

