#pragma once

#include <iostream>
#include <set>
#include "ExecutionFilter.pb.h"

// This is a stub header file to satisfy the include in EClient.cpp
// when Protocol Buffers are not available.

#ifdef IBAPI_NO_PROTOBUF
namespace protobuf {
    // Stub class for ExecutionRequest
    class ExecutionRequest {
    public:
        void set_reqid(int id) {}
        void set_allocated_executionfilter(ExecutionFilter* filter) {}
        void release_executionfilter() {}
        // Keep original capitalization for API compatibility
        // NOLINTNEXTLINE(readability-identifier-naming)
        bool SerializeToOstream(std::ostream* output) { return true; }
    };
}

// Define constants that IBAPI is using for protobuf functionality
constexpr int PROTOBUF_MSG_ID = 1000;
namespace ibapi {
    namespace client_constants {
        const std::set<int> PROTOBUF_MSG_IDS = { 
            61  // REQ_EXECUTIONS
        };
    }
}
#endif // IBAPI_NO_PROTOBUF
