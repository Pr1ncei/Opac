DROP DATABASE IF EXISTS opac_db;

-- Create database
CREATE DATABASE opac_db;
USE opac_db;

-- =========================
-- USERS TABLE
-- =========================
CREATE TABLE users (
    username VARCHAR(50) NOT NULL PRIMARY KEY,
    password VARCHAR(255) NOT NULL,
    role VARCHAR(10) NOT NULL
);

-- Default admin account
INSERT INTO users (username, password, role)
VALUES ('admin', 'admin', 'admin');

-- =========================
-- BOOKS TABLE
-- =========================
CREATE TABLE books (
    book_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    title VARCHAR(100) NOT NULL,
    author VARCHAR(100) NOT NULL,
    dewey VARCHAR(20) NOT NULL,
    available BOOLEAN NOT NULL DEFAULT 1,
    borrowed_by VARCHAR(50),
    borrow_date DATETIME,
    due_date DATETIME,
    return_date DATETIME
);

-- Optional: sample data
INSERT INTO books (title, author, dewey, available)
VALUES 
('The Great Gatsby', 'F. Scott Fitzgerald', '813.52', 1),
('1984', 'George Orwell', '823.912', 1),
('To Kill a Mockingbird', 'Harper Lee', '813.54', 1);