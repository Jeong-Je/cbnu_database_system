#include <stdlib.h>
#include <iostream>
#include <string>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

using namespace std;

// �׽�Ʈ�����θ� ���: ��й�ȣ�� �ڵ忡 �������� ������!
const string server = "tcp://192.168.56.101:4567"; // MySQL ���� �ּ�
const string username = "jeongje";               // ����� �̸�
const string password = "1234";                   // ����� ��й�ȣ

// �޴� ��� �Լ�
void showMenu() {
    cout << "1. View Book List" << endl;
    cout << "2. Add New Book" << endl;
    cout << "3. Search Book" << endl;
    cout << "4. Exit" << endl;
    cout << "Choose an option: ";
}

// ���� ����� ����ϴ� �Լ�
void showBooks(sql::Connection* con) {
    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    pstmt = con->prepareStatement("SELECT * FROM Book");
    res = pstmt->executeQuery();

    cout << "\nBook List:" << endl;
    while (res->next()) {
        // �� ������ ������ ���
        cout << "bookid: " << res->getInt(1)
            << ", bookname: " << res->getString(2)
            << ", publisher: " << res->getString(3)
            << ", price: " << res->getInt(4) << endl;
    }

    delete res;
    delete pstmt;
}

// �� ������ �߰��ϴ� �Լ�
void addBook(sql::Connection* con) {
    string bookname, publisher;
    int bookid, price;

    cout << "Enter the details for the new book." << endl;
    cout << "Book ID: ";
    cin.ignore();
    cin >> bookid;
    cout << "Book Name: ";
    cin.ignore(); // �Է� ���� ����
    getline(cin, bookname);
    cout << "Publisher: ";
    getline(cin, publisher);
    cout << "Price: ";
    cin >> price;

    // PreparedStatement�� ����Ͽ� �� ������ �����ͺ��̽��� �߰�
    sql::PreparedStatement* pstmt = con->prepareStatement(
        "INSERT INTO Book (bookid, bookname, publisher, price) VALUES (?, ?, ?, ?)"
    );
    pstmt->setInt(1, bookid);
    pstmt->setString(2, bookname);
    pstmt->setString(3, publisher);
    pstmt->setInt(4, price);

    pstmt->executeUpdate();

    cout << "New book has been added!" << endl;

    delete pstmt;
}

// �������� �˻��ϴ� �Լ�
void searchBook(sql::Connection* con) {
    string searchTerm;
    cout << "Enter the book name to search: ";
    cin.ignore(); // �Է� ���� ����
    getline(cin, searchTerm);

    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    // �Է��� ���������� �˻�
    pstmt = con->prepareStatement("SELECT * FROM Book WHERE bookname LIKE ?");
    pstmt->setString(1, "%" + searchTerm + "%");

    res = pstmt->executeQuery();

    cout << "\nSearch Results:" << endl;
    bool found = false;
    while (res->next()) {
        // �˻��� ���� ��� ���
        cout << "bookid: " << res->getInt(1)
            << ", bookname: " << res->getString(2)
            << ", publisher: " << res->getString(3)
            << ", price: " << res->getInt(4) << endl;
        found = true;
    }

    if (!found) {
        cout << "No books found." << endl;
    }

    delete res;
    delete pstmt;
}

int main() {
    // �ܼ��� ���ڼ��� UTF-8�� ����
    system("chcp 65001");

    sql::Driver* driver;
    sql::Connection* con;

    try {
        // ����̹� �ν��Ͻ� ��������
        driver = get_driver_instance();

        // ������ ����
        con = driver->connect(server, username, password);

        // �����ͺ��̽� ����
        con->setSchema("madang");

        int choice;
        do {
            showMenu(); // �޴� ���
            cin >> choice;

            switch (choice) {
            case 1:
                showBooks(con);  // ���� ��� ����
                break;
            case 2:
                addBook(con);    // �� ���� �߰�
                break;
            case 3:
                searchBook(con); // ���� �˻�
                break;
            case 4:
                cout << "Exiting the program." << endl; // ���α׷� ���� �޽���
                break;
            default:
                cout << "Invalid input. Please try again." << endl; // �߸��� �Է� ó��
            }

        } while (choice != 4); // ���� ������ �ݺ�

        delete con; // ���� ����
    }
    catch (sql::SQLException& e) {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }

    system("pause");
    return 0;
}
