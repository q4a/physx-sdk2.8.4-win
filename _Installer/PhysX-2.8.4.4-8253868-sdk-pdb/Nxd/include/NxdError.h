#ifndef NXD_ERROR_H
#define NXD_ERROR_H

#if _MSC_VER < 1400
#define _snprintf_s _snprintf
#endif

#ifndef _WIN32
#define _snprintf snprintf
#endif

void NxdReportError(const char *z, const char *file, unsigned int line);
void NxdReportWarning(const char *z, const char *file, unsigned int line);

#endif
