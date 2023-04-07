#pragma once

#include <string>

struct db_pool_config
{
    std::string host;
    std::string user;
    std::string password;
    std::string database;
    int port;
    int max_connections;
    int min_connections;
    int max_idle_time;
};
