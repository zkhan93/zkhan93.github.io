---
title: "Duplicate File Finder Web Based Solution"
date: 2022-09-27T23:22:29-05:00
categories: [diy, selfhost]
tags: ["docker", "docker-compose", "rdfind", "web", "vuejs"]
---

I was trying to [backup my home server](/posts/backup-home-server-data/) when I saw my sister's folder had around 300GB worth of files, and since she had migrated/upgraded to different machines over time and finally moved all the data over to the home server. 
I knew some(~50GB) of that would be duplicate files.

So on a weekend, I decide to clean up some of that duplicate data before backing it up and start paying for it.

I wanted to find true duplicated i.e., based on file contents not the file names, there are great tools available in the market, but I picked [rdfind](https://rdfind.pauldreik.se/) as my tool for the task.

this is a quick run of `rdfind` to finds the duplicate files in a directory.
```text
Now scanning ".", found 11204 files.
Now have 11204 files in total.
Removed 0 files due to nonunique device and inode.
Total size is 21283217738 bytes or 20 GiB
Removed 4272 files due to unique sizes from list.6932 files left.
Now eliminating candidates based on first bytes:removed 59 files from list.6873 files left.
Now eliminating candidates based on last bytes:removed 20 files from list.6853 files left.
Now eliminating candidates based on sha1 checksum:removed 48 files from list.6805 files left.
It seems like you have 6805 files that are not unique
Totally, 3 GiB can be reduced.
Now making results file results.txt
```
So, as you can see it does a pretty good job of finding duplicate files based on contents, and it produces a results.txt file,
the result file looks like this

```text
# Automatically generated
# duptype id depth size device inode priority name
DUPTYPE_FIRST_OCCURRENCE 3912 4 43 2081 96993694 1 ./Documents/Home/Mammi/Untitled Page_files/1x1.gif
DUPTYPE_WITHIN_SAME_TREE -3912 4 43 2081 96993703 1 ./Documents/Home/Mammi/Untitled Page_files/acttr
DUPTYPE_WITHIN_SAME_TREE -3912 5 43 2081 96863141 1 ./Documents/Family and Friends/IDs/Untitled Page_files/1x1.gif
DUPTYPE_WITHIN_SAME_TREE -3912 5 43 2081 96863150 1 ./Documents/Family and Friends/IDs/Untitled Page_files/acttr
DUPTYPE_WITHIN_SAME_TREE -3912 5 43 2081 97126975 1 ./Documents/Ultimate Backup on hard disk/Backup/Mammi/Untitled Page_files/acttr
DUPTYPE_WITHIN_SAME_TREE -3912 5 43 2081 97126966 1 ./Documents/Ultimate Backup on hard disk/Backup/Mammi/Untitled Page_files/1x1.gif
DUPTYPE_FIRST_OCCURRENCE 5587 5 43 2081 96993615 1 ./Documents/Home/Abbu/sbi card/STATE BANK OF INDIA __ INDIA's LARGEST BANK_files/seg
DUPTYPE_WITHIN_SAME_TREE -5587 6 43 2081 97124458 1 ./Documents/Ultimate Backup on hard disk/Backup/Abbu/sbi card/STATE BANK OF INDIA __ INDIA's LARGEST BANK_files/seg
DUPTYPE_FIRST_OCCURRENCE 5574 5 72 2081 96993601 1 ./Documents/Home/Abbu/sbi card/STATE BANK OF INDIA __ INDIA's LARGEST BANK_files/abchk.js
DUPTYPE_WITHIN_SAME_TREE -5574 6 72 2081 97124444 1 ./Documents/Ultimate Backup on hard disk/Backup/Abbu/sbi card/STATE BANK OF INDIA __ INDIA's LARGEST BANK_files/abchk.js
....
DUPTYPE_WITHIN_SAME_TREE -2407 4 42405298 2081 96864595 1 ./Documents/Family and Friends/photos/Image.png
# end of file
```
a possible duplicate file can have 2 labels `DUPTYPE_FIRST_OCCURRENCE` when that file was found first,
 and `DUPTYPE_FIRST_OCCURRENCE` which means its identical to another file.
`rdfind` also has option to delete duplicate file and to analyze files with a minimum size.
those option are great but I wanted some control over which file I want to delete.

I wanted this results.txt file to be presented as a interactive table where I can select the files to delete and then let the system delete those.
I looked for some programs which offer something similar [dupeGuru](https://dupeguru.voltaicideas.net/), [czkawka](https://github.com/qarmin/czkawka) and a couple others, 
but I did not find them appealing for a headless home server. 

So I wrote a webapp [rdfind-web](https://github.com/zkhan93/rdfind-web) that will present the result file of `rdfind` on a modern web page. I can sort the result by file size and select the files I want to delete.

here is how it works
![](rdfind-web-sample.mp4)

with that I can now host it on my home serve and cleanup duplicate files.

let me know what you think of this, any ideas or comments to improve the. see you in another post :wink:
