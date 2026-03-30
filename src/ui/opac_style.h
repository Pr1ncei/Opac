#pragma once
#pragma once
#include <QString>

namespace OpacStyle {

    inline QString sheet() {
        return R"(

/* ── Base ── */
QWidget {
    background-color: #f5f4f0;
    color: #1a1a1a;
    font-family: "Segoe UI", sans-serif;
    font-size: 13px;
}

/* ── Sidebar ── */
#Sidebar {
    background-color: #ffffff;
    border-right: 1px solid #e5e3db;
}

#SidebarLogo {
    font-size: 13px;
    font-weight: 600;
    color: #1a1a1a;
}

#SidebarSub {
    font-size: 11px;
    color: #888780;
}

#NavSection {
    font-size: 10px;
    font-weight: 600;
    color: #aaa89f;
    letter-spacing: 1px;
    padding: 0px 16px;
    margin-top: 8px;
    background: transparent;
    border: none;
}

QPushButton#NavBtn {
    text-align: left;
    padding: 7px 16px;
    border: none;
    border-radius: 0px;
    background: transparent;
    color: #5f5e5a;
    font-size: 13px;
}

QPushButton#NavBtn:hover {
    background-color: #f5f4f0;
    color: #1a1a1a;
}

QPushButton#NavBtn[active="true"] {
    background-color: #eeedfe;
    color: #3c3489;
    font-weight: 600;
}

#SidebarFooter {
    border-top: 1px solid #e5e3db;
    background: #ffffff;
    padding: 10px 16px;
}

#UserName {
    font-size: 13px;
    font-weight: 600;
    color: #1a1a1a;
}

#UserRole {
    font-size: 11px;
    color: #888780;
}

/* ── Main content area ── */
#MainArea {
    background-color: #f5f4f0;
    padding: 24px;
}

#PageTitle {
    font-size: 16px;
    font-weight: 600;
    color: #1a1a1a;
}

/* ── Stat cards ── */
#StatCard {
    background-color: #eeecea;
    border-radius: 8px;
    padding: 12px 16px;
}

#StatLabel {
    font-size: 11px;
    color: #888780;
}

#StatValue {
    font-size: 22px;
    font-weight: 600;
    color: #1a1a1a;
}

#StatValue[warn="true"]   { color: #854f0b; }
#StatValue[danger="true"] { color: #a32d2d; }

/* ── Cards / panels ── */
#Card {
    background-color: #ffffff;
    border: 1px solid #e5e3db;
    border-radius: 12px;
}

#CardHeader {
    font-size: 13px;
    font-weight: 600;
    color: #1a1a1a;
    padding: 10px 16px;
    border-bottom: 1px solid #e5e3db;
    background: #ffffff;
    border-top-left-radius: 12px;
    border-top-right-radius: 12px;
}

/* ── Tables ── */
QTableWidget {
    background-color: #ffffff;
    border: none;
    gridline-color: #f0eee8;
    selection-background-color: #eeedfe;
    selection-color: #1a1a1a;
    outline: none;
}

QTableWidget::item {
    padding: 8px 10px;
    border-bottom: 1px solid #f0eee8;
    color: #1a1a1a;
}

QTableWidget::item:selected {
    background-color: #eeedfe;
    color: #1a1a1a;
}

QHeaderView::section {
    background-color: #ffffff;
    color: #888780;
    font-size: 11px;
    font-weight: 600;
    text-transform: uppercase;
    padding: 8px 10px;
    border: none;
    border-bottom: 1px solid #e5e3db;
}

/* ── Inputs ── */
QLineEdit, QDateEdit {
    height: 34px;
    padding: 0 10px;
    border: 1px solid #d3d1c7;
    border-radius: 8px;
    background-color: #ffffff;
    color: #1a1a1a;
    font-size: 13px;
}

QLineEdit:focus, QDateEdit:focus {
    border-color: #534ab7;
}

QLineEdit[placeholderText] {
    color: #aaa89f;
}

/* ── Buttons ── */
QPushButton#BtnPrimary {
    height: 36px;
    padding: 0 16px;
    background-color: #534ab7;
    color: #ffffff;
    border: none;
    border-radius: 8px;
    font-size: 13px;
    font-weight: 600;
}

QPushButton#BtnPrimary:hover    { background-color: #3c3489; }
QPushButton#BtnPrimary:pressed  { background-color: #26215c; }

QPushButton#BtnSecondary {
    height: 36px;
    padding: 0 16px;
    background-color: transparent;
    color: #1a1a1a;
    border: 1px solid #d3d1c7;
    border-radius: 8px;
    font-size: 13px;
}

QPushButton#BtnSecondary:hover  { background-color: #f5f4f0; }
QPushButton#BtnSecondary:pressed{ background-color: #eeecea; }

QPushButton#BtnDanger {
    height: 36px;
    padding: 0 16px;
    background-color: transparent;
    color: #a32d2d;
    border: 1px solid #f09595;
    border-radius: 8px;
    font-size: 13px;
}

QPushButton#BtnDanger:hover  { background-color: #fcebeb; }

QPushButton#BtnSuccess {
    height: 36px;
    padding: 0 16px;
    background-color: #1d9e75;
    color: #ffffff;
    border: none;
    border-radius: 8px;
    font-size: 13px;
    font-weight: 600;
}

QPushButton#BtnSuccess:hover   { background-color: #0f6e56; }
QPushButton#BtnSuccess:pressed { background-color: #085041; }

QPushButton#BtnLink {
    background: transparent;
    border: none;
    color: #534ab7;
    font-size: 12px;
    padding: 2px 6px;
    text-align: left;
}

QPushButton#BtnLink:hover { color: #3c3489; }

/* ── Login card ── */
#LoginCard {
    background-color: #ffffff;
    border: 1px solid #e5e3db;
    border-radius: 12px;
    padding: 32px 36px;
    min-width: 360px;
    max-width: 400px;
}

#LoginTitle {
    font-size: 18px;
    font-weight: 600;
    color: #1a1a1a;
}

#LoginSub {
    font-size: 13px;
    color: #888780;
}

/* ── Badges ── */
#BadgeAvail {
    background-color: #eaf3de;
    color: #27500a;
    border-radius: 999px;
    padding: 2px 10px;
    font-size: 11px;
    font-weight: 600;
}

#BadgeOut {
    background-color: #fcebeb;
    color: #a32d2d;
    border-radius: 999px;
    padding: 2px 10px;
    font-size: 11px;
    font-weight: 600;
}

#BadgeOverdue {
    background-color: #faeeda;
    color: #633806;
    border-radius: 999px;
    padding: 2px 10px;
    font-size: 11px;
    font-weight: 600;
}

/* ── Receipt / info panel ── */
#ReceiptPanel {
    background-color: #f5f4f0;
    border-radius: 8px;
    padding: 12px 14px;
}

#ReceiptTitle {
    font-size: 10px;
    font-weight: 600;
    color: #888780;
    letter-spacing: 1px;
}

#ReceiptKey   { font-size: 13px; color: #5f5e5a; }
#ReceiptValue { font-size: 13px; font-weight: 600; color: #1a1a1a; }

#FeeAmount       { font-size: 18px; font-weight: 600; color: #a32d2d; }
#FeeAmountNone   { font-size: 18px; font-weight: 600; color: #27500a; }

/* ── Terms panel (borrow) ── */
#TermsPanel {
    background-color: #eeedfe;
    border-radius: 8px;
    padding: 10px 14px;
}

#TermKey { font-size: 12px; color: #534ab7; }
#TermVal { font-size: 12px; font-weight: 600; color: #3c3489; }

/* ── Dividers ── */
#HDivider {
    background-color: #e5e3db;
    max-height: 1px;
    min-height: 1px;
}

/* ── Search bar ── */
#SearchInput {
    height: 32px;
    padding: 0 10px;
    border: 1px solid #d3d1c7;
    border-radius: 8px;
    background: #ffffff;
    font-size: 13px;
}

#SearchInput:focus { border-color: #534ab7; }

/* ── Avatar circle ── */
#AvatarAdmin {
    background-color: #eeedfe;
    color: #3c3489;
    border-radius: 14px;
    font-size: 11px;
    font-weight: 600;
    min-width: 28px;
    max-width: 28px;
    min-height: 28px;
    max-height: 28px;
}

#AvatarUser {
    background-color: #e1f5ee;
    color: #085041;
    border-radius: 14px;
    font-size: 11px;
    font-weight: 600;
    min-width: 28px;
    max-width: 28px;
    min-height: 28px;
    max-height: 28px;
}

/* ── Scrollbars ── */
QScrollBar:vertical {
    width: 6px;
    background: transparent;
}
QScrollBar::handle:vertical {
    background: #d3d1c7;
    border-radius: 3px;
    min-height: 30px;
}
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical { height: 0; }

QScrollBar:horizontal {
    height: 6px;
    background: transparent;
}
QScrollBar::handle:horizontal {
    background: #d3d1c7;
    border-radius: 3px;
    min-width: 30px;
}
QScrollBar::add-line:horizontal,
QScrollBar::sub-line:horizontal { width: 0; }

)";
    }

} // namespace OpacStyle