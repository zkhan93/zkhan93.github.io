---
layout: default
title: Articles
permalink: /articles/
---
# Latest articles on {{site.title}}
{% for post in (site.categories.update | sort: 'date') limit:10 %}
<a class="post-link" href="{{post.url}}" >{{post.title}}</a>
{{ post.date | date: "%b %-d, %Y" }} {% if post.author %} • {{ post.author }} {% endif %}{% if post.meta %} • {{ post.meta }}{% endif %}
{% endfor %}
