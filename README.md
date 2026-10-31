# CIS-277 Assignment 3: Text Messages and Dynamic Memory

## Student
Nathan Park

## Design
The MessageInbox class uses a singly linked list to track messages. Nodes are defined as a private struct containing a TextMessage object and a Node pointer. The nodes are dynamically allocated with new in the receive() function and released with delete in the erase() or clear() functions. The class tracks both the head and tail pointers. Counters messageCount_ and usedTextBytes_ are manually incremented/decremented to ensure O(1) retrieval without traversing the list.

**Changes during testing:** I initially forgot that after unlinking the head node, if the list becomes completely empty, the tail pointer must also be set to nullptr. I added a conditional check inside erase() to address this behavior.

## How to Compile
g++ -std=c++23 -Wall -Wextra -pedantic main.cpp MessageInbox.cpp -o message_inbox

## How to Run
./message_inbox

## Tests and Results
I wrote comprehensive tests in main.cpp using standard assert() statements to automatically verify expected versus actual results. 

**Empty operations:**
Attempting to read or erase from an empty inbox successfully returned false without crashing. 

**Duplicate and invalid arrivals:**
receive() correctly rejected ID 0, empty strings, and duplicate IDs (returning false and making no state changes).

**Unchanged failed-read output:**
When querying a missing ID, the passed-in TextMessage object remained completely unchanged.

**Budget Boundaries:**
Confirmed that a 5-byte inbox accepted exactly a 5-byte message, but rejected a message that would push it to 6 bytes.

**Repeated reading:**
Verified that reading the same message multiple times leaves the budget exactly the same.

**Changing snapshot copies:**
Modifying the vector returned by messages() did not change the actual text stored inside the MessageInbox heap nodes.

**Deletion at all positions:**
Successfully deleted the head, the tail, and the only node, updating pointers properly in all cases.

**Append after singleton deletion:**
Confirmed that if the inbox drops to 0 messages, a subsequent receive() successfully rebuilds the head and tail pointers.

**Clearing:**
Calling clear() twice in a row did not crash. The inbox was successfully reused to store new messages afterward.

## Analysis Questions
**Which pointers change when the first, last, or only message is deleted? Include diagrams.**
When the first message is deleted, only the head pointer is changed to head->next
Before: head -> [1] -> [2] <- tail | 
After:  head -> [2] <- tail

When the last message is deleted, only the tail pointer is changed to the second to last node
Before: head -> [1] -> [2] <- tail | 
After:  head -> [1] <- tail

WHen the only message is deleted, both head and tail are set to null ptr
Before: head -> [1] <- tail | 
After:  head -> nullptr, tail -> nullptr

**Why does reading leave storage budget unchanged while deleting makes room?**
The budget counts how many text bytes are currently used by the text in the nodes of the linked list. The read() function only changes the boolean isRead inside the node and copies the TextMessage struct into result. The node is not deleted, nor is a new node being added, so the budget stays the same. When deleting a node, the text that was inside that node is no longer counted towards the budget, meaning the budget changes.

**What does the byte counter measure, and which actual memory costs are excluded?**
The byte counter only measures text.size(). It does not count sender names, node fields, string capacity, allocator overhead, or the null terminator.

**Why does receive take O(n) search work even though appending takes O(1) link changes? Account for string copying too.**
The receive function takes O(n) time because one of our stipulations is that we cannot recieve a message whose ID already exists in the linked list. Therefore, we must iterate through the linked list to check if a duplicate ID already exists. Appending only takes O(1) time because we have access to the tail pointer at all times, meaning it takes a constant amount of link changes always. 

**What goes wrong if nodes remain allocated after destruction, or two inbox objects share ownership of the same chain?**
If a node remains allocated after being destroyed, then it will leak memory on the heap until the program ends. 
If two inbox objects share ownership of the same chain, then calling a function like erase on one of the inbox objects will also erase the same node on the other inbox because they point to the same memory address. However, the other inbox will have no idea that it was destroyed, meaning that there will be dangling pointers.

## AI Use
AI was used to understand that Node* current inside of my function does not need to be created with new or released with delete because it is inherently stack allocated, unlike the linked list. After the function call, it is automatically destroyed. AI was also used to help make the test cases. 
The AI used was Gemini.