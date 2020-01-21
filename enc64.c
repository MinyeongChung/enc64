#include "enc64.h"


char decode64(const char c) {
	if (c >= 97) {
		/* lower case alphabet */
		/* char(a) = 97, a = 26 */
		return c - 71;
	}
	else if (c >= 65) {
		/* upper case alphabet */
		/* char(A) = 65, A = 0 */
		return c - 65;
	}
	else if (c >= 48) {
		/* numerics */
		/* char(0) = 48, 0 = 52 */
		return c + 4;
	}
	else if (c == 47) {
		/* slash */
		/* char(slash) = 47, slash = 63 */
		return c + 16;
	}
	else if (c == 43) {
		/* + */
		/* char(+) = 43, + = 62 */
		return c + 19;
	}
}

void read_64enc(const char *name, char *arr, int size) {
	int read_size;
	int remainder;
	int iterate;
	char *arr_64enc;
	FILE *f;

	f = fopen(name, "r");
	fseek(f, 0, SEEK_END);
	read_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	arr_64enc = malloc(read_size);
	fscanf(f, "%s", arr_64enc);

	iterate = read_size / 4;

	for(int i = 0; i < iterate; i++) {
		arr[3 * i] 		= decode64(arr_64enc[4 * i]) << 2 | decode64(arr_64enc[4 * i + 1]) >> 4;
		arr[3 * i + 1] 	= decode64(arr_64enc[4 * i + 1]) << 4 | decode64(arr_64enc[4 * i + 2]) >> 2;
		arr[3 * i + 2] 	= decode64(arr_64enc[4 * i + 2]) << 6 | decode64(arr_64enc[4 * i + 3]);
	}

	if (arr_64enc[read_size - 1] == '=') {
		if (arr_64enc[read_size - 2] == '=') {
			arr[size - 1] = decode64(arr_64enc[read_size - 4]) << 2 | decode64(arr_64enc[read_size - 3]) >> 4;
		} else {
			arr[size - 2] = decode64(arr_64enc[read_size - 4]) << 2 | decode64(arr_64enc[read_size - 3]) >> 4;
			arr[size - 1] = decode64(arr_64enc[read_size - 3]) << 4 | decode64(arr_64enc[read_size - 2]) >> 2;
		}
	}
}	

void write_64enc(const char *name, char *arr, int size) {
	const char base[] =  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	
	int remainder = size % 3;
	int iterate = size / 3;
	int new_size;

	switch(remainder) {
		case 0:
			new_size = 4 * iterate;
			break;
		case 1:
			new_size = 4 * iterate + 4; // 1 8bit = 2 6bit, and 2 '='
			break;
		case 2:
			new_size = 4 * iterate + 4; // 2 8bit = 3 6bit, and 1 '='
			break;
		default:
			new_size = 0;
			break;
	}

	char *arr_64enc = malloc(new_size);

	for(int i = 0; i < iterate; i++) {
		arr_64enc[4*i] 		= base[(arr[3 * i] 		>> 2) & 0x3F];
		arr_64enc[4*i + 1] 	= base[(arr[3 * i] 		<< 4 | arr[3 * i + 1] >> 4) & 0x3F];
		arr_64enc[4*i + 2] 	= base[(arr[3 * i + 1] 	<< 2 | arr[3 * i + 2] >> 6) & 0x3F];
		arr_64enc[4*i + 3] 	= base[(arr[3 * i + 2] & 0x3F)];
	}

	if (remainder == 1) {
		arr_64enc[new_size - 4] = base[(arr[size - 1] >> 2) & 0x3F];
		arr_64enc[new_size - 3] = base[(arr[size - 1] << 4) & 0x3F];
		arr_64enc[new_size - 2] = '=';
		arr_64enc[new_size - 1] = '=';
	}
	else if(remainder == 2) {
		arr_64enc[new_size - 4] = base[(arr[size - 2] >> 2) & 0x3F];
		arr_64enc[new_size - 3] = base[(arr[size - 2] << 4 | arr[size - 1] >> 4)];
		arr_64enc[new_size - 2] = base[(arr[size - 1] << 2) & 0x3C];
		arr_64enc[new_size - 1] = '=';
	}

	FILE *f;
	f = fopen(name, "w");
	fprintf(f, "%s", arr_64enc);
	fclose(f);

	free(arr_64enc);

	return;
}
