---
title: "IP Based Throttling"
date: 2021-12-19T20:29:40+05:30
draft: true
tag: algo
---

I was in the third round of Goldman Sachs job interview process. I didn't clear it as I was struggling to solve the following problem.
I was able to get to a solution after a couple of hints.

The problem was to produce an algorithm to throttle traffic based on IP in real-time.
Clients should be allowed to make 100 requests in 5 seconds sliding window.

Initially, I was trying to save the IP and time of the clients in a dictionary-like structure so that later we can look up the time for an IP and do some math and check if it is within the last 5 seconds or not. The problem with this approach was that the dictionary would continue to grow. We would have to loop over the dictionary to clear out the old entries. This makes the time complexity of the algorithm to be linear but also increasing over time.

After rejecting the above idea, I started to think of a simple data structure to solve the problem.
We can have a fixed-length array which when full can start to drop items from the other end when an entry gets added from the other end.
An item in array contains the client IP and the request time. for every request we start to loop from the last item in the array to the first and for each item we check if the IP matches the new one then the current time and the time stored in the item should be less than 5 seconds if that is true we move ahead.

