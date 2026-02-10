# If not stated otherwise in this file or this component's Licenses.txt file the
# following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# CloudStore plugin configuration
#

option(PLUGIN_CLOUDSTORE "PLUGIN_CLOUDSTORE" ON)

if (BUILD_DBUS)
    message("Building for DBUS")

    add_definitions (-DBUILD_DBUS)
    option(BUILD_DBUS "BUILD_DBUS" ON)
    add_definitions (-DIARM_USE_DBUS)
    option(IARM_USE_DBUS "IARM_USE_DBUS" ON)
endif()

if (BUILD_ENABLE_DEVICE_MANUFACTURER_INFO)
    message("Building with device manufacturer info")
    add_definitions (-DENABLE_DEVICE_MANUFACTURER_INFO)
endif()

if (BUILD_ENABLE_TELEMETRY_LOGGING)
    message("Building with telemetry logging")
    add_definitions (-DENABLE_TELEMETRY_LOGGING)
endif()
