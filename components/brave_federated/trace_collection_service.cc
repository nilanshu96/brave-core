/* Copyright 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>
#include <string>

#include "brave/components/brave_federated/trace_collection_service.h"

#include "base/json/json_writer.h"
#include "base/strings/string_util.h"
#include "base/timer/timer.h"
#include "bat/ads/pref_names.h"
#include "brave/components/brave_stats/browser/brave_stats_updater_util.h"
#include "brave/components/brave_federated/trace_collection_features.h"
#include "brave/components/p3a/pref_names.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "components/prefs/pref_service.h"
#include "components/prefs/pref_registry_simple.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "url/gurl.h"

namespace brave {

namespace {

static constexpr char federatedLearningUrl[] = "https://fl.brave.software/";

constexpr char kLastCheckedSlot[] = "brave.federated.last_checked_slot";
constexpr char kEphemeralID[] = "brave.federated.ephemeral_id";

net::NetworkTrafficAnnotationTag GetNetworkTrafficAnnotationTag() {
    return net::DefineNetworkTrafficAnnotation("trace_collection_service", R"(
        semantics {
          sender: "User Trace Collection Service"
          description:
            "Report of anonymized usage statistics. For more info see "
            "TODO: https://wikilink_here"
          trigger:
            "Reports are automatically generated on startup and at intervals "
            "while Brave is running."
          data:
            "A protocol buffer with anonymized and encrypted usage data."
          destination: WEBSITE
        }
        policy {
          cookies_allowed: NO
          setting:
            "Users can enable or disable it in brave://settings/privacy"
          policy_exception_justification:
            "Not implemented."
        }
    )");
}

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
        base::TimeDelta::FromSeconds(fake_update_duration_*60),
        this, &BraveTraceCollectionService::OnFakeUpdateTimerFired
    );
    
    // Recurrent timer
    DCHECK(!collection_slot_periodic_timer_);
    collection_slot_periodic_timer_ = std::make_unique<base::RepeatingTimer>();
    collection_slot_periodic_timer_->Start(
        FROM_HERE,
        base::TimeDelta::FromSeconds(2*fake_update_duration_*60),
        this, &BraveTraceCollectionService::OnCollectionSlotStartTimerFired
    );
}

void BraveTraceCollectionService::LoadPrefs() {

    collection_slot_size_ = trace_collection::features::GetCollectionSlotSizeValue();
    fake_update_duration_ = trace_collection::features::GetFakeUpdateDurationValue();
    trace_collection_endpoint_ = GURL(federatedLearningUrl);

    last_checked_slot_ = pref_service_->GetInteger(kLastCheckedSlot);
    platform_ = GetPlatformIdentifier();
    std::string ephemeral_ID = pref_service_->GetString(kEphemeralID);
    if (ephemeral_ID.empty()) {
        ephemeral_ID_ = 
                base::ToUpperASCII(base::UnguessableToken::Create().ToString());
        // TODO(lminto): May want to set expiration date for ephemeral ID, after which
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
    fake_update_activity_timer_->Reset();
}

void BraveTraceCollectionService::OnFakeUpdateTimerFired() {
    SendCollectionSlot();
}

void BraveTraceCollectionService::SendCollectionSlot() {
    const int current_collection_slot = GetCurrentCollectionSlot();
    if (current_collection_slot == last_checked_slot_) {
        return;
    }

    auto resource_request = std::make_unique<network::ResourceRequest>();
    resource_request->url = trace_collection_endpoint_;
    resource_request->headers.SetHeader("X-Brave-Trace", "?1");

    resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;
    resource_request->method = "POST";

    url_loader_ = network::SimpleURLLoader::Create(
      std::move(resource_request),
      GetNetworkTrafficAnnotationTag());
    url_loader_->AttachStringForUpload(BuildTraceCollectionPayload(), "application/base64");

    //TODO: Send request, when endpoint is available
    /*
    url_loader_->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
        url_loader_factory_.get(),
        base::BindOnce(&BraveTraceCollectionService::OnUploadComplete,
                        base::Unretained(this)));
    */

    last_checked_slot_ = current_collection_slot;
    SavePrefs();
}

std::string BraveTraceCollectionService::BuildTraceCollectionPayload() const {
  base::Value root(base::Value::Type::DICTIONARY);

  root.SetKey("ephemeral_ID", base::Value(ephemeral_ID_));
  root.SetKey("platform", base::Value(platform_));
  root.SetKey("collection_slot", base::Value(GetCurrentCollectionSlot()));

  std::string result;
  base::JSONWriter::Write(root, &result);

  return result;
}

int BraveTraceCollectionService::GetCurrentCollectionSlot() const {
    base::Time::Exploded now;
    base::Time::Now().LocalExplode(&now);
     
    return (now.hour*60+now.minute)/collection_slot_size_;
}
     
std::string BraveTraceCollectionService::GetPlatformIdentifier() {
    return brave_stats::GetPlatformIdentifier();
}

bool BraveTraceCollectionService::isTraceCollectionEnabled() {
    return trace_collection::features::IsTraceCollectionEnabled();
}

bool BraveTraceCollectionService::isAdsEnabled() {
    return ProfileManager::GetPrimaryUserProfile()->GetPrefs()->GetBoolean(
      ads::prefs::kEnabled);
}

bool BraveTraceCollectionService::isP3AEnabled() {
    return pref_service_->GetBoolean(brave::kP3AEnabled);
}


} // namespace brave