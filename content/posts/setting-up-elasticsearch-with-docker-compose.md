---
title: "Setting up Elasticsearch with Docker Compose for Indexing Files on NAS"
date: 2023-03-21T15:32:23-05:00
draft: false
tags: ["elasticsearch", "docker", "compose", "self-hosting", "traefik", "SSL", "Kibana", "indexing", "NAS", "Samba"]
---
If you have a NAS (Samba) and want to index files, Elasticsearch is an excellent solution. However, setting up Elasticsearch with Docker Compose can be a bit challenging. In this post, we will outline the necessary steps to set up Elasticsearch with Docker Compose.

The desired state is Elasticsearch and Kibana to get a web interface to manage users/roles/indexes.

In Elasticsearch version 7.x, you need to take the following steps:

- Start Elasticsearch instance
- Run elasticsearch-setup-passwords auto inside the container to generate passwords
- Set that password for kibana_system in the docker-compose.yml file
- Rebuild Kibana to pick up the password

Assuming you are inside the es7 folder, below are the steps:

### 1. Start Elasticsearch and Kibana containers
Create a compose file es7/docker-compose.yml

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
To start the containers, run the following command:

```bash
docker compose up --build -d
``` 

### 2. Generate Passwords for users
Run the command 
`docker exec -it es7-elasticsearch-1 elasticsearch-setup-passwords auto`
to generate passwords for users. Once you run the command, you will see the following output:

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
To set up Kibana, copy the password for kibana_system and set the environment variable ELASTICSEARCH_PASSWORD under the kibana service in the docker-compose.yml file. Then, rebuild and restart Kibana by running the following command:

```bash
docker compose up --build -d kibana
```

Visit Kibana UI on port 5601 and log in using the elastic user credentials obtained in the previous step. From there, you can navigate to stack management and create indexes, roles, and users according to your needs.


Note: This is obviously not how you would want to setup your production EKL stack 
