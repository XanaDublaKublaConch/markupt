#pragma once

#include <QWidget>

#include <functional>

class QTextBrowser;
class CodeHighlighter;

namespace KSyntaxHighlighting
{
class Repository;
}

class MarkdownTab : public QWidget
{
    Q_OBJECT

public:
    // repositoryProvider lazily supplies the app's single shared
    // KSyntaxHighlighting::Repository instance the first time (if ever)
    // this tab's document actually contains a fenced code block, since
    // constructing a Repository is a moderately expensive scan of all
    // bundled syntax definitions and a prose-only file shouldn't pay for it.
    explicit MarkdownTab(std::function<KSyntaxHighlighting::Repository &()> repositoryProvider, QWidget *parent = nullptr);

    // Reads and renders the markdown file at path, resolving local images
    // relative to its directory. Returns true on success; on failure the
    // browser displays the error message instead of content.
    bool loadFile(const QString &path);

    QString filePath() const { return m_filePath; }
    QString fileName() const;

protected:
    void changeEvent(QEvent *event) override;

private:
    void setupCodeHighlightingIfNeeded();
    // Looks up the KSyntaxHighlighting theme matching the app's current
    // QPalette and applies it to the code highlighter, then rehighlights.
    // Called once at startup and again on every Plasma color-scheme change
    // (FR12), so the code-block colors track light/dark switches live.
    void applyCodeHighlighterTheme();

    QString m_filePath;
    QTextBrowser *m_browser;
    std::function<KSyntaxHighlighting::Repository &()> m_repositoryProvider;
    CodeHighlighter *m_codeHighlighter = nullptr;
};
