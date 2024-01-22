#pragma once
#include <string>
#include <vector>

#include "Order.h"
#include "Customer.h"
#include "Volunteer.h"

class Action;
class Volunteer;

// Warehouse responsible for Volunteers, Customers and Actions.

class WareHouse
{

public:
    WareHouse(const string &configFilePath);
    virtual ~WareHouse();
    void start();
    const vector<Action *> &getActions() const; //d
    void addOrder(Order *order);
    void addAction(Action *action);
    void printActionsLogs();
    Customer &getCustomer(int customerId) const; //d
    Volunteer &getVolunteer(int volunteerId) const; //d
    Order &getOrder(int orderId) const; //d
    void close();
    void open();
    int getOrderCounter();
    void setOrderCounter(int orderCounter);
    int getCustomerCounter();
    void setCustomerCounter(int customerCounter);
    int getVolunteerCounter();

    vector<Order *> &getPendingOrders(); // added by myself
    vector<Order *> &getInProcessOrders();
    vector<Order *> &getCompletedOrders();
    vector<Volunteer *> &getVolunteers();

private:
    bool isOpen;                    
    vector<Action *> actionsLog; 
    vector<Volunteer *> volunteers;  
    vector<Order *> pendingOrders;   
    vector<Order *> inProcessOrders; 
    vector<Order *> completedOrders; 
    vector<Customer *> customers;    
    int customerCounter;             // For assigning unique customer IDs
    int volunteerCounter;            //For assigning unique volunteer IDs
    int orderCounter;                //For assinging unique order IDs
};