void OpacSystem::deleteBook() {
    int book_id;
    cout << "Enter Book ID to delete: ";
    cin >> book_id;

    sql::Connection* con = db.getConnection();
    if (!con) return;

    sql::Statement* stmt = con->createStatement();
    stmt->execute("DELETE FROM books WHERE book_id=" + to_string(book_id));

    cout << "Book Deleted!\n";
    delete stmt;
}