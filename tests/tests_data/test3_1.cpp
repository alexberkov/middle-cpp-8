#include <vector>
#include <list>
#include <string>
#include <map>
#include <iostream>

struct Point {
    double x;
    double y;
};

struct Employee {
    int id;
    std::string department;
};

enum class Status {
    Active,
    Inactive
};

void iterateList() {
    std::list<std::string> names = {"Alice", "Bob"};

    for (const auto name : names) {
        std::cout << name;
    }
}

void iterateMap() {
    std::map<int, Employee> staff = {{1, {100, "IT"}}, {2, {200, "HR"}}};

    for (const auto entry : staff) {
        std::cout << entry.first;
    }
}

void iteratePoints() {
    std::vector<Point> points = {{1.0, 2.0}, {3.0, 4.0}};

    for (const Point p : points) {
        std::cout << p.x;
    }
}

void iterateEnum() {
    std::vector<Status> statuses = {Status::Active, Status::Inactive};

    for (const Status s : statuses) {
        std::cout << static_cast<int>(s);
    }
}

void iterateFundamental() {
    std::vector<double> values = {1.0, 2.0};
    for (const double v : values) {
        std::cout << v;
    }

    std::vector<char> chars = {'a', 'b'};
    for (const char c : chars) {
        std::cout << c;
    }
}

void iterateWithRef() {
    std::vector<Employee> team = {{1, "HR"}, {2, "IT"}};

    for (const auto& member : team) {
        std::cout << member.id;
    }

    for (const Employee& emp : team) {
        std::cout << emp.department;
    }
}
