#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

using namespace std;

struct Contact {
    string name;
    string number;
};

struct Node {
    Node* links[26];
    bool flag = false;
    vector<Contact> contacts;
    
    bool containskey(char ch) {
        return (links[ch - 'a'] != nullptr);
    }
    
    Node* get(char ch) {
        return links[ch - 'a'];
    }
    
    void put(char ch, Node* node) {
        links[ch - 'a'] = node;
    }
    
    void markend() {
        flag = true;
    }
    
    bool isend() {
        return flag;
    }
};

class Trie {
private:
    Node* root;
    
    void dfs(Node* node, string prefix, vector<Contact>& result) {
        if (node->isend()) {
            result.insert(result.end(), node->contacts.begin(), node->contacts.end());
        }
        for (char ch = 'a'; ch <= 'z'; ++ch) {
            if (node->containskey(ch)) {
                dfs(node->get(ch), prefix + ch, result);
            }
        }
    }     
public:
    Trie() {
        root = new Node();
    }
    
    void insert(const Contact& contact) {
        Node* node = root;
        for (char ch : contact.name) {
            if (!node->containskey(tolower(ch))) {
                node->put(tolower(ch), new Node());
            }
            node = node->get(tolower(ch));
        }
        node->markend();
        node->contacts.push_back(contact);
    }
    
    vector<Contact> getSuggestions(string prefix) {
        Node* node = root;
        vector<Contact> result;
        for (char& ch : prefix) {
            ch = tolower(ch);
            if (!node->containskey(ch)) {
                    return {};
                }
                node = node->get(ch);
        }
        
        dfs(node, prefix, result);
        return result;
    }
};

void saveContactsToFile(const vector<Contact>& contacts, const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto& contact : contacts) {
            file << contact.name << "," << contact.number << "\n";
        }
        file.close();
        if (file.fail()) {
            throw runtime_error("Failed to write to file: " + filename);
        }
        cout << "Contacts saved to " << filename << endl;
    } else {
        throw runtime_error("Unable to open file for writing: " + filename);
    }
}

vector<Contact> loadContactsFromFile(const string& filename) {
    vector<Contact> contacts;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            size_t comma = line.find(',');
            if (comma != string::npos) {
                Contact contact;
                contact.name = line.substr(0, comma);
                contact.number = line.substr(comma + 1);
                contacts.push_back(contact);
            }
        }
        file.close();
        cout << "Contacts loaded from " << filename << endl;
    } else {
        cout << "No existing contacts file found. A new one will be created." << endl;
    }
    return contacts;
}

int main() {
    Trie trie;
    vector<Contact> contacts;
    const string filename = "contacts.txt";
    
    try {
        contacts = loadContactsFromFile(filename);
        for (const auto& contact : contacts) {
            trie.insert(contact);
        }
    } catch (const exception& e) {
        cerr << "Error loading contacts: " << e.what() << endl;
    }
    
    while (true) {
        cout << "\n1. Add contact\n2. Search contacts\n3. Exit\nEnter your choice: ";
        int choice;
        cin >> choice;
        cin.ignore();  // Clear newline from input buffer
        
        if (choice == 1) {
            Contact newContact;
            cout << "Enter name: ";
            getline(cin, newContact.name);
            cout << "Enter number: ";
            getline(cin, newContact.number);
            contacts.push_back(newContact);
            trie.insert(newContact);
            try {
                saveContactsToFile(contacts, filename);
            } catch (const exception& e) {
                cerr << "Error saving contact: " << e.what() << endl;
            }
        } else if (choice == 2) {
            string searchQuery;
            cout << "Enter name to search (type character by character): ";
            char ch;
            while (cin.get(ch) && ch != '\n') {
                searchQuery += ch;
                vector<Contact> suggestions = trie.getSuggestions(searchQuery);
                cout << "\nSuggestions for '" << searchQuery << "':\n";
                if (suggestions.empty()) {
                    cout << "No matches found.\n";
                } else {
                    for (const auto& contact : suggestions) {
                        cout << contact.name << " - " << contact.number << "\n";
                    }
                }
            }
        } else if (choice == 3) {
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
    
    return 0;
}