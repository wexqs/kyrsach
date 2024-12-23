#include "Logger.h"
#include "Connector.h"
#include "Calculator.h"
#include "Communicate.h"
#include <UnitTest++/UnitTest++.h>
#include <limits>
#include <chrono>
#include <thread>
#include <boost/numeric/conversion/cast.hpp>

#define max_pos 32767
#define min_pos 0
#define _UNITTEST_ 1


SUITE(Connector_test) {
    TEST(newl_in_path) {
        Connector Con;
        std::string path = "test_files/auth\n";
        CHECK_THROW(Con.connect(path), crit_err);
    }
    TEST(incorrect_path) {
        Connector Con;
        std::string path = "1111";
        CHECK_THROW(Con.connect(path), crit_err);
    }
    TEST(void_path) {
        Connector Con;
        std::string path = "";
        CHECK_THROW(Con.connect(path), crit_err);
    }
    TEST(wrong_path) {
        Connector Con;
        std::string path = "/sudo/auth.txt";
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
        CHECK(data.find(check_login) != data.end() && data[check_login] == check_pass);
    }
}


SUITE(Calculator_test) {
    TEST(positiv_overflow) {
        std::vector<uint16_t> v = {32767, 2}; 
        Calculator c(v);
        CHECK_EQUAL(32767, c.send_res());
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
	TEST(single_value) {
        std::vector<uint16_t> v = {5}; 
        Calculator c(v);
        CHECK_EQUAL(5, c.send_res());
    }
	TEST(million_ones) {
        std::vector<uint16_t> v(1000000, 1);
        Calculator c(v);
        CHECK_EQUAL(1, c.send_res());
    }
}

// Logger tests

SUITE(Logger_test) {
    TEST(wrong_path) {
        Logger l;
        std::string path = "sv1cha/jojik/to/log.txt";
        CHECK_THROW(l.set_path(path), crit_err);
    }
    TEST(void_path) {
        Logger l;
        std::string path = "";
        CHECK_THROW(l.set_path(path), std::invalid_argument);
    }
    TEST(correct_path) {
        Logger l;
        std::string path = "test_files/log.txt";
        CHECK_EQUAL(0, l.set_path(path));
    }
    TEST(newl_in_path_log) {
        Logger l;
        std::string path = "test_files/baze\n";
        CHECK_THROW(l.set_path(path), std::invalid_argument);
    }
}


SUITE(Client_Communicate) {
    TEST(salt_generator) {
        Communicate com;
        std::string salt_one = com.generate_salt();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::string salt_two = com.generate_salt();
        CHECK(salt_one != salt_two);
    }
    TEST(salt_len) {
        Communicate com;
        std::string salt = com.generate_salt();
        CHECK_EQUAL(16, salt.length());
    }
    TEST(sha256_gen) {
        std::string hash_check = "4AD020C61365451DC2C8CCD88F5063ABBBFE796210A9D6090BCECBF434F078B4";
        Communicate com;
        std::string SALT = "2A2B91EEBB29DC1C";
        std::string hash = com.sha256(SALT);
        CHECK_EQUAL(hash_check, hash);
    }
	TEST(hash_salt_password) {
        Communicate com;
        std::string salt = "2A2B91EEBB29DC1C";
        std::string password = "P@ssW0rd";
        std::string expected_hash = "8DE7D7E8F5721869813573DD3FF6F0BB9F9B028A09A1F8F07FA25307EA3EF02F";
        std::string combined = salt + password;
        std::string hash = com.sha256(combined);
        CHECK_EQUAL(expected_hash, hash);
    }
}

int main(int argc, char **argv) {
    return UnitTest::RunAllTests();
}
