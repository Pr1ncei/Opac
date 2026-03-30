#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <string>

class UserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserWidget(const std::string& username, QWidget* parent = nullptr);
    void refresh();

signals:
    void logoutRequested();

private slots:
    void onBorrow();
    void onReturn();

private:
    void buildUI();
    void loadMyBooks();
    void loadAllBooks(const std::string& keyword = "");
    void updateStats();

    std::string   username_;
    QLabel* statBorrowed_;
    QLabel* statOverdue_;
    QLabel* statFee_;
    QTableWidget* borrowTable_;
};