#include "System.h"
#include "../ui/main_window.h"
#include "../utils/Logger.h"
#include "../utils/EventBus.h"
#include <QApplication>
#include <chrono>

System::System() : running_(false)
{
    Logger::getInstance().info("=== OPAC System starting ===");
}

System::~System()
{
    stopAuditThread();
    Logger::getInstance().info("=== OPAC System stopped ===");
}

void System::run()
{
    startAuditThread();
    int    argc = 0;
    char** argv = nullptr;
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    app.exec(); 

    stopAuditThread();
}

void System::startAuditThread()
{
    running_.store(true);
    auditThread_ = std::thread(&System::auditLoop, std::ref(running_));
    Logger::getInstance().info("Audit thread started.");
}

void System::stopAuditThread()
{
    if (running_.load())
    {
        running_.store(false);
        if (auditThread_.joinable())
            auditThread_.join();
        Logger::getInstance().info("Audit thread stopped.");
    }
}

void System::auditLoop(std::atomic<bool>& running)
{
    EventBus::getInstance().subscribe("BOOK_BORROWED", [](const Event& e) {
        Logger::getInstance().info("[AUDIT] Borrow recorded - " + e.payload);
        });
    EventBus::getInstance().subscribe("BOOK_RETURNED", [](const Event& e) {
        Logger::getInstance().info("[AUDIT] Return recorded - " + e.payload);
        });

    while (running.load())
    {
        for (int i = 0; i < 60 && running.load(); ++i)
            std::this_thread::sleep_for(std::chrono::seconds(1));
        if (running.load())
            Logger::getInstance().info("[AUDIT] Heartbeat - system is running.");
    }
}