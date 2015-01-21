---
layout: default
title: Articles
permalink: /articles/ 
---
<h1 class="post-title">Latest Articles</h1>

{% for post in (site.categories.update | sort: 'date') limit:5 %}

<a class="inline post-link" href="{{post.url}}" >{{post.title}}</a>
 
{{ post.date | date: "%b %-d, %Y" }} {% if post.author %} • {{ post.author }} {% endif %}{% if post.meta %} • {{ post.meta }}{% endif %}
{{ post.excerpt }} <a class="btn btn-success" href="{{post.url}}" >Contnue Reading .. </a>

<i class="fa fa-tags"></i> 
{% for tag in (post.tags) %} {{ tag }} {% if forloop.last == false %}, {%endif%} {% endfor %}
{% endfor %}

subscribe [via RSS]({{ "/feed.xml" | prepend: site.baseurl }})
