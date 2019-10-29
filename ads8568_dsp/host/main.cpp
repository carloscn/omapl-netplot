#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <QtGui/QApplication>
#include "mainwindow.h"

void usage_and_exit(int status) {
    printf("dual core ad7606 sampling example v1.1 by tronglong tech ltd(www.tronlong.com)\n");
    printf("usage: host_app rate_power channel\n");
    printf("       The sample rate would be 2^rate_power (K), and up to 128K.\n");
    printf("       So that the value of rate_power only can be 0 ~ 7.\n");
    printf("       The sampling channel must be 0 ~ 7.\n");
    exit(status);
}

static void parse_cmdline(int argc, char **argv, int &rate_power, int &channel) {
    int ret = -1;
    while ((ret = getopt(argc, argv, "r:c:h")) != -1) {
		switch (ret) {
        case 'r':
            rate_power = atoi(optarg);
            break;
        case 'c':
            channel = atoi(optarg);
            break;
        case 'h':
            usage_and_exit(-1);
		default:
            usage_and_exit(0);
		}
	}

    if (rate_power < 0 || rate_power > 7) {
        printf("the power of sampling rate must be 0 ~ 7\n");
        usage_and_exit(-1);
    }

    if (channel < 0 || channel > 7) {
        printf("the sampling channel index must be 0 ~ 7\n");
        usage_and_exit(-1);
    }
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    int rate_power = -1;
    int channel = -1; 
    parse_cmdline(argc, argv, rate_power, channel);
    MainWindow w(pow(2, rate_power), channel);
    w.show();

    return a.exec();
}
