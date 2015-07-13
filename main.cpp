#include <hidapi.h>
#include <cstdlib>
#include <ctime>
#include "cmdline/cmdline.h"
#include <string>

int main(int argc, char **argv) {
    int i, num, mon_cnt = 0;
    float temp, mon_temp, offset = 0;
    unsigned char buf[64];
    std::string pwr;

    cmdline::parser a;
    a.add<int>("offset", 'o', "offset from measured temperature", false, 0);
    a.add("monitoring", 'm', "monitoring mode", false, false);
    a.parse_check(argc, argv);

    hid_device *handle = hid_open(0x16c0, 0x0480, nullptr);
    if (!handle) {
        fprintf(stderr, "No temperature sensor found!\n");
        return 1;
    }

    offset = a.get<int>("offset");
    
    while (true) {
        num = hid_read(handle, buf, 64);
        if (num < 0) {
            fprintf(stderr, "Could not read from device!\n");
            return 4;
        }
        if (num == 64) {
            temp = *(short *)&buf[4];
            temp = (float)temp/10.0f-(float)offset;

            if (a.exist("monitoring")) {
                if (mon_cnt < buf[0]) {
                    mon_temp = (mon_temp + temp);
                    mon_cnt = (mon_cnt + 1);
                    continue;
                } else {
                    fprintf(stdout, "%.1f\n", (mon_temp / mon_cnt));
                    break;
                }
            }

            if (buf[2]) { pwr = "Extern"; }
            else { pwr = "Parasite"; }

            time_t now;
            time(&now);
            tm loctm;
            localtime_r(&now, &loctm);

            fprintf(stdout, "[%02d:%02d:%02d] Sensor #%d of %d: \t %.1f\u2103 \t Power: %-10s \t ID: ", loctm.tm_hour, loctm.tm_min, loctm.tm_sec, buf[1], buf[0], temp, pwr.c_str());
            for (i = 0x08; i < 0x10; i++) {
                fprintf(stdout, "%02X ", (unsigned char)buf[i]);
            }
            printf("\n");
        }
    }
    return 0;
}
