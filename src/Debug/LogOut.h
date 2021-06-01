/*
 * Copyright (C) 2021. Huawei Device Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the MIT License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * MIT License for more details.
 */

#ifndef DEBUG_LOG_OUT_H
#define DEBUG_LOG_OUT_H

#include <fstream>
#include <string>
#include <filesystem>
#include <sstream>

using namespace std;

#define LOGGER_DEBUG(message) \
    {}
#define LOGGER_TRACE(message) \
    {}
#define LOGGER_INFO(message) \
    {}
#define LOGGER_ERROR(message) \
    {}
#define LOG_DEBUG(message) \
    {}
#define LOG_ERROR(message) \
    {}

#define LOGGER_DEBUG_TAG(message) \
    {}
#define LOGGER_TRACE_TAG(message) \
    {}
#define LOGGER_INFO_TAG(message) \
    {}

#endif