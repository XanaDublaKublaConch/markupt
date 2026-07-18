#pragma once

#include <QMainWindow>

#include <KSyntaxHighlighting/repository.h>

#include <optional>

class QTabWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    // Opens path in a new tab, or switches to it if already open (FR14).
    void openFile(const QString &path);

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void refreshActiveTab();
    void closeTab(int index);
    void updateWindowTitle();

private:
    // Lazily constructs the app's single shared KSyntaxHighlighting::Repository
    // (see MarkdownTab for why this must stay lazy), shared by every tab.
    KSyntaxHighlighting::Repository &repository();

    QTabWidget *m_tabWidget;
    std::optional<KSyntaxHighlighting::Repository> m_repository;
};
