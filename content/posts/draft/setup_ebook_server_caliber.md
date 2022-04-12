---
title:  "Setup your own E-book server Caliber"
date:   2020-01-02 12:00:00
categories: [diy]
tags: ["caliber", "raspberry-pi"]
draft: true
---

I had a lot of PDF and E-books on my hard disk from my college time, A lot of them, so I thought, In the world full of amazing software products isn't there a product to organize these PDFs and E-books ?
The answer I settled on was Calibre.

Calibre at first can be confusing, I struggled a bit while setting it up and hence this post.
Calibre is a tool (executable binary) that can do a lot of things. It has some GUI implementations also, however I wanted some web-based solution so that I can host it on my home server.

so its a tool that maintains a database (SQLite) to keep track of imported books and their properties.
you say something like "This is a folder full of PDF and E-books please import it" and it goes and parses all the PDF try to extract some basic information and add it to a database file.

Calibre library - is the folder location where all your PDFs or E-Books are present 
Calibre Database - is the SQLite database file that it generated
