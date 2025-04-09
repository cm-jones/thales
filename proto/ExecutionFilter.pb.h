#pragma once

// This is a stub header file to satisfy the include in EClient.cpp
// when Protocol Buffers are not available.

// Define macros to disable Protocol Buffer functionality in IBAPI
#ifdef IBAPI_NO_PROTOBUF
#define GOOGLE_PROTOBUF_VERIFY_VERSION

namespace protobuf {
    // Stub classes for ExecutionFilter
    class ExecutionFilter {
    public:
        void set_clientid(long id) {}
        void set_acctcode(const char* code) {}
        void set_time(const char* time) {}
        void set_symbol(const char* symbol) {}
        void set_sectype(const char* secType) {}
        void set_exchange(const char* exchange) {}
        void set_side(const char* side) {}
        void set_lastndays(int days) {}
        void add_specificdates(long date) {}
    };
}
#endif // IBAPI_NO_PROTOBUF
