---
title: "Elasticsearch Kibana Setup"
date: 2023-03-21T15:32:23-05:00
draft: false
tags: ["elasticsearch", "kibana", "docker", "compose", "traefik", "self-hosted"]
---

I was trying to setup a elasticsearch with docker compose to index files on my NAS(Samba) and I had to struggle a bit to get that up and running, 
this post outlines the steps needed to setup elasticsearch with docker compose


All my self-hosted services are behind traefik proxy to manage SSL certificates and I have chosen to keep communication between traefik and elasticsearch to HTTP only,
so I have disabled SSL security in elasticsearch.

Desired State
- ElasticSearch
- Kibana to get a web interface to manage users/roles/indexes

In elasticsearch version 7.x, I had to take the following steps
 - start elasticsearch instance
 - run `elasticsearch-setup-passwords auto` inside the container to generate passwords
 - set that password for kibana_system in docker-compose.yml file
 - rebuild kibana to pickup the password

I'm assuming we are inside a folder es7, this will be used in below command, change it accordingly

### 1. Start elasticsearch and kibana containers
create a compose file es7/docker-compose.yml
```yml

version: "3"

services:
  elasticsearch:
    image: elasticsearch:${STACK_VERSION}
    environment:
      - "discovery.type=single-node"
      - "ES_JAVA_OPTS=-Xms4096m -Xmx4096m"
      - xpack.security.http.ssl.enabled=false
      - xpack.security.enabled=true
    ports:
      - 9200:9200
    volumes:
      - elastic-data:/usr/share/elasticsearch/data
      - elastic-config:/usr/share/elasticsearch/config
      - elastic-plugins:/usr/share/elasticsearch/plugins
    ulimits:
      memlock:
        soft: -1
        hard: -1
    restart: always

  kibana:
    depends_on:
      - elasticsearch
    image: kibana:${STACK_VERSION}
    volumes:
      - kibana-data:/usr/share/kibana/data
      - ./kibana.yml:/usr/share/kibana/config/kibana.yml
    ports:
     - 5601:5601
    restart: always
    environment:
      - SERVER_NAME=kibana.example.com
      - ELASTICSEARCH_HOSTS=http://elasticsearch:9200
      - ELASTICSEARCH_USERNAME=kibana_system
      - ELASTICSEARCH_PASSWORD=<generate_password>

volumes:
  elastic-data:
  elastic-config:
  elastic-plugins:
  kibana-data:

```
create a .env file
```bash
STACK_VERSION=7.17.9
```
to start the containers run
```bash
docker compose up --build -d
``` 

### 2. Generate Passwords for users
`docker exec -it es7-elasticsearch-1 elasticsearch-setup-passwords auto`
you will get something like this 

```bash
Changed password for user apm_system
PASSWORD apm_system = <secret>

Changed password for user kibana_system
PASSWORD kibana_system = <secret>

Changed password for user kibana
PASSWORD kibana = <secret>

Changed password for user logstash_system
PASSWORD logstash_system = <secret>

Changed password for user beats_system
PASSWORD beats_system = <secret>

Changed password for user remote_monitoring_user
PASSWORD remote_monitoring_user = <secret>

Changed password for user elastic
PASSWORD elastic = <secret>
```

to setup kibana we need the password for kibana_system
copy that and set the environment variable ELASTICSEARCH_PASSWORD under kibana service in docker-compose.yml

rebuild and restart kibana
```bash
docker compose up --build -d kibana
```

visit kibana UI on 5601 and login using `elastic` user credentials from the above step 

from here you can navigate to stack management and create indexes, roles and users to suit your needs.


since both the services were behind traefik proxy, I was able to access them securely over HTTPS! 

I use rules.toml to define my services, so here is how you would configure it in traefik


Note: This is obviously not how you would want to setup your production EKL stack 
