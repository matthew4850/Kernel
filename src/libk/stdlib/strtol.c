long int strtol (const char* str, char** endptr, int base){
	int neg = 0;
	long val = 0;

	while (*str == ' ' || *str == '\t') {
		str ++;
	}

	if (*str == '+') {
		str ++;
	}
	else if (*str == '-') {
		str ++, neg = 1;
	}

	if ((base == 0 || base == 16) && (str[0] == '0' && str[1] == 'x')) {
		str += 2, base = 16;
	}
	else if (base == 0 && str[0] == '0') {
		str ++, base = 8;
	}
	else if (base == 0) {
		base = 10;
	}

	while (1) {
		int dig;

		if (*str >= '0' && *str <= '9') {
			dig = *str - '0';
		}
		else if (*str >= 'a' && *str <= 'z') {
			dig = *str - 'a' + 10;
		}
		else if (*str >= 'A' && *str <= 'Z') {
			dig = *str - 'A' + 10;
		}
		else {
			break;
		}
		if (dig >= base) {
			break;
		}
		str ++, val = (val * base) + dig;
	}

	if (endptr) {
		*endptr = (char *) str;
	}
	return (neg ? -val : val);
}