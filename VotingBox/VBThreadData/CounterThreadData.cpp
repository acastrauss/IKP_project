#include "CounterThreadData.h"

CounterThreadData::CounterThreadData(SOCKET socket, Common::VotesToCount votesToCount)
: ConnectSocket(socket), VotesToCount(votesToCount)
{}

CounterThreadData::CounterThreadData(const CounterThreadData & ref)
: ConnectSocket(ref.ConnectSocket), VotesToCount(ref.VotesToCount) {}
