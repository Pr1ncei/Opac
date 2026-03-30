#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget* parent = nullptr);

signals:
    void loginAsAdmin(const QString& username);
    void loginAsUser(const QString& username);

private slots:
    void onLogin();
    void onRegister();
    void showRegisterForm();
    void showLoginForm();

private:
    void buildLoginForm();
    void buildRegisterForm();

    QStackedWidget* stack_;

    // Login form
    QWidget* loginPage_;
    QLineEdit* usernameEdit_;
    QLineEdit* passwordEdit_;
    QLabel* loginError_;

    // Register form
    QWidget* registerPage_;
    QLineEdit* regUsernameEdit_;
    QLineEdit* regPasswordEdit_;
    QLineEdit* regConfirmEdit_;
    QLabel* regError_;
};