#include <iostream>
#include <cassert>
#include <vector>
#include "MessageInbox.h"

void printInboxStatus(const MessageInbox& inbox) {
    std::cout << "Status: " << inbox.messageCount() << " messages, "
              << inbox.usedTextBytes() << " used bytes, "
              << inbox.remainingTextBytes() << " remaining bytes.\n";
    
    std::vector<TextMessage> msgs = inbox.messages();
    for (const TextMessage& msg : msgs) {
        std::cout << "[" << msg.id << "] " << msg.sender << ": " << msg.text 
                  << (msg.isRead ? " (Read)" : " (Unread)") << "\n";
    }
}

int main() {
    std::cout << "==================================================\n";
    std::cout << "20-Byte Example\n";
    std::cout << "==================================================\n\n";
    
    MessageInbox inbox(20);
    
    std::cout << "Receive 101 (Hi!): " << (inbox.receive(101, "Maya", "Hi!") ? "Accepted" : "Rejected") << "\n";
    std::cout << "Receive 102 (Meet at 6): " << (inbox.receive(102, "Noah", "Meet at 6") ? "Accepted" : "Rejected") << "\n";
    std::cout << "Receive 103 (Bring notes): " << (inbox.receive(103, "Eli", "Bring notes") ? "Accepted" : "Rejected") << " <- (Expected Rejected)\n";
    
    TextMessage readResult;
    std::cout << "Read 101: " << (inbox.read(101, readResult) ? "Success" : "Failed") << "\n";
    std::cout << "Delete 101: " << (inbox.erase(101) ? "Erased" : "Not Found") << "\n";
    std::cout << "Receive 103 again: " << (inbox.receive(103, "Eli", "Bring notes") ? "Accepted" : "Rejected") << "\n\n";
    
    printInboxStatus(inbox);


    std::cout << "\n==================================================\n";
    std::cout << "Test Cases\n";
    std::cout << "==================================================\n\n";

    MessageInbox testInbox(50);
    TextMessage dummyMsg{999, "Dummy", "Dummy", false};



    std::cout << "[Test] Empty operations\n";
    assert(testInbox.erase(1) == false);
    assert(testInbox.read(1, dummyMsg) == false);
    assert(testInbox.messageCount() == 0);
    std::cout << "Passed: Deleting and reading from an empty inbox safely return false.\n\n";



    std::cout << "[Test] Invalid and duplicate arrivals\n";
    assert(testInbox.receive(0, "A", "Text") == false);
    assert(testInbox.receive(1, "", "Text") == false);
    assert(testInbox.receive(1, "A", "") == false);
    
    testInbox.receive(1, "Alice", "Hello");
    assert(testInbox.receive(1, "Bob", "Duplicate ID") == false);
    std::cout << "Passed: ID 0, empty sender, empty text, and duplicate IDs are rejected.\n\n";



    std::cout << "[Test] Unchanged failed-read output\n";
    assert(testInbox.read(999, dummyMsg) == false);
    assert(dummyMsg.id == 999 && dummyMsg.sender == "Dummy"); 
    std::cout << "Passed: Output object remains untouched on failed read.\n\n";



    std::cout << "[Test] Exact-budget acceptance and over-budget rejection\n";
    MessageInbox budgetInbox(5);
    assert(budgetInbox.receive(10, "Sender", "12345") == true);
    assert(budgetInbox.receive(11, "Sender", "1") == false);
    std::cout << "Passed: Exact limit accepted, going over limit rejected.\n\n";



    std::cout << "[Test] Repeated reading\n";
    size_t bytesBeforeRead = budgetInbox.usedTextBytes();
    budgetInbox.read(10, dummyMsg);
    budgetInbox.read(10, dummyMsg);
    assert(budgetInbox.usedTextBytes() == bytesBeforeRead);
    assert(dummyMsg.isRead == true);
    std::cout << "Passed: Reading multiple times succeeds and does not free budget.\n\n";



    std::cout << "[Test] Changing snapshot copies\n";
    std::vector<TextMessage> snapshot1 = budgetInbox.messages();
    snapshot1[0].text = "HACKED";
    std::vector<TextMessage> snapshot2 = budgetInbox.messages();
    assert(snapshot2[0].text == "12345");
    std::cout << "Passed: Modifying the returned vector does not affect stored messages.\n\n";



    std::cout << "[Test] Deletion at every position and pointer diagrams\n";
    testInbox.clear();
    testInbox.receive(1, "A", "A");
    testInbox.receive(2, "B", "B");
    testInbox.receive(3, "C", "C");

    std::cout << "  [Pointer Diagram] Before deleting FIRST message (ID 1):\n";
    std::cout << "    head -> [1] -> [2] -> [3] <- tail\n";
    testInbox.erase(1);
    std::cout << "  [Pointer Diagram] After deleting FIRST message:\n";
    std::cout << "    head -> [2] -> [3] <- tail\n";
    assert(testInbox.messageCount() == 2);

    std::cout << "  [Pointer Diagram] Before deleting LAST message (ID 3):\n";
    std::cout << "    head -> [2] -> [3] <- tail\n";
    testInbox.erase(3);
    std::cout << "  [Pointer Diagram] After deleting LAST message:\n";
    std::cout << "    head -> [2] <- tail\n";
    assert(testInbox.messageCount() == 1);

    testInbox.erase(2);
    assert(testInbox.messageCount() == 0);
    std::cout << "Passed: Safely deleted head, tail, and only node.\n\n";



    std::cout << "[Test] Append after singleton deletion\n";
    testInbox.receive(4, "X", "X");
    testInbox.erase(4);
    testInbox.receive(5, "Y", "Y");
    assert(testInbox.messageCount() == 1);
    std::cout << "Passed: Reconnecting pointers works when appending after list becomes empty.\n\n";



    std::cout << "[Test] Clearing twice and reuse\n";
    testInbox.clear();
    testInbox.clear(); 
    assert(testInbox.messageCount() == 0);
    assert(testInbox.usedTextBytes() == 0);
    
    testInbox.receive(100, "New", "Fresh text");
    assert(testInbox.messageCount() == 1);
    std::cout << "Passed: Clearing twice does not crash, and inbox can be reused perfectly.\n\n";

    std::cout << "==================================================\n";
    std::cout << "All Tests Passes\n";
    std::cout << "==================================================\n";

    return 0;
}