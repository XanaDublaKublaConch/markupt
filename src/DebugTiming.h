#pragma once

// Optional startup-timing instrumentation for validating NFR1 (<300ms cold
// start). Active only when MDVIEWER_DEBUG_TIMING is set in the environment;
// otherwise every call is a single cached-bool check. No CLI flag, no
// release-build impact.
namespace DebugTiming
{
// Call once, as early as possible in main().
void start();

// Logs elapsed time (ms) since start() to stderr, labeled. No-op if
// MDVIEWER_DEBUG_TIMING isn't set or start() wasn't called.
void mark(const char *label);
}
