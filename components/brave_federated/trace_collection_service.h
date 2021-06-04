/* Copyright 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_FEDERATED_TRACE_COLLECTION_SERVICE_H_
#define BRAVE_COMPONENTS_BRAVE_FEDERATED_TRACE_COLLECTION_SERVICE_H_

#include <memory>
#include <string> 

#include "base/timer/timer.h"
#include "components/prefs/pref_service.h"
#include "url/gurl.h"

class PrefRegistrySimple;

namespace network {
class SharedURLLoaderFactory;
class SimpleURLLoader;
}  // namespace network

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

     std::string BuildTraceCollectionPayload() const;
     int GetCurrentCollectionSlot() const;
     std::string GetPlatformIdentifier();
     std::string GetEphemeralID();

     bool isP3AEnabled();
     bool isAdsEnabled();
     bool isTraceCollectionEnabled();

     PrefService* pref_service_;
     GURL trace_collection_endpoint_;
     std::unique_ptr<base::RepeatingTimer> collection_slot_periodic_timer_;
     std::unique_ptr<base::RetainingOneShotTimer> fake_update_activity_timer_;
     std::unique_ptr<network::SimpleURLLoader> url_loader_;

     int last_checked_slot_;
     int collection_slot_size_;
     int fake_update_duration_;
     std::string ephemeral_ID_;
     std::string platform_;

     // Do we need a callback to update the last collected slot
};

} // namespace brave

#endif  // BRAVE_COMPONENTS_BRAVE_FEDERATED_TRACE_COLLECTION_SERVICE_H_