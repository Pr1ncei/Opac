#include "user_widget.h"
#include "borrow_dialog.h"
#include "return_dialog.h"
#include "../managers/books/BookManager.h"
#include "../managers/borrow/BorrowManager.h"
#include "../managers/fee/FeeCalculator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QFrame>
#include <QDate>

UserWidget::UserWidget(const std::string& username, QWidget* parent)
    : QWidget(parent), username_(username)
{
    buildUI();
    refresh();
}

void UserWidget::buildUI()
{
    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // Sidebar
    auto* sidebar = new QWidget;
    sidebar->setObjectName("Sidebar");
    sidebar->setFixedWidth(192);
    auto* sLay = new QVBoxLayout(sidebar);
    sLay->setContentsMargins(0, 0, 0, 0);
    sLay->setSpacing(0);

    auto addSection = [&](const QString& lbl) {
        auto* l = new QLabel(lbl.toUpper());
        l->setObjectName("NavSection");
        sLay->addWidget(l);
        };
    auto addNav = [&](const QString& text, auto slot) {
        auto* btn = new QPushButton(text);
        btn->setObjectName("NavBtn");
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, slot);
        sLay->addWidget(btn);
        };

    addSection("Catalog");
    addNav("Browse books", [=] { loadAllBooks(); });
    addSection("My books");
    addNav("My borrowed books", [=] { loadMyBooks(); });
    addNav("Borrow a book", [=] { onBorrow(); });
    addSection("Account");
    addNav("Logout", [=] { emit logoutRequested(); });
    sLay->addStretch();
    root->addWidget(sidebar);

    // Main area
    auto* mainArea = new QWidget; mainArea->setObjectName("MainArea");
    auto* mLay = new QVBoxLayout(mainArea);
    mLay->setContentsMargins(24, 24, 24, 24);
    mLay->setSpacing(16);

    // Top bar
    auto* topBar = new QHBoxLayout;
    auto* pageTitle = new QLabel("My borrowed books");
    pageTitle->setObjectName("PageTitle");
    auto* returnBtn = new QPushButton("Return a book");
    returnBtn->setObjectName("BtnSecondary");
    returnBtn->setCursor(Qt::PointingHandCursor);
    auto* borrowBtn = new QPushButton("+ Borrow book");
    borrowBtn->setObjectName("BtnPrimary");
    borrowBtn->setCursor(Qt::PointingHandCursor);
    topBar->addWidget(pageTitle);
    topBar->addStretch();
    topBar->addWidget(returnBtn);
    topBar->addSpacing(8);
    topBar->addWidget(borrowBtn);
    mLay->addLayout(topBar);

    // Stats row
    auto* statsRow = new QHBoxLayout; statsRow->setSpacing(10);
    auto makeStat = [&](const QString& label, QLabel*& valLbl, bool warn = false) {
        auto* card = new QWidget; card->setObjectName("StatCard");
        auto* cLay = new QVBoxLayout(card); cLay->setSpacing(2);
        auto* lbl = new QLabel(label); lbl->setObjectName("StatLabel");
        valLbl = new QLabel("0"); valLbl->setObjectName("StatValue");
        if (warn) valLbl->setProperty("warn", true);
        cLay->addWidget(lbl); cLay->addWidget(valLbl);
        statsRow->addWidget(card);
        };
    makeStat("Currently borrowed", statBorrowed_);
    makeStat("Overdue books", statOverdue_, true);
    makeStat("Total fee due", statFee_, true);
    mLay->addLayout(statsRow);

    // Table card
    auto* card = new QWidget; card->setObjectName("Card");
    auto* cardLay = new QVBoxLayout(card);
    cardLay->setContentsMargins(0, 0, 0, 0);
    cardLay->setSpacing(0);

    auto* cardHead = new QLabel("Books");
    cardHead->setObjectName("CardHeader");
    cardLay->addWidget(cardHead);

    auto* divLine = new QFrame;
    divLine->setFrameShape(QFrame::HLine);
    divLine->setObjectName("HDivider");
    cardLay->addWidget(divLine);

    borrowTable_ = new QTableWidget(0, 5); // single table for both views
    borrowTable_->setHorizontalHeaderLabels({ "ID","Title","Author","Dewey","Available" });
    borrowTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    borrowTable_->setColumnWidth(0, 40);
    borrowTable_->setColumnWidth(2, 100);
    borrowTable_->setColumnWidth(3, 80);
    borrowTable_->setColumnWidth(4, 80);
    borrowTable_->verticalHeader()->setVisible(false);
    borrowTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    borrowTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    borrowTable_->setShowGrid(false);
    cardLay->addWidget(borrowTable_);
    mLay->addWidget(card);
    root->addWidget(mainArea);

    connect(borrowBtn, &QPushButton::clicked, this, &UserWidget::onBorrow);
    connect(returnBtn, &QPushButton::clicked, this, &UserWidget::onReturn);
}

void UserWidget::refresh()
{
    loadMyBooks();
    updateStats();
}

void UserWidget::loadMyBooks()
{
    borrowTable_->setRowCount(0);
    try {
        BorrowManager mgr;
        auto records = mgr.getActiveBorrowsForUser(username_);
        QString today = QDate::currentDate().toString("yyyy-MM-dd");

        for (auto& r : records) {
            int row = borrowTable_->rowCount();
            borrowTable_->insertRow(row);
            borrowTable_->setItem(row, 0, new QTableWidgetItem(QString::number(r.borrowId)));
            borrowTable_->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r.bookTitle)));
            borrowTable_->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(r.dueDate)));
            borrowTable_->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(r.dueDate < today.toStdString() ? "Overdue" : "On time")));
            borrowTable_->setItem(row, 4, new QTableWidgetItem("—"));
        }
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void UserWidget::loadAllBooks(const std::string& keyword)
{
    borrowTable_->setRowCount(0);
    try {
        BookManager mgr;
        auto books = keyword.empty() ? mgr.getAllBooks() : mgr.searchBooksGui(keyword);

        for (auto& b : books) {
            int row = borrowTable_->rowCount();
            borrowTable_->insertRow(row);
            borrowTable_->setItem(row, 0, new QTableWidgetItem(QString::number(b.bookId)));
            borrowTable_->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(b.title)));
            borrowTable_->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(b.author)));
            borrowTable_->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(b.dewey)));
            borrowTable_->setItem(row, 4, new QTableWidgetItem(QString::number(b.availableCopies)));
        }
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void UserWidget::updateStats()
{
    try {
        BorrowManager mgr;
        auto records = mgr.getActiveBorrowsForUser(username_);
        QString today = QDate::currentDate().toString("yyyy-MM-dd");
        int overdue = 0;
        double totalFee = 0.0;

        for (auto& r : records) {
            QString due = QString::fromStdString(r.dueDate).left(10);
            if (due < today) {
                overdue++;
                int days = FeeCalculator::overdueDays(r.dueDate, today.toStdString());
                totalFee += FeeCalculator::overdueFee(days);
            }
        }

        statBorrowed_->setText(QString::number(records.size()));
        statOverdue_->setText(QString::number(overdue));
        statFee_->setText("₱" + QString::number(totalFee, 'f', 2));
    }
    catch (...) {}
}

void UserWidget::onBorrow()
{
    BorrowDialog dlg(username_, this);
    if (dlg.exec() == QDialog::Accepted)
        refresh();
}

void UserWidget::onReturn()
{
    ReturnDialog dlg(username_, this);
    if (dlg.exec() == QDialog::Accepted)
        refresh();
}