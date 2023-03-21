---
title: "Using Elasticsearch to search files on NAS"
date: 2023-03-14T12:08:45-05:00
draft: true
tags: ["elasticsearch", "samba", "nas"]
---

Till now I was happy by just giving my family members network shared folders using samba, 
so people can just mount network drive on there systems and store files on it, and I can back it all up from there.

I also hosted NextCloud on home server so that they can access those files when not at home.

but recently my sister started to complain that searching files in shared folders is taking long time and yields poor results,
so I had a problem to solve :)

so I looked it up and quickly found out that I was using samba version 4.13.13 which can use elasticsearch as a search backend,
so when a MAC user do spotlight search, the search can be forwarded to samba and samba can use elastic search to get the result.

### Components
Its a fairly simple setup that needs these following components apart from samba server and client (obviously)
 
- `Elasticsearch` - I used docker to host this on an external system, my plan was to use the same instance for other things as well, you can host it on the same system as well
- `fs2es-indexer` - A service/process that will scan the local files and index them to elasticsearch index, this needs to be configured and run periodically

### Elasticsearch
on all the docs/references I got I see that the elasticsearch was hosted on the same system, but in my case I hosted the elasticsearch instance
somewhere else so I had ssl enabled endpoint
along with elasticsearch I also setup kibana so that I can manage elasticsearch from a web UI, 
lot of docs on how to do that I mostly followed the [official elasticsearch docs](https://www.elastic.co/guide/en/elasticsearch/reference/8.6/docker.html).

 - create a index `files` in elasticsearch 
 - create a user `fs2es-indexer` and set its password
 - create a role `fs2es-indexer` which has all permission to the index `fs2es-indexer`
 - create a role `fs2es-indexer-ro` which has read/query permission to the index `fs2es-indexer`
 - assign the role `fs2es-indexer` to user `fs2es-indexer`
 - create a security exception that allows anonymous user to have `fs2es-indexer-ro` role so that they can query the index - samba will query with no user credentials

elasticsearch endpoint: https://domain.in
elasticsearch port: 443

### fs2es-indexer
we need a different process that periodically runs and indexes the shared folder.
you can setup a daemon, a cron or docker process to achieve this .

The github [repo](https://github.com/Ellerhold/fs2es-indexer) explains how you can install and set up the fs2es-indexer as a systemd service.

I would probably create a docker image out of it for easy deployment later
but here is how I did it, 
 - downloaded and extract the source code from latest tag 0.4.3 
 - created a python virtual environment
 - installed fs2es-indexer in virtual environment

#### commands to install fs2es-indexer in python virtual environment
```bash
$ wget https://github.com/Ellerhold/fs2es-indexer/archive/refs/tags/0.4.3.zip
# unzip it to get the folder `fs2es-indexer-0.4.3` in the current directory
$ unzip -d . 0.4.3.zip
$ cd fs2es-indexer-0.4.3
# install python virtualenv if not already installed
$ sudo apt-get install python3-venv
# create a python virtual env
$ python -m venv venv
# activate the newly created python virtual env
$ source venv/bin/activate
# install fs2es-indexer
$ pip install .
```

#### Index files
At this point you have installed fs2es-indexer you are ready to index the files.

copy config.dist.yml to config.yml and edit it to set elasticsearch credentials,
folders to index, and exclusion rules if you want any.

the docs on fs2es-indexer will tell you to copy the config file to /etc/config/ but since we have installed it in virtual env we will have to specify the config file

2 useful commands that I encountered were

check on the client side that the search is enabled

```bash
$ mdutil -s /Volumes/kymedia            
/System/Volumes/Data/Volumes/kymedia:
	Server search enabled.
```

issue a search on that volume to verify the system works
```bash
$ mdfind -onlyin /Volumes/kymedia marvel
```
