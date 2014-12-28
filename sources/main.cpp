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

using namespace boost::program_options;

std::map<std::string, std::string> name_pass_map;
std::map<std::string, int> name_money_map;
std::map<std::string, std::string> name_token_map;
std::mutex money_mutex;

std::string file_to_string(const std::string& filename) {
    std::ifstream ifs(filename.c_str());
    if (!ifs.is_open())
        throw std::runtime_error("could not open JSON file : " + filename);
    return std::string(
        (std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>());
}

std::string token_from_header(const crow::ci_map& header) {
    std::stringstream ss;
    for (auto it : header)
        ss << it.first << ":" << it.second << "\n";
    return ss.str();
}

void fill_user_pass(const crow::json::rvalue& val) {
    for (int i = 0; i < val["users"].size(); ++i)
        name_pass_map.insert(
            std::make_pair(
                val["users"][i]["name"].s(),
                val["users"][i]["pass"].s()));
}

void fill_user_money(const crow::json::rvalue& val) {
    for (int i = 0; i < val["users"].size(); ++i)
        name_money_map.insert(
            std::make_pair(
                val["users"][i]["name"].s(),
                (int)val["users"][i]["money"].d()));
}

int main(int ac, char** av)
{
    std::string path = "./";
    std::string data_file = "data.JSON";
    try {
        options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("input-path,i", value<std::string>(), "input path for files")
            ("data-file,d", value<std::string>(), "JSON data file")
            ;
        variables_map vm;
        store(command_line_parser(ac, av).options(desc).run(), vm);
        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 1;
        }
        if (vm.count("input-path")) {
            path = vm["input-path"].as<std::string>();
        }
        if (vm.count("data-file")) {
            data_file = vm["data-file"].as<std::string>();
        }
        data_file = path + data_file;
        crow::SimpleApp app;
        crow::mustache::set_base("./");
        std::string data_str = file_to_string("./data.JSON");
        crow::json::rvalue val = crow::json::load(data_str);
        fill_user_pass(val);
        fill_user_money(val);

        CROW_ROUTE(app, "/")
        ([]{
            crow::mustache::context ctx;
            return crow::mustache::load("index.html").render();
        });

        CROW_ROUTE(app, "/api/list/")
        ([]{
            crow::json::wvalue x;
            int i = 0;
            for (auto it : name_money_map) {
                x["message"][i]["name"] = it.first;
                x["message"][i]["money"] = it.second;
                std::string status = "offline";
                if (name_token_map.find(it.first) != name_token_map.end())
                    status = "online";
                x["message"][i]["status"] = status;
                i++;
            }
            return x;
        });

        CROW_ROUTE(app, "/api/login/")
        ([](const crow::request& req){
            std::string user_name = "";
            std::string user_pass = "";
            if (req.url_params.get("user"))
                user_name = req.url_params.get("user");
            else
                return crow::response(400, "need a 'user' in the request!");
            if (req.url_params.get("pass"))
                user_pass = req.url_params.get("pass");
            else
                return crow::response(400, "need a 'pass' in the request!");
            std::string token = token_from_header(req.headers);
            auto name_pass_it = name_pass_map.find(user_name);
            if (name_pass_it == name_pass_map.end())
                return crow::response(400, "unknown 'user'!");
            if (name_pass_it->second != user_pass)
                return crow::response(400, "password mismatch!");
            // register new token
            name_token_map.insert(std::make_pair(user_name, token));
            return crow::response("login successfull!");
        });

        CROW_ROUTE(app, "/api/send/")
        ([](const crow::request& req){
            std::string from_name = "";
            std::string to_name = "";
            int value = 0;
            if (req.url_params.get("from"))
                from_name = req.url_params.get("from");
            else
                return crow::response(400, "need a 'from' in the request!");
            if (req.url_params.get("to"))
                to_name = req.url_params.get("to");
            else
                return crow::response(400, "need a 'to' in the request!");
            if (req.url_params.get("value"))
                value = atoi(req.url_params.get("value"));
            else
                return crow::response(400, "need a 'value' in the request!");
            // check user has right (own the account)
            std::string token = token_from_header(req.headers);
            auto token_it = name_token_map.find(from_name);
            auto from_it = name_money_map.find(from_name);
            auto to_it = name_money_map.find(to_name);
            if (token != token_it->second)
                return crow::response(500, "invalid credential!");
            if (from_it == name_money_map.end())
                return crow::response(400, "unknown 'from' user");
            if (to_it == name_money_map.end())
                return crow::response(400, "unknown 'to' user");
            if (value < 0)
                return crow::response(400, "'value' cannot be negative");
            if (from_it == to_it)
                return crow::response(400, "'from' and 'to' are the same");
            if (value > from_it->second)
                return crow::response(400, "'from' doesn't have enough money");
            money_mutex.lock();
            from_it->second -= value;
            to_it->second += value;
            money_mutex.unlock();
            std::stringstream ss;
            ss << "sending : " << value << " from " << from_name << " to " << to_name;
            return crow::response(ss.str());
        });

        app.port(8080).multithreaded().run();
    } catch (std::exception& ex) {
        std::cerr << "exception (std) : " << ex.what() << std::endl;
        return -1;
    }
    return 0;
}
