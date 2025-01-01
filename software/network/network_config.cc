#include <string.h>

#include "config.h"
#include "small_printf.h"
#include "product.h"
#include "network_config.h"

static char default_hostname[42];

// Network configuration settings
struct t_cfg_definition network_config[] = {
    { CFG_NETWORK_HOSTNAME,        CFG_TYPE_STRING,     "Host Name",                        "%s", NULL,       3, 31, (int)default_hostname},
    { CFG_NETWORK_PASSWORD,        CFG_TYPE_STRPASS,    "Network Password",                 "%s", NULL,       3, 31, (int)"" },
    { CFG_NETWORK_ULTIMATE_IDENT_SERVER, CFG_TYPE_ENUM, "Ultimate Ident Server",            "%s", en_dis,     0,  1, 1 },
    { CFG_NETWORK_ULTIMATE_DMA_SERVER, CFG_TYPE_ENUM,   "Ultimate DMA Server",              "%s", en_dis,     0,  1, 1 },
    { CFG_NETWORK_TELNET_SERVER,   CFG_TYPE_ENUM,       "Telnet Remote Menu Server",        "%s", en_dis,     0,  1, 1 },
    { CFG_NETWORK_FTP_SERVER,      CFG_TYPE_ENUM,       "FTP File Management Server",       "%s", en_dis,     0,  1, 1 },
    { CFG_NETWORK_HTTP_SERVER,     CFG_TYPE_ENUM,       "Web Remote Control Server",        "%s", en_dis,     0,  1, 1 },
    { CFG_TYPE_END,                CFG_TYPE_END,        "",                                 "",   NULL,       0,  0, 0 }
};


NetworkConfig :: NetworkConfig() {
    strcpy(default_hostname, getProductDefaultHostname(default_hostname, sizeof(default_hostname)));
    cfg = ConfigManager :: getConfigManager()->register_store(0x4E455400, "Network Settings", network_config, this);
}

NetworkConfig :: ~NetworkConfig() {
}

NetworkConfig networkConfig;
