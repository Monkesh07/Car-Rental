// Include necessary header
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <string>
#include <thread>

using namespace std;
using namespace sql;

// Sleep function to make time delay
void sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// Function to check if a string is a valid integer
bool validInt(string s) {
    for (int i = 0; i < s.length(); i++) {
        if (s[i] < '0' || s[i] > '9') {
            return false;
        }
    }
    return true;
}

// Function to connect the database to our program
class DatabaseConnector {
private:
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;
    bool stat = 1;

public:
    // Contructor
    DatabaseConnector(const string& host, const string& user, const string& password, const string& database) {
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect(host, user, password);
            Statement* stmt = con->createStatement();
            sql::ResultSet* s = stmt->executeQuery("SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = '" + database + "'");
            if (s->next()) stat = 0;
            stmt->execute("CREATE DATABASE IF NOT EXISTS " + database);
            delete stmt;
            con->setSchema(database);
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    // Deconstructor
    ~DatabaseConnector() {
        delete con;
    }

    // Function to return Connector
    sql::Connection* getConnection() const {
        return con;
    }

    // Return if table was already present or not
    bool getStat() {
        return stat;
    }
};

// Initialize values in the database
class DatabaseInitiate {
private :
    bool ifcreate;
public :
    DatabaseInitiate(const bool& ifcreate) : ifcreate(ifcreate) {}

    // Create necessary database tables
    void InitiateTables(Connection* con){
        try {
            Statement* stmt = con->createStatement();
            string insertQueryCars
                = "CREATE TABLE IF NOT EXISTS cars ("
                "CarID INT NOT NULL PRIMARY KEY,"
                "Model VARCHAR(255) NOT NULL,"
                "Colour VARCHAR(255) NOT NULL,"
                "Car_Condition VARCHAR(255) NOT NULL,"
                "Availability VARCHAR(255) NOT NULL,"
                "Rented_By VARCHAR(255) DEFAULT NULL,"
                "Rent_Rate INT NOT NULL"
                ")";
            stmt->execute(insertQueryCars);

            string insertQueryCustomers
                = "CREATE TABLE IF NOT EXISTS customers ("
                "CustomerID VARCHAR(255) NOT NULL PRIMARY KEY,"
                "Password VARCHAR(255) NOT NULL,"
                "Customer_Name VARCHAR(255) NOT NULL,"
                "Customer_Record INT NOT NULL DEFAULT 0,"
                "Fine_Due INT NOT NULL DEFAULT 0"
                ")";
            stmt->execute(insertQueryCustomers);

            string insertQueryEmployees
                = "CREATE TABLE IF NOT EXISTS employees ("
                "EmployeeID VARCHAR(255) NOT NULL PRIMARY KEY,"
                "Password VARCHAR(255) NOT NULL,"
                "Employee_Name VARCHAR(255) NOT NULL,"
                "Employee_Record INT NOT NULL DEFAULT 0,"
                "Fine_Due INT NOT NULL DEFAULT 0"
                ")";
            stmt->execute(insertQueryEmployees);

            string insertQueryManager
                = "CREATE TABLE IF NOT EXISTS managers ("
                "ManagerID VARCHAR(255) NOT NULL PRIMARY KEY,"
                "Password VARCHAR(255) NOT NULL,"
                "Manager_Name VARCHAR(255) NOT NULL"
                ")";
            stmt->execute(insertQueryManager);

            // Add data to the newly created tables
            if (ifcreate) {
                string insertCarValues
                    = "INSERT INTO cars (CarID, Model, Colour, Car_condition, Availability, Rent_Rate)"
                    "VALUES ('100', 'BMW Limousine', 'Red', 'Good', 'Yes', '600'),"
                    "('200', 'Hyundai Creta', 'Blue', 'Average', 'Yes', '250'),"
                    "('300', 'Maruti Suzuki', 'White', 'Good', 'Yes', '100'),"
                    "('400', 'Mercedez Benz', 'Black', 'Bad', 'No', '400'),"
                    "('500', 'Mahindra Bolera', 'White', 'Bad', 'Yes', '55')";
                stmt->execute(insertCarValues);

                string insertCustomerValues
                    = "INSERT INTO customers (CustomerID, Password, Customer_Name)"
                    "VALUES ('C100', 'Aditya1', 'Aditya'),"
                    "('C200', 'Shivam2', 'Shivam'),"
                    "('C300', 'Saksham3', 'Saksham'),"
                    "('C400', 'Astha4', 'Astha'),"
                    "('C500', 'Rahul5', 'Rahul')";
                stmt->execute(insertCustomerValues);

                string insertEmployeeValues
                    = "INSERT INTO employees (EmployeeID, Password, Employee_Name)"
                    "VALUES ('E100', 'Arvind1', 'Arvind'),"
                    "('E200', 'Raj2', 'Raj'),"
                    "('E300', 'Vivek3', 'Vivek'),"
                    "('E400', 'Suhail4', 'Suhail'),"
                    "('E500', 'Nishant5', 'Nishant')";
                stmt->execute(insertEmployeeValues);

                string insertManagerValues
                    = "INSERT INTO managers (ManagerID, Password, Manager_Name)"
                    "VALUES ('M100', 'Monkesh1', 'Monkesh')";
                stmt->execute(insertManagerValues);
            }
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }
};

// User class
class User {
protected:
    string UserID;
    string UserPassword;
    string UserName;
public:
    User(const string& UserID, const string& UserPassword, const string& UserName) : UserID(UserID), UserPassword(UserPassword), UserName(UserName) {}
};

// Customer Class
class Customer : public User {
private:
    int Customer_Record;
    int Fine_Due;

public:
    Customer(string CustomerID, string Password, string Customer_Name, int Customer_Record, int Fine_Due) : User(CustomerID, Password, Customer_Name), Customer_Record(Customer_Record), Fine_Due(Fine_Due) {}

    void showRentedCars(Connection* con) {
        try {
            Statement* stmt = con->createStatement();
            string availableCarsQuery
                = "SELECT * FROM cars WHERE Availability = 'Yes' AND Rented_By = '" + UserID + "'";
            sql::ResultSet* carsList = stmt->executeQuery(availableCarsQuery);
            bool flag = 0;
            while (carsList->next()) {
                if (!flag) {
                    cout << setw(10) << left << "Car ID" << "| ";
                    cout << setw(25) << left << "Model" << "| ";
                    cout << setw(12) << left << "Colour" << "| ";
                    cout << setw(15) << left << "Car Condition" << "| ";
                    cout << setw(8) << left << "Rent Rate";
                    cout << "\n-------------------------------------------------------------------------------\n";
                    flag = 1;
                }
                cout << setw(10) << left << carsList->getInt("CarID") << "| ";
                cout << setw(25) << left << carsList->getString("Model") << "| ";
                cout << setw(12) << left << carsList->getString("Colour") << "| ";
                cout << setw(15) << left << carsList->getString("Car_Condition") << "| ";
                cout << setw(8) << left << carsList->getInt("Rent_Rate") << "\n";
            }
            if (!flag) {
                cout << "No car is rented by you";
            }
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void showAvailableCars(Connection* con) {
        try {
            Statement* stmt = con->createStatement();
            string availableCarsQuery
                = "SELECT * FROM cars WHERE Availability = 'Yes'";
            sql::ResultSet* carsList = stmt->executeQuery(availableCarsQuery);
            bool flag = 0;
            while (carsList->next()) {
                if (!flag) {
                    cout << setw(10) << left << "Car ID" << "| ";
                    cout << setw(25) << left << "Model" << "| ";
                    cout << setw(12) << left << "Colour" << "| ";
                    cout << setw(15) << left << "Car Condition" << "| ";
                    cout << setw(8) << left << "Rent Rate";
                    cout << "\n-------------------------------------------------------------------------------\n";
                    flag = 1;
                }
                cout << setw(10) << left << carsList->getInt("CarID") << "| ";
                cout << setw(25) << left << carsList->getString("Model") << "| ";
                cout << setw(12) << left << carsList->getString("Colour") << "| ";
                cout << setw(15) << left << carsList->getString("Car_Condition") << "| ";
                cout << setw(8) << left << carsList->getInt("Rent_Rate") << "\n";
            }
            if (!flag) {
                cout << "No cars are available to rent";
            }
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    string rentCar(Connection* con, int CarID) {
        try {
            Statement* stmt = con->createStatement();
            sql::ResultSet* carsList = stmt->executeQuery("SELECT * FROM cars WHERE Availability = 'Yes' and CarID = " + to_string(CarID));
            string output;
            if (Customer_Record == 3) {
                // Customer cant rent cars
                output = "You cant rent more cars";
            }
            else if (carsList->next()) {
                stmt->execute("UPDATE cars SET Availability = 'No', Rented_by = '" + UserID + "' WHERE CarID = '" + to_string(CarID) + "'");
                stmt->execute("UPDATE customers SET Customer_Record = '" + to_string(Customer_Record+1) + "' WHERE CustomerID = '" + UserID + "'");
                Customer_Record += 1;
                // Car rented
                output = "Car renting confirmed";
            }
            else {
                // Tell no car with this id is available
                output = "Car with given ID not found";
            }
            delete stmt;
            return output;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    string returnCar(Connection* con, int CarID) {
        try {
            Statement* stmt = con->createStatement();
            sql::ResultSet* carsList = stmt->executeQuery("SELECT * FROM cars WHERE CarID = " + to_string(CarID));
            string output;
            if (carsList->next()) {
                string tempUserID = carsList->getString("Rented_By");
                if (UserID != tempUserID) {
                    // Car not rented by this user
                    return "You have not rented this car";
                }
                else {
                    stmt->execute("UPDATE cars SET Availability = 'Yes', Rented_by = NULL WHERE CarID = '" + to_string(CarID) + "'");
                    stmt->execute("UPDATE customers SET Customer_Record = '" + to_string(Customer_Record-1) + "' WHERE CustomerID = '" + UserID + "'");
                    Customer_Record -= 1;
                    return "Car returned successfully";
                }
            }
            else {
                return "No car ith given ID found";
            }
            delete stmt;
            return output;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }
};

// Employee Class
class Employee : public User {
private:
    int Employee_Record;
    int Fine_Due;

public:
    Employee(string EmployeeID, string Password, string Employee_Name, int Employee_Record, int Fine_Due) : User(EmployeeID, Password, Employee_Name), Employee_Record(Employee_Record), Fine_Due(Fine_Due) {}

    void showRentedCars(Connection* con) {
        try {
            Statement* stmt = con->createStatement();
            string availableCarsQuery
                = "SELECT * FROM cars WHERE Availability = 'Yes' AND Rented_By = '" + UserID + "'";
            sql::ResultSet* carsList = stmt->executeQuery(availableCarsQuery);
            bool flag = 0;
            while (carsList->next()) {
                if (!flag) {
                    cout << setw(10) << left << "Car ID" << "| ";
                    cout << setw(25) << left << "Model" << "| ";
                    cout << setw(12) << left << "Colour" << "| ";
                    cout << setw(15) << left << "Car Condition" << "| ";
                    cout << setw(8) << left << "Rent Rate";
                    cout << "\n-------------------------------------------------------------------------------\n";
                    flag = 1;
                }
                int cost = carsList->getInt("Rent_Rate");
                cost = round(cost * 0.85);
                cout << setw(10) << left << carsList->getInt("CarID") << "| ";
                cout << setw(25) << left << carsList->getString("Model") << "| ";
                cout << setw(12) << left << carsList->getString("Colour") << "| ";
                cout << setw(15) << left << carsList->getString("Car_Condition") << "| ";
                cout << setw(8) << left << cost << "\n";
            }
            if (!flag) {
                cout << "No car is rented by you";
            }
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void showAvailableCars(Connection* con) {
        try {
            Statement* stmt = con->createStatement();
            string availableCarsQuery
                = "SELECT * FROM cars WHERE Availability = 'Yes'";
            sql::ResultSet* carsList = stmt->executeQuery(availableCarsQuery);
            bool flag = 0;
            while (carsList->next()) {
                if (!flag) {
                    cout << setw(10) << left << "Car ID" << "| ";
                    cout << setw(25) << left << "Model" << "| ";
                    cout << setw(12) << left << "Colour" << "| ";
                    cout << setw(15) << left << "Car Condition" << "| ";
                    cout << setw(8) << left << "Rent Rate";
                    cout << "\n-------------------------------------------------------------------------------\n";
                    flag = 1;
                }
                int cost = carsList->getInt("Rent_Rate");
                cost = round(cost * 0.85);
                cout << setw(10) << left << carsList->getInt("CarID") << "| ";
                cout << setw(25) << left << carsList->getString("Model") << "| ";
                cout << setw(12) << left << carsList->getString("Colour") << "| ";
                cout << setw(15) << left << carsList->getString("Car_Condition") << "| ";
                cout << setw(8) << left << cost << "\n";
            }
            if (!flag) {
                cout << "No cars are available to rent";
            }
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    string rentCar(Connection* con, int CarID) {
        try {
            Statement* stmt = con->createStatement();
            sql::ResultSet* carsList = stmt->executeQuery("SELECT * FROM cars WHERE Availability = 'Yes' and CarID = " + to_string(CarID));
            string output;
            if (Employee_Record == 3) {
                // Customer cant rent cars
                output = "You cant rent more cars";
            }
            else if (carsList->next()) {
                stmt->execute("UPDATE cars SET Availability = 'No', Rented_by = '" + UserID + "' WHERE CarID = '" + to_string(CarID) + "'");
                stmt->execute("UPDATE employees SET Employee_Record = '" + to_string(Employee_Record + 1) + "' WHERE EmployeeID = '" + UserID + "'");
                Employee_Record += 1;
                // Car rented
                output = "Car renting confirmed";
            }
            else {
                // Tell no car with this id is available
                output = "Car with given ID not found";
            }
            delete stmt;
            return output;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    string returnCar(Connection* con, int CarID) {
        try {
            Statement* stmt = con->createStatement();
            sql::ResultSet* carsList = stmt->executeQuery("SELECT * FROM cars WHERE CarID = " + to_string(CarID));
            string output;
            if (carsList->next()) {
                string tempUserID = carsList->getString("Rented_By");
                if (UserID != tempUserID) {
                    // Car not rented by this user
                    return "You have not rented this car";
                }
                else {
                    stmt->execute("UPDATE cars SET Availability = 'Yes', Rented_by = NULL WHERE CarID = '" + to_string(CarID) + "'");
                    stmt->execute("UPDATE employees SET Employee_Record = '" + to_string(Employee_Record - 1) + "' WHERE CustomerID = '" + UserID + "'");
                    Employee_Record -= 1;
                    return "Car returned successfully";
                }
            }
            else {
                return "No car with given ID found";
            }
            delete stmt;
            return output;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }
};

// Manager Class
class Manager : public User {
public:
    Manager(string ManagerID, string Password, string Manager_Name) : User(ManagerID, Password, Manager_Name) {}

    void showAllCars(Connection* con) {
        try {
            Statement* stmt = con->createStatement();
            string availableCarsQuery
                = "SELECT * FROM cars";
            sql::ResultSet* carsList = stmt->executeQuery(availableCarsQuery);
            bool flag = 0;
            while (carsList->next()) {
                if (!flag) {
                    cout << setw(10) << left << "Car ID" << "| ";
                    cout << setw(25) << left << "Model" << "| ";
                    cout << setw(12) << left << "Colour" << "| ";
                    cout << setw(15) << left << "Car Condition" << "| ";
                    cout << setw(15) << left << "Rented By" << "| ";
                    cout << setw(8) << left << "Rent Rate";
                    cout << "\n------------------------------------------------------------------------------------------------\n";
                    flag = 1;
                }
                cout << setw(10) << left << carsList->getInt("CarID") << "| ";
                cout << setw(25) << left << carsList->getString("Model") << "| ";
                cout << setw(12) << left << carsList->getString("Colour") << "| ";
                cout << setw(15) << left << carsList->getString("Car_Condition") << "| ";
                cout << setw(15) << left << carsList->getString("Rented_By") << "| ";
                cout << setw(8) << left << carsList->getInt("Rent_Rate") << "\n";
            }
            if (!flag) {
                cout << "No car to show";
            }
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    string addCar(Connection* con, int CarID, string Model, string Colour, string Car_Condition, int Rent_Rate) {
        try {
            Statement* stmt = con->createStatement();
            sql::ResultSet* carFound = stmt->executeQuery("SELECT * FROM cars WHERE CarId = " + to_string(CarID));
            string result;
            if (carFound->next()) {
                result = "Car with given Car ID already exists";
            }
            else {
                string insertCarValues
                    = "INSERT INTO cars (CarID, Model, Colour, Car_condition, Availability, Rent_Rate)"
                    "VALUES ('" + to_string(CarID) + "', '" + Model + "', '" + Colour + "', '" + Car_Condition + "', 'Yes', '" + to_string(Rent_Rate) + "')";
                stmt->execute(insertCarValues);
                result = "Car added successfully to database";
            }
            delete stmt;
            return result;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void removeCar(Connection* con, int CarID, string Model) {
        try {
            Statement* stmt = con->createStatement();
            string removeCarQuery= "DELETE FROM cars WHERE CarID = '" + to_string(CarID) +"' and Model = '" + Model + "'";
            stmt->execute(removeCarQuery);
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void modifyCarCondition(Connection* con, int CarID, string Car_Condition) {
        try {
            Statement* stmt = con->createStatement();
            string modifyCarValues = "UPDATE cars SET Car_Condition = '" + Car_Condition + "' WHERE CarID = '" + to_string(CarID) + "'";
            stmt->execute(modifyCarValues);
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }
    void modifyCarAvailability(Connection* con, int CarID, string Availability) {
        try {
            Statement* stmt = con->createStatement();
            string modifyCarValues = "UPDATE cars SET Availability = '" + Availability + "' WHERE CarID = '" + to_string(CarID) + "'";
            stmt->execute(modifyCarValues);
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }
    void modifyCarRent(Connection* con, int CarID, int Rent_Rate) {
        try {
            Statement* stmt = con->createStatement();
            string modifyCarValues = "UPDATE cars SET Rent_Rate = '" + to_string(Rent_Rate) + "' WHERE CarID = '" + to_string(CarID) + "'";
            stmt->execute(modifyCarValues);
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void showAllCustomers(Connection* con) {
        try {
            Statement* stmt = con->createStatement();
            string availableCarsQuery
                = "SELECT * FROM customers";
            sql::ResultSet* customerList = stmt->executeQuery(availableCarsQuery);
            bool flag = 0;
            while (customerList->next()) {
                if (!flag) {
                    cout << setw(8) << left << "ID" << "| ";
                    cout << setw(25) << left << "Customer Name" << "| ";
                    cout << setw(12) << left << "Car Record" << "| ";
                    cout << setw(15) << left << "Fine Due" << "| ";
                    cout << setw(15) << left << "Password";
                    cout << "\n-----------------------------------------------------------------------------\n";
                    flag = 1;
                }
                cout << setw(8) << left << customerList->getString("CustomerID") << "| ";
                cout << setw(25) << left << customerList->getString("Customer_Name") << "| ";
                cout << setw(12) << left << customerList->getInt("Customer_Record") << "| ";
                cout << setw(15) << left << customerList->getInt("Fine_Due") << "| ";
                cout << setw(15) << left << customerList->getString("Password") << "\n";
            }
            if (!flag) {
                cout << "No customer to show";
            }
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    string addCustomer(Connection* con, string CustomerID, string Customer_Password, string Customer_Name) {
        try {
            Statement* stmt = con->createStatement();
            sql::ResultSet* customerFound = stmt->executeQuery("SELECT * FROM customers WHERE CustomerId = '" + CustomerID + "'");
            string result;
            if (customerFound->next()) {
                result = "Customer with given Customer ID already exists";
            }
            else {
                string insertCustomerValues
                    = "INSERT INTO customers (CustomerID, Password, Customer_Name)"
                    "VALUES ('" + CustomerID + "', '" + Customer_Password + "', '" + Customer_Name + "')";
                stmt->execute(insertCustomerValues);
                result = "Customer added successfully to database";
            }
            delete stmt;
            return result;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void removeCustomer(Connection* con, string CustomerID, string Customer_Name) {
        try {
            Statement* stmt = con->createStatement();
            string removeCustomerValues = "DELETE FROM customers WHERE CustomerID = '" + CustomerID + "' and Customer_Name = '" + Customer_Name + "'";
            stmt->execute(removeCustomerValues);
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void modifyCustomerPassword(Connection* con, string CustomerID, string Customer_Password) {
        try {
            Statement* stmt = con->createStatement();
            string modifyCustomerValues = "UPDATE customers SET Password = '" + Customer_Password + "' WHERE CustomerID = '" + CustomerID + "'";
            stmt->execute(modifyCustomerValues);
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }
    void modifyCustomerFine(Connection* con, string CustomerID, int Fine_Due) {
        try {
            Statement* stmt = con->createStatement();
            sql::ResultSet* cust = stmt->executeQuery("SELECT * FROM customers WHERE CustomerID = '" + CustomerID + "'");
            cust->next();
            int oldFine = cust->getInt("Fine_Due");
            Fine_Due += oldFine;
            string modifyCustomerValues = "UPDATE customers SET Fine_Due = '" + to_string(Fine_Due) + "' WHERE CustomerID = '" + CustomerID + "'";
            stmt->execute(modifyCustomerValues);
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void showAllEmployees(Connection* con) {
        try {
            Statement* stmt = con->createStatement();
            string availableCarsQuery
                = "SELECT * FROM employees";
            sql::ResultSet* customerList = stmt->executeQuery(availableCarsQuery);
            bool flag = 0;
            while (customerList->next()) {
                if (!flag) {
                    cout << setw(8) << left << "ID" << "| ";
                    cout << setw(25) << left << "Employee Name" << "| ";
                    cout << setw(12) << left << "Car Record" << "| ";
                    cout << setw(15) << left << "Fine Due" << "| ";
                    cout << setw(15) << left << "Password";
                    cout << "\n-----------------------------------------------------------------------------\n";
                    flag = 1;
                }
                cout << setw(8) << left << customerList->getString("EmployeeID") << "| ";
                cout << setw(25) << left << customerList->getString("Employee_Name") << "| ";
                cout << setw(12) << left << customerList->getInt("Employee_Record") << "| ";
                cout << setw(15) << left << customerList->getInt("Fine_Due") << "| ";
                cout << setw(15) << left << customerList->getString("Password") << "\n";
            }
            if (!flag) {
                cout << "No customer to show";
            }
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    string addEmployee(Connection* con, string EmployeeID, string Employee_Password, string Employee_Name) {
        try {
            Statement* stmt = con->createStatement();
            sql::ResultSet* employeeFound = stmt->executeQuery("SELECT * FROM employees WHERE EmployeeId = '" + EmployeeID + "'");
            string result;
            if (employeeFound->next()) {
                result = "Customer with given Employee ID already exists";
            }
            else {
                string insertEmployeeValues
                    = "INSERT INTO employees (EmployeeID, Password, Employee_Name)"
                    "VALUES ('" + EmployeeID + "', '" + Employee_Password + "', '" + Employee_Name + "')";
                stmt->execute(insertEmployeeValues);
                result = "Employee added successfully to database";
            }
            delete stmt;
            return result;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void removeEmployee(Connection* con, string EmployeeID, string Employee_Name) {
        try {
            Statement* stmt = con->createStatement();
            string removeEmployeeValues = "DELETE FROM employees WHERE EmployeeID = '" + EmployeeID + "' and Employee_Name = '" + Employee_Name + "'";
            stmt->execute(removeEmployeeValues);
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }
    
    void modifyEmployeePassword(Connection* con, string EmployeeID, string Employee_Password) {
        try {
            Statement* stmt = con->createStatement();
            string modifyEmployeeValues = "UPDATE employees SET Password = '" + Employee_Password + "' WHERE EmployeeID = '" + EmployeeID + "'";
            stmt->execute(modifyEmployeeValues);
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }
    void modifyEmployeeFine(Connection* con, string EmployeeID, int Fine_Due) {
        try {
            Statement* stmt = con->createStatement();
            sql::ResultSet* empl = stmt->executeQuery("SELECT * FROM Employees WHERE EmployeeID = '" + EmployeeID + "'");
            empl->next();
            int oldFine = empl->getInt("Fine_Due");
            Fine_Due += oldFine;
            string modifyEmployeeValues = "UPDATE Employees SET Fine_Due = '" + to_string(Fine_Due) + "' WHERE EmployeeID = '" + EmployeeID + "'";
            stmt->execute(modifyEmployeeValues);
            delete stmt;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        } 
    }
};

// Frontend class to manager all interactions with the user
class FrontEnd {
private:
    string userIDs[4] = { "_", "CustomerID", "EmployeeID", "ManagerID" };
    string users[4] = { "_", "customers", "employees", "managers" };
public:
    void rentalHead() {
        cout << "___________________________";
        cout << "\n\n";
        cout << "Monkesh's Car Rental System";
        cout << "\n";
        cout << "___________________________";
        cout << "\n\n";
    }

    int CommonHead() {
        cout << "Choose an action to perform\n\n";
        cout << "1. Show all available cars to rent\n";
        cout << "2. Show all rented cars\n";
        cout << "3. Rent a car\n";
        cout << "4. Return a car\n";
        cout << "5. Logout";
        cout << "\n\nEnter option : ";
        string option;
        getline(cin, option);
        if (option == "1") return 1;
        else if (option == "2") return 2;
        else if (option == "3") return 3;
        else if (option == "4") return 4;
        else if (option == "5") return 5;
        else return 0;
    }

    int ManagerHead() {
        cout << "Choose an action to perform\n\n";
        cout << "1. Show all available cars\n";
        cout << "2. Add new car\n";
        cout << "3. Remove existing car\n";
        cout << "4. Modify car details\n";
        cout << "5. Show all customers\n";
        cout << "6. Add new customer\n";
        cout << "7. Remove existing customer\n";
        cout << "8. Modify customer details\n";
        cout << "9. Show all employees\n";
        cout << "10. Add new employee\n";
        cout << "11. Remove existing employee\n";
        cout << "12. Modify employee details\n";
        cout << "13. Logout\n";
        cout << "\n\nEnter option : ";
        string option;
        getline(cin, option);
        if (option == "1") return 1;
        else if (option == "2") return 2;
        else if (option == "3") return 3;
        else if (option == "4") return 4;
        else if (option == "5") return 5;
        else if (option == "6") return 6;
        else if (option == "7") return 7;
        else if (option == "8") return 8;
        else if (option == "9") return 9;
        else if (option == "10") return 10;
        else if (option == "11") return 11;
        else if (option == "12") return 12;
        else if (option == "13") return 13;
        else return 0;
    }

    void CustomerLoggedIn(Customer* ctmr, Connection* con) {
        rentalHead();
        int choice = CommonHead();
        system("cls");
        rentalHead();
        if (choice == 0) {
            cout << "\nOption not entered from given choices\n";
            cout << "Redirecting...";
            sleep(4000);
            system("cls");
            return CustomerLoggedIn(ctmr, con);
        }
        else if (choice == 1) {
            ctmr->showAvailableCars(con);
            cout << "\nPress enter to continue";
            string temp;
            getline(cin, temp);
            cout << "\nRedirecting...";
            sleep(2000);
            system("cls");
            CustomerLoggedIn(ctmr, con);
        }
        else if (choice == 2) {
            ctmr->showRentedCars(con);
            cout << "\nPress enter to continue";
            string temp;
            getline(cin, temp);
            cout << "\nRedirecting...";
            sleep(2000);
            system("cls");
            CustomerLoggedIn(ctmr, con);
        }
        else if (choice == 3) {
            string CarID;
            cout << "Enter the ID of the car you want to rent : ";
            getline(cin, CarID);
            if (!validInt(CarID)) {
                cout << "\nInvalid Car ID entered!";
            }
            else {
                string result = ctmr->rentCar(con, stoi(CarID));
                cout << "\n" << result;
            }
            cout << "\nRedirecting...";
            sleep(3000);
            system("cls");
            CustomerLoggedIn(ctmr, con);
        }
        else if (choice == 4) {
            string CarID;
            cout << "Enter the ID of the car you want to return : ";
            getline(cin, CarID);
            if (!validInt(CarID)) {
                cout << "\nInvalid Car ID entered!";
            }
            else {
                string result = ctmr->returnCar(con, stoi(CarID));
                cout << "\n" << result;
            }
            cout << "\nRedirecting...";
            sleep(3000);
            system("cls");
            CustomerLoggedIn(ctmr, con);
        }
        else if (choice == 5) {
            //deconstruct ctmr ??
            cout << "\nLogging out...";
            sleep(2000);
            system("cls");
            rentalHead();
            login(con);
        }

    }

    void EmployeeLoggedIn(Employee* empl, Connection* con) {
        rentalHead();
        int choice = CommonHead();
        system("cls");
        rentalHead();
        if (choice == 0) {
            cout << "\nOption not entered from given choices\n";
            cout << "Redirecting...";
            sleep(3000);
            system("cls");
            return EmployeeLoggedIn(empl, con);
        }
        else if (choice == 1) {
            empl->showAvailableCars(con);
            cout << "\nPress enter to continue";
            string temp;
            getline(cin, temp);
            cout << "\nRedirecting...";
            sleep(2000);
            system("cls");
            EmployeeLoggedIn(empl, con);
        }
        else if (choice == 2) {
            empl->showRentedCars(con);
            cout << "\nPress enter to continue";
            string temp;
            getline(cin, temp);
            cout << "\nRedirecting...";
            sleep(2000);
            system("cls");
            EmployeeLoggedIn(empl, con);
        }
        else if (choice == 3) {
            string CarID;
            cout << "Enter the ID of the car you want to rent : ";
            getline(cin, CarID);
            if (!validInt(CarID)) {
                cout << "\nInvalid Car ID entered!";
            }
            else {
                string result = empl->rentCar(con, stoi(CarID));
                cout << "\n" << result;
            }
            cout << "\nRedirecting...";
            sleep(3000);
            system("cls");
            EmployeeLoggedIn(empl, con);
        }
        else if (choice == 4) {
            string CarID;
            cout << "Enter the ID of the car you want to return : ";
            getline(cin, CarID);
            if (!validInt(CarID)) {
                cout << "\nInvalid Car ID entered!";
            }
            else {
                string result = empl->returnCar(con, stoi(CarID));
                cout << "\n" << result;
            }
            cout << "\nRedirecting...";
            sleep(3000);
            system("cls");
            EmployeeLoggedIn(empl, con);
        }
        else if (choice == 5) {
            //deconstruct ctmr ??
            cout << "\nLogging out...";
            sleep(2000);
            system("cls");
            rentalHead();
            login(con);
        }
    }

    void ManagerLoggedIn(Manager* mgr, Connection* con) {
        rentalHead();
        int choice = ManagerHead();
        system("cls");
        rentalHead();
        if (choice == 0) {
            cout << "\nOption not entered from given choices\n";
            cout << "Redirecting...";
            sleep(2000);
            system("cls");
            return ManagerLoggedIn(mgr, con);
        }
        else if (choice == 1) {
            mgr->showAllCars(con);
            cout << "\nPress enter to continue";
            string temp;
            getline(cin, temp);
            cout << "\nRedirecting...";
            sleep(2000);
            system("cls");
            ManagerLoggedIn(mgr, con);
        }
        else if (choice == 2) { //add new car
            string CarID, Model, Colour, Car_Condition, Rent_Rate;
            cout << "Enter new car ID : ";
            getline(cin, CarID);
            cout << "Enter car model : ";
            getline(cin, Model);
            cout << "Enter car colour : ";
            getline(cin, Colour);
            cout << "Enter car condition : ";
            getline(cin, Car_Condition);
            cout << "Enter rent rate : ";
            getline(cin, Rent_Rate);

            if (!validInt(CarID) || !validInt(Rent_Rate)) {
                cout << "\n\nInvalid value entered!!\n";
            }
            else {
                cout << "\n\n";
                cout << "CarID : " << CarID << "\n";
                cout << "Model : " << Model << "\n";
                cout << "Colour : " << Colour << "\n";
                cout << "Car Condition : " << Car_Condition << "\n";
                cout << "Rent Rate : " << Rent_Rate << "\n\n";
                cout << "Please verify the Car details(yes/no) : ";
                string confirm;
                getline(cin, confirm);
                if (confirm == "yes") {
                    string result = mgr->addCar(con, stoi(CarID), Model, Colour, Car_Condition, stoi(Rent_Rate));
                    cout << "\n\n" << result << "\n";
                }
                else {
                    cout << "\n\nCar not added to database\n";
                }
            }
            cout << "Redirecting...";
            sleep(3000);
            system("cls");
            return ManagerLoggedIn(mgr, con);
        }
        else if (choice == 3) { //remove car
            string CarID, Model;
            cout << "Enter the Car ID : ";
            getline(cin, CarID);
            cout << "Enter car model : ";
            getline(cin, Model);
            if (!validInt(CarID)) {
                cout << "\n\nInvalid Car ID entered\n";
            }
            else {
                cout << "\n\n";
                cout << "Car ID : " << CarID << "\n";
                cout << "Model : " << Model << "\n";
                string confirm;
                cout << "\nConfirm the Car details(yes/no) : ";
                getline(cin, confirm);
                if (confirm == "yes") {
                    mgr->removeCar(con, stoi(CarID), Model);
                    cout << "\n\nGiven CarID, if existed, was removed from the database";
                }
                else {
                    cout << "\n\nNo changes made to database";
                }
            }
            cout << "\nRedirecting...";
            sleep(3000);
            system("cls");
            return ManagerLoggedIn(mgr, con);
        }
        else if (choice == 4) { //modify car details
            string CarID, option;
            cout << "Enter the Car Id to modify : ";
            getline(cin, CarID);
            if (!validInt(CarID)) {
                cout << "\n\nInvalid Car ID entered!!\n";
            }
            else {
                cout << "\n\n";
                cout << "1. Car Condition\n";
                cout << "2. Availability\n";
                cout << "3. Rent Rate\n";
                cout << "\n\nSelect a field to modiy : ";
                getline(cin, option);
                if (option == "1") {
                    string Car_Condition;
                    cout << "\n\nEnter new car condition : ";
                    getline(cin, Car_Condition);
                    mgr->modifyCarCondition(con, stoi(CarID), Car_Condition);
                    cout << "\n\nModification made";
                }
                else if (option == "2") {
                    string Availability;
                    cout << "\n\nEnter new car availability : ";
                    getline(cin, Availability);
                    mgr->modifyCarAvailability(con, stoi(CarID), Availability);
                    cout << "\n\nModification made";
                }
                else if (option == "3") {
                    string Rent_Rate;
                    cout << "\n\nEnter new car rent : ";
                    getline(cin, Rent_Rate);
                    if (!validInt(Rent_Rate)) {
                        cout << "\n\nInvalid value for car rent!!";
                        cout << "\nRedirecting...";
                        sleep(3000);
                        system("cls");
                        return ManagerLoggedIn(mgr, con);
                    }
                    mgr->modifyCarRent(con, stoi(CarID), stoi(Rent_Rate));
                    cout << "\n\nModification made";
                }
                else cout << "\n\nNo modification made";
            }
            cout << "\nRedirecting...";
            sleep(3000);
            system("cls");
            return ManagerLoggedIn(mgr, con);
        }
        else if (choice == 5) {
            mgr->showAllCustomers(con);
            cout << "\nPress enter to continue";
            string temp;
            getline(cin, temp);
            cout << "\nRedirecting...";
            sleep(2000);
            system("cls");
            ManagerLoggedIn(mgr, con);
        }
        else if (choice == 6) {
            string CustomerID, Customer_Password, Customer_Name;
            cout << "Enter new Customer ID : ";
            getline(cin, CustomerID);
            cout << "Enter Customer Password : ";
            getline(cin, Customer_Password);
            cout << "Enter Customer Name : ";
            getline(cin, Customer_Name);

            cout << "\n\n";
            cout << "Customer ID : " << CustomerID << "\n";
            cout << "Customer Password : " << Customer_Password << "\n";
            cout << "Customer Name : " << Customer_Name << "\n\n";
            cout << "Please verify the Customer details(yes/no) : ";
            string confirm;
            getline(cin, confirm);
            if (confirm == "yes") {
                string result = mgr->addCustomer(con, CustomerID, Customer_Password, Customer_Name);
                cout << "\n\n" << result << "\n";
            }
            else {
                cout << "\n\nCustomer not added to database\n";
            }
            cout << "Redirecting...";
            sleep(3000);
            system("cls");
            return ManagerLoggedIn(mgr, con);
        }
        else if (choice == 7) {
            string CustomerID, Customer_Name;
            cout << "Enter the Customer ID : ";
            getline(cin, CustomerID);
            cout << "Enter Customer Name : ";
            getline(cin, Customer_Name);
            cout << "\n\n";
            cout << "Customer ID : " << CustomerID << "\n";
            cout << "Customer Name : " << Customer_Name << "\n";
            string confirm;
            cout << "\nConfirm the Customer details(yes/no) : ";
            getline(cin, confirm);
            if (confirm == "yes") {
                mgr->removeCustomer(con, CustomerID, Customer_Name);
                cout << "\n\nGiven CustomerID, if existed, was removed from the database";
            }
            else {
                cout << "\n\nNo changes made to database";
            }
            cout << "\nRedirecting...";
            sleep(3000);
            system("cls");
            return ManagerLoggedIn(mgr, con);
        }
        else if (choice == 8) {
            string CustomerID, option;
            cout << "Enter the Customer Id to modify : ";
            getline(cin, CustomerID);
            cout << "\n\n";
            cout << "1. Customer Password\n";
            cout << "2. Fine_Due\n";
            cout << "\n\nSelect a field to modiy : ";
            getline(cin, option);
            if (option == "1") {
                string Customer_Password;
                cout << "\n\nEnter new customer password : ";
                getline(cin, Customer_Password);
                mgr->modifyCustomerPassword(con, CustomerID, Customer_Password);
                cout << "\n\nModification made";
            }
            else if (option == "2") {
                string Fine_Due;

                cout << "\n\n1. Impose fine\n";
                cout << "2.Customer fine deposition\n";
                cout << "Enter your choice : ";
                string choice;
                getline(cin, choice);
                if (choice == "1") {
                    cout << "\n\nEnter fine value : ";
                    getline(cin, Fine_Due);
                    if (!validInt(Fine_Due)) {
                        cout << "\n\nInvalid value entered!!";
                    }
                    else {
                        mgr->modifyCustomerFine(con, CustomerID, stoi(Fine_Due));
                        cout << "\n\nModification made";
                    }
                }
                else if (choice == "2") {
                    cout << "\n\nEnter fine deposit value : ";
                    getline(cin, Fine_Due);
                    if (!validInt(Fine_Due)) {
                        cout << "\n\nInvalid value entered!!";
                    }
                    else {
                        mgr->modifyCustomerFine(con, CustomerID, (-1)*stoi(Fine_Due));
                        cout << "\n\nModification made";
                    }
                }
                else {
                    cout << "Invalid choice entered!!";
                }
            }
            else cout << "\n\nNo modification made";
            cout << "\nRedirecting...";
            sleep(3000);
            system("cls");
            return ManagerLoggedIn(mgr, con);
        }
        else if (choice == 9) {
            mgr->showAllEmployees(con);
            cout << "\nPress enter to continue";
            string temp;
            getline(cin, temp);
            cout << "\nRedirecting...";
            sleep(2000);
            system("cls");
            ManagerLoggedIn(mgr, con);
        }
        else if (choice == 10) {
            string EmployeeID, Employee_Password, Employee_Name;
            cout << "Enter new Employee ID : ";
            getline(cin, EmployeeID);
            cout << "Enter Employee Password : ";
            getline(cin, Employee_Password);
            cout << "Enter Employee Name : ";
            getline(cin, Employee_Name);

            cout << "\n\n";
            cout << "Employee ID : " << EmployeeID << "\n";
            cout << "Employee Password : " << Employee_Password << "\n";
            cout << "Employee Name : " << Employee_Name << "\n\n";
            cout << "Please verify the Employee details(yes/no) : ";
            string confirm;
            getline(cin, confirm);
            if (confirm == "yes") {
                string result = mgr->addEmployee(con, EmployeeID, Employee_Password, Employee_Name);
                cout << "\n\n" << result << "\n";
            }
            else {
                cout << "\n\nCustomer not added to database\n";
            }
            cout << "Redirecting...";
            sleep(3000);
            system("cls");
            return ManagerLoggedIn(mgr, con);
        }
        else if (choice == 11) {
            string EmployeeID, Employee_Name;
            cout << "Enter the Employee ID : ";
            getline(cin, EmployeeID);
            cout << "Enter Employee Name : ";
            getline(cin, Employee_Name);
            cout << "\n\n";
            cout << "Employee ID : " << EmployeeID << "\n";
            cout << "Employee Name : " << Employee_Name << "\n";
            string confirm;
            cout << "\nConfirm the Employee details(yes/no) : ";
            getline(cin, confirm);
            if (confirm == "yes") {
                mgr->removeEmployee(con, EmployeeID, Employee_Name);
                cout << "\n\nGiven EmployeeID, if existed, was removed from the database";
            }
            else {
                cout << "\n\nNo changes made to database";
            }
            cout << "\nRedirecting...";
            sleep(3000);
            system("cls");
            return ManagerLoggedIn(mgr, con);
        }
        else if (choice == 12) {
            string EmployeeID, option;
            cout << "Enter the Employee Id to modify : ";
            getline(cin, EmployeeID);
            cout << "\n\n";
            cout << "1. Employee Password\n";
            cout << "2. Fine_Due\n";
            cout << "\n\nSelect a field to modiy : ";
            getline(cin, option);
            if (option == "1") {
                string Employee_Password;
                cout << "\n\nEnter new Employee password : ";
                getline(cin, Employee_Password);
                mgr->modifyEmployeePassword(con, EmployeeID, Employee_Password);
                cout << "\n\nModification made";
            }
            else if (option == "2") {
                string Fine_Due;

                cout << "\n\n1. Impose fine\n";
                cout << "2.Employee fine deposition\n";
                cout << "Enter your choice : ";
                string choice;
                getline(cin, choice);
                if (choice == "1") {
                    cout << "\n\nEnter fine value : ";
                    getline(cin, Fine_Due);
                    if (!validInt(Fine_Due)) {
                        cout << "\n\nInvalid value entered!!";
                    }
                    else {
                        mgr->modifyEmployeeFine(con, EmployeeID, stoi(Fine_Due));
                        cout << "\n\nModification made";
                    }
                }
                else if (choice == "2") {
                    cout << "\n\nEnter fine deposit value : ";
                    getline(cin, Fine_Due);
                    if (!validInt(Fine_Due)) {
                        cout << "\n\nInvalid value entered!!";
                    }
                    else {
                        mgr->modifyEmployeeFine(con, EmployeeID, (-1) * stoi(Fine_Due));
                        cout << "\n\nModification made";
                    }
                }
                else {
                    cout << "Invalid choice entered!!";
                }
            }
            else cout << "\n\nNo modification made";
            cout << "\nRedirecting...";
            sleep(3000);
            system("cls");
            return ManagerLoggedIn(mgr, con);
        }
        else if (choice == 13) {
            cout << "\nLogging out...";
            sleep(2000);
            system("cls");
            rentalHead();
            login(con);
        }
    }

    void login(Connection* con) {
        cout << "Welcome!\n";
        cout << "Choose from given Users to login\n\n";
        cout << "1. Customer\n";
        cout << "2. Employee\n";
        cout << "3. Manager\n";
        cout << "4. Exit\n\n";
        string option;
        cout << "Enter option(1/2/3/4) : ";
        getline(cin, option);
        if (option != "1" && option != "2" && option != "3" && option != "4") {
            cout << "\nOption not entered from given choices\n";
            cout << "Redirecting...";
            sleep(3000);
            system("cls");
            rentalHead();
            return login(con);
        }
        if (option == "4") {
            cout << "\nExiting...";
            sleep(2000);
            return;
        }
        system("cls");
        rentalHead();
        sleep(500);
        string UserID, UserPassword;
        int choice = stoi(option);
        cout << "Enter UserID : ";
        getline(cin, UserID);
        cout << "\nEnter password : ";
        getline(cin, UserPassword);
        try {
            Statement* stmt = con->createStatement();
            string findUser = "SELECT * FROM " + users[choice] + " WHERE " + userIDs[choice] + " = '" + UserID + "' and Password = '" + UserPassword + "'";
            sql::ResultSet* userData = stmt->executeQuery(findUser);
            if (userData->next()) {
                delete stmt;
                system("cls");
                if (choice == 1) {
                    string CustomerID = userData->getString("CustomerID");
                    string Password = userData->getString("Password");
                    string Customer_Name = userData->getString("Customer_Name");
                    int Customer_Record = userData->getInt("Customer_Record");
                    int Fine_Due = userData->getInt("Fine_Due");
                    Customer ctmr(CustomerID, Password, Customer_Name, Customer_Record, Fine_Due);
                    CustomerLoggedIn(&ctmr, con);
                }
                else if (choice == 2) {
                    string EmployeeID = userData->getString("EmployeeID");
                    string Password = userData->getString("Password");
                    string Employee_Name = userData->getString("Employee_Name");
                    int Employee_Record = userData->getInt("Employee_Record");
                    int Fine_Due = userData->getInt("Fine_Due");
                    Employee empl(EmployeeID, Password, Employee_Name, Employee_Record, Fine_Due);
                    EmployeeLoggedIn(&empl, con);
                }
                else if (choice == 3) {
                    string ManagerID = userData->getString("ManagerID");
                    string Password = userData->getString("Password");
                    string Manager_Name = userData->getString("Manager_Name");
                    Manager mgr(ManagerID, Password, Manager_Name);
                    ManagerLoggedIn(&mgr, con);
                }
                delete userData;
            }
            else {
                cout << "\nNo User with above credentials found\n";
                cout << "Redirecting...";
                sleep(3000);
                system("cls");
                rentalHead();
                login(con);
            }
            
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }

    }
    
    void nothing(){}
};

int main() {
    DatabaseConnector dbConnector("tcp://localhost:3306", "root", "1234", "Car_Rental_System");
    Connection* con = dbConnector.getConnection();
    bool stat = dbConnector.getStat();
    DatabaseInitiate dbInitiate(stat);
    dbInitiate.InitiateTables(con);

    FrontEnd fe;
    fe.rentalHead();
    fe.login(con);

    return 0;
}