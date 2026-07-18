#include "DebugTiming.h"

#include <QDebug>
#include <QElapsedTimer>

#include <optional>

namespace DebugTiming
{
namespace
{
bool enabled()
{
    static const bool e = qEnvironmentVariableIsSet("MDVIEWER_DEBUG_TIMING");
    return e;
}

std::optional<QElapsedTimer> timer;
}

void start()
{
    if (enabled()) {
        timer.emplace();
        timer->start();
    }
}

void mark(const char *label)
{
    if (enabled() && timer)
        qDebug("[timing] %8lldms  %s", static_cast<long long>(timer->elapsed()), label);
}
}
