#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <string>

class AdminWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdminWidget(const std::string& username, QWidget* parent = nullptr);
    void refresh();

signals:
    void logoutRequested();

private slots:
    void onSearch(const QString& text);
    void onAddBook();
    void onEditBook(int row);
    void onDeleteBook(int row);
    void switchTab(int index);

private:
    void buildUI();
    void buildSidebar();
    void buildBookTable();
    void buildBorrowTable();
    void loadBooks(const QString& filter = "");
    void loadBorrowed();
    void updateStats();

    std::string username_;

    // Stats labels
    QLabel* statTotal_;
    QLabel* statAvail_;
    QLabel* statBorrowed_;
    QLabel* statOverdue_;

    // Tables
    QTableWidget* bookTable_;
    QTableWidget* borrowTable_;
    QWidget* bookPage_;
    QWidget* borrowPage_;
    QLineEdit* searchEdit_;

    // Sidebar nav buttons
    QPushButton* navBooks_;
    QPushButton* navBorrowed_;
    QPushButton* navAddBook_;
};