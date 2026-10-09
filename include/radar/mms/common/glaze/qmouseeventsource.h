#pragma once

#include <radar/mms/common/config.h>

#ifndef RCOMMON_WITH_QT
#  error "Qt glaze adapters require rcommon to be built with with_qt=True"
#endif

#include <qnamespace.h>

#include <radar/mms/common/glaze/detail.h>

// `Qt::MouseEventSource` is a plain enum, which glaze reads and writes as its integer value without
// any metadata, matching the nlohmann adapter.
