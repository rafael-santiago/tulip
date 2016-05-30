#include <system/version.h>

static const char *g_tulip_system_version = "v3";

const char *get_tulip_system_version() {
    return g_tulip_system_version;
}
