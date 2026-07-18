#include "SingleInstanceGuard.h"
#include "MainWindow.h"

#include <KDBusService>
#include <KWindowSystem>

#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>

SingleInstanceGuard::SingleInstanceGuard(QCommandLineParser &parser, QObject *parent)
    : QObject(parent)
    , m_parser(parser)
    , m_service(std::make_unique<KDBusService>(KDBusService::Unique))
{
    connect(m_service.get(), &KDBusService::activateRequested, this, &SingleInstanceGuard::handleActivateRequest);
}

SingleInstanceGuard::~SingleInstanceGuard() = default;

void SingleInstanceGuard::setMainWindow(MainWindow *window)
{
    m_window = window;
}

void SingleInstanceGuard::handleActivateRequest(const QStringList &arguments, const QString &workingDirectory)
{
    // arguments[0] is the executable path (see KDBusService::activateRequested
    // docs); a bare re-activation (e.g. via the desktop file's Activate()
    // D-Bus call) can also arrive with an empty list entirely.
    if (arguments.size() > 1) {
        m_parser.parse(arguments);
        const QStringList positional = m_parser.positionalArguments();
        if (!positional.isEmpty()) {
            const QString requestedPath = positional.first();
            const QString absolutePath = QFileInfo(requestedPath).isRelative() ? QDir(workingDirectory).absoluteFilePath(requestedPath) : requestedPath;
            m_window->openFile(absolutePath);
        }
    }

    // Raise and focus the window even if it's minimized or on another
    // virtual desktop (FR14's verification checklist). updateStartupId()
    // consumes the XDG activation token KDBusService stashed in the
    // environment for this signal emission, which Wayland compositors
    // require before honoring an activation request from another process.
    m_window->show();
    KWindowSystem::updateStartupId(m_window->windowHandle());
    m_window->raise();
    KWindowSystem::activateWindow(m_window->windowHandle());
}
