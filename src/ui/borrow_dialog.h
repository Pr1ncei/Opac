#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <string>

class BorrowDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BorrowDialog(const std::string& username, QWidget* parent = nullptr);

private slots:
    void onLookup();
    void onConfirm();

private:
    void buildUI();
    void updateBookPreview(int bookId);
    void setPreviewVisible(bool visible);

    std::string username_;
    int         foundBookId_ = -1;

    QLineEdit* bookIdEdit_;
    QLabel* previewId_;
    QLabel* previewTitle_;
    QLabel* previewMeta_;
    QLabel* previewCopies_;
    QLabel* previewBadge_;
    QWidget* previewPanel_;
    QWidget* termsPanel_;
    QLabel* termDue_;
    QLabel* lookupError_;
    QPushButton* confirmBtn_;
};