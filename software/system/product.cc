#include <string.h>

#include "flash.h"
#include "versions.h"
#include "product.h"
#include "small_printf.h"
#include "u64.h"

// Longest supported product_name string is 16. Update below functions and callers if this increases!

static char *product_name[] = {
    "Ultimate",  // Default if unknown product
    "Ultimate II",
    "Ultimate II+",
    "Ultimate II+L",
    "Ultimate 64",
    "Ultimate 64E",
    "Ultimate 64E2",
};

static char *product_hostname[] = {
    "Ultimate",  // Default if unknown product
    "Ultimate-II",
    "Ultimate-IIp",
    "Ultimate-IIpL",
    "Ultimate-64",
    "Ultimate-64E",
    "Ultimate-64E2",
};

const char *getBoardRevision(void)
{
    uint8_t rev = (U2PIO_BOARDREV >> 3);

    switch (rev) {
    case 0x10:
        return "U64 Prototype";
    case 0x11:
        return "U64 V1.1 (Null Series)";
    case 0x12:
        return "U64 V1.2 (Mass Prod)";
    case 0x13:
        return "U64 V1.3 (Elite)";
    case 0x14:
        return "U64 V1.4 (Std/Elite)";
    case 0x15:
        return "U64E V2.0 (Early Proto)";
    case 0x16:
        return "U64E V2.1 (Null Series)";
    case 0x17:
        return "U64E V2.2 (Mass Prod)";
    }
    return "Unknown";
}

bool isEliteBoard(void)
{
#if U64
    uint8_t rev = (U2PIO_BOARDREV >> 3);
    if (rev == 0x13) {
        return true;
    }
    if (rev == 0x14) { // may be either!
        uint8_t joyswap = C64_PLD_JOYCTRL;
        if (joyswap & 0x80) {
            return true;
        }
        return false;
    }
#endif
    return false;
}

uint8_t getProductId() {
    uint32_t capabilities = getFpgaCapabilities();
    if(capabilities & CAPAB_ULTIMATE64) {
        if (isEliteBoard()) {
            return PRODUCT_ULTIMATE_64_ELITE;
        } else {
            return PRODUCT_ULTIMATE_64;
        }
    } else if(capabilities & CAPAB_ULTIMATE2PLUS) {
        if (capabilities & CAPAB_FPGA_TYPE) {
            return PRODUCT_ULTIMATE_II_PLUS_L;
        } else {
            return PRODUCT_ULTIMATE_II_PLUS;
        }
    } else {
        return PRODUCT_ULTIMATE_II;
    }
    // Never reached
    return 0;
}

const char *getProductString() {
    uint8_t product = getProductId();
    if (product >= sizeof(product_name) / sizeof(char *))
        product = 0;
    return  product_name[product];
}

char *getProductVersionString(char *buf, int sz) {
    uint8_t fpga_version;
    const char *format;

    // Required size is max of all fields: Format chars + product + fpga_version + APPL_VERSION + NULL-byte
    if (sz < 7 + 16 + 2 + 5 + 1) {
        return NULL;
    }

    // FIXME: Should order of APPL vs fpga version be unified?
    switch(getProductId()) {
        case PRODUCT_ULTIMATE_II:
        case PRODUCT_ULTIMATE_II_PLUS:
        case PRODUCT_ULTIMATE_II_PLUS_L:
            format = "%s %s (1%b)";
            fpga_version = getFpgaVersion();
            sprintf(buf, format, getProductString(), APPL_VERSION, fpga_version);
            break;
        case PRODUCT_ULTIMATE_64:
        case PRODUCT_ULTIMATE_64_ELITE:
        case PRODUCT_ULTIMATE_64_ELITE_II:
            format = "%s (V1.%b) %s";
            fpga_version = C64_CORE_VERSION;
            sprintf(buf, format, getProductString(), fpga_version, APPL_VERSION);
            break;
        default:
            strcpy(buf, "Unknown product");
    }
    return buf;
}

char *getProductTitleString(char *buf, int sz)
{
    char product_version[41];

    if (sz < 17)
        return NULL;
    if (sz < sizeof(product_version) || !getProductVersionString(product_version, sizeof(product_version))) {
       strcpy(buf, "*** Ultimate ***");
       return buf;
    }

    // Add as many stars around the title as space will allow
    int space = 40 - strlen(product_version);
    char *format = "%s";
    if (space >= 8) format = "*** %s ***";
    else if (space >= 6) format = "** %s **";
    else if (space >= 4) format = "* %s *";
    sprintf(buf, format, product_version);
    return buf;
}

char *getProductDefaultHostname(char *buf, int sz) {
    if (sz < 16 + 7 + 1) {   // Name-XXYYZZ<NULL>
        return NULL;
    }

    // Base hostname
    uint8_t product = getProductId();
    if (product >= sizeof(product_hostname) / sizeof(char *))
        product = 0;
    char *hostname = product_hostname[product];
    strcpy(buf, hostname);

    // Try to make hostname unique by adding the last 3 octets of MAC to hostname (same algo used
    // by RMII driver, based on flash chip unique id).
    uint8_t serial[8];
    Flash *flash = get_flash();
    if (flash) {
        flash->read_serial(serial);
        uint8_t mac4 = serial[1] ^ serial[5];
        uint8_t mac5 = serial[2] ^ serial[6];
        uint8_t mac6 = serial[3] ^ serial[7];
        sprintf(buf, "%s-%b%b%b", hostname, mac4, mac5, mac6);
    }
    return buf;
}
