#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

#include "config.h"

#define CFG_NETWORK_HOSTNAME               0x10
#define CFG_NETWORK_PASSWORD               0x11
#define CFG_NETWORK_ULTIMATE_IDENT_SERVER  0x21
#define CFG_NETWORK_ULTIMATE_DMA_SERVER    0x22
#define CFG_NETWORK_TELNET_SERVER          0x23
#define CFG_NETWORK_FTP_SERVER             0x24
#define CFG_NETWORK_HTTP_SERVER            0x25


class NetworkConfig : ConfigurableObject {
public:
    NetworkConfig();
    ~NetworkConfig();

    ConfigStore *cfg;
};

extern NetworkConfig networkConfig;

#endif /* NETWORK_CONFIG_H */
