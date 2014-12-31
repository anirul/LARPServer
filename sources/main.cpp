#include <crow.h>
#include <json.h>
#include <mustache.h>

#include <boost/program_options.hpp>

#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <thread>
#include <random>
#include <list>
#include <chrono>

#include "lite_db.hpp"

using namespace boost::program_options;
using std::chrono::system_clock;

struct transaction {
	std::string from;
	std::string to;
	int money;
};

db_key_value name_pass_db("name_pass.db", "name_pass");
db_key_value name_money_db("name_money.db", "name_money");
db_key_value name_desc_db("name_desc.db", "name_desc");
std::map<std::string, int> name_id_map;
std::map<std::string, std::string> name_token_map;
std::map<std::string, int> name_seed_map;
db_key_value transaction_db("debcred.db", "debcred");

std::mutex money_mutex;
std::mutex transaction_mutex;

crow::json::wvalue json_from_transaction(const transaction& t) {
	crow::json::wvalue jv;
	jv["from"] = t.from;
	jv["to"] = t.to;
	jv["money"] = t.money;
	return jv;
}

std::string json_string_transaction(const transaction& t) {
	std::stringstream ss;
	ss << "{\"from\":\"" << t.from << "\",";
	ss << "\"to\":\"" << t.to << "\",";
	ss << "\"money\":" << t.money << "}";
	return ss.str();
}

std::string file_to_string(const std::string& filename) {
	std::ifstream ifs(filename.c_str());
	if (!ifs.is_open())
		throw std::runtime_error("could not open file : " + filename);
	return std::string(
        (std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>());
}

std::string token_from_header(const crow::ci_map& header) {
	std::stringstream ss;
	auto it = header.find("User-Agent");
	ss << it->second;
	return ss.str();
}

void fill_user_pass(const crow::json::rvalue& val) {
	for (int i = 0; i < val["users"].size(); ++i) {
		std::string key = val["users"][i]["name"].s();
		std::string value = val["users"][i]["pass"].s();
		name_pass_db.update(key, value);
	}
}

void fill_user_money(const crow::json::rvalue& val) {
	for (int i = 0; i < val["users"].size(); ++i) {
		std::string key = val["users"][i]["name"].s();
		std::string value = std::to_string(val["users"][i]["money"].d());
		if (name_money_db.find(key) == std::string(""))
			name_money_db.insert(key, value);
	}
}

void fill_user_desc(const crow::json::rvalue& val) {
	for (int i = 0; i < val["users"].size(); ++i) {
		std::string key = val["users"][i]["name"].s();
		std::string value = val["users"][i]["desc"].s();
		name_desc_db.update(key, value);
	}
}

bool authenticate(
				  const std::string& user,
				  const std::string& token,
				  int seed,
				  bool verbose)
{
	auto seed_it = name_seed_map.find(user);
	auto token_it = name_token_map.find(user);
	if (seed_it == name_seed_map.end() || token_it == name_token_map.end()) {
		CROW_LOG_DEBUG << "missing user seed or token";
		return false;
	}
	if (seed_it->second != seed) {
		CROW_LOG_DEBUG
		<< "seed missmatch : ["
		<< seed << " != "
		<< seed_it->second << "]";
		return false;
	}
	if (token_it->second != token) {
		CROW_LOG_DEBUG << "token missmatch";
		return false;
	}
	return true;
}

int main(int ac, char** av)
{
	bool verbose = false;
	bool multithreaded = false;
	unsigned short port = 8080;
	std::string path = "./";
	std::string data_file = "data.JSON";
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(1, 65535);
	auto seed_value = std::bind(distribution, generator);
	try {
		options_description desc("Allowed options");
		desc.add_options()
		("help,h", "produce help message")
		("input-path,i", value<std::string>(), "input path for files")
		("data-file,d", value<std::string>(), "JSON data file")
		("port,p", value<unsigned short>(), "listening port")
		("verbose,v", "more verbose (DEBUG logs)")
		("multithreaded,m", "more threads!")
		;
		variables_map vm;
		store(command_line_parser(ac, av).options(desc).run(), vm);
		if (vm.count("help")) {
			std::cout << desc << std::endl;
			return 1;
		}
		if (vm.count("input-path"))
			path = vm["input-path"].as<std::string>();
		if (vm.count("data-file"))
			data_file = vm["data-file"].as<std::string>();
		if (vm.count("port"))
			port = vm["port"].as<unsigned short>();
		if (vm.count("verbose"))
			verbose = true;
		if (vm.count("multithreaded"))
			multithreaded = true;
		data_file = path + data_file;
		crow::SimpleApp app;
		crow::mustache::set_base(path);
		std::string data_str = file_to_string(data_file);
		crow::json::rvalue val = crow::json::load(data_str);
		fill_user_pass(val);
		fill_user_money(val);
		fill_user_desc(val);
		
		CROW_ROUTE(app, "/")
		([]{
			crow::mustache::context ctx;
			return crow::mustache::load("index.html").render();
		});
		
		CROW_ROUTE(app, "/index.html")
		([]{
			crow::mustache::context ctx;
			return crow::mustache::load("index.html").render();
		});
		
		CROW_ROUTE(app, "/do_stuff.js")
		([]{
			crow::mustache::context ctx;
			return crow::mustache::load("do_stuff.js").render();
		});
		
		CROW_ROUTE(app, "/LARPstyle.css")
		([]{
			crow::mustache::context ctx;
			return crow::mustache::load("LARPstyle.css").render();
		});
		
		CROW_ROUTE(app, "/api/images/photo.jpg")
		([&](const crow::request& req){
			std::string user = "pc";
			if(req.url_params.get("user"))
				user = req.url_params.get("user");
			try {
				std::stringstream ss;
				ss << path;
				ss << "id/";
				ss << user;
				ss << ".jpg";
				return file_to_string(ss.str());
			} catch (std::exception& ex) {
				std::stringstream ss;
				ss << path;
				ss << "id/cp.jpg";
				return file_to_string(ss.str());
			}
		});
		
		CROW_ROUTE(app, "/api/list/")
		([]{
			crow::json::wvalue x;
			int i = 0;
			std::map<std::string, std::string> name_list;
			name_pass_db.list(name_list);
			for (auto it : name_list) {
				x["users"][i]["name"] = it.first;
				std::string status = "offline";
				if (name_token_map.find(it.first) != name_token_map.end())
					status = "online";
				x["users"][i]["status"] = status;
				i++;
			}
			return x;
		});
		
		CROW_ROUTE(app, "/api/history/")
		([&](const crow::request& req){
			std::string user_name = "";
			int seed = 0;
			std::string token = token_from_header(req.headers);
			if (req.url_params.get("user")) {
				user_name = req.url_params.get("user");
			} else {
				CROW_LOG_DEBUG << "(0) Hacking detected!";
				return crow::response(400, "HACKER!!!!");
			}
			if (req.url_params.get("seed")) {
				seed = atoi(req.url_params.get("seed"));
			} else {
				CROW_LOG_DEBUG << "(1) Hacking detected!";
				return crow::response(400, "HACKER!!!!");
			}
			if (!authenticate(user_name, token, seed, verbose)) {
				CROW_LOG_DEBUG << "failed authentication?";
				return crow::response(500, "HACKER!!!!");
			}
			crow::json::wvalue jwv;
			int i = 0;
			std::map<std::string, std::string> transaction_map;
			transaction_mutex.lock();
			transaction_db.list(transaction_map);
			transaction_mutex.unlock();
			for (auto it : transaction_map) {
				crow::json::rvalue jrv = crow::json::load(it.second);
				if ((user_name != jrv["from"].s()) &&
					(user_name != jrv["to"].s()))
					continue;
				jwv["history"][i]["at"] = it.first;
				jwv["history"][i]["from"] = jrv["from"].s();
				jwv["history"][i]["to"] = jrv["to"].s();
				jwv["history"][i]["money"] = jrv["money"].d();
				i++;
			}
			money_mutex.lock();
			jwv["balance"] = name_money_db.find(user_name);
			money_mutex.unlock();
			return crow::response(jwv);
		});
		
		CROW_ROUTE(app, "/api/login/")
		([&](const crow::request& req){
			std::string user_name = "";
			std::string user_pass = "";
			if (req.url_params.get("user")) {
				user_name = req.url_params.get("user");
			} else {
				CROW_LOG_DEBUG << "(0) Hacking detected!";
				return crow::response(400, "HACKER!!!!");
			}
			if (req.url_params.get("pass")) {
				user_pass = req.url_params.get("pass");
			} else {
				CROW_LOG_DEBUG << "(1) Hacking detected!";
				return crow::response(400, "HACKER!!!!");
			}
			std::string token = token_from_header(req.headers);
			auto pass = name_pass_db.find(user_name);
			if (pass == std::string("")) {
				CROW_LOG_DEBUG << "(2) Hacking detected!";
				return crow::response(400, "HACKER!!!!");
			}
			
			if (pass != user_pass)
				return crow::response(500, "login failed!");
			// register new token
			int seed = seed_value();
			{
				auto it = name_seed_map.find(user_name);
				if (it != name_seed_map.end())
					name_seed_map.erase(it);
			}
			name_seed_map.insert(std::make_pair(user_name, seed));
			{
				auto it = name_token_map.find(user_name);
				if (it != name_token_map.end())
					name_token_map.erase(it);
			}
			name_token_map.insert(std::make_pair(user_name, token));
			auto money = name_money_db.find(user_name);
			crow::json::wvalue jv;
			jv["money"] = atoi(money.c_str());
			jv["seed"] = seed;
			jv["desc"] = name_desc_db.find(user_name);
			return crow::response(jv);
		});
		
		CROW_ROUTE(app, "/api/send/")
		([&](const crow::request& req){
			std::string from_name = "";
			std::string to_name = "";
			int value = 0;
			int seed = 0;
			if (req.url_params.get("from")) {
				from_name = req.url_params.get("from");
			} else {
				CROW_LOG_DEBUG << "(0) Hacking detected!";
				return crow::response(400, "HACKER!!!!");
			}
			if (req.url_params.get("to")) {
				to_name = req.url_params.get("to");
			} else {
				CROW_LOG_DEBUG << "(1) Hacking detected!";
				return crow::response(400, "HACKER!!!!");
			}
			if (req.url_params.get("value")) {
				value = atoi(req.url_params.get("value"));
			} else {
				CROW_LOG_DEBUG << "(2) Hacking detected!";
				return crow::response(400, "HACKER!!!!");
			}
			if (req.url_params.get("seed")) {
				seed = atoi(req.url_params.get("seed"));
			} else {
				CROW_LOG_DEBUG << "(3) Hacking detected!";
				return crow::response(400, "HACKER!!!!");
			}
			// check user has right (own the account)
			std::string token = token_from_header(req.headers);
			money_mutex.lock();
			int from_money = atoi(name_money_db.find(from_name).c_str());
			int to_money = atoi(name_money_db.find(to_name).c_str());
			if (!authenticate(from_name, token, seed, verbose)) {
				CROW_LOG_DEBUG << "failed authentication?";
				return crow::response(500, "HACKER!!!!");
			}
			if (value < 0) {
				CROW_LOG_DEBUG << "(6) Hacking detected!";
				return crow::response(400, "HACKER!!!!");
			}
			if (from_name == to_name)
				return crow::response(400, "'from' and 'to' are the same");
			if (value > from_money)
				return crow::response(400, "'from' doesn't have enough money");
			from_money -= value;
			to_money += value;
			name_money_db.update(from_name, std::to_string(from_money));
			name_money_db.update(to_name, std::to_string(to_money));
			money_mutex.unlock();
			transaction t;
			t.from = from_name;
			t.to = to_name;
			t.money = value;
			uint64_t milliseconds_since_epoch =
			std::chrono::system_clock::now().time_since_epoch() /
			std::chrono::milliseconds(1);
			transaction_mutex.lock();
			transaction_db.insert(
								  std::to_string(milliseconds_since_epoch),
								  json_string_transaction(t));
			transaction_mutex.unlock();
			return crow::response(json_from_transaction(t));
		});
		if (verbose)
			crow::logger::setLogLevel(crow::LogLevel::DEBUG);
		if (multithreaded)
			app.port(port).multithreaded().run();
		else
			app.port(port).run();
	} catch (std::exception& ex) {
		std::cerr << "exception (std) : " << ex.what() << std::endl;
		return -1;
	}
	return 0;
}
