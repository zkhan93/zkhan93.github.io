---
title: "Extracting Structured Content from PDFs Using OCR and LLM"
date: 2024-08-30T21:14:30-05:00
tags: ["python", "ocr", "llm", "content-extraction", "openai", "langchain"]
---

## Introduction

In this post, I want to document the steps I took to parse PDFs and extract structured output using OCR and LLM. The goal is to extract structured content from PDFs and other documents with high accuracy.

The results of this experiment were quite impressive. I was able to extract structured content from various documents with a high degree of accuracy. The process was straightforward, and the extracted data was well-structured and useful.

For small files, you can send the extracted text to the LLM and request the desired output, which generally works well. However, as file sizes increase—often spanning hundreds of pages with multiple sections—the process becomes more complex, requiring a more structured approach to extraction.

This approach is particularly useful for documents with a large number of pages where specific sections repeat, making it necessary to extract structured data efficiently.

## Types of PDFs

There are primarily two types of PDF documents:

1. **Searchable PDFs** – These contain embedded text, making it easy to extract text directly.
2. **Scanned PDFs** – These contain images of text, requiring Optical Character Recognition (OCR) to extract text before processing.

For scanned PDFs, OCR is necessary to convert images into machine-readable text.

## Text Layout

### What is Text Layout?

OCR models provide text along with coordinate information, allowing us to reconstruct the text layout similar to its original visual structure. This reconstructed representation is referred to as the **text layout** of the page.

         I'll add some sample here for better understanding, later.


## Creating a Template

This step involves manually onboarding a new document type by creating a **template** that defines how to extract structured content.

### Steps to Create a Template

1. Convert the document to a text layout.
2. Manually mark sections by selecting a line number range. Each section represents relevant information.
3. For each marked section:
   - Send the selected text to an LLM to generate a JSON output.
   - Present the JSON output to the user for manual adjustments.
4. Once confirmed, send the JSON to the LLM to generate a JSON Schema.
5. Allow users to review and adjust the JSON Schema.
6. Provide at least **2-3 sample examples** for each section.
7. Save the pages where the sections are detected.
8. Use these saved examples to help the LLM recognize similar sections in future documents.

         Add screenshots of creating template tool for better understanding, later.


## Parsing a File Using the Template

Once a template is created, it can be used to process similar documents.

### Steps to Parse a File

1. Convert the file to a text layout.
2. Retrieve the corresponding template configuration for the document type.
3. For each **section** defined in the template:
   - For each **page** in the document:
     - Extract the text based on the section's coordinates.
     - Send the extracted text to the LLM along with:
       - Sample examples of the section.
       - Page and section details, including line numbers.
     - The LLM identifies the section using the provided samples and marks relevant line numbers.
     - Extract the identified section text.
     - Send the section text to the LLM with the JSON Schema and ask it to parse the text into structured output.
     - Save the structured JSON output.
4. Perform **post-processing**:
   - Remove duplicate sections.
   - Merge split sections across multiple pages.
5. Save the final structured output.
6. Implement a **human-in-the-loop** step for verification.


         I'll add some sample here for better understanding, later.


## Conclusion

By combining OCR with LLM-powered parsing, we can extract structured data from PDFs efficiently. Defining templates ensures consistency and accuracy, while human verification helps maintain data quality. This method enables automated document processing across various domains, making structured data extraction more accessible and reliable.

