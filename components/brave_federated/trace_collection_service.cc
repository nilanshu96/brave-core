/* Copyright 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>
#include <string>

#include "brave/components/brave_federated/trace_collection_service.h"

#include "base/timer/timer.h"
#include "bat/ads/pref_names.h"
#include "brave/components/brave_stats/browser/brave_stats_updater_util.h"
#include "brave/components/p3a/pref_names.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "components/prefs/pref_service.h"
#include "components/prefs/pref_registry_simple.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "url/gurl.h"

// DEBUG
#include <iostream>

namespace brave {

namespace {

//static constexpr char federatedLearningUrl[] = "https://fl.brave.com/";
//static constexpr char traceCollectionWikiUrl[] = "https://#########";

constexpr int kDefaultFakeUpdateDuration = 1 * 60; // 1 minute (while debugging).
constexpr int kDefaultCollectionSlotSize = 30; // 30 minutes.

constexpr char kLastCheckedSlot[] = "brave.federated.last_checked_slot";
constexpr char kEphemeralID[] = "brave.federated.ephemeral_id";
//onstexpr char kCollectionSlotSize[] = "brave.federated.collection_slot_size";
//constexpr char kFakeUpdateDuration[] = "brave.federated.fake_update_duration"; 

} // anonymous namespace

BraveTraceCollectionService::BraveTraceCollectionService(PrefService* pref_service)
        : pref_service_(pref_service) {}

BraveTraceCollectionService::~BraveTraceCollectionService() {}

void BraveTraceCollectionService::RegisterLocalStatePrefs(PrefRegistrySimple* registry) {
  registry->RegisterIntegerPref(kLastCheckedSlot, -1);
  registry->RegisterStringPref(kEphemeralID, {});
}

void BraveTraceCollectionService::Start() {
    if (!isAdsEnabled() || !isP3AEnabled() || !isTraceCollectionEnabled()) {
        return;
    }

    LoadPrefs();

    // Fake update timer for current slot
    DCHECK(!fake_update_activity_timer_);
    fake_update_activity_timer_ = std::make_unique<base::RetainingOneShotTimer>();
    fake_update_activity_timer_->Start(
        FROM_HERE,
        base::TimeDelta::FromSeconds(fake_update_duration_),
        this, &BraveTraceCollectionService::OnFakeUpdateTimerFired
    );
    
    // Recurrent timer
    DCHECK(!collection_slot_periodic_timer_);
    collection_slot_periodic_timer_ = std::make_unique<base::RepeatingTimer>();
    collection_slot_periodic_timer_->Start(
        FROM_HERE,
        base::TimeDelta::FromSeconds(2*fake_update_duration_), //TODO: Need logic
        this, &BraveTraceCollectionService::OnCollectionSlotStartTimerFired
    );
}


void BraveTraceCollectionService::LoadPrefs() {

    collection_slot_size_ = kDefaultCollectionSlotSize;
    fake_update_duration_ = kDefaultFakeUpdateDuration;

    last_checked_slot_ = pref_service_->GetInteger(kLastCheckedSlot);
    std::string ephemeral_ID = pref_service_->GetString(kEphemeralID);
    if (ephemeral_ID.empty()) {
        ephemeral_ID_ = 
                base::ToUpperASCII(base::UnguessableToken::Create().ToString());
        // TODO: May want to set expiration date for ephemeral ID, after which
        // it needs to be rotated.
    } else {
        ephemeral_ID_ = ephemeral_ID;
    }
}

void BraveTraceCollectionService::SavePrefs() {
    pref_service_->SetInteger(kLastCheckedSlot, last_checked_slot_);
    pref_service_->SetString(kEphemeralID, ephemeral_ID_);
}

void BraveTraceCollectionService::OnCollectionSlotStartTimerFired() {
    std::cerr << "FL-Trace-Collection: Collection slot timer fired \n";
    fake_update_activity_timer_->Reset();
}

void BraveTraceCollectionService::OnFakeUpdateTimerFired() {
    std::cerr << "FL-Trace-Collection: Fake computation timer fired \n";
    SendCollectionSlot();
}

void BraveTraceCollectionService::SendCollectionSlot() {
    const int current_collection_slot = GetCurrentCollectionSlot();
    if (current_collection_slot == last_checked_slot_) {
        return;
    }

    // TODO: Post HTTP request to collection endpoint
    /*
    auto resource_request = std::make_unique<network::ResourceRequest>();
    resource_request->url = trace_collection_endpoint_;
    //resource_request->headers.SetHeader("X-Brave-Trace", "?1"); Needed??

    resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;
    resource_request->method = "POST";
    */

    // Get payload information
    const std::string ephemeral_ID = GetEphemeralID();
    const std::string platform = GetPlatformIdentifier();

    //TODO: Send request

    last_checked_slot_ = current_collection_slot;
    SavePrefs();
}

int BraveTraceCollectionService::GetCurrentCollectionSlot() {
    base::Time::Exploded now;
    base::Time::Now().LocalExplode(&now);
     
    std::cerr << "FL-Trace-Collection: H:" << now.hour
              << ", M:" << now.minute << "\n";
    return (now.hour*60+now.minute)/collection_slot_size_;
}
     
std::string BraveTraceCollectionService::GetPlatformIdentifier() {
    return brave_stats::GetPlatformIdentifier();
}

std::string BraveTraceCollectionService::GetEphemeralID() {
    return ephemeral_ID_;
}

bool BraveTraceCollectionService::isTraceCollectionEnabled() {
    // TODO: Hook up with Griffin feature switch
    return true;
}

bool BraveTraceCollectionService::isAdsEnabled() {
    return ProfileManager::GetPrimaryUserProfile()->GetPrefs()->GetBoolean(
      ads::prefs::kEnabled);
}

bool BraveTraceCollectionService::isP3AEnabled() {
    return pref_service_->GetBoolean(brave::kP3AEnabled);
}


} // namespace brave