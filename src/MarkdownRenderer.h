#pragma once

#include <QString>

class QTextDocument;

class MarkdownRenderer
{
public:
    // Reads the markdown file at filePath and sets it as the contents of
    // document. Returns true on success; on failure returns false and, if
    // errorMessage is non-null, sets it to a human-readable message.
    static bool render(const QString &filePath, QTextDocument *document, QString *errorMessage = nullptr);
};
