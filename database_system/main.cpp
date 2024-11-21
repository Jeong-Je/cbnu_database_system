#include <stdlib.h>
#include <iostream>
#include <string>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

using namespace std;

// 비밀번호를 코드에 저장X
const string server = ""; // MySQL 서버 주소
const string username = "";               // 사용자 이름
const string password = "";                   // 사용자 비밀번호

// 메뉴 출력 함수
void showMenu() {
    cout << "1. View Book List" << endl;
    cout << "2. Add New Book" << endl;
    cout << "3. Search Book" << endl;
    cout << "4. Exit" << endl;
    cout << "Choose an option: ";
}

// 도서 목록을 출력하는 함수
void showBooks(sql::Connection* con) {
    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    pstmt = con->prepareStatement("SELECT * FROM Book");
    res = pstmt->executeQuery();

    cout << "\nBook List:" << endl;
    while (res->next()) {
        // 각 도서의 정보를 출력
        cout << "bookid: " << res->getInt(1)
            << ", bookname: " << res->getString(2)
            << ", publisher: " << res->getString(3)
            << ", price: " << res->getInt(4) << endl;
    }

    delete res;
    delete pstmt;
}

// 새 도서를 추가하는 함수
void addBook(sql::Connection* con) {
    string bookname, publisher;
    int bookid, price;

    cout << "Enter the details for the new book." << endl;
    cout << "Book ID: ";
    cin.ignore();
    cin >> bookid;
    cout << "Book Name: ";
    cin.ignore(); // 입력 버퍼 비우기
    getline(cin, bookname);
    cout << "Publisher: ";
    getline(cin, publisher);
    cout << "Price: ";
    cin >> price;

    // PreparedStatement를 사용하여 새 도서를 데이터베이스에 추가
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

// 도서명을 검색하는 함수
void searchBook(sql::Connection* con) {
    string searchTerm;
    cout << "Enter the book name to search: ";
    cin.ignore(); // 입력 버퍼 비우기
    getline(cin, searchTerm);

    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    // 입력한 도서명으로 검색
    pstmt = con->prepareStatement("SELECT * FROM Book WHERE bookname LIKE ?");
    pstmt->setString(1, "%" + searchTerm + "%");

    res = pstmt->executeQuery();

    cout << "\nSearch Results:" << endl;
    bool found = false;
    while (res->next()) {
        // 검색된 도서 목록 출력
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
    // 콘솔의 문자셋을 UTF-8로 변경
    system("chcp 65001");

    sql::Driver* driver;
    sql::Connection* con;

    try {
        // 드라이버 인스턴스 가져오기
        driver = get_driver_instance();

        // 서버에 연결
        con = driver->connect(server, username, password);

        // 데이터베이스 선택
        con->setSchema("madang");

        int choice;
        do {
            showMenu(); // 메뉴 출력
            cin >> choice;

            switch (choice) {
            case 1:
                showBooks(con);  // 도서 목록 보기
                break;
            case 2:
                addBook(con);    // 새 도서 추가
                break;
            case 3:
                searchBook(con); // 도서 검색
                break;
            case 4:
                cout << "Exiting the program." << endl; // 프로그램 종료 메시지
                break;
            default:
                cout << "Invalid input. Please try again." << endl; // 잘못된 입력 처리
            }

        } while (choice != 4); // 종료 전까지 반복

        delete con; // 연결 종료
    }
    catch (sql::SQLException& e) {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }

    system("pause");
    return 0;
}
