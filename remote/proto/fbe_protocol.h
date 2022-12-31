//------------------------------------------------------------------------------
// Automatically generated by the Fast Binary Encoding compiler, do not modify!
// https://github.com/chronoxor/FastBinaryEncoding
// Source: FBE
// FBE version: 1.11.0.0
//------------------------------------------------------------------------------

#pragma once

#if defined(__clang__)
#pragma clang system_header
#elif defined(__GNUC__)
#pragma GCC system_header
#elif defined(_MSC_VER)
#pragma system_header
#endif

#include "fbe.h"

namespace FBE {

// Fast Binary Encoding base sender
class Sender
{
public:
    Sender(const Sender&) = delete;
    Sender(Sender&&) noexcept = delete;
    virtual ~Sender() = default;

    Sender& operator=(const Sender&) = delete;
    Sender& operator=(Sender&&) noexcept = delete;

    // Get the sender buffer
    FBEBuffer& buffer() noexcept { return *_buffer; }
    const FBEBuffer& buffer() const noexcept { return *_buffer; }

    // Get the final protocol flag
    bool final() const noexcept { return _final; }

    // Get the logging flag
    bool logging() const noexcept { return _logging; }
    // Enable/Disable logging
    void logging(bool enable) noexcept { _logging = enable; }

    // Reset the sender buffer
    void reset() noexcept { _buffer->reset(); }

    // Send serialized buffer.
    // Direct call of the method requires knowledge about internals of FBE models serialization.
    // Use it with care!
    size_t send_serialized(size_t serialized);

protected:
    // Send message handler
    virtual size_t onSend(const void* data, size_t size) = 0;
    // Send log message handler
    virtual void onSendLog(const std::string& message) const {}

protected:
    std::shared_ptr<FBEBuffer> _buffer;
    bool _logging;
    bool _final;

    Sender() : Sender(nullptr) {}
    Sender(const std::shared_ptr<FBEBuffer>& buffer) : _logging(false), _final(false) { _buffer = buffer ? buffer : std::make_shared<FBEBuffer>(); }

    // Enable/Disable final protocol
    void final(bool enable) noexcept { _final = enable; }
};

// Fast Binary Encoding base receiver
class Receiver
{
public:
    Receiver(const Receiver&) = delete;
    Receiver(Receiver&&) = delete;
    virtual ~Receiver() = default;

    Receiver& operator=(const Receiver&) = delete;
    Receiver& operator=(Receiver&&) = delete;

    // Get the receiver buffer
    FBEBuffer& buffer() noexcept { return *_buffer; }
    const FBEBuffer& buffer() const noexcept { return *_buffer; }

    // Get the final protocol flag
    bool final() const noexcept { return _final; }

    // Get the logging flag
    bool logging() const noexcept { return _logging; }
    // Enable/Disable logging
    void logging(bool enable) noexcept { _logging = enable; }

    // Reset the receiver buffer
    void reset() noexcept { _buffer->reset(); }

    // Receive data
    void receive(const void* data, size_t size);

protected:
    // Receive message handler
    virtual bool onReceive(size_t type, const void* data, size_t size) = 0;
    // Receive log message handler
    virtual void onReceiveLog(const std::string& message) const {}

protected:
    std::shared_ptr<FBEBuffer> _buffer;
    bool _logging;
    bool _final;

    Receiver() : Receiver(nullptr) {}
    Receiver(const std::shared_ptr<FBEBuffer>& buffer) : _logging(false), _final(false) { _buffer = buffer ? buffer : std::make_shared<FBEBuffer>(); }

    // Enable/Disable final protocol
    void final(bool enable) noexcept { _final = enable; }
};

} // namespace FBE
