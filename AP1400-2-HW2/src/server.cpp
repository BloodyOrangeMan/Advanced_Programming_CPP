#include "server.h"
#include "client.h"
#include "crypto.h"

#include <algorithm>
#include <openssl/rand.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

Server::Server() = default;
std::vector<std::string> pending_trxs;

std::shared_ptr<Client> Server::add_client(std::string id)
{
    std::string new_id = id;
    while (clients.count(get_client(new_id)) > 0)
    {
        // If a client with the same ID already exists, add a random 4-digit number to the end
        new_id = id + std::to_string(rand() % 9000 + 1000);
    }
    // Create a new client with the updated ID
    std::shared_ptr<Client> new_client = std::make_shared<Client>(new_id, *this);
    // Assign 5 coins to the new client
    double starting_balance = 5.0;
    clients[new_client] = starting_balance;
    return new_client;
}

std::shared_ptr<Client> Server::get_client(std::string id) const
{
    for (auto const &[client_ptr, _] : clients)
    {
        if (client_ptr->get_id() == id)
        {
            return client_ptr;
        }
    }
    return nullptr;
}

double Server::get_wallet(std::string id) const
{
    auto client = get_client(id);
    return clients.at(client);
}

bool Server::parse_trx(std::string trx, std::string &sender, std::string &receiver, double &value)
{
    std::vector<std::string> seglist;
    std::stringstream _trx(trx);
    std::string segment;

    while (std::getline(_trx, segment, '-'))
    {
        seglist.push_back(segment);
    }

    if (seglist.size() != 3)
    {
        throw std::runtime_error("The trx string is not standard!");
    }

    try
    {
        sender = seglist[0];
        receiver = seglist[1];
        value = std::stod(seglist[2]);
    }
    catch (const std::invalid_argument &e)
    {
        throw std::runtime_error("The trx string is not standard!");
    }
    return true;
}

bool Server::add_pending_trx(std::string trx, std::string signature) const
{
    std::string sender;
    std::string receiver;
    double value;

    if (!parse_trx(trx, sender, receiver, value))
        return false;

    auto sender_ptr = get_client(sender);
    auto receiver_ptr = get_client(receiver);

    if (!sender_ptr || !receiver_ptr)
        return false;

    if (!crypto::verifySignature(sender_ptr->get_publickey(), trx, signature))
        return false;

    if (get_wallet(sender) < value)
        return false;

    pending_trxs.push_back(trx);
    return true;
}

size_t Server::mine()
{
    std::map<std::shared_ptr<Client>, std::pair<std::string, double>> trx_map;
    std::string mempool;

    std::string sender;
    std::string receiver;
    double value;

    for (std::string trx : pending_trxs)
    {
        mempool += trx;
        parse_trx(trx, sender, receiver, value);
        auto sender_ptr = get_client(sender);
        trx_map[sender_ptr] = std::make_pair(receiver, value);
    }

    while (true)
    {
        size_t nonce;
        for (const auto &[sender_ptr, receiver_and_value] : trx_map)
        {
            nonce = sender_ptr->generate_nonce();
            auto _mempool = mempool + std::to_string(nonce);
            std::string hash = crypto::sha256(_mempool);
            if (hash.substr(0, 10).find("0") != std::string::npos)
            {
                clients[sender_ptr] += 6.25;
                for (auto &[_sender_ptr, receiver_and_value] : trx_map)
                {   
                    clients[_sender_ptr] -= receiver_and_value.second;
                    clients[get_client(receiver_and_value.first)] += receiver_and_value.second;
                }
                trx_map.clear();
                return nonce;
            }
        }
    }
}

void show_wallets(const Server &server)
{
    std::cout << std::string(20, '*') << std::endl;
    for (const auto &client : server.clients)
        std::cout << client.first->get_id() << " : " << client.second << std::endl;
    std::cout << std::string(20, '*') << std::endl;
}