DROP DATABASE IF EXISTS opac_db;
CREATE DATABASE opac_db;
USE opac_db;

-- =============================================================
-- USERS
-- =============================================================
CREATE TABLE users (
    username VARCHAR(50)  NOT NULL PRIMARY KEY,
    password VARCHAR(255) NOT NULL,
    role     VARCHAR(10)  NOT NULL DEFAULT 'user'
);

-- Default admin
INSERT INTO users (username, password, role)
VALUES ('admin', 'admin123', 'admin');

-- =============================================================
-- BOOKS
-- =============================================================
CREATE TABLE books (
    book_id          INT          NOT NULL AUTO_INCREMENT PRIMARY KEY,
    title            VARCHAR(150) NOT NULL,
    author           VARCHAR(100) NOT NULL,
    dewey            VARCHAR(20)  NOT NULL,
    total_copies     INT          NOT NULL DEFAULT 1,
    available_copies INT          NOT NULL DEFAULT 1,

    CONSTRAINT chk_copies CHECK (available_copies >= 0
                              AND available_copies <= total_copies)
);

-- Sample data
INSERT INTO books (title, author, dewey, total_copies, available_copies) VALUES
('1984', 'George Orwell', '823.912', 2, 2),
('Artificial Intelligence: A Modern Approach', 'Stuart Russell', '006.3', 3, 3),
('Clean Code', 'Robert C. Martin', '005.1', 5, 5),
('Computer Networks', 'Andrew S. Tanenbaum', '004.6', 2, 2),
('Database System Concepts', 'Abraham Silberschatz', '005.74', 4, 4),
('Deep Learning', 'Ian Goodfellow', '006.31', 2, 2),
('Design Patterns', 'Erich Gamma', '005.72', 3, 3),
('Dune', 'Frank Herbert', '813.54', 1, 1),
('Harry Potter and the Sorcerer''s Stone', 'J.K. Rowling', '823.914', 6, 6),
('Introduction to Algorithms', 'Thomas H. Cormen', '005.1', 4, 4),
('Operating System Concepts', 'Abraham Silberschatz', '005.43', 3, 3),
('Rich Dad Poor Dad', 'Robert Kiyosaki', '650.1', 5, 5),
('The Alchemist', 'Paulo Coelho', '869.3', 3, 3),
('The Catcher in the Rye', 'J.D. Salinger', '813.54', 2, 2),
('The Great Gatsby', 'F. Scott Fitzgerald', '813.52', 3, 3),
('The Hobbit', 'J.R.R. Tolkien', '823.912', 4, 4),
('The Lord of the Rings', 'J.R.R. Tolkien', '823.912', 3, 3),
('The Pragmatic Programmer', 'Andrew Hunt', '005.1', 2, 2),
('Think and Grow Rich', 'Napoleon Hill', '650.1', 4, 4),
('To Kill a Mockingbird', 'Harper Lee', '813.54', 2, 2);

-- =============================================================
-- BORROW RECORDS
-- =============================================================
CREATE TABLE borrow_records (
    borrow_id    INT          NOT NULL AUTO_INCREMENT PRIMARY KEY,
    book_id      INT          NOT NULL,
    username     VARCHAR(50)  NOT NULL,
    borrow_date  DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    due_date     DATETIME     NOT NULL,
    return_date  DATETIME              DEFAULT NULL,   -- NULL = still borrowed
    overdue_days INT                   DEFAULT 0,
    fee          DECIMAL(10,2)         DEFAULT 0.00,

    FOREIGN KEY (book_id)  REFERENCES books(book_id) ON DELETE CASCADE,
    FOREIGN KEY (username) REFERENCES users(username) ON DELETE CASCADE
);

-- =============================================================
-- VIEWS
-- =============================================================
CREATE VIEW v_currently_borrowed AS
SELECT
    br.borrow_id,
    b.book_id,
    b.title,
    b.author,
    b.dewey,
    br.username,
    br.borrow_date,
    br.due_date,
    DATEDIFF(CURDATE(), br.due_date) AS days_overdue,
    GREATEST(0, DATEDIFF(CURDATE(), br.due_date)) * 20.00 AS projected_fee
FROM borrow_records br
JOIN books b ON br.book_id = b.book_id
WHERE br.return_date IS NULL;

-- Book inventory summary
CREATE VIEW v_book_inventory AS
SELECT
    book_id,
    title,
    author,
    dewey,
    total_copies,
    available_copies,
    (total_copies - available_copies) AS borrowed_copies
FROM books;