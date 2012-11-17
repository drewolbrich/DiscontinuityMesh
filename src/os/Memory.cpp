// Copyright 2010 Retroactive Fiasco.

#include "Memory.h"

#include <sstream>

#include <boost/format.hpp>

#include <os/Platform.h>

#ifdef OS_APPLE
#include <mach/mach_init.h>
#include <mach/task.h>
#endif

namespace os {

#ifdef OS_APPLE
static int mac_os_x_getmem(unsigned int *rss, unsigned int *vs);
#endif

unsigned long
GetProcessKilobytesUsed()
{
#ifdef OS_APPLE

    unsigned int rss = 0;
    unsigned int vs = 0;
#ifdef DEBUG
    int result = 
#endif
        mac_os_x_getmem(&rss, &vs);
    assert(result != -1);

    return vs/1024;

#else

#error "Not yet implemented."

#endif
}

std::string
GetProcessMemoryUsedAsString()
{
    std::ostringstream ostr;

    unsigned long kilobytes = GetProcessKilobytesUsed();

    if (kilobytes < 1024) {

        ostr << boost::format("%0.3g") % kilobytes << " kilobytes";

    } else if (kilobytes < 1024*1024) {

        float megabytes = kilobytes/1024.0f;
        ostr << boost::format("%0.3g") % megabytes << " megabytes";

    } else {

        float gigabytes = kilobytes/1024.0f/1024.0f;
        ostr << boost::format("%0.3g") % gigabytes << " gigabytes";
    }

    return ostr.str();
}

static int 
mac_os_x_getmem(unsigned int *rss, unsigned int *vs)
{
    // From http://blog.kuriositaet.de/?p=257

    // The following system call is undocumented and might break in future
    // versions of Mac OS X.

    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    if (KERN_SUCCESS != task_info(mach_task_self(),
            TASK_BASIC_INFO, (task_info_t) &t_info, &t_info_count)) {
        return -1;
    }

    *rss = t_info.resident_size;
    *vs = t_info.virtual_size;

    return 0;
}

} // namespace os
