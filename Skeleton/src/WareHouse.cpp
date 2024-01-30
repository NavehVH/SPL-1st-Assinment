#include "../include/Order.h"
#include "../include/Volunteer.h"
#include "../include/Customer.h"
#include "../include/Action.h"
#include "../include/WareHouse.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>

WareHouse::WareHouse(const string &configFilePath):  isOpen(false), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(0), volunteerCounter(0), orderCounter(0) 
{
    std::ifstream file(configFilePath);
    if (file.is_open())
    {
        std::cout << "Successfully opened file" << std::endl;

        processFile(file);
        std::cout << "WareHouse is open" << std::endl;
        open();
        start();
    }
    else
    {
        std::cout << "Failed to open file" << std::endl;
        exit(1);
    }
}

WareHouse::WareHouse(const WareHouse &other) : isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter)
{

    for (const Action *a : other.actionsLog)
    {
        actionsLog.push_back(a->clone());
    }
    for (const Volunteer *v : other.volunteers)
    {
        volunteers.push_back(v->clone());
    }
    for (const Order *o : other.pendingOrders)
    {
        pendingOrders.push_back(new Order(*o));
    }
    for (const Order *o : other.inProcessOrders)
    {
        inProcessOrders.push_back(new Order(*o));
    }
    for (const Order *o : other.completedOrders)
    {
        completedOrders.push_back(new Order(*o));
    }
    for (const Customer *c : other.customers)
    {
        customers.push_back(c->clone());
    }
}

WareHouse &WareHouse::operator=(const WareHouse &other)
{
    if (this != &other)
    {

        isOpen = other.isOpen;
        orderCounter = other.orderCounter;
        volunteerCounter = other.volunteerCounter;
        customerCounter = other.customerCounter;

        for (Action *a : actionsLog)
        {
            delete a;
        }
        actionsLog.clear();

        for (Volunteer *v : volunteers)
        {
            delete v;
        }
        volunteers.clear();

        for (Order *o : pendingOrders)
        {
            delete o;
        }
        pendingOrders.clear();

        for (Order *o : inProcessOrders)
        {
            delete o;
        }
        inProcessOrders.clear();

        for (Order *o : completedOrders)
        {
            delete o;
        }
        completedOrders.clear();

        for (Customer *c : customers)
        {
            delete c;
        }
        customers.clear();

        for (const Action *a : other.actionsLog)
        {
            actionsLog.push_back(a->clone());
        }
        for (const Volunteer *v : other.volunteers)
        {
            volunteers.push_back(v->clone());
        }
        for (const Order *o : other.pendingOrders)
        {
            pendingOrders.push_back(new Order(*o));
        }
        for (const Order *o : other.inProcessOrders)
        {
            inProcessOrders.push_back(new Order(*o));
        }
        for (const Order *o : other.completedOrders)
        {
            completedOrders.push_back(new Order(*o));
        }
        for (const Customer *c : other.customers)
        {
            customers.push_back(c->clone());
        }
    }
    return *this;
}

WareHouse::WareHouse(WareHouse &&other) noexcept
    : isOpen(other.isOpen), actionsLog(std::move(other.actionsLog)),
      volunteers(std::move(other.volunteers)),
      pendingOrders(std::move(other.pendingOrders)), inProcessOrders(std::move(other.inProcessOrders)),
      completedOrders(std::move(other.completedOrders)), customers(std::move(other.customers)), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter)
{

    other.actionsLog.clear();
    other.volunteers.clear();
    other.pendingOrders.clear();
    other.inProcessOrders.clear();
    other.completedOrders.clear();
    other.customers.clear();
}

WareHouse &WareHouse::operator=(WareHouse &&other) noexcept
{
    if (this != &other)
    {
        for (Action *a : actionsLog)
        {
            delete a;
        }
        actionsLog.clear();

        for (Volunteer *v : volunteers)
        {
            delete v;
        }
        volunteers.clear();

        for (Order *o : pendingOrders)
        {
            delete o;
        }
        pendingOrders.clear();

        for (Order *o : inProcessOrders)
        {
            delete o;
        }
        inProcessOrders.clear();

        for (Order *o : completedOrders)
        {
            delete o;
        }
        completedOrders.clear();

        for (Customer *c : customers)
        {
            delete c;
            customers.clear();
        }
        isOpen = std::move(other.isOpen);
        orderCounter = std::move(other.orderCounter);
        volunteerCounter = std::move(other.volunteerCounter);
        customerCounter = std::move(other.customerCounter);
        actionsLog = std::move(other.actionsLog);
        volunteers = std::move(other.volunteers);
        pendingOrders = std::move(other.pendingOrders);
        inProcessOrders = std::move(other.inProcessOrders);
        completedOrders = std::move(other.completedOrders);
        customers = std::move(other.customers);
    }
    return *this;
}

WareHouse::~WareHouse()
{
    for (Action *a : actionsLog)
    {
        if (a)
            delete a;
        a = nullptr;
    }
    for (Volunteer *a : volunteers)
    {
        if (a)
            delete a;
        a = nullptr;
    }
    for (Order *a : pendingOrders)
    {
        if (a)
            delete a;
        a = nullptr;
    }
    for (Order *a : inProcessOrders)
    {
        if (a)
            delete a;
        a = nullptr;
    }
    for (Order *a : completedOrders)
    {
        if (a)
            delete a;
        a = nullptr;
    }
    for (Customer *a : customers)
    {
        if (a)
            delete a;
        a = nullptr;
    }
}

Order &WareHouse::getNextOrder(Volunteer *v)
{
    Order *emptyOrder = new Order(-1);
    if (typeid(*v) == typeid(CollectorVolunteer) || typeid(*v) == typeid(LimitedCollectorVolunteer))
    {
        for (Order *o : pendingOrders)
            if (o->getStatus() == OrderStatus::PENDING)
                return *o;
    }

    if (typeid(*v) == typeid(DriverVolunteer) || typeid(*v) == typeid(LimitedDriverVolunteer))
    {
        for (Order *o : pendingOrders)
            if (o->getStatus() == OrderStatus::COLLECTING)
                return *o;

        for (Order *o : inProcessOrders)
            if (o->getStatus() == OrderStatus::COLLECTING)
                return *o;
    }
    return *emptyOrder;
}

// not sure i did this correctly
void WareHouse::DeleteLimitedVolunteer(Volunteer *v)
{
    if (std::find(volunteers.begin(), volunteers.end(), v) != volunteers.end())
    {
        //volunteers.erase(std::remove(volunteers.begin(), volunteers.end(), v), volunteers.end());
    }
    //delete v;
}

void WareHouse::processFile(std::ifstream &inFile)
{
    std::string line, word, firstWord, name, type;
    int coolDown, maxOrders, distance, maxDistance, distancePerStep;

    while (getline(inFile, line))
    {
        std::istringstream iss(line);
        iss >> firstWord;
        if (firstWord == "customer")
        {
            iss >> name >> type >> distance >> maxOrders;
            if (type == "soldier")
            {
                AddCustomer a = AddCustomer(name, CustomerType::Soldier, distance, maxOrders);
                a.act(*this);
            }
            else if (type == "civilian")
            {
                AddCustomer a = AddCustomer(name, CustomerType::Civilian, distance, maxOrders);
                a.act(*this);
            }
        }   
        else if (firstWord == "volunteer")
        {
            iss >> name >> type;
            if (type == "limited_collector")
            {
                iss >> coolDown >> maxOrders;
                LimitedCollectorVolunteer *limitedCollectorVolunteer = new LimitedCollectorVolunteer(volunteerCounter, name, coolDown, maxOrders);
                volunteers.push_back(limitedCollectorVolunteer);
                volunteerCounter++;
            }
            else if (type == "collector")
            {
                iss >> coolDown;
                CollectorVolunteer *collectorVolunteer = new CollectorVolunteer(volunteerCounter, name, coolDown);
                volunteers.push_back(collectorVolunteer);
                volunteerCounter++;
            }
            else if (type == "limited_driver")
            {
                iss >> maxDistance >> distancePerStep >> maxOrders;
                LimitedDriverVolunteer *limitedDriverVolunteer = new LimitedDriverVolunteer(volunteerCounter, name, maxDistance, distancePerStep, maxOrders);
                volunteers.push_back(limitedDriverVolunteer);
                volunteerCounter++;
            }
            else if (type == "driver")
            {
                iss >> maxDistance >> distancePerStep;
                DriverVolunteer *driverVolunteer = new DriverVolunteer(volunteerCounter, name, maxDistance, distancePerStep);
                volunteers.push_back(driverVolunteer);
                volunteerCounter++;
            }
        }
    }
}

const vector<Action *> &WareHouse::getActions() const
{
    return actionsLog;
}

void WareHouse::addAction(Action *action)
{
    actionsLog.push_back(action);
}

// Am I suppose to return null if not found? .-.
Customer &WareHouse::getCustomer(int customerId) const
{
    SoldierCustomer *emptyCustomer = new SoldierCustomer(-1);
    for (Customer *c : customers)
        if (c->getId() == customerId)
            return *c;
    return *emptyCustomer;
}

Volunteer &WareHouse::getVolunteer(int volunteerId) const
{
    CollectorVolunteer *emptyVolunteer = new CollectorVolunteer(-1);   
    for (Volunteer *v : volunteers)
        if (v->getId() == volunteerId)
            return *v;
    return *emptyVolunteer;
}

vector<Volunteer *> &WareHouse::getVolunteers()
{
    return volunteers;
}

vector<Order *> &WareHouse::getPendingOrders()
{
    return pendingOrders;
}

vector<Customer *> &WareHouse::getCustomers()
{
    return customers;
}

Order &WareHouse::getOrder(int orderId) const
{
    Order *emptyOrder = new Order(-1);
    for (Order *o : pendingOrders)
        if (o->getId() == orderId)
            return *o;
    for (Order *o : inProcessOrders)
        if (o->getId() == orderId)
            return *o;
    for (Order *o : completedOrders)
        if (o->getId() == orderId)
            return *o;

    return *emptyOrder;
}

int WareHouse::getOrderCounter()
{
    return orderCounter;
}

int WareHouse::getCustomerCounter()
{
    return customerCounter;
}

int WareHouse::getVolunteerCounter()
{
    return volunteerCounter;
}

void WareHouse::open()
{
    isOpen = true;
}

void WareHouse::addOrder(Order *order)
{
    if (std::find(inProcessOrders.begin(), inProcessOrders.end(), order) != inProcessOrders.end())
    {
        if (order->getStatus() == OrderStatus::DELIVERING)
        {
            inProcessOrders.erase(std::remove(inProcessOrders.begin(), inProcessOrders.end(), order), inProcessOrders.end());
            completedOrders.push_back(order);
            order->setStatus(OrderStatus::COMPLETED);
        }
        else if (order->getStatus() == OrderStatus::COLLECTING) {
            order->setStatus(OrderStatus::DELIVERING);
            std::cout << "ITS DELIVERING!" << std::endl;
        }
    }
    else if (std::find(pendingOrders.begin(), pendingOrders.end(), order) != pendingOrders.end())
    {
        if (order->getStatus() == OrderStatus::DELIVERING)
        {
            pendingOrders.erase(std::remove(pendingOrders.begin(), pendingOrders.end(), order), pendingOrders.end());
            completedOrders.push_back(order);
            order->setStatus(OrderStatus::COMPLETED);
        }
        else if (order->getStatus() == OrderStatus::PENDING)
        {
            pendingOrders.erase(std::remove(pendingOrders.begin(), pendingOrders.end(), order), pendingOrders.end());
            inProcessOrders.push_back(order);
            order->setStatus(OrderStatus::COLLECTING);
        }
    }
}

void WareHouse::start()
{
    while (isOpen)
    {
        string userInput, firstWord;
        std::getline(std::cin, userInput);
        std::istringstream iss(userInput);
        std::cout << "" << std::endl; //just empty line
        iss >> firstWord;
        if (firstWord == "step")
        {
            int numberOfSteps;
            iss >> numberOfSteps;
            SimulateStep simulateStep(numberOfSteps);
            simulateStep.act(*this);
        }
        else if (firstWord == "order")
        {
            int customerId;
            iss >> customerId;
            AddOrder addOrder(customerId);
            addOrder.act(*this);
        }
        else if (firstWord == "customer")
        { //wth is this
            string name, type;
            int distance, maxOrders;
            iss >> name >> type >> distance >> maxOrders;
            if (type == "soldier")
            {
                AddCustomer a = AddCustomer(name, CustomerType::Soldier, distance, maxOrders);
                a.act(*this);
            }
            else
            {
                AddCustomer a = AddCustomer(name, CustomerType::Civilian, distance, maxOrders);
                a.act(*this);
            }
            
        }
        else if (firstWord == "orderStatus")
        {
            int orderId;
            iss >> orderId;
            PrintOrderStatus printOrderStatus(orderId);
            printOrderStatus.act(*this);
        }
        else if (firstWord == "customerStatus")
        {
            int customerId;
            iss >> customerId;
            PrintCustomerStatus printCustomerStatus(customerId);
            printCustomerStatus.act(*this);
        }
        else if (firstWord == "volunteerStatus")
        {
            int volunteerId;
            iss >> volunteerId;
            PrintVolunteerStatus printVolunteerStatus(volunteerId);
            printVolunteerStatus.act(*this);
        }
        else if (firstWord == "log")
        {
            PrintActionsLog printActionsLog;
            printActionsLog.act(*this);
        }
        else if (firstWord == "close")
        {
            Close close;
            close.act(*this);
        }
        else if (firstWord == "backup")
        {
            BackupWareHouse backupWareHouse;
            backupWareHouse.act(*this);
        }
        else if (firstWord == "restore")
        {
            RestoreWareHouse restoreWareHouse;
            restoreWareHouse.act(*this);
        }
        else
        {
            std::cout << "Invalid input" << std::endl;
        }
        std::cout << "" << std::endl; //just empty line
    }
}

void WareHouse::setCustomerCounter(int counter)
{
    customerCounter = counter;
}

void WareHouse::setOrderCounter(int counter)
{
    orderCounter = counter;
}