#include <QApplication>
#include <QCommandLineParser>

#include "DebugTiming.h"
#include "MainWindow.h"
#include "SingleInstanceGuard.h"

int main(int argc, char *argv[])
{
    DebugTiming::start();

    QApplication app(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("markdownviewer"));
    QApplication::setApplicationName(QStringLiteral("markdownviewer"));
    QApplication::setOrganizationDomain(QStringLiteral("markupt.io"));
    DebugTiming::mark("QApplication constructed");

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("A lightweight Markdown viewer for KDE Plasma"));
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("file"), QStringLiteral("Markdown file to open"), QStringLiteral("[file]"));

    // Register on D-Bus before parsing arguments: if another instance is
    // already running, this call forwards our raw argv to it and terminates
    // this process here, so parsing (and --help/--version handling) is only
    // ever done by the primary instance.
    SingleInstanceGuard guard(parser);
    DebugTiming::mark("D-Bus registered");

    parser.process(app);

    MainWindow window;
    guard.setMainWindow(&window);

    const QStringList args = parser.positionalArguments();
    if (!args.isEmpty())
        window.openFile(args.first());
    DebugTiming::mark("about to call show()");

    window.show();
    DebugTiming::mark("first show()");

    return app.exec();
}
