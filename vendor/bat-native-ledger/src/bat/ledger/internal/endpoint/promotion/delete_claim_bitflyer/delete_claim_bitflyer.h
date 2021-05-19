/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_ENDPOINT_PROMOTION_DELETE_CLAIM_BITFLYER_DELETE_CLAIM_BITFLYER_H_
#define BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_ENDPOINT_PROMOTION_DELETE_CLAIM_BITFLYER_DELETE_CLAIM_BITFLYER_H_

#include <string>

#include "bat/ledger/ledger.h"

// DELETE /v3/wallet/bitflyer/{payment_id}/claim
//
// Request body:
// {}
//
// Success code:
// HTTP_OK (200)
//
// Error codes:
// HTTP_BAD_REQUEST (400)
// HTTP_NOT_FOUND (404)
// HTTP_CONFLICT (409)
// HTTP_INTERNAL_SERVER_ERROR (500)
//
// Response body:
// {Empty}

namespace ledger {
class LedgerImpl;

namespace endpoint {
namespace promotion {

using DeleteClaimBitflyerCallback =
    std::function<void(const type::Result result)>;

class DeleteClaimBitflyer {
 public:
  explicit DeleteClaimBitflyer(LedgerImpl* ledger);
  ~DeleteClaimBitflyer();

  void Request(DeleteClaimBitflyerCallback callback);

 private:
  std::string GetUrl();

  type::Result CheckStatusCode(const int status_code);

  void OnRequest(const type::UrlResponse& response,
                 DeleteClaimBitflyerCallback callback);

  LedgerImpl* ledger_;  // NOT OWNED
};

}  // namespace promotion
}  // namespace endpoint
}  // namespace ledger

#endif  // BRAVE_VENDOR_BAT_NATIVE_LEDGER_SRC_BAT_LEDGER_INTERNAL_ENDPOINT_PROMOTION_DELETE_CLAIM_BITFLYER_DELETE_CLAIM_BITFLYER_H_
