#include "return_dialog.h"
#include "../managers/books/BookManager.h"
#include "../managers/borrow/BorrowManager.h"
#include "../managers/fee/FeeCalculator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>
#include <QGridLayout>
#include <QFrame>
#include <QMessageBox>
#include <QDate>

ReturnDialog::ReturnDialog(const std::string& username, QWidget* parent)
    : QDialog(parent), username_(username)
{
    setWindowTitle("Return a book");
    setFixedWidth(440);
    setModal(true);
    buildUI();
}

void ReturnDialog::buildUI()
{
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(24, 24, 24, 24);
    lay->setSpacing(12);

    auto* title = new QLabel("Return a book");
    title->setObjectName("PageTitle");
    lay->addWidget(title);

    // Book ID lookup row
    auto* idRow = new QHBoxLayout;
    bookIdEdit_ = new QLineEdit;
    bookIdEdit_->setPlaceholderText("Enter book ID to return…");
    bookIdEdit_->setObjectName("SearchInput");
    auto* lookupBtn = new QPushButton("Look up");
    lookupBtn->setObjectName("BtnSecondary");
    lookupBtn->setFixedWidth(90);
    lookupBtn->setCursor(Qt::PointingHandCursor);
    idRow->addWidget(bookIdEdit_);
    idRow->addWidget(lookupBtn);
    lay->addLayout(idRow);

    lookupError_ = new QLabel;
    lookupError_->setStyleSheet("color:#a32d2d; font-size:12px;");
    lookupError_->setVisible(false);
    lay->addWidget(lookupError_);

    // Book preview panel
    previewPanel_ = new QWidget;
    previewPanel_->setObjectName("ReceiptPanel");
    auto* pLay = new QVBoxLayout(previewPanel_);
    pLay->setSpacing(3);
    previewTitle_ = new QLabel;
    previewTitle_->setObjectName("ReceiptValue");
    previewTitle_->setStyleSheet("font-size:13px; font-weight:600;");
    previewDue_ = new QLabel; previewDue_->setObjectName("ReceiptKey");
    previewBadge_ = new QLabel; previewBadge_->setFixedHeight(20);
    auto* topRow = new QHBoxLayout;
    topRow->addWidget(previewDue_);
    topRow->addStretch();
    topRow->addWidget(previewBadge_);
    pLay->addWidget(previewTitle_);
    pLay->addLayout(topRow);
    previewPanel_->setVisible(false);
    lay->addWidget(previewPanel_);

    // Return date section
    dateSection_ = new QWidget;
    auto* dLay = new QVBoxLayout(dateSection_);
    dLay->setContentsMargins(0, 0, 0, 0);
    dLay->setSpacing(4);
    auto* dateLabel = new QLabel("Return date");
    dateLabel->setStyleSheet("color:#5f5e5a; font-size:12px;");
    dLay->addWidget(dateLabel);
    returnDateEdit_ = new QDateEdit(QDate::currentDate());
    returnDateEdit_->setCalendarPopup(true);
    returnDateEdit_->setDisplayFormat("yyyy-MM-dd");
    returnDateEdit_->setObjectName("SearchInput");
    dLay->addWidget(returnDateEdit_);
    dateSection_->setVisible(false);
    lay->addWidget(dateSection_);

    // Receipt panel
    receiptPanel_ = new QWidget;
    receiptPanel_->setObjectName("ReceiptPanel");
    auto* rLay = new QVBoxLayout(receiptPanel_);
    rLay->setSpacing(6);

    auto* rTitle = new QLabel("RETURN RECEIPT");
    rTitle->setObjectName("ReceiptTitle");
    rLay->addWidget(rTitle);

    auto addRow = [&](const QString& key, QLabel*& valLbl) {
        auto* row = new QHBoxLayout;
        auto* k = new QLabel(key); k->setObjectName("ReceiptKey");
        valLbl = new QLabel("—");  valLbl->setObjectName("ReceiptValue");
        row->addWidget(k);
        row->addStretch();
        row->addWidget(valLbl);
        rLay->addLayout(row);
        };

    addRow("Due date", receiptDue_);
    addRow("Return date", receiptReturn_);
    addRow("Overdue days", receiptDays_);
    addRow("Fee rate", receiptFee_);

    auto* divLine = new QFrame;
    divLine->setFrameShape(QFrame::HLine);
    divLine->setObjectName("HDivider");
    rLay->addWidget(divLine);

    auto* totalRow = new QHBoxLayout;
    auto* totalLbl = new QLabel("Total fee");
    totalLbl->setStyleSheet("font-size:13px; font-weight:600;");
    feeAmount_ = new QLabel("₱0.00");
    feeAmount_->setObjectName("FeeAmountNone");
    totalRow->addWidget(totalLbl);
    totalRow->addStretch();
    totalRow->addWidget(feeAmount_);
    rLay->addLayout(totalRow);
    receiptPanel_->setVisible(false);
    lay->addWidget(receiptPanel_);

    // Confirm + cancel buttons
    confirmBtn_ = new QPushButton("Confirm payment & return");
    confirmBtn_->setObjectName("BtnSuccess");
    confirmBtn_->setCursor(Qt::PointingHandCursor);
    confirmBtn_->setVisible(false);
    lay->addWidget(confirmBtn_);

    auto* cancelBtn = new QPushButton("Cancel");
    cancelBtn->setObjectName("BtnSecondary");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    lay->addWidget(cancelBtn);

    connect(lookupBtn, &QPushButton::clicked, this, &ReturnDialog::onLookup);
    connect(bookIdEdit_, &QLineEdit::returnPressed, this, &ReturnDialog::onLookup);
    connect(returnDateEdit_, &QDateEdit::dateChanged, this, &ReturnDialog::onDateChanged);
    connect(confirmBtn_, &QPushButton::clicked, this, &ReturnDialog::onConfirm);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void ReturnDialog::onLookup()
{
    // Reset all state
    lookupError_->setVisible(false);
    previewPanel_->setVisible(false);
    dateSection_->setVisible(false);
    receiptPanel_->setVisible(false);
    confirmBtn_->setVisible(false);
    borrowId_ = -1;
    bookId_ = -1;
    dueDate_.clear();

    bool ok;
    int id = bookIdEdit_->text().trimmed().toInt(&ok);
    if (!ok || id <= 0) {
        lookupError_->setText("Please enter a valid book ID.");
        lookupError_->setVisible(true);
        return;
    }

    try {
        BorrowManager mgr;
        // getActiveBorrow(int bookId, const std::string& username)
        BorrowRow record = mgr.getActiveBorrow(id, username_);

        if (record.borrowId < 0) {
            lookupError_->setText("No active borrow record found for this book.");
            lookupError_->setVisible(true);
            return;
        }

        // Store state from the BorrowRow struct fields
        borrowId_ = record.borrowId;
        dueDate_ = record.dueDate;
        bookId_ = id;

        // Show book details in preview
        BookManager bMgr;
        Book book = bMgr.getBookById(id);
        previewTitle_->setText(QString::fromStdString(book.getTitle()));

        QString due = QString::fromStdString(dueDate_).left(10);
        QString today = QDate::currentDate().toString("yyyy-MM-dd");
        bool overdue = (due < today);

        previewDue_->setText("Due: " + due);
        previewBadge_->setObjectName(overdue ? "BadgeOverdue" : "BadgeAvail");
        previewBadge_->setText(overdue ? "Overdue" : "On time");
        previewBadge_->style()->unpolish(previewBadge_);
        previewBadge_->style()->polish(previewBadge_);

        previewPanel_->setVisible(true);
        dateSection_->setVisible(true);
        onDateChanged();   // trigger receipt calculation immediately
    }
    catch (std::exception& e) {
        lookupError_->setText(QString::fromStdString(e.what()));
        lookupError_->setVisible(true);
    }
}

void ReturnDialog::onDateChanged()
{
    if (dueDate_.empty()) return;

    QString     returnDateStr = returnDateEdit_->date().toString("yyyy-MM-dd");
    std::string retStd = returnDateStr.toStdString();

    int    days = FeeCalculator::overdueDays(dueDate_, retStd);
    double fee = FeeCalculator::overdueFee(days);

    receiptDue_->setText(QString::fromStdString(dueDate_).left(10));
    receiptReturn_->setText(returnDateStr);
    receiptDays_->setText(days > 0 ? QString::number(days) + " day(s)" : "None");
    receiptFee_->setText("₱" + QString::number(FeeCalculator::FEE_PER_DAY, 'f', 2) + " / day");

    if (days > 0) {
        feeAmount_->setObjectName("FeeAmount");
        feeAmount_->setText("₱" + QString::number(fee, 'f', 2));
    }
    else {
        feeAmount_->setObjectName("FeeAmountNone");
        feeAmount_->setText("₱0.00 — no fee!");
    }
    feeAmount_->style()->unpolish(feeAmount_);
    feeAmount_->style()->polish(feeAmount_);

    receiptPanel_->setVisible(true);
    confirmBtn_->setVisible(true);
}

void ReturnDialog::onConfirm()
{
    if (borrowId_ < 0 || bookId_ < 0) return;

    try {
        QString     returnDateStr = returnDateEdit_->date().toString("yyyy-MM-dd");
        std::string retStd = returnDateStr.toStdString();

        // Calculate fee to decide if payment confirmation is needed
        int    days = FeeCalculator::overdueDays(dueDate_, retStd);
        double fee = FeeCalculator::overdueFee(days);

        // If there is a fee, ask for explicit payment confirmation
        bool confirmed = true;
        if (fee > 0.0) {
            auto reply = QMessageBox::question(
                this, "Confirm payment",
                QString("A fee of ₱%1 is due.\nConfirm payment received?")
                .arg(fee, 0, 'f', 2),
                QMessageBox::Yes | QMessageBox::No);
            confirmed = (reply == QMessageBox::Yes);
        }

        BorrowManager mgr;
        // returnBookGui(int bookId, const std::string& username,
        //               const std::string& returnDate, bool confirmPayment)
        bool ok = mgr.returnBookGui(bookId_, username_, retStd, confirmed);

        if (!ok) {
            QMessageBox::warning(this, "Cancelled",
                "Return was not processed. Payment must be confirmed to proceed.");
            return;
        }

        QMessageBox::information(this, "Success", "Book returned successfully. Thank you!");
        accept();
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    }
}