---
title: "Conky - Lightweight System Monitor"
date: 2021-11-27T11:22:01+05:30
cover:
    image: https://github.com/zkhan93/conky/raw/master/desktop.png
    alt: Conky Desktop
---

I use ubuntu as my daily driver so all the commands are going to be ubuntu/debian based
### Installation
Install conky-all package by `sudo apt-get install conky-all`

### Setup
conky takes a config file to know what to draw on screen.
the default location of config file is `~/.conkyrc`

### Run
you can start the conky by just typing `conky` in terminal, and a and ugly black window should come up.

### Customization
There are thousands of options to pick conky config, and if that's not enough you can just google for more.
I'll add a link to my favorites one here, 
[Conky Orange](https://www.gnome-look.org/p/1115398/) its 11 years old still looks decent

you can search for more on [gnome-look](https://www.gnome-look.org/browse?cat=124&ord=latest) or [deviantart](https://www.deviantart.com/tag/conky)

### My Conky Configurations
I have 3 conky configs setup for my use,
link to configs is here [zkhan93/conky](https://github.com/zkhan93/conky)

#### 1. System monitor and time
I use a modified version of conky Orange to use with my wallpaper

configs and scripts used are [zkhan93/conky/sidebar](https://github.com/zkhan93/conky/tree/master/sidebar)

#### 2. Quotes
A conky script that shows a quote from a JSON file on disk.

for this I used startup apps in gnome to trigger a python script that downloads a Github gist which contains a bunch of my favorite quotes and pick one randomly. 
I had to use little bit of Lua since the quote can be arbitrarily long so Lua function takes care of multi line and alignment of text based on the font in use and its size.

content of gist file have the following structure 
```json
[
  {
    "quote": "Patience Ensures Victory.",
    "author": "Ali ibn Abi Talib"
  }
]
```

configs and scripts used are [zkhan93/conky/quotes](https://github.com/zkhan93/conky/tree/master/quotes)

#### 3. parcel tracking
I want to stay updated on the location of my parcels, so invested some time to make this so the latest update on parcel is directly on my screen.
this conky config reads a special file, each line of this file contains tracking information, a bash script then make query to [tracktry](https://www.tracktry.com/) who provides free API query credits per month for people like me, the result of the query is then parsed to show the latest information.

source text file
```text
Raspberry Pi 4,bluedart,8931267875
Aeroplane,bluedart,893114466875
```
configs and scripts used are [zkhan93/conky/parcel](https://github.com/zkhan93/conky/tree/master/parcel)


#### Important links
- [conky variables](http://conky.sourceforge.net/variables.html)
- [conky wiki](https://github.com/brndnmtthws/conky/wiki)