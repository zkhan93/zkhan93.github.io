---
title: "Journey of my Home Server"
date: 2022-02-06T10:52:44+05:30
categories: [diy]
tags: ["caliber", "openmediavault", "omv", "raspberry-pi"]
---


I had a couple of Raspberry Pi 3 (a credit card size SBC) with me, and I wanted to build something to satisfy my DIY spirit in me.
If you search on the Internet, everyone will suggest you build a Home Server :smile:
I had also recently replaced all the old HDD in home computers with SSDs, which means the systems were substantially fast now but had low storage capacity.
So the project of building a NAS or Home Server was a good fit for my home.

## Initial Setup
 - Raspberry pi 3B+
 - external powered USB hub
 - 2 X 2.5" spare internal HDD
 - 1 Laptop Charger to power the hub and raspberry pi 
 - 2 Buck converters to convert drop laptop charger voltage 
 - power button
 - 3D printer case to host all the above

## Operating System
There are a ton of OS to choose from, and you can build a server using any of these
some were general-purpose OS, others were tuned to a specific use case, for building a Home server that majorly serves as file storage, 
I went with OpenMediaVault. 
The Web interface is not the best in class, however, it works and provides all the features I needed. 
OMV also had good community support.


## Services that run on my home server
 - **Portrainer** - web interface to manage containers
 - **Calibre Library** - to Organize all E-Books
 - **Aria2 and Aria-web** - as download manager
 - **Nextcloud** - cloud storage 
 - **Plex** - Media Manager/Player
 - **Radarr** - Picks the correct torrent and schedule downloads of the content (uses aria2 as downloader)
 - **Jackett** - search different torrent and usernet sites to provide content to empower Radarr.
 - **Grafana** - Monitoring everything 
 - **Prometheus** - Monitoring everything 
 - **Loki** - collect logs from all the above services
 - **CCTV camera recording** - 4 ffmpeg docker containers to record different rtsp feeds from CCTV cams.
 - **traefik** - to router all traffic to proper services

I'll add some photos for the current setup and blog posts for some services listed above in more detail.