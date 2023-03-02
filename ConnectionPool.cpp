#include "ConnectionPool.h"


ConnectionPool* ConnectionPool::getConnection() {
    static ConnectionPool pool;
    return &pool;
}
