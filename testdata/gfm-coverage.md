# GFM Coverage Test

This file exercises the CommonMark + GFM-subset surface area that
`QTextDocument::setMarkdown()` needs to handle per FR4/FR6/FR8.

## Task list

- [ ] Unchecked item
- [x] Checked item

## Strikethrough

This is ~~struck through~~ text.

## Bare autolink

A bare URL with no special syntax: https://example.com

A URL wrapped in angle brackets: <https://example.com>

A URL as a proper markdown link: [example](https://example.com)

## Table

| Language | Extension |
|----------|-----------|
| Python   | .py       |
| C++      | .cpp      |

## Blockquote

> A quoted passage that spans
> more than one line.

## Local image (FR6)

![Test image](images/test-image.png)

## Fenced code block

```python
def hello(name: str) -> str:
    return f"Hello, {name}!"
```

## Mermaid fence (FR7 — should render as plain code, not a diagram)

```mermaid
graph TD
  A --> B
```
