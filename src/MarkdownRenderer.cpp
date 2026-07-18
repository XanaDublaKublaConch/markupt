#include "MarkdownRenderer.h"

#include "DebugTiming.h"

#include <QFile>
#include <QTextDocument>
#include <QTextStream>

// FR8 — empirical GFM extension coverage of Qt's MD4C-based
// QTextDocument::setMarkdown(MarkdownDialectGitHub), verified by inspecting
// the parsed document's toHtml() against testdata/gfm-coverage.md on Qt 6.11.1:
//   - Task list checkboxes ([ ] / [x]) -> rendered as non-interactive
//     <li class="unchecked"|"checked"> with a CSS ::marker content of
//     U+2610 (☐) / U+2612 (☒). Not clickable/toggleable (expected — view-only).
//   - Strikethrough (~~text~~)         -> supported, text-decoration: line-through.
//   - Bare autolinks (raw https://... with no <> or [] syntax) -> ARE
//     auto-linked to a clickable <a href>, same as <url> and [text](url) forms.
//   - Tables                           -> supported, renders as <table>.
//   - Blockquotes                      -> rendered as an indented <p>
//     (margin-left), not a semantic <blockquote> element.
// Fenced code blocks are tagged per-line with QTextFormat::BlockCodeLanguage
// taken from the fence info string (confirmed via block format inspection),
// which Phase 3's CodeHighlighter relies on directly.
bool MarkdownRenderer::render(const QString &filePath, QTextDocument *document, QString *errorMessage)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage)
            *errorMessage = QStringLiteral("Could not open file: %1").arg(filePath);
        return false;
    }

    QTextStream stream(&file);
    const QString markdownText = stream.readAll();
    DebugTiming::mark("file read done");

    document->setMarkdown(markdownText, QTextDocument::MarkdownDialectGitHub);
    DebugTiming::mark("setMarkdown() returned");
    return true;
}
