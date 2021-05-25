/* Copyright 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>
#include <string> 

#include "base/timer/timer.h"
#include "components/prefs/pref_service.h"
#include "url/gurl.h"

class PrefRegistrySimple;

namespace brave {

class BraveTraceCollectionService {
    public:
     explicit BraveTraceCollectionService(PrefService* pref_service);
     ~BraveTraceCollectionService();

     static void RegisterLocalStatePrefs(PrefRegistrySimple* registry);

     void Start();

    private:

     void OnCollectionSlotStartTimerFired();
     void OnFakeUpdateTimerFired();

     void SendCollectionSlot();

     void SavePrefs();
     void LoadPrefs();

     int GetCurrentCollectionSlot();
     std::string GetPlatformIdentifier();
     std::string GetEphemeralID();

     bool isP3AEnabled();
     bool isAdsEnabled();
     bool isTraceCollectionEnabled();

     PrefService* pref_service_;
     GURL trace_collection_endpoint_;
     std::unique_ptr<base::RepeatingTimer> collection_slot_periodic_timer_;
     std::unique_ptr<base::RetainingOneShotTimer> fake_update_activity_timer_;

     int last_checked_slot_;
     int collection_slot_size_;
     int fake_update_duration_;
     std::string ephemeral_ID_;

     // Do we need a callback to update the last collected slot
};

} // namespace brave