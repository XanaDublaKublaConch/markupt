#pragma once

#include <KSyntaxHighlighting/definition.h>
#include <KSyntaxHighlighting/syntaxhighlighter.h>

namespace KSyntaxHighlighting
{
class Repository;
}

// Highlights fenced code blocks within a rendered Markdown QTextDocument.
//
// Unlike KSyntaxHighlighting::SyntaxHighlighter's normal one-Definition-per-
// document usage, a single Markdown document can contain multiple fenced
// blocks in different languages interleaved with prose. This class switches
// Definition per block based on the QTextFormat::BlockCodeLanguage property
// that Qt's markdown importer already attaches to each code-block QTextBlock,
// and tracks per-fenced-region highlighting State via QTextBlockUserData so
// multi-line constructs (e.g. block comments) highlight correctly across the
// lines of one fence.
class CodeHighlighter : public KSyntaxHighlighting::SyntaxHighlighter
{
    Q_OBJECT

public:
    CodeHighlighter(KSyntaxHighlighting::Repository &repository, QTextDocument *document);

protected:
    void highlightBlock(const QString &text) override;

private:
    KSyntaxHighlighting::Repository &m_repository;
    QString m_currentLanguage;
    KSyntaxHighlighting::Definition m_currentDefinition;
};
