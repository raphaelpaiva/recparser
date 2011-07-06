void do_something()
{
	return;
}

int main() {
	int a; int b;
	
	a = 10; b = 5;

	while(a && b) {
		a = a - b;
		b = b - 1;
	}

	return a;
}
