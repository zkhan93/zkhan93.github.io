---
title:  "Vuejs SEO without ssr using nginx"
date:   2020-01-02 12:00:00
categories: [programming]
tags: [vuejs, nginx, seo, ssr]
draft: true
---

Vuejs was easy to get started with, so I picked it up for my new project
idea was to finally compile into a single page application, completely static
I was happy with the result, but SEO and link sharing was not working as expected, since all the dynamic data was pulled after
but soon SEO  became the requirement
SSR was too much rework
looked for alternative 
Nuxt was there but again that meant lot of rework and re-structuring
then I thought why to filter the traffic from bots (mainly the platforms where the links were going to be shared)
created a small flask app to handle that traffic and return proper meta tags
added nginx config to send traffic to flask app based on user agents 
and voila!!