#pragma once

#include <QtCore/qglobal.h>

#if defined(APPLICATIONLOGLIB_LIBRARY)
#  define APPLICATIONLOGLIB_EXPORT Q_DECL_EXPORT
#else
#  define APPLICATIONLOGLIB_EXPORT Q_DECL_IMPORT
#endif
