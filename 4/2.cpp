#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <thread>
#include <mutex>
#include <algorithm>
#include <condition_variable>
#include <string>

// 4.2

using namespace std;

const int BUFFER_SIZE = 10;

struct Buffer {
    vector<char> data, data_ciphered;
    int count;
    mutex mtx;
    condition_variable cv_read, cv_encrypt, cv_write;
    bool done_reading;
    bool done_encrypting;
    bool done_writing;

    Buffer() : data(BUFFER_SIZE), data_ciphered(BUFFER_SIZE), count(0), done_reading(false), done_encrypting(false), done_writing(false) {}
};

void reader(const string &input_file, Buffer &buffer) {
    ifstream infile(input_file);
    char ch;
    while (infile.get(ch)) {
        unique_lock<mutex> lock(buffer.mtx);
        buffer.data[buffer.count++] = ch;
        if (buffer.count == BUFFER_SIZE) {
            buffer.cv_encrypt.notify_all();
            while (buffer.count == BUFFER_SIZE) buffer.cv_read.wait(lock);
        }
    }
    infile.close();
    buffer.done_reading = true;
    buffer.cv_encrypt.notify_all();
}

char encrypt_char(char ch) {
    if (isalpha(ch)) {
        if (islower(ch)) return ch == 'z' ? 'A' : toupper(ch + 1);
        if (isupper(ch)) return ch == 'Z' ? 'a' : tolower(ch + 1);
    }
    return ch;
}

void encryptor(Buffer &buffer) {
    while (true) {
        unique_lock<mutex> lock(buffer.mtx);
        while (buffer.count != BUFFER_SIZE && !buffer.done_reading || buffer.done_encrypting) buffer.cv_encrypt.wait(lock);
        for (int i = 0; i < buffer.count; i++) {
            buffer.data[i] = encrypt_char(buffer.data[i]);
        }
        buffer.done_encrypting = true;
        buffer.cv_write.notify_all();
        if (buffer.done_reading) break;
    }
}

void writer(const string &output_file, Buffer &buffer) {
    ofstream outfile(output_file);
    while (true) {
        unique_lock<mutex> lock(buffer.mtx);
        while (!buffer.done_encrypting) buffer.cv_write.wait(lock);
        for (int i = 0; i < buffer.count; i++) {
            outfile.put(buffer.data[i]);
        }
        buffer.count = 0;
        buffer.done_encrypting = false;
        if (buffer.done_reading && buffer.count == 0) break;
        buffer.cv_read.notify_all();
    }
    outfile.close();
}

void decryptor(const string &input_file, const string &output_file) {
    ifstream infile(input_file);
    ofstream outfile(output_file);
    char ch;
    char outputCh;
    while (infile.get(ch)) {
        if (isalpha(ch)) {
            if (islower(ch)) {
                outputCh = ch == 'a' ? 'Z' : toupper(ch - 1);
            } else if (isupper(ch)) {
                outputCh = ch == 'A' ? 'z' : tolower(ch - 1);
            }
            outfile.put(outputCh);
        } else {
            outfile.put(ch);
        }
    }
    infile.close();
    outfile.close();
}

int main() {
    string input_file = "input.txt";
    string encrypted_file = "encrypted.txt";
    string decrypted_file = "decrypted.txt";
    Buffer buffer;

    thread t1(reader, input_file, ref(buffer));
    thread t2(encryptor, ref(buffer));
    thread t3(writer, encrypted_file, ref(buffer));
    t1.join();
    t2.join();
    t3.join();
    decryptor(encrypted_file, decrypted_file);

    cout << "Complete!" << endl;
    return 0;
}