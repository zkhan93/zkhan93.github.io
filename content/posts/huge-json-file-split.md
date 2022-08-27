---
title: "Huge Json File Split"
date: 2022-08-24T17:32:49-05:00
draft: true
tag: JSON Spark split
---


Today we are going to look at a problem which deals with a huge JSON file.

The actual JSON data was in the neighborhood of 1TB,
But for quicker results we tested our solution with a smaller sample file of ~12GB.

in the JSON majority of content was contained in 2 arrays like below
``` 
{
    ...
    "in_network": [
        ....
    ],
    "preferences": [
        ....
    ]
    ...
}
```
my friend made some attempts to load this data in Spark, however he failed to do so, since both these array were going as a column's value and due to memory issues it was failing.
the solution was to pre-process the data and ditch the outer JSON structure and have one object per line in a file
```
{...}
{...}
{...}
{...}
```
we were able to load this structure to spark with ease.

Now, we had different problem, how do we convert the original structure to this?
we tried a couple of JSON parsers like `jq` with stream processing and other python libraries like `ijson`, but we quickly understood any JSON parser program will parse and create structure and it will be slow for our use case....
we didn't really care about the structure, we just wanted to identify an object in the array and it had to be super quick dues to the sheer amount of data we had to process.

So, we wrote a little program which takes the starting positing of the array and runs though the file only keeping track of two characters '{' and '}', Now when it finds a matching '}' for every '{' encountered, It says "Hey! I have an object starting from i to j", that information is then used to dump an object in output file, and the program continues till the EOF

even with this low cost parsing, a single process was too slow for us. we had to parlalise  this


