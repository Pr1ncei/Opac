#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <string>

class LoginWidget;
class AdminWidget;
class UserWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void showAdmin(const QString& username);
    void showUser(const QString& username);
    void showLogin();

private:
    QStackedWidget* stack_;
    LoginWidget* loginWidget_;
    AdminWidget* adminWidget_ = nullptr;
    UserWidget* userWidget_ = nullptr;
};