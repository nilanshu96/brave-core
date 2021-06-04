/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_federated/trace_collection_features.h"

#include "base/metrics/field_trial_params.h"

namespace trace_collection {
namespace features {

namespace {
const char kFeatureName[] = "UserTraceCollection";

const char kFieldTrialParameterCollectionSlotSize[] = "collection_slot_size";
const int kDefaultCollectionSlotSize = 30;

const char kFieldTrialParameterFakeUpdateDuration[] = "fake_update_duration";
const int kDefaultFakeUpdateDuration = 5;
}  // namespace

const base::Feature kTraceCollection{kFeatureName,
                                         base::FEATURE_DISABLED_BY_DEFAULT};

bool IsTraceCollectionEnabled() {
  return base::FeatureList::IsEnabled(kTraceCollection);
}

int GetCollectionSlotSizeValue() {
  return GetFieldTrialParamByFeatureAsInt(kTraceCollection,
                                             kFieldTrialParameterCollectionSlotSize,
                                             kDefaultFakeUpdateDuration);
}

int GetFakeUpdateDurationValue() {
  return GetFieldTrialParamByFeatureAsInt(kTraceCollection,
                                             kFieldTrialParameterFakeUpdateDuration,
                                             kDefaultCollectionSlotSize);
}

}  // namespace features
}  // namespace trace_collection