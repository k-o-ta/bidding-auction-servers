/*
 * Copyright 2023 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SERVICES_COMMON_ENCRYPTION_CRYPTO_CLIENT_WRAPPER_H_
#define SERVICES_COMMON_ENCRYPTION_CRYPTO_CLIENT_WRAPPER_H_

#include <memory>
#include <utility>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "cc/public/cpio/interface/crypto_client/crypto_client_interface.h"
#include "services/common/encryption/crypto_client_wrapper_interface.h"
#include "src/cpp/encryption/key_fetcher/interface/private_key_fetcher_interface.h"

namespace privacy_sandbox::bidding_auction_servers {

// Value for HPKE shared info field; this is not a sensitive field and just
// needs to be the same on both the sending and receiving end.
inline constexpr char kSharedInfo[] = "shared_info";

// Constants for error messages.
inline constexpr char kHpkeEncrypt[] = "HpkeEncrypt";
inline constexpr char kHpkeDecrypt[] = "HpkeDecrypt";
inline constexpr char kAeadEncrypt[] = "AeadEncrypt";
inline constexpr char kAeadDecrypt[] = "AeadDecrypt";
inline constexpr char kCryptoOperationFailureError[] =
    "Failure during %s: (error: %s)";

class CryptoClientWrapper : public CryptoClientWrapperInterface {
 public:
  CryptoClientWrapper() = default;

  CryptoClientWrapper(
      std::unique_ptr<google::scp::cpio::CryptoClientInterface> crypto_client);

  virtual ~CryptoClientWrapper();

  // Encrypts a plaintext payload using HPKE.
  absl::StatusOr<google::cmrt::sdk::crypto_service::v1::HpkeEncryptResponse>
  HpkeEncrypt(const google::cmrt::sdk::public_key_service::v1::PublicKey& key,
              absl::string_view plaintext_payload) noexcept;

  // Decrypts a ciphertext using HPKE.
  absl::StatusOr<google::cmrt::sdk::crypto_service::v1::HpkeDecryptResponse>
  HpkeDecrypt(const server_common::PrivateKey& private_key,
              absl::string_view ciphertext) noexcept;

  // Encrypts plaintext payload using AEAD and a secret derived from the HPKE
  // decrypt operation.
  absl::StatusOr<google::cmrt::sdk::crypto_service::v1::AeadEncryptResponse>
  AeadEncrypt(absl::string_view plaintext_payload,
              absl::string_view secret) noexcept;

  // Decrypts a ciphertext using AEAD and a secret derived from the HPKE
  // encrypt operation.
  absl::StatusOr<google::cmrt::sdk::crypto_service::v1::AeadDecryptResponse>
  AeadDecrypt(absl::string_view ciphertext, absl::string_view secret) noexcept;

 private:
  std::unique_ptr<google::scp::cpio::CryptoClientInterface> crypto_client_;
};

}  // namespace privacy_sandbox::bidding_auction_servers

#endif  // SERVICES_COMMON_ENCRYPTION_CRYPTO_CLIENT_WRAPPER_H_
