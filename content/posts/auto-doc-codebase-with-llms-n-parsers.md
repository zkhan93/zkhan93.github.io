---
title: "Auto-Documenting Codebases Using LLMs and Language Parsers"
date: 2025-03-31
tags: ["LLM", "documentation", "golang", "reactjs", "openai", "code parsing", "babel"]
description: "How to generate documentation for poorly documented codebases using LLMs combined with language-specific parsers in Golang and ReactJS."
---

# Introduction

In this blog, I want to share a documentation approach that leverages Large Language Models (LLMs) to transform a completely undocumented or partially documented codebase into a well-documented one.

By combining LLMs with language-specific parsers, we can extract function definitions and send each function, along with its surrounding “context,” to the LLM for generating documentation. While the method isn’t perfect, it provides a solid starting point for automating code documentation.

In this post, I’ll walk you through how I implemented this approach for Golang and ReactJS—but the same idea can be extended to other languages as well.

---

## Process

The entire workflow is simple and consists of three key steps:

### 1. Extractor  
The extractor takes in a project directory, loops through all the files, and pulls out function definitions along with their context. This context includes the function itself, relevant imports, existing documentation (if any), and the start and end lines of the function. The output is a JSON file containing all this metadata.

### 2. Generator  
The generator processes the JSON file produced by the extractor. It sends each function definition and its context to an LLM using the `openai` package to generate documentation. To avoid duplicate processing, I use a hash of the function content for caching responses.  

Once all functions are processed, the generator creates module-level (file-level) documentation by summarizing each function’s generated docs. Finally, it produces an overall project summary by aggregating summaries from all modules. This step outputs a new JSON file containing AI-generated documentation for each function, module, and the entire project.

### 3. Updater  
The updater reads the JSON output from the generator and inserts the generated documentation back into the source files. It adds function-level documentation inline, places module-level docs at the top of each file, and includes the overall summary in the main file.

---

## Golang

Go has a built-in package called `go/parser`, which makes parsing Go code and extracting function definitions straightforward. It can be used to retrieve both the function definitions and their relevant context from a file or directory.

I’ve packaged this approach into a Go module available here: [`golang-docai`](https://github.com/zkhan93/golang-docai/)

---

## ReactJS

For ReactJS, I used [`@babel/parser`](https://www.npmjs.com/package/@babel/parser) and [`@babel/traverse`](https://www.npmjs.com/package/@babel/traverse) to build a tool called `react-docai`. It extracts function components along with their surrounding context from React code. Just like the Golang version, it works seamlessly with both individual files and entire directories.

You can find the published npm package here: [`react-docai`](https://www.npmjs.com/package/react-docai)

