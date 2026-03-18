# CLAUDE.md - Project Guide

## Project Overview

Personal blog and portfolio site for Zeeshan Khan, hosted at **https://zkhan.in** (CNAME: `www.zkhan.in`). Built with Hugo static site generator and deployed to GitHub Pages via GitHub Actions.

## Tech Stack

- **Static Site Generator**: Hugo (v0.155.0, extended edition with Dart Sass)
- **Theme**: [PaperMod](https://github.com/adityatelange/hugo-PaperMod) (primary) + [hugo-video](https://github.com/martignoni/hugo-video) (video shortcode support), both as git submodules
- **Deployment**: GitHub Actions → GitHub Pages (on push to `master`)
- **Comments**: Disqus (shortname: `coderzkhan`)
- **Analytics**: Google Analytics (`G-CRSPPCQRYL`)
- **Ads**: Google AdSense (`ca-pub-8023468202540314`)
- **Search**: Fuse.js client-side search (requires JSON output in `outputs.home`)

## Repository Structure

```
.
├── archetypes/default.md        # New content template (title from filename, draft: true)
├── config.yml                   # Hugo site configuration (NOT hugo.toml)
├── content/
│   ├── archives.md              # Archive page (layout: archives)
│   ├── search.md                # Search page (layout: search)
│   ├── projects.md              # Projects showcase page
│   └── posts/                   # Blog posts
│       ├── c/                   # C programming tutorials
│       ├── java/                # Java tutorials
│       ├── draft/               # Draft posts (not published)
│       └── <post>/index.md      # Posts with local assets use page bundles
├── layouts/partials/
│   ├── comments.html            # Disqus integration (uses Hugo internal template)
│   ├── extend_head.html         # AdSense script injection
│   └── extend_footer.html       # Custom footer override (copyright + year)
├── static/
│   ├── CNAME                    # Custom domain: www.zkhan.in
│   ├── me.jpg                   # Author profile photo
│   ├── favicon.ico              # Site favicon
│   └── ads.txt                  # Ad verification
├── themes/
│   ├── PaperMod/                # Git submodule
│   └── hugo-video/              # Git submodule
└── .github/workflows/hugo.yml   # CI/CD: build Hugo site and deploy to GitHub Pages
```

## Key Configuration (`config.yml`)

- **Base URL**: `https://zkhan.in/`
- **Default theme**: dark mode
- **Menu items**: Blog (`/posts/`), Tags, Archive, Projects, Search
- **Features enabled**: reading time, breadcrumbs, share buttons, code copy buttons, post nav links, robots.txt, emoji
- **Drafts/future/expired posts**: all disabled in builds
- **Profile mode**: disabled (uses homeInfoParams instead)

## Content Conventions

- Posts live under `content/posts/`; use front matter with `title`, `date`, `tags`, `description`
- Posts with images/assets use Hugo page bundles (`<post-name>/index.md`)
- Simple posts are standalone `.md` files
- Drafts go in `content/posts/draft/` with `draft: true`
- New posts created via `hugo new` use `archetypes/default.md` template

## Layout Customizations

The site overrides three PaperMod partials:
1. **`comments.html`** — Integrates Disqus via Hugo's `_internal/disqus.html`
2. **`extend_head.html`** — Injects Google AdSense script in `<head>`
3. **`extend_footer.html`** — Custom footer showing copyright year and site title

## Build & Deploy

- **Local development**: `hugo server -D` (include drafts)
- **Production build**: `hugo --minify` (run by GitHub Actions)
- **Deploy trigger**: Push to `master` branch
- **Hugo version**: Pinned to 0.155.0 in `.github/workflows/hugo.yml`
- Git submodules must be checked out recursively (`--recurse-submodules`)

## Important Notes

- Config file is `config.yml` (not `hugo.toml`)
- Themes are git submodules — update with `git submodule update --remote`
- The `master` branch is the default/deploy branch (not `main`)
- Search requires JSON in home outputs config to work with Fuse.js
