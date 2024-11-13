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
        std::string hash_check = "4B433CF519B97321B9341690085EF206439AF0C8A98F606B7C06C3B7F189ED40";
        Communicate com;
        std::string SALT = "F9E622969DCEDABE";
        std::string hash = com.sha256(SALT);
        CHECK_EQUAL(hash_check, hash);
    }
}

int main(int argc, char **argv) {
    return UnitTest::RunAllTests();
}
