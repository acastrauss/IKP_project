#include "pch.h"
#include <WS2tcpip.h>
#include <ThreadPool/ThreadingMethods.h>
#include <Common/CommonMethods.h>
#include <iostream>

void WaitCloseThreadHandles(std::deque<void*>* threadHandles)
{
    while (threadHandles->size() != 0)
    {
        auto it = std::find_if(
            threadHandles->begin(),
            threadHandles->end(),
            [&](HANDLE h) {
                if (
                    WaitForSingleObject(h, 1) == WAIT_OBJECT_0
                    ) {
                    SAFE_DELETE_HANDLE(h);
                    return true;
                }
                else {
                    return false;
                }
            }
        );

        if (it != threadHandles->end()) {
            threadHandles->erase(it);
        }
    }
}