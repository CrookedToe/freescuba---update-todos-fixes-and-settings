#pragma once

#include <openvr.h>
#include "../ipc_protocol.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Add this at the top of the file, after the include guards
enum class ConnectResult {
    Success,
    PipeOpenError,
    PipeTimeout,
    SetPipeModeError,
    HandshakeError,
    VersionMismatch
};

class IPCClient {
public:
	~IPCClient();

	ConnectResult Connect();

	protocol::Response_t SendBlocking(const protocol::Request_t& request) const;

	void Send(const protocol::Request_t& request) const;
	protocol::Response_t Receive() const;

private:
	HANDLE pipe = INVALID_HANDLE_VALUE;
};