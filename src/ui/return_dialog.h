#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QLabel>
#include <QPushButton>
#include <string>

class ReturnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReturnDialog(const std::string& username, QWidget* parent = nullptr);

private slots:
    void onLookup();
    void onDateChanged();
    void onConfirm();

private:
    void buildUI();

    std::string username_;
    int         borrowId_ = -1;
    int         bookId_ = -1;
    std::string dueDate_;

    QLineEdit* bookIdEdit_;
    QLabel* lookupError_;
    QWidget* previewPanel_;
    QLabel* previewTitle_;
    QLabel* previewDue_;
    QLabel* previewBadge_;
    QDateEdit* returnDateEdit_;
    QWidget* receiptPanel_;
    QLabel* receiptDue_;
    QLabel* receiptReturn_;
    QLabel* receiptDays_;
    QLabel* receiptFee_;
    QLabel* feeAmount_;
    QPushButton* confirmBtn_;
    QWidget* dateSection_;
};