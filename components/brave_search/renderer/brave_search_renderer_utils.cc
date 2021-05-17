// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/components/brave_search/renderer/brave_search_renderer_utils.h"

#include <string>
#include <vector>

#include "base/no_destructor.h"
#include "url/gurl.h"

namespace {

static base::NoDestructor<std::vector<std::string>> g_vetted_hosts(
    {"search.brave.com", "search-dev.brave.com", "search-dev-local.brave.com",
     "search.brave.software", "search.bravesoftware.com"});

}  // namespace

namespace brave_search {

bool IsAllowedHost(const GURL& url) {
  if (!url.SchemeIs("https")) {
    return false;
  }
  std::string host = url.host();
  for (size_t i = 0; i < g_vetted_hosts->size(); i++) {
    if ((*g_vetted_hosts)[i] == host)
      return true;
  }
  return false;
}

}  // namespace brave_search
