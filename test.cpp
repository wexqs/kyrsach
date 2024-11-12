#include "Includer.h"
#include <limits>
#include <chrono>
#include <thread>
#include <UnitTest++/UnitTest++.h>
#include <boost/numeric/conversion/cast.hpp>
#define max_pos 32767 // uint16_t max value for testing overflow logic
#define min_pos 0     // assuming uint16_t min value for logic
#define _UNITTEST_ 1

// Connector_to_base is now just Connector

SUITE(Connector_test) {
    TEST(newl_in_path) {
        Connector Con;
        std::string path = "test_files/auth\n";
        CHECK_THROW(Con.connect(path), crit_err);
    }
    TEST(incorrect_path) {
        Connector Con;
        std::string path = "%$#(*";
        CHECK_THROW(Con.connect(path), crit_err);
    }
    TEST(void_path) {
        Connector Con;
        std::string path = "";
        CHECK_THROW(Con.connect(path), crit_err);
    }
    TEST(wrong_path) {
        Connector Con;
        std::string path = "/gendalf/auth.txt";
        CHECK_THROW(Con.connect(path), crit_err);
    }
    TEST(right_path) {
        Connector Con;
        std::string path = "test_files/auth.txt";
        CHECK_EQUAL(0, Con.connect(path));
    }
    TEST(check_reading) {
        Connector Con;
        std::string check_pass = "P@ssW0rd";
        std::string check_login = "user";
        std::string path = "test_files/auth.txt";
        Con.connect(path);
        auto data = Con.get_data();
        if (data[check_login] != check_pass || data.find(check_login) == data.end()) {
            CHECK(false);
        } else {
            CHECK(true);
        }
    }
}

// Calculator uses uint16_t multiplication

SUITE(Calculator_test) {
    TEST(positiv_overflow) {
        std::vector<uint16_t> v = {max_pos, 2}; // Expect overflow handling
        Calculator c(v);
        CHECK_EQUAL(max_pos, c.send_res());
    }
    TEST(negative_number_1) {
        std::vector<uint16_t> v = {10, 5};
        Calculator c(v);
        CHECK_EQUAL(50, c.send_res());
    }
    TEST(negative_number_2) {
        std::vector<uint16_t> v = {10, 10};
        Calculator c(v);
        CHECK_EQUAL(100, c.send_res());
    }
}

// Logger tests

SUITE(Logger_test) {
    TEST(wrong_path) {
        Logger l;
        std::string path = "false/way/to/file.txt";
        CHECK_THROW(l.set_path(path), crit_err);
    }
    TEST(void_path) {
        Logger l;
        std::string path = "";
        CHECK_THROW(l.set_path(path), std::invalid_argument);
    }
    TEST(correct_path) {
        Logger l;
        std::string path = "test_files/free.txt";
        CHECK_EQUAL(0, l.set_path(path));
    }
    TEST(newl_in_path_log) {
        Logger l;
        std::string path = "test_files/baze\n";
        CHECK_THROW(l.set_path(path), std::invalid_argument);
    }
}

// Client_Communicate with updated salt and SHA256

SUITE(Client_Communicate) {
    TEST(salt_generator) {
        Communicate com;
        std::string salt_one = com.generate_salt();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::string salt_two = com.generate_salt();
        if (salt_one == salt_two) {
            CHECK(false);
        } else {
            CHECK(true);
        }
    }
    TEST(salt_len) {
        Communicate com;
        std::string salt = com.generate_salt();
        CHECK_EQUAL(salt.length(), 16); // Adjust according to salt generation logic
    }
    TEST(sha256_gen) {
        std::string hash_check = "6f4b6612125fb3a0daecd2799dfd6c9c299424fd920f9b308110a2c1fbd8f443"; // Example SHA256 hash
        Communicate com;
        std::string SALT = "3e74235568ba8f1e";
        std::string hash = com.sha256(SALT);
        CHECK_EQUAL(hash, hash_check);
    }
}

// Test runner
int main(int argc, char **argv) {
    return UnitTest::RunAllTests();
}
