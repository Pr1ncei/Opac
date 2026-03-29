#pragma once
#include <string>
#include <thread>
#include <atomic>

class System
{
public:
    System();
    ~System();

    // Start the application.
    void run();

private:
    // Parallel Programming 
    std::thread      auditThread_;
    std::atomic<bool> running_;

    void startAuditThread();
    void stopAuditThread();
    static void auditLoop(std::atomic<bool>& running);
};