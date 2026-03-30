#include "borrow_dialog.h"
#include "../managers/books/BookManager.h"
#include "../managers/borrow/BorrowManager.h"
#include "../managers/fee/FeeCalculator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStyle>
#include <QMessageBox>
#include <QDate>

BorrowDialog::BorrowDialog(const std::string& username, QWidget* parent)
    : QDialog(parent), username_(username)
{
    setWindowTitle("Borrow a book");
    setFixedWidth(420);
    setModal(true);
    buildUI();
}

void BorrowDialog::buildUI()
{
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(24, 24, 24, 24);
    lay->setSpacing(12);

    // Title
    auto* title = new QLabel("Borrow a book");
    title->setObjectName("PageTitle");
    lay->addWidget(title);

    // Book ID row
    auto* idRow = new QHBoxLayout;
    bookIdEdit_ = new QLineEdit;
    bookIdEdit_->setPlaceholderText("Enter book ID…");
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
    pLay->setSpacing(2);
    previewId_ = new QLabel; previewId_->setObjectName("ReceiptTitle");
    previewTitle_ = new QLabel; previewTitle_->setObjectName("CardHeader");
    previewTitle_->setStyleSheet(
        "font-size:13px; font-weight:600; border:none; padding:0; border-radius:0;");
    previewMeta_ = new QLabel; previewMeta_->setObjectName("ReceiptKey");
    auto* copiesRow = new QHBoxLayout;
    previewCopies_ = new QLabel; previewCopies_->setObjectName("ReceiptKey");
    previewBadge_ = new QLabel; previewBadge_->setFixedHeight(20);
    copiesRow->addWidget(previewCopies_);
    copiesRow->addStretch();
    copiesRow->addWidget(previewBadge_);
    pLay->addWidget(previewId_);
    pLay->addWidget(previewTitle_);
    pLay->addWidget(previewMeta_);
    pLay->addLayout(copiesRow);
    previewPanel_->setVisible(false);
    lay->addWidget(previewPanel_);

    // Terms panel
    termsPanel_ = new QWidget;
    termsPanel_->setObjectName("TermsPanel");
    auto* tLay = new QGridLayout(termsPanel_);
    tLay->setSpacing(4);

    auto addTerm = [&](int row, const QString& key, QLabel*& valLabel,
        const QString& val = "") {
            auto* k = new QLabel(key); k->setObjectName("TermKey");
            valLabel = new QLabel(val); valLabel->setObjectName("TermVal");
            tLay->addWidget(k, row, 0);
            tLay->addWidget(valLabel, row, 1, Qt::AlignRight);
        };

    QLabel* periodVal = nullptr;
    QLabel* feeVal = nullptr;
    addTerm(0, "Borrow period", periodVal,
        QString::number(FeeCalculator::BORROW_PERIOD_DAYS) + " days");
    addTerm(1, "Due date", termDue_, "—");
    addTerm(2, "Overdue fee", feeVal,
        "₱" + QString::number(FeeCalculator::FEE_PER_DAY, 'f', 2) + " / day");

    termsPanel_->setVisible(false);
    lay->addWidget(termsPanel_);

    // Buttons
    confirmBtn_ = new QPushButton("Confirm borrow");
    confirmBtn_->setObjectName("BtnPrimary");
    confirmBtn_->setCursor(Qt::PointingHandCursor);
    confirmBtn_->setVisible(false);
    lay->addWidget(confirmBtn_);

    auto* cancelBtn = new QPushButton("Cancel");
    cancelBtn->setObjectName("BtnSecondary");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    lay->addWidget(cancelBtn);

    connect(lookupBtn, &QPushButton::clicked, this, &BorrowDialog::onLookup);
    connect(bookIdEdit_, &QLineEdit::returnPressed, this, &BorrowDialog::onLookup);
    connect(confirmBtn_, &QPushButton::clicked, this, &BorrowDialog::onConfirm);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void BorrowDialog::onLookup()
{
    lookupError_->setVisible(false);
    previewPanel_->setVisible(false);
    termsPanel_->setVisible(false);
    confirmBtn_->setVisible(false);
    foundBookId_ = -1;

    bool ok;
    int id = bookIdEdit_->text().trimmed().toInt(&ok);
    if (!ok || id <= 0) {
        lookupError_->setText("Please enter a valid book ID.");
        lookupError_->setVisible(true);
        return;
    }

    try {
        BookManager mgr;
        Book book = mgr.getBookById(id);   // throws if not found
        foundBookId_ = id;
        updateBookPreview(id);

        QDate due = QDate::currentDate().addDays(FeeCalculator::BORROW_PERIOD_DAYS);
        termDue_->setText(due.toString("yyyy-MM-dd"));

        previewPanel_->setVisible(true);
        termsPanel_->setVisible(true);
        confirmBtn_->setVisible(book.isAvailable());

        if (!book.isAvailable()) {
            lookupError_->setText("All copies are currently borrowed.");
            lookupError_->setVisible(true);
        }
    }
    catch (std::exception& e) {
        lookupError_->setText(QString::fromStdString(e.what()));
        lookupError_->setVisible(true);
    }
}

void BorrowDialog::updateBookPreview(int bookId)
{
    try {
        BookManager mgr;
        Book book = mgr.getBookById(bookId);
        previewId_->setText("Book #" + QString::number(book.getBookId()));
        previewTitle_->setText(QString::fromStdString(book.getTitle()));
        previewMeta_->setText(
            QString::fromStdString(book.getAuthor()) + " · " +
            QString::fromStdString(book.getDewey()));
        previewCopies_->setText(
            QString::number(book.getAvailableCopies()) + " of " +
            QString::number(book.getTotalCopies()) + " copies available");

        if (book.isAvailable()) {
            previewBadge_->setObjectName("BadgeAvail");
            previewBadge_->setText("Available");
        }
        else {
            previewBadge_->setObjectName("BadgeOut");
            previewBadge_->setText("Out");
        }
        previewBadge_->style()->unpolish(previewBadge_);
        previewBadge_->style()->polish(previewBadge_);
    }
    catch (...) {}
}

void BorrowDialog::onConfirm()
{
    if (foundBookId_ < 0) return;
    try {
        BorrowManager mgr;
        bool ok = mgr.borrowBookGui(foundBookId_, username_);
        if (!ok) {
            QMessageBox::warning(this, "Unavailable",
                "Sorry, no copies are available right now.");
            return;
        }
        QMessageBox::information(this, "Success",
            "Book borrowed successfully!\nReturn within " +
            QString::number(FeeCalculator::BORROW_PERIOD_DAYS) +
            " days to avoid fees.");
        accept();
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    }
}