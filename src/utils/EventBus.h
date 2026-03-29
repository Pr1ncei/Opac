#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <mutex>

// An event carries a name and an optional string payload.
struct Event
{
    std::string name;
    std::string payload;
};

using EventHandler = std::function<void(const Event&)>;

class EventBus
{
public:
    static EventBus& getInstance();

    // Register a handler for a named event.
    void subscribe(const std::string& eventName, EventHandler handler);

    // Fire an event — all subscribed handlers are called synchronously.
    void publish(const Event& event);

    // Convenience overload: publish by name + payload string.
    void publish(const std::string& name, const std::string& payload = "");

    EventBus(const EventBus&)            = delete;
    EventBus& operator=(const EventBus&) = delete;

private:
    EventBus() = default;

    std::unordered_map<std::string, std::vector<EventHandler>> handlers_;
    std::mutex mutex_;
};
