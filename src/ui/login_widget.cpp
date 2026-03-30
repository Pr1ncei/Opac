#include "login_widget.h"
#include "../managers/auth/auth.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>

LoginWidget::LoginWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setAlignment(Qt::AlignCenter);
    root->setContentsMargins(0, 0, 0, 0);

    stack_ = new QStackedWidget(this);
    stack_->setMinimumWidth(360);
    stack_->setMaximumWidth(800);
    stack_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    buildLoginForm();
    buildRegisterForm();

    stack_->addWidget(loginPage_);
    stack_->addWidget(registerPage_);
    stack_->setCurrentIndex(0);

    root->addWidget(stack_, 0, Qt::AlignCenter);
}

// ── Login form ──────────────────────────────────────────────
void LoginWidget::buildLoginForm()
{
    loginPage_ = new QWidget;
    loginPage_->setObjectName("LoginCard");

    auto* lay = new QVBoxLayout(loginPage_);
    lay->setSpacing(0);
    lay->setContentsMargins(36, 32, 36, 32);
    
    auto* title = new QLabel("SmartLib: OPAC System");
    title->setObjectName("LoginTitle");
    lay->addWidget(title);

    auto* sub = new QLabel("Sign in to your account to continue");
    sub->setObjectName("LoginSub");
    lay->addWidget(sub);
    lay->addSpacing(24);

    // Username
    auto* userLabel = new QLabel("Username");
    userLabel->setStyleSheet("color:#5f5e5a; font-size:12px;");
    lay->addWidget(userLabel);
    lay->addSpacing(4);
    usernameEdit_ = new QLineEdit;
    usernameEdit_->setPlaceholderText("e.g. admin");
    usernameEdit_->setObjectName("SearchInput");
    lay->addWidget(usernameEdit_);
    lay->addSpacing(12);

    // Password
    auto* passLabel = new QLabel("Password");
    passLabel->setStyleSheet("color:#5f5e5a; font-size:12px;");
    lay->addWidget(passLabel);
    lay->addSpacing(4);
    passwordEdit_ = new QLineEdit;
    passwordEdit_->setEchoMode(QLineEdit::Password);
    passwordEdit_->setPlaceholderText("••••••••");
    passwordEdit_->setObjectName("SearchInput");
    lay->addWidget(passwordEdit_);
    lay->addSpacing(4);

    // Error label
    loginError_ = new QLabel;
    loginError_->setStyleSheet("color:#a32d2d; font-size:12px;");
    loginError_->setVisible(false);
    lay->addWidget(loginError_);
    lay->addSpacing(12);

    // Sign in button
    auto* loginBtn = new QPushButton("Sign in");
    loginBtn->setObjectName("BtnPrimary");
    loginBtn->setCursor(Qt::PointingHandCursor);
    lay->addWidget(loginBtn);
    lay->addSpacing(12);

    // Divider
    auto* div = new QFrame;
    div->setFrameShape(QFrame::HLine);
    div->setObjectName("HDivider");
    lay->addWidget(div);
    lay->addSpacing(12);

    // Register link
    auto* regBtn = new QPushButton("Create an account");
    regBtn->setObjectName("BtnSecondary");
    regBtn->setCursor(Qt::PointingHandCursor);
    lay->addWidget(regBtn);

    auto* footer = new QLabel("DIPROGLANG · OPAC Project by CS202's C++ Group");
    footer->setAlignment(Qt::AlignCenter);
    footer->setStyleSheet("color:#aaa89f; font-size:11px; margin-top:12px;");
    lay->addWidget(footer);

    connect(loginBtn, &QPushButton::clicked, this, &LoginWidget::onLogin);
    connect(regBtn, &QPushButton::clicked, this, &LoginWidget::showRegisterForm);
    connect(passwordEdit_, &QLineEdit::returnPressed, this, &LoginWidget::onLogin);
}

// ── Register form ────────────────────────────────────────────
void LoginWidget::buildRegisterForm()
{
    registerPage_ = new QWidget;
    registerPage_->setObjectName("LoginCard");

    auto* lay = new QVBoxLayout(registerPage_);
    lay->setSpacing(0);
    lay->setContentsMargins(36, 32, 36, 32);

    auto* title = new QLabel("Create an account");
    title->setObjectName("LoginTitle");
    lay->addWidget(title);

    auto* sub = new QLabel("Fill in the details below to register");
    sub->setObjectName("LoginSub");
    lay->addWidget(sub);
    lay->addSpacing(24);

    auto addField = [&](const QString& label, QLineEdit*& edit,
        bool password = false) {
            auto* lbl = new QLabel(label);
            lbl->setStyleSheet("color:#5f5e5a; font-size:12px;");
            lay->addWidget(lbl);
            lay->addSpacing(4);
            edit = new QLineEdit;
            edit->setObjectName("SearchInput");
            if (password) edit->setEchoMode(QLineEdit::Password);
            lay->addWidget(edit);
            lay->addSpacing(12);
        };

    addField("Username", regUsernameEdit_);
    addField("Password", regPasswordEdit_, true);
    addField("Confirm password", regConfirmEdit_, true);

    regError_ = new QLabel;
    regError_->setStyleSheet("color:#a32d2d; font-size:12px;");
    regError_->setVisible(false);
    lay->addWidget(regError_);
    lay->addSpacing(8);

    auto* regBtn = new QPushButton("Register");
    regBtn->setObjectName("BtnPrimary");
    regBtn->setCursor(Qt::PointingHandCursor);
    lay->addWidget(regBtn);
    lay->addSpacing(12);

    auto* backBtn = new QPushButton("Back to sign in");
    backBtn->setObjectName("BtnSecondary");
    backBtn->setCursor(Qt::PointingHandCursor);
    lay->addWidget(backBtn);

    connect(regBtn, &QPushButton::clicked, this, &LoginWidget::onRegister);
    connect(backBtn, &QPushButton::clicked, this, &LoginWidget::showLoginForm);
}

// ── Slots ────────────────────────────────────────────────────
void LoginWidget::onLogin()
{
    loginError_->setVisible(false);
    QString user = usernameEdit_->text().trimmed();
    QString pass = passwordEdit_->text();

    if (user.isEmpty() || pass.isEmpty()) {
        loginError_->setText("Please enter your username and password.");
        loginError_->setVisible(true);
        return;
    }

    AuthService auth;
    Account account = auth.login(user.toStdString(), pass.toStdString());

    if (account.isEmpty()) {
        loginError_->setText("Invalid username or password.");
        loginError_->setVisible(true);
        passwordEdit_->clear();
        return;
    }

    if (account.isAdmin())
        emit loginAsAdmin(QString::fromStdString(account.getUsername()));
    else
        emit loginAsUser(QString::fromStdString(account.getUsername()));
}

void LoginWidget::onRegister()
{
    regError_->setVisible(false);
    QString user = regUsernameEdit_->text().trimmed();
    QString pass = regPasswordEdit_->text();
    QString confirm = regConfirmEdit_->text();

    if (user.length() < 3) {
        regError_->setText("Username must be at least 3 characters.");
        regError_->setVisible(true);
        return;
    }
    if (pass.length() < 4) {
        regError_->setText("Password must be at least 4 characters.");
        regError_->setVisible(true);
        return;
    }
    if (pass != confirm) {
        regError_->setText("Passwords do not match.");
        regError_->setVisible(true);
        return;
    }

    AuthService auth;
    if (auth.userExists(user.toStdString())) {
        regError_->setText("Username already taken.");
        regError_->setVisible(true);
        return;
    }

    if (auth.registerUser(user.toStdString(), pass.toStdString(), "user")) {
        regUsernameEdit_->clear();
        regPasswordEdit_->clear();
        regConfirmEdit_->clear();
        showLoginForm();
    }
    else {
        regError_->setText("Registration failed. Please try again.");
        regError_->setVisible(true);
    }
}

void LoginWidget::showRegisterForm() { stack_->setCurrentIndex(1); }
void LoginWidget::showLoginForm() { stack_->setCurrentIndex(0); }