#include "client.h"
#include "server.h"

Client::Client(std::string id, const Server& server) : id(id), server(&server)
{
    crypto::generate_key(public_key, private_key);
}

std::string Client::get_id() const
{
    return id;
}

std::string Client::get_publickey() const
{
    return public_key;
}

double Client::get_wallet() const
{
    return server->get_wallet(id);
}

std::string Client::sign(std::string txt) const
{
    return crypto::signMessage(private_key, txt);
}

bool Client::transfer_money(std::string receiver, double value) const
{
    auto trx = id + "-" + receiver + "-" + std::to_string(value);
    auto sig = this->sign(trx);
    return server->add_pending_trx(trx, sig);
}

size_t Client::generate_nonce()
{
    // Seed the random number generator with the current time
    std::srand(std::time(nullptr));

    // Generate a random number between 0 and 9999
    size_t nonce = std::rand() % 10000;

    return nonce;
}

