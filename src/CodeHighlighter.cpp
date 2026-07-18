#include "CodeHighlighter.h"

#include <KSyntaxHighlighting/abstracthighlighter.h>
#include <KSyntaxHighlighting/repository.h>
#include <KSyntaxHighlighting/state.h>

#include <QTextBlock>
#include <QTextDocument>
#include <QTextFormat>

namespace
{
// Carries the highlighting State forward from one line of a fenced code
// block to the next, so multi-line constructs highlight correctly.
class HighlightBlockData : public QTextBlockUserData
{
public:
    KSyntaxHighlighting::State state;
};
}

CodeHighlighter::CodeHighlighter(KSyntaxHighlighting::Repository &repository, QTextDocument *document)
    : KSyntaxHighlighting::SyntaxHighlighter(document)
    , m_repository(repository)
{
}

void CodeHighlighter::highlightBlock(const QString &text)
{
    const QString lang = currentBlock().blockFormat().property(QTextFormat::BlockCodeLanguage).toString();

    if (lang.isEmpty()) {
        // Prose, not a fenced code block: nothing to highlight.
        setCurrentBlockState(-1);
        return;
    }

    if (lang != m_currentLanguage) {
        m_currentLanguage = lang;
        m_currentDefinition = m_repository.definitionForName(lang);
        // Call AbstractHighlighter's own setDefinition() directly (bypassing
        // virtual dispatch to SyntaxHighlighter::setDefinition()), which
        // would otherwise trigger a fresh rehighlight() of the whole
        // document from within highlightBlock() itself -> infinite
        // recursion. AbstractHighlighter::setDefinition() just stores the
        // Definition for the next highlightLine() call, no side effects.
        KSyntaxHighlighting::AbstractHighlighter::setDefinition(m_currentDefinition);
    }

    if (!m_currentDefinition.isValid()) {
        // No syntax definition found for this fence language: leave the
        // block as plain unhighlighted text, do not crash. Note this is
        // NOT how "```mermaid" fences degrade (FR7) -- KSyntaxHighlighting
        // actually ships a bundled Mermaid.xml definition, so mermaid
        // fences get real token highlighting here, same as any other
        // language. That still satisfies FR7 ("render as plain highlighted
        // code blocks, not as diagrams") since no diagram is ever rendered.
        return;
    }

    // Only continue the previous line's highlighting State if it belonged
    // to the same fenced region (same language); otherwise start fresh.
    KSyntaxHighlighting::State previousState;
    const QTextBlock previousBlock = currentBlock().previous();
    if (previousBlock.isValid()) {
        const QString previousLang = previousBlock.blockFormat().property(QTextFormat::BlockCodeLanguage).toString();
        if (previousLang == lang) {
            if (auto *data = dynamic_cast<HighlightBlockData *>(previousBlock.userData()))
                previousState = data->state;
        }
    }

    const KSyntaxHighlighting::State newState = highlightLine(text, previousState);

    auto *data = new HighlightBlockData;
    data->state = newState;
    setCurrentBlockUserData(data);
}
