/*
 * Copyright 2022 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SERVICES_COMMON_UTIL_REQUEST_METADATA_H_
#define SERVICES_COMMON_UTIL_REQUEST_METADATA_H_

#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/ascii.h"
#include "include/grpcpp/server_context.h"
#include "services/common/clients/async_client.h"
namespace privacy_sandbox::bidding_auction_servers {

// Maps metadata from a server context to a hash map.
// Only maps metadata keys passed in source_target_key_map.
// server_context: The request context from which contains the metadata.
// source_target_key_map: A map of source key to target key, eg. -
// Source metadata - {"ABC-ID": "1234" }
// Target metadata - {"X-ABC-ID": "1234" }
// source_target_key_map - {"ABC-ID" : "X-ABC-ID"}
template <std::size_t N>
inline RequestMetadata GrpcMetadataToRequestMetadata(
    const std::multimap<grpc::string_ref, grpc::string_ref>& client_metadata,
    const std::array<std::pair<std::string_view, std::string_view>, N>&
        source_target_key_map) {
  RequestMetadata mapped_metadata;
  for (auto const& key_pair : source_target_key_map) {
    // All GRPC metadata is automatically converted to lower case
    // https://github.com/grpc/grpc-go/blob/master/Documentation/grpc-metadata.md
    std::string source_key = absl::AsciiStrToLower(key_pair.first);
    if (auto const& found_metadata_itr = client_metadata.find(source_key);
        found_metadata_itr != client_metadata.end()) {
      mapped_metadata.insert({key_pair.second.data(),
                              std::string(found_metadata_itr->second.begin(),
                                          found_metadata_itr->second.end())});
    }
  }
  return mapped_metadata;
}

// Makes a list of HTTP Headers strings from a map of metadata key value pairs.
// The Http Headers have the standard Key:Value format as per
// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers.
inline std::vector<std::string> RequestMetadataToHttpHeaders(
    const RequestMetadata& metadata_map) {
  std::vector<std::string> headers;
  for (const auto& it : metadata_map) {
    headers.push_back(absl::StrCat(it.first, ":", it.second));
  }
  return headers;
}

// Makes a list of HTTP Headers strings from a map of metadata key value pairs
// and inserts mandatory_headers with empty values in case they're not in the
// map. The Http Headers have the standard Key:Value format as per
// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers.
template <std::size_t N>
inline std::vector<std::string> RequestMetadataToHttpHeaders(
    const RequestMetadata& metadata_map,
    const std::array<std::string_view, N>& mandatory_headers) {
  std::vector<std::string> headers;
  for (const auto& it : mandatory_headers) {
    if (const auto& found_itr = metadata_map.find(it.data());
        found_itr == metadata_map.end()) {
      headers.push_back(absl::StrCat(it.data(), ":"));
    }
  }
  for (const auto& it : metadata_map) {
    headers.push_back(absl::StrCat(it.first, ":", it.second));
  }
  return headers;
}

}  // namespace privacy_sandbox::bidding_auction_servers

#endif  // SERVICES_COMMON_UTIL_REQUEST_METADATA_H_
