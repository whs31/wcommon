#pragma once

#include <whs31/common/config.h>

#ifndef WHS31_COMMON_WITH_QT
#  error "Qt glaze adapters require wcommon to be built with with_qt=True"
#endif

#include <qnamespace.h>

#include <whs31/common/glaze/detail.h>

// `Qt::Key` is a plain enum, which glaze reads and writes as its integer value without any
// metadata, matching the nlohmann adapter.
