#pragma once

#include <QObject>

#include <memory>

class KDBusService;
class QCommandLineParser;
class MainWindow;

// Registers this process as the single instance of the app on D-Bus (FR13).
//
// Constructing this is a checkpoint, not a mere object creation: if another
// instance is already registered, KDBusService forwards our arguments to it
// and terminates this process before the constructor returns. Code after
// constructing a SingleInstanceGuard only ever runs in the one true primary
// instance.
//
// Registration happens before the MainWindow exists (and before CLI parsing,
// per KDBusService's own docs), so the window is supplied afterwards via
// setMainWindow() rather than the constructor. This is safe: D-Bus signals
// are only dispatched once the Qt event loop is running (app.exec()), which
// happens well after setMainWindow() is called in main().
class SingleInstanceGuard : public QObject
{
    Q_OBJECT

public:
    // parser must outlive this guard; it is reused (not reconstructed) to
    // re-parse arguments forwarded from later invocations, matching the
    // pattern KDBusService::activateRequested's docs recommend.
    explicit SingleInstanceGuard(QCommandLineParser &parser, QObject *parent = nullptr);
    // Declared (not defaulted here) because std::unique_ptr<KDBusService>'s
    // destructor needs KDBusService's full definition, which this header
    // deliberately doesn't include.
    ~SingleInstanceGuard() override;

    void setMainWindow(MainWindow *window);

private:
    void handleActivateRequest(const QStringList &arguments, const QString &workingDirectory);

    QCommandLineParser &m_parser;
    MainWindow *m_window = nullptr;
    std::unique_ptr<KDBusService> m_service;
};
