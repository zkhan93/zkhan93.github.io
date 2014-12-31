---
layout: default
title: Articles
permalink: /articles/
---

{% for post in site.categories.update %}
<a class="post-link" href="{{post.url}}" >{{post.title}}</a>
{{ post.date | date: "%b %-d, %Y" }} {% if post.author %} • {{ post.author }} {% endif %}{% if post.meta %} • {{ post.meta }}{% endif %}
{% endfor %}
