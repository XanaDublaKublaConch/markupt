#include "MainWindow.h"
#include "MarkdownTab.h"

#include <QCloseEvent>
#include <QFileInfo>
#include <QMouseEvent>
#include <QSettings>
#include <QShortcut>
#include <QTabBar>
#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabWidget(new QTabWidget(this))
{
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setDocumentMode(true);
    // Qt has no built-in middle-click-to-close signal; watch the tab bar's
    // own mouse events for it instead.
    m_tabWidget->tabBar()->installEventFilter(this);
    setCentralWidget(m_tabWidget);

    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateWindowTitle);

    auto *refreshKeyF5 = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(refreshKeyF5, &QShortcut::activated, this, &MainWindow::refreshActiveTab);
    auto *refreshKeyCtrlR = new QShortcut(QKeySequence(QStringLiteral("Ctrl+R")), this);
    connect(refreshKeyCtrlR, &QShortcut::activated, this, &MainWindow::refreshActiveTab);

    resize(800, 600);
    QSettings settings;
    restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());

    updateWindowTitle();
}

KSyntaxHighlighting::Repository &MainWindow::repository()
{
    if (!m_repository)
        m_repository.emplace();
    return *m_repository;
}

void MainWindow::openFile(const QString &path)
{
    const QString absolutePath = QFileInfo(path).absoluteFilePath();

    for (int i = 0; i < m_tabWidget->count(); ++i) {
        auto *tab = qobject_cast<MarkdownTab *>(m_tabWidget->widget(i));
        if (tab && QFileInfo(tab->filePath()).absoluteFilePath() == absolutePath) {
            m_tabWidget->setCurrentIndex(i);
            return;
        }
    }

    auto *tab = new MarkdownTab([this]() -> KSyntaxHighlighting::Repository & { return repository(); }, m_tabWidget);
    tab->loadFile(path);
    const int index = m_tabWidget->addTab(tab, tab->fileName());
    m_tabWidget->setCurrentIndex(index);
    updateWindowTitle();
}

void MainWindow::refreshActiveTab()
{
    auto *tab = qobject_cast<MarkdownTab *>(m_tabWidget->currentWidget());
    if (!tab)
        return;

    tab->loadFile(tab->filePath());
    m_tabWidget->setTabText(m_tabWidget->currentIndex(), tab->fileName());
    updateWindowTitle();
}

void MainWindow::closeTab(int index)
{
    QWidget *widget = m_tabWidget->widget(index);
    m_tabWidget->removeTab(index);
    delete widget;

    if (m_tabWidget->count() == 0)
        close(); // triggers closeEvent (geometry save) then quits via quitOnLastWindowClosed
}

void MainWindow::updateWindowTitle()
{
    auto *tab = qobject_cast<MarkdownTab *>(m_tabWidget->currentWidget());
    setWindowTitle(tab ? tab->fileName() : QStringLiteral("Markdown Viewer"));
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_tabWidget->tabBar() && event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::MiddleButton) {
            const int index = m_tabWidget->tabBar()->tabAt(mouseEvent->pos());
            if (index >= 0) {
                closeTab(index);
                return true;
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    QMainWindow::closeEvent(event);
}
