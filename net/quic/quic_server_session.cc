// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/quic/quic_server_session.h"

#include "base/logging.h"
#include "net/quic/crypto/source_address_token.h"
#include "net/quic/quic_connection.h"
#include "net/quic/quic_flags.h"
#include "net/quic/quic_spdy_server_stream.h"
#include "net/quic/reliable_quic_stream.h"

namespace net {

QuicServerSession::QuicServerSession(
    const QuicConfig& config,
    QuicConnection* connection,
    QuicServerSessionVisitor* visitor)
    : QuicSession(connection, config),
      visitor_(visitor),
      bandwidth_estimate_sent_to_client_(QuicBandwidth::Zero()),
      last_server_config_update_time_(QuicTime::Zero()) {}

QuicServerSession::~QuicServerSession() {}

void QuicServerSession::InitializeSession(
    const QuicCryptoServerConfig& crypto_config) {
  QuicSession::InitializeSession();
  crypto_stream_.reset(CreateQuicCryptoServerStream(crypto_config));
}

QuicCryptoServerStream* QuicServerSession::CreateQuicCryptoServerStream(
    const QuicCryptoServerConfig& crypto_config) {
  return new QuicCryptoServerStream(crypto_config, this);
}

void QuicServerSession::OnConfigNegotiated() {
  QuicSession::OnConfigNegotiated();
  if (!FLAGS_enable_quic_fec ||
      !config()->HasReceivedConnectionOptions() ||
      !ContainsQuicTag(config()->ReceivedConnectionOptions(), kFHDR)) {
    return;
  }
  // kFHDR config maps to FEC protection always for headers stream.
  // TODO(jri): Add crypto stream in addition to headers for kHDR.
  headers_stream_->set_fec_policy(FEC_PROTECT_ALWAYS);
}

void QuicServerSession::OnConnectionClosed(QuicErrorCode error,
                                           bool from_peer) {
  QuicSession::OnConnectionClosed(error, from_peer);
  // In the unlikely event we get a connection close while doing an asynchronous
  // crypto event, make sure we cancel the callback.
  if (crypto_stream_.get() != nullptr) {
    crypto_stream_->CancelOutstandingCallbacks();
  }
  visitor_->OnConnectionClosed(connection()->connection_id(), error);
}

void QuicServerSession::OnWriteBlocked() {
  QuicSession::OnWriteBlocked();
  visitor_->OnWriteBlocked(connection());
}

void QuicServerSession::OnCongestionWindowChange(QuicTime now) {
  if (connection()->version() <= QUIC_VERSION_21) {
    return;
  }

  // If not enough time has passed since the last time we sent an update to the
  // client, then return early.
  const QuicSentPacketManager& sent_packet_manager =
      connection()->sent_packet_manager();
  int64 srtt_ms =
      sent_packet_manager.GetRttStats()->SmoothedRtt().ToMilliseconds();
  int64 now_ms = now.Subtract(last_server_config_update_time_).ToMilliseconds();
  if (now_ms < (kMinIntervalBetweenServerConfigUpdatesRTTs * srtt_ms) ||
      now_ms < kMinIntervalBetweenServerConfigUpdatesMs) {
    return;
  }

  // If the bandwidth recorder does not have a valid estimate, return early.
  const QuicSustainedBandwidthRecorder& bandwidth_recorder =
      sent_packet_manager.SustainedBandwidthRecorder();
  if (!bandwidth_recorder.HasEstimate()) {
    return;
  }

  // The bandwidth recorder has recorded at least one sustained bandwidth
  // estimate. Check that it's substantially different from the last one that
  // we sent to the client, and if so, send the new one.
  QuicBandwidth new_bandwidth_estimate = bandwidth_recorder.BandwidthEstimate();

  int64 bandwidth_delta =
      std::abs(new_bandwidth_estimate.ToBitsPerSecond() -
               bandwidth_estimate_sent_to_client_.ToBitsPerSecond());

  // Define "substantial" difference as a 50% increase or decrease from the
  // last estimate.
  bool substantial_difference =
      bandwidth_delta >
      0.5 * bandwidth_estimate_sent_to_client_.ToBitsPerSecond();
  if (!substantial_difference) {
    return;
  }

  bandwidth_estimate_sent_to_client_ = new_bandwidth_estimate;
  DVLOG(1) << "Server: sending new bandwidth estimate (KBytes/s): "
           << bandwidth_estimate_sent_to_client_.ToKBytesPerSecond();

  // Include max bandwidth in the update.
  QuicBandwidth max_bandwidth_estimate =
      bandwidth_recorder.MaxBandwidthEstimate();
  int32 max_bandwidth_timestamp = bandwidth_recorder.MaxBandwidthTimestamp();

  // Fill the proto before passing it to the crypto stream to send.
  CachedNetworkParameters cached_network_params;
  cached_network_params.set_bandwidth_estimate_bytes_per_second(
      bandwidth_estimate_sent_to_client_.ToBytesPerSecond());
  cached_network_params.set_max_bandwidth_estimate_bytes_per_second(
      max_bandwidth_estimate.ToBytesPerSecond());
  cached_network_params.set_max_bandwidth_timestamp_seconds(
      max_bandwidth_timestamp);
  cached_network_params.set_min_rtt_ms(
      sent_packet_manager.GetRttStats()->min_rtt().ToMilliseconds());
  cached_network_params.set_previous_connection_state(
      bandwidth_recorder.EstimateRecordedDuringSlowStart()
          ? CachedNetworkParameters::SLOW_START
          : CachedNetworkParameters::CONGESTION_AVOIDANCE);
  cached_network_params.set_timestamp(
      connection()->clock()->WallNow().ToUNIXSeconds());
  if (!serving_region_.empty()) {
    cached_network_params.set_serving_region(serving_region_);
  }

  crypto_stream_->SendServerConfigUpdate(&cached_network_params, false);
  last_server_config_update_time_ = now;
}

bool QuicServerSession::ShouldCreateIncomingDataStream(QuicStreamId id) {
  if (id % 2 == 0) {
    DVLOG(1) << "Invalid incoming even stream_id:" << id;
    connection()->SendConnectionClose(QUIC_INVALID_STREAM_ID);
    return false;
  }
  if (GetNumOpenStreams() >= get_max_open_streams()) {
    DVLOG(1) << "Failed to create a new incoming stream with id:" << id
             << " Already " << GetNumOpenStreams() << " streams open (max "
             << get_max_open_streams() << ").";
    connection()->SendConnectionClose(QUIC_TOO_MANY_OPEN_STREAMS);
    return false;
  }
  return true;
}

QuicDataStream* QuicServerSession::CreateIncomingDataStream(
    QuicStreamId id) {
  if (!ShouldCreateIncomingDataStream(id)) {
    return nullptr;
  }

  return new QuicSpdyServerStream(id, this);
}

QuicDataStream* QuicServerSession::CreateOutgoingDataStream() {
  DLOG(ERROR) << "Server push not yet supported";
  return nullptr;
}

QuicCryptoServerStream* QuicServerSession::GetCryptoStream() {
  return crypto_stream_.get();
}

}  // namespace net
