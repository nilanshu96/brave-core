// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/components/brave_search/browser/brave_search_default_host_private.h"

#include <string>
#include <utility>

#include "components/prefs/pref_service.h"
#include "components/search_engines/template_url_service.h"

namespace brave_search {

BraveSearchDefaultHostPrivate::BraveSearchDefaultHostPrivate(
    const std::string& host,
    TemplateURLService* template_url_service,
    PrefService* prefs)
    : weak_factory_(this) {}

BraveSearchDefaultHostPrivate::~BraveSearchDefaultHostPrivate() {}

void BraveSearchDefaultHostPrivate::GetCanSetDefaultSearchProvider(
    GetCanSetDefaultSearchProviderCallback callback) {
  std::move(callback).Run(false);
}

void BraveSearchDefaultHostPrivate::SetIsDefaultSearchProvider() {}

}  // namespace brave_search
