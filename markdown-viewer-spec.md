# Markdown Viewer — Technical Specification

## 1. Overview

A lightweight, fast, native KDE desktop application for quick, view-only
preview of Markdown files. Optimized for cold-start speed and Plasma
integration over feature completeness.

## 2. Goals

- Sub-second cold start (target: <300ms to rendered content for files
  under ~1MB)
- Native Plasma look-and-feel, automatically matching the active
  light/dark color scheme — no manual theme toggle
- Syntax-highlighted fenced code blocks
- Launch via file association (double-click `.md` in Dolphin)
- Manual, on-demand refresh via keyboard shortcut
- Opening additional files reuses a single running window as tabs,
  rather than spawning new windows/processes

## 3. Non-Goals (explicitly out of scope for v1)

- Mermaid / diagram rendering — sacrificed for startup speed and to avoid
  an embedded browser engine
- Live file-watching / automatic reload on change
- Editing capability (strictly view-only)
- Cross-platform support (Windows/macOS) — KDE Plasma on Linux only

## 4. Target Platform

- Linux desktop running KDE Plasma 6.x (X11 or Wayland)
- Targets KDE Frameworks 6 (KF6)

## 5. Technology Stack

| Layer | Choice |
|---|---|
| Language | C++20 |
| UI framework | Qt 6 |
| Code syntax highlighting | KSyntaxHighlighting (KF6) — same engine as Kate/KWrite, hundreds of language definitions included |
| Theme integration | Native QPalette inheritance from Plasma + KColorScheme (KF6) |
| Markdown parsing | Qt's built-in `QTextDocument::setMarkdown()` (MD4C-based CommonMark + GFM-subset parser, bundled since Qt 5.14 — no external dependency) |
| Single-instance IPC | KDBusService (KF6) — detects an already-running instance and forwards new file paths to it over D-Bus |
| Build system | CMake + Extra CMake Modules (ECM) |
| Packaging | `PKGBUILD` (Arch/CachyOS), built via CMake, installed with `makepkg` |

**Rationale:** With Mermaid off the table, there's no need for an
embedded browser engine (pywebview/QWebEngine), which removes the
biggest startup-time cost. A compiled C++/Qt app skips both the Python
interpreter startup and PyQt/PySide binding-initialization overhead that
would otherwise eat into the speed budget. KSyntaxHighlighting and
Plasma's native palette handling mean theme-matching and code
highlighting come essentially "for free" from the KDE ecosystem rather
than needing to be built or configured manually.

## 6. Functional Requirements

### 6.1 File Opening
- **FR1:** Register as an available "Open With" handler for `.md` /
  `.markdown` MIME types via a `.desktop` file
- **FR2:** Accept a file path as a CLI argument
  (`markdownviewer /path/to/file.md`)
- **FR3:** Double-clicking a `.md` file in Dolphin opens it in the
  running instance (see §6.5), or launches a new instance if none is
  running

### 6.2 Rendering
- **FR4:** Render CommonMark + GFM-subset Markdown (headings, lists,
  tables, blockquotes, links, images, inline code, fenced code blocks)
  via `QTextDocument::setMarkdown()`
- **FR5:** Apply syntax highlighting to fenced code blocks using
  KSyntaxHighlighting, auto-detecting the language from the fence info
  string (e.g. ` ```python `)
- **FR6:** Resolve and render local image references relative to the
  Markdown file's directory
- **FR7:** ` ```mermaid ` fences render as plain highlighted code
  blocks, not as diagrams (see §3, Non-Goals)
- **FR8:** During implementation, empirically verify which GFM
  extensions the target Qt 6 version's MD4C-based parser actually
  supports (task-list checkboxes, strikethrough, autolinks) using a
  test document, and document the real coverage rather than assuming
  full GFM compliance

### 6.3 Refresh
- **FR9:** Manual refresh via keyboard shortcut (e.g. `F5` or `Ctrl+R`)
  for the active tab, re-reading its file from disk and re-rendering
- **FR10:** No automatic file-watching or auto-reload in v1

### 6.4 Theming
- **FR11:** Inherit Plasma's active color scheme automatically
  (light/dark) via Qt's native palette
- **FR12:** Code block syntax highlighting tracks the active Plasma
  color scheme (KSyntaxHighlighting supports Kate-schema-based theming)

### 6.5 Window & Tab Behavior
- **FR13:** The application runs as a single instance. Launching it
  again (e.g. via double-click on another `.md` file, or a second CLI
  invocation) detects the running instance via D-Bus (KDBusService) and
  forwards the file path to it instead of starting a new process
- **FR14:** A file path forwarded to the running instance opens in a
  new tab
- **FR15:** The window title reflects the active tab's file name
- **FR16:** Tabs can be closed individually (close button and/or
  middle-click); closing the last tab closes the window and terminates
  the process
- **FR17:** Last window size/position persisted between launches via
  `QSettings`

## 7. Non-Functional Requirements

- **NFR1:** Cold start to rendered content <300ms for files under ~1MB
  on typical hardware
- **NFR2:** No network access required — fully offline
- **NFR3:** No telemetry or analytics

## 8. Architecture Sketch

```
main.cpp             — entry point; parses CLI arg, checks for a running
                        instance via SingleInstanceGuard, either forwards
                        the file path via D-Bus or creates a MainWindow
SingleInstanceGuard   — wraps KDBusService; registers this process as the
                        instance owner, or detects an existing owner and
                        forwards the file path to it
MainWindow            — QMainWindow subclass hosting a QTabWidget; handles
                        tab creation/closing, refresh shortcut (applies to
                        the active tab), QSettings persistence
MarkdownTab           — one tab's content: file path, QTextBrowser,
                        MarkdownRenderer instance, CodeHighlighter instance
MarkdownRenderer      — thin wrapper around QTextDocument::setMarkdown();
                        post-processes fenced code blocks for highlighting
CodeHighlighter       — QSyntaxHighlighter subclass wired to
                        KSyntaxHighlighting::Repository; tracks Plasma
                        color scheme changes
resources/
  markdownviewer.desktop  — MIME association + launcher entry
packaging/
  PKGBUILD                — CachyOS/Arch package build definition
```

## 9. Implementation Notes / Verification Checklist

- Confirm actual GFM extension coverage of Qt 6's MD4C-based parser
  against a test document (see FR8) before relying on task lists,
  strikethrough, or autolinks
- Confirm KDBusService behavior when the running instance's window is
  minimized or on a different virtual desktop (should still raise/focus
  it when a new file arrives)

## 10. Future Enhancements (explicitly deferred)

- Opt-in Mermaid rendering via external SVG generation (e.g. shelling
  out to `mmdc` if present on the system) — must not block or slow
  down normal startup when unused
- Optional auto-reload toggle (file watcher)
- Cross-platform build (Windows/macOS)
