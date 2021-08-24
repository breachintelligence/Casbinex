/*
* Copyright 2020 The casbin Authors. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef CASBIN_CPP_UTIL_BUILT_IN_FUNCTIONS
#define CASBIN_CPP_UTIL_BUILT_IN_FUNCTIONS

#include "../model/scope_config.h"

namespace casbin {

// KeyMatch determines whether key1 matches the pattern of key2 (similar to RESTful path), key2 can contain a *.
// For example, "/foo/bar" matches "/foo/*"
ReturnType KeyMatch(Scope scope);
bool KeyMatch(std::string key1, std::string key2);

// KeyMatch2 determines whether key1 matches the pattern of key2 (similar to RESTful path), key2 can contain a *.
// For example, "/foo/bar" matches "/foo/*", "/resource1" matches "/:resource"
ReturnType KeyMatch2(Scope scope);
bool KeyMatch2(std::string key1, std::string key2);

// KeyMatch3 determines whether key1 matches the pattern of key2 (similar to RESTful path), key2 can contain a *.
// For example, "/foo/bar" matches "/foo/*", "/resource1" matches "/{resource}"
ReturnType KeyMatch3(Scope scope);
bool KeyMatch3(std::string key1, std::string key2);

// RegexMatch determines whether key1 matches the pattern of key2 in regular expression.
ReturnType RegexMatch(Scope scope);
bool RegexMatch(std::string key1, std::string key2);

// IPMatch determines whether IP address ip1 matches the pattern of IP address ip2, ip2 can be an IP address or a CIDR pattern.
// For example, "192.168.2.123" matches "192.168.2.0/24"
ReturnType IPMatch(Scope scope);
bool IPMatch(std::string ip1, std::string ip2);

// GFunction is the method of the g(_, _) function.
ReturnType GFunction(Scope scope);

} // namespace casbin

#endif