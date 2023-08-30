#ifndef STORAGEINTERFACE_H_
#define STORAGEINTERFACE_H_

#include "utils/Storage.h"

class StorageInterface {
public:
    StorageInterface() = default;
    virtual ~StorageInterface() = default;
    virtual std::shared_ptr<Storage> getStorage() = 0;
};

#endif /* STORAGEINTERFACE_H_ */
