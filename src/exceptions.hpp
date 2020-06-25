#ifndef EXCEPTIONS_CLASS_HPP
#define EXCEPTIONS_CLASS_HPP

#include <iostream>

namespace MLJ {

class exception {
protected:
	std::string variant = "";
	std::string detail = "";
public:
	exception() {}
	exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
	virtual std::string what() const {
		return variant + " " + detail;
	}
};

class illegal_arg: public exception {
public:
    illegal_arg(const std::string &de = ""): exception() {
        variant = "illegal_argument"; detail = de;
    }
};

class bad_para: public exception {
public:
    bad_para(const std::string &de = ""): exception() {
        variant = "bad_parameters"; detail = de;
    }
};

class illegal_datentime: public exception {
public:
    illegal_datentime(const std::string &de = ""): exception() {
        variant = "illegal_datentime"; detail = de;
    }
};

class unknown_wrong: public exception {
public:
    unknown_wrong(const std::string &de = ""): exception() {
        variant = "unknown_wrong"; detail = de;
    }
};

}

#endif