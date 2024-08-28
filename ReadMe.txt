This is a Car Rental System developed using OOPs concepts in C++. MySql has been used for database.

Setup the Microsoft Visual Studio, C++ connector and MySql to run program by following the instructions here :
https://www.geeksforgeeks.org/how-to-setup-mysql-database-in-visual-studio-2022-for-a-cpp-application/

Note : You may need to setup the folder as exception in your Antivirus Software to run the code

How to run the program :
	-> Use Microsoft Visual Studio to open a project folder as guided in link above
	-> Copy the cpp file to the project files
	-> Build the code (Shortcut : Ctrl + Shift + B)
	-> Run the code (Shortcut : Ctrl + F5)

Enter the corresponding option on the terminal and press enter navigate through the program.

User can be Customer, Employee or Manager.

Customer
	-> Can see all available cars and cars rented by the customer
	-> Can rent or return a car
Employee
	-> Can see all available cars and cars rented by the customer
	-> Can rent or return a car
Manager
	-> Can see all cars, customers and employees
	-> Can add, remove or modify a customer, employee or car details
	-> Can impose or remove fine on customers and employees

Initial Car database values :
CarID    Model             Colour     Car_condition     Availability    Rent_Rate
100      BMW Limousine     Red        Good              Yes             600
200      Hyundai Creta     Blue       Average           Yes             250
300      Maruti Suzuki     White      Good              Yes             100
400      Mercedez Benz     Black      Bad               No              400
500      Mahindra Bolera   White      Bad               Yes             55

Initial Customer database values :
CustomerID     Password     Customer_Name
C100           Aditya1      Aditya
C200           Shivam2      Shivam
C300           Saksham3     Saksham
C400           Astha4       Astha
C500           Rahul5       Rahul

Initial Employee database values :
EmployeeID     Password     Employee_Name
E100           Arvind1      Arvind'),"
E200           Raj2         Raj'),"
E300           Vivek3       Vivek'),"
E400           Suhail4      Suhail'),"
E500           Nishant5     Nishant')";

Initial Manager database values :
ManagerID     Password     Manager_Name
M100          Monkesh1     Monkesh