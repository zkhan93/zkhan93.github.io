---
layout: default
title: Articles
permalink: /articles/
order: 4
---
<h1 class="post-title">Latest Articles</h1>

{% for post in (site.categories.update | sort: 'date') limit:5 %}

<a class="post-link" href="{{post.url}}" >{{post.title}}</a>

{{ post.date | date: "%b %-d, %Y" }} {% if post.author %} • {{ post.author }} {% endif %}{% if post.meta %} • {{ post.meta }}{% endif %}
{{ post.excerpt }} <a class="btn btn-small btn-more" href="{{post.url}}" >Read More <i class="fa fa-chevron-circle-right"></i></a>

<i class="fa fa-tags"></i>
{% for tag in (post.tags) %} {{ tag }} {% if forloop.last == false %}, {%endif%} {% endfor %}

{% endfor %}

<a class="post-link btn btn-big btn-archive" href="{{ "/archive" | prepend: site.baseurl }}">Archive <i class="fa fa-arrow-circle-right"></i></a>

subscribe [via RSS <i class="fa fa-rss"></i>]({{ "/feed.xml" | prepend: site.baseurl }})
