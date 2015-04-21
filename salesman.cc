//
// Assignment 3
// CPSC 5135G
// Joshua Boyd <boyd_joshua@columbusstate.edu>
//
// Simulates the actions of a traveling salesman. 
//

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>
#include <stdlib.h>

using namespace std;

// Extends runtime_error for a stack overflow exception. Sample usage:
// throw StackOverflow();
class StackOverflow: public runtime_error {
    public:
        StackOverflow() : runtime_error("Stack Overflow!") {}
};

// Extends runtime_error for a stack underflow exception. Sample usage:
// throw StackUnderflow();
class StackUnderflow: public runtime_error {
    public:
        StackUnderflow() : runtime_error("Stack Underflow!") {}
};

// A stack implementation using linked lists as the backing store. The stack
// is limited in size to 32 elements.
template <typename TYPE>
class Stack{
    const int MAX_SIZE = 32;

    private:
        int size;

        // Node to be stored in the linked list. Sample usage:
        // node * tmp = new node(x);
        class node{
            public:
                TYPE data;
                node* next;
                node(TYPE);
        };

        node* head;

    public:
        Stack();
        ~Stack();
        void pop();
        void push(TYPE);
        TYPE top();
        bool empty();
        bool full();
        friend ostream& operator<<(ostream&,const Stack&);
};

// Constructor. Sample usage:
// Stack<foo> bar; 
template <typename TYPE>
Stack<TYPE>::Stack() {
    head = NULL;
    size = 0;
}

template <typename TYPE>
Stack<TYPE>::~Stack() {
    while(!empty()) {
        pop();
    }

    return;
}

// If the stack is not empty, removes the node from the top of the stack and
// moves head to the next node. Sample usage:
// Stack<foo> bar;
// bar.pop();
template <typename TYPE>
void Stack<TYPE>::pop() {
    if (empty()) { throw StackUnderflow(); }

    node* tmp = head->next;
    delete head;
    head = tmp;
    size--;

    return;
}

// Add a node to the top of the stack, setting it's next pointer to the previous head.
// Sample usage:
// Stack<foo> bar;
// bar.push(x);
template <typename TYPE>
void Stack<TYPE>::push(TYPE x) {
    if (full()) { throw StackOverflow(); }

    node* tmp = new node(x);
    tmp->next = head;
    head = tmp;
    size++;

    return;
}

// Retrieve the top node of the stack. Sample usage:
// Stack<foo> bar;
// bar.push(x);
// foo y = bar.top();
template <typename TYPE>
TYPE Stack<TYPE>::top() {
    if (empty()) { throw StackUnderflow(); }

    return head->data;
}

// Test to see if the stack is full or not.
template <typename TYPE>
bool Stack<TYPE>::full() {
    return size == MAX_SIZE;
}

// Test to see if the stack is empty or not.
template <typename TYPE>
bool Stack<TYPE>::empty() {
    return size == 0;
}

// Print the members of the stack. Sample usage:
// Stack<foo> bar;
// bar.push(x);
// cout << bar;
template <typename TYPE>
ostream& operator<<(ostream& out, const Stack<TYPE>& rhs) {
    typename Stack<TYPE>::node* tmp = rhs.head;

    while(tmp != NULL) {
        out << "[" << tmp->data << "]->";
        tmp = tmp->next;
    }

    out << endl;

    return out;
}

// Construct a node to insert into the stack
template <typename TYPE>
Stack<TYPE>::node::node(TYPE x) {
    data = x;
    next = NULL;
}

// Storage of appointment information
struct Card {
    int timeReceived, x, y, estimatedTime;
};

// Storage of runtime statistics
struct Statistics {
    int totalTime, totalCalls, timeWaiting, timeOnRoad, timeInMeetings,
        maxWaitTime, avgWaitTime;
    Statistics() : totalTime(0),
        totalCalls(0),
        timeWaiting(0),
        timeOnRoad(0),
        timeInMeetings(0),
        maxWaitTime(0),
        avgWaitTime(0)
    { }
};

// Calculate the distance between two points on the xy plane
int distance(int x1, int y1, int x2, int y2) {
    return static_cast<int> (ceil(sqrt(pow(x2-x1,2) + pow(y2-y1,2))));
}

int main() {
    int xPos = 0, yPos = 0;

    Stack<Card> cards;
    vector<Card> userInput;
    Statistics stats;

    // Read in user input and build the stack of cards
    for (string line; getline(cin, line);) {
        Card card;
        vector<string> tokens;
        stringstream ss(line);
        string buf;

        while (ss >> buf) {
            tokens.push_back(buf);
        }

        card.timeReceived = atoi(tokens[0].data());
        card.x = atoi(tokens[1].data());
        card.y = atoi(tokens[2].data());
        card.estimatedTime = atoi(tokens[3].data());

        userInput.push_back(card);

        stats.timeInMeetings += card.estimatedTime;
    }

    while (!userInput.empty() || !cards.empty()) {
        // insert cards into the stack once the time is correct
        if (!userInput.empty()) {
            for(vector<Card>::iterator it = userInput.begin(); it != userInput.end(); ++it) {
                if (it->timeReceived <= stats.totalTime) {
                    cards.push(*it);
                    userInput.erase(it--);
                }
            }
        }

        // If there are cards, process calls
        if (!cards.empty()) {
            try {
                Card firstCard = cards.top();
                Card cardToProcess = firstCard;
                cards.pop();

                if (!cards.empty()) {
                    // There's a second card present on the stack
                    
                    Card secondCard = cards.top();
                    cards.pop();
                    
                    int d1 = distance(xPos, yPos, firstCard.x, firstCard.y);
                    int d2 = distance(xPos, yPos, secondCard.x, secondCard.y);

                    if (d1 < d2) {
                        // Second card is further away, process the first card
                        cards.push(secondCard);
                    } else if (d1 == d2) {
                        // Distance is the same
                        if (firstCard.timeReceived > secondCard.timeReceived) {
                            // Second card was received before first, so process the second card
                            cardToProcess = secondCard;
                            cards.push(firstCard);
                        }
                    } else {
                        // Second card is closer, process the first card
                        cardToProcess = secondCard;
                        cards.push(firstCard);
                    }
                }

                // Update program run statistics
                int d = distance(xPos,yPos,cardToProcess.x,cardToProcess.y);
                stats.timeOnRoad += d;
                stats.totalTime += d;

                int w = stats.totalTime - cardToProcess.timeReceived;

                if (w > stats.maxWaitTime) {
                    stats.maxWaitTime = w;
                }

                stats.totalTime += cardToProcess.estimatedTime;
                xPos = cardToProcess.x;
                yPos = cardToProcess.y;

                stats.avgWaitTime = ((stats.avgWaitTime * stats.totalCalls) + w)/++stats.totalCalls;
            } catch (exception& e) {
                // Report the exception, but swallow it.
                cout << e.what() << endl;
            }
        } else {
            // There are no calls, wait in a bookstore for a call
            stats.timeWaiting++;
            stats.totalTime++;
        }
    }

    cout << "It took " << stats.totalTime << " minutes for the salesman to process ";
    cout << stats.totalCalls << " calls." << endl;
    cout << "The salesman spent " << stats.timeWaiting << " minutes in bookstores, ";
    cout << stats.timeOnRoad << " minutes on the road, and " << stats.timeInMeetings;
    cout << " minutes in meetings with clients." << endl;
    cout << "Clients spent an average of " << stats.avgWaitTime << " minutes waiting for the salesperson to see them." << endl;
    cout << "The maximum amount of time any client spent waiting was " << stats.maxWaitTime << " minutes." << endl;

    return 0;
}
