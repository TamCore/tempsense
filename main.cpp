#include <hidapi.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>

int main(int argc, char **argv) {
    int i, num;
    float temp, offset = 0;
    unsigned char buf[64];
    char *pwr;

    hid_device *handle = hid_open(0x16c0, 0x0480, nullptr);
    if (!handle) {
        fprintf(stderr, "No temperature sensor found!\n");
        return 1;
    }

    if (argc == 2) {
        offset = atof(argv[1]);
    }
    
    while (true) {
        num = hid_read(handle, buf, 64);
        if (num < 0) {
            fprintf(stderr, "Could not read from device!\n");
            return 4;
        }
        if (num == 64) {
            if (buf[2]) { pwr = "Extern"; }
            else { pwr = "Parasite"; }

            temp = *(short *)&buf[4];
            temp = (float)temp/10.0f-(float)offset;

            time_t now;
            time(&now);
            tm loctm;
            localtime_r(&now, &loctm);

            fprintf(stdout, "[%02d:%02d:%02d] Sensor #%d of %d: \t %.1f\u2103 \t Power: %-10s \t ID: ", loctm.tm_hour, loctm.tm_min, loctm.tm_sec, buf[1], buf[0], temp, pwr);
            for (i = 0x08; i < 0x10; i++) {
                fprintf(stdout, "%02X ", (unsigned char)buf[i]);
            }
            printf("\n");
        }
    }
    return 0;
}

