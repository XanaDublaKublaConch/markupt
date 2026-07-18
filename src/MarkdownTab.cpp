#include "MarkdownTab.h"
#include "CodeHighlighter.h"
#include "DebugTiming.h"
#include "MarkdownRenderer.h"

#include <KSyntaxHighlighting/repository.h>
#include <KSyntaxHighlighting/theme.h>

#include <QFileInfo>
#include <QGuiApplication>
#include <QTextBlock>
#include <QTextBrowser>
#include <QTextDocument>
#include <QTextFormat>
#include <QVBoxLayout>

MarkdownTab::MarkdownTab(std::function<KSyntaxHighlighting::Repository &()> repositoryProvider, QWidget *parent)
    : QWidget(parent)
    , m_browser(new QTextBrowser(this))
    , m_repositoryProvider(std::move(repositoryProvider))
{
    m_browser->setReadOnly(true);
    m_browser->setOpenExternalLinks(true);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_browser);
}

bool MarkdownTab::loadFile(const QString &path)
{
    // Resolve local image references relative to the markdown file's
    // directory (FR6) via QTextBrowser's built-in search-path handling.
    m_browser->setSearchPaths({QFileInfo(path).absolutePath()});

    QString errorMessage;
    if (!MarkdownRenderer::render(path, m_browser->document(), &errorMessage)) {
        m_browser->setPlainText(errorMessage);
        return false;
    }

    m_filePath = path;
    setupCodeHighlightingIfNeeded();
    return true;
}

void MarkdownTab::setupCodeHighlightingIfNeeded()
{
    if (m_codeHighlighter)
        return;

    QTextDocument *doc = m_browser->document();
    bool hasCodeBlock = false;
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        if (!block.blockFormat().property(QTextFormat::BlockCodeLanguage).toString().isEmpty()) {
            hasCodeBlock = true;
            break;
        }
    }

    if (!hasCodeBlock)
        return;

    KSyntaxHighlighting::Repository &repository = m_repositoryProvider();
    DebugTiming::mark("Repository ready (constructed or reused)");
    m_codeHighlighter = new CodeHighlighter(repository, doc);
    applyCodeHighlighterTheme();
    DebugTiming::mark("code highlighting applied");
}

void MarkdownTab::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);

    // Live-track Plasma color-scheme changes (FR12). QGuiApplication's
    // paletteChanged() signal is deprecated in favor of this event.
    if (event->type() == QEvent::ApplicationPaletteChange)
        applyCodeHighlighterTheme();
}

void MarkdownTab::applyCodeHighlighterTheme()
{
    if (!m_codeHighlighter)
        return;

    KSyntaxHighlighting::Repository &repository = m_repositoryProvider();
    m_codeHighlighter->setTheme(repository.themeForPalette(qApp->palette()));
    m_codeHighlighter->rehighlight();
}

QString MarkdownTab::fileName() const
{
    return QFileInfo(m_filePath).fileName();
}
