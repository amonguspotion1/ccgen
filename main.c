#include "random.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

char checksum_digit(char* card_number) {
    int total = 0;
    for (int i = 0; i < 15; i++) {
        int digit = card_number[i] - 0x30;
        if (i % 2 == 0) {
            digit *= 2;
            if (digit > 9) digit -= 9;
        }
        total += digit;
    }

    return 0x30 + ((10 - (total % 10)) % 10);
}

int main(int argc, char** argv) {
    int count = 10;
    if ((argc != 2 && argc != 3)
        || (argc == 3 && sscanf(argv[2], "%u", &count) != 1)) {
        printf("ccgen <BIN> [count]\n");
        return 1;
    } else if (count == 0) {
        return 0;
    }

    char bin[17];
    int bin_len;
    {
        memset(bin, 0, 17);
        const char* arg = argv[1];
        int arg_len = strlen(arg);
        for (int i = 0, j = 0; i < arg_len; i++) {
            char chr = arg[i];
            if (chr >= 48 && chr <= 57) {
                if (j > 15) break;
                bin[j] = chr;
                j += 1;
            }
        }
        bin_len = strlen((const char*)bin);
    }
    
    int current_year;
    {
        time_t t = time(0);
        current_year = 1900 + localtime(&t)->tm_year;
    }

    char* output_buf = malloc(
        (
            16 // credit card number
            + 1 // pipe
            + 2 // month
            + 1 // pipe
            + 4 // year
            + 1 // pipe
            + 4 // cvv
            + 1 // newline
        ) * count 
        + 1 // trailing zero
    );
    int output_cursor = 0;

    for (int _i = 0; _i < count; _i++) {
        char card_number[17];
        memcpy(card_number, bin, 17);
        for (int i = bin_len; i < 15; i++) {
            card_number[i] = rnd(48, 57);
        }
        card_number[15] = checksum_digit(card_number);

        int month = rnd(1,12);

        int year = current_year + rnd(2, 6);

        int ccv;
        if (card_number[0] == '3') {
            ccv = rnd(1102, 9998);
        } else {
            ccv = rnd(112, 998);
        }

        output_cursor += sprintf(
            output_buf + output_cursor, 
            "%s|%02d|%d|%d\n", 
            card_number, 
            month, 
            year, 
            ccv
        );
    }
    output_buf[output_cursor] = 0;

    printf("%s", output_buf);

    free(output_buf);

    return 0;
}
